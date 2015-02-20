/**************************************************************************
*   Copyright (C) 2010 by Stoian Ivanov                                   *
*   sdr@tera-com.com                                                      *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU Library General Public License version  *
*   2 as published by the Free Software Foundation;                       *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU Library General Public     *
*   License along with this program; if not, write to the                 *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/
#define _GNU_SOURCE

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <poll.h>
#include <signal.h>
#include <errno.h>

#include "superio.h"

#include "dllists.h"

#undef POLL_IN
#define POLL_IN this_is_a_horrible_mistake_for_POLLIN

#undef POLL_OUT
#define POLL_OUT this_is_a_horrible_mistake_for_POLLOUT


sio_ctx_t *sio_alloc_sio (int start_fds_cap, int blocksize){
	if (blocksize<sizeof(sio_ctx_t)) blocksize=sizeof(sio_ctx_t);
	sio_ctx_t *ctx=(sio_ctx_t *)dll_alloc_list(blocksize);
	if (!ctx) return NULL;
	memset (ctx,0,blocksize);
	if (start_fds_cap>0){
		ctx->fds_grow=start_fds_cap;
		ctx->fds_cap=start_fds_cap;
		ctx->fds=malloc(sizeof(struct pollfd)*ctx->fds_cap);
		if (!ctx->fds){
			free(ctx); return NULL;
		}
	}
	if (!ctx->fds_grow) ctx->fds_grow=SIO_FDS_GROW;
	sigemptyset (&ctx->sigtomask);
	return ctx;
}

sio_fh_t *sio_addfh (sio_ctx_t *ctx,int fd, short events,sio_read_ok_t sio_read_ok,sio_write_ok_t sio_write_ok,sio_closed_t sio_closed,int blocksize,int initsize){
	if (blocksize<sizeof(sio_fh_t)) blocksize=sizeof(sio_fh_t);
	sio_fh_t *fhe=(sio_fh_t *)dll_alloc_block(blocksize,initsize);
	if (!fhe) return NULL;

	sio_initfh (fhe, fd,  events, sio_read_ok,sio_write_ok,sio_closed,-1,SIO_FH_FREEABLE);
	sio_addrdyfh(ctx,fhe);
	return fhe;
}

void sio_initfh (sio_fh_t *fhe, int fd, short int events, sio_read_ok_t sio_read_ok, sio_write_ok_t sio_write_ok, sio_closed_t sio_closed, int initsize,int freeable){
	if (initsize!=-1 && initsize<sizeof(sio_fh_t)) initsize=sizeof(sio_fh_t);
	
	if (freeable==SIO_FH_FREEABLE ) dll_init_block (&(fhe->fhlisth),initsize,DLL_BLOCK_MEM_FLAG_FREE_HEAD);
	else dll_init_block (&(fhe->fhlisth),initsize,DLL_BLOCK_MEM_FLAG_FREE_NONE);
	
	fhe->fd=fd;
	fhe->events=events;
	fhe->sio_read_ok=sio_read_ok;
	fhe->sio_write_ok=sio_write_ok;
	fhe->sio_closed=sio_closed;
	
}

void sio_mutefh (sio_fh_t* fhe, int close) {
	if (!fhe) return;
	if (close) sio_fdclose(&fhe->fd);
	fhe->events=0;
	fhe->sio_read_ok=NULL;
	fhe->sio_write_ok=NULL;
	fhe->sio_closed=NULL;
}


sio_fh_t *sio_addrdyfh (sio_ctx_t* ctx, sio_fh_t* fhe){
	dll_add_after(&ctx->fhlist,ctx->fhlist.dll_tail,&fhe->fhlisth);
	ctx->listsz++; ctx->rebuild_fds=1;
	if ((fhe->fhlisth.dll_block_mem_flags & DLL_BLOCK_MEM_FLAG_FREE_HEAD) ==0) ctx->composedfhs=1;
	return fhe;
	
}

void sio_delfh_ex (sio_fh_t *fhe,int mute,int close) {
	if (!fhe ) return;
	if (mute) sio_mutefh(fhe,close);
	sio_delfh(fhe);
}

void sio_delfh (sio_fh_t* fhe){
	if (!fhe ) return;
	fhe->tobecleaned=1;
	fhe->events=0;
	sio_ctx_t *ctx=sio_fh2sio(fhe); 
	if (ctx) ctx->rebuild_fds=1;
}

void sio_update_events (sio_fh_t *fhe,int newevents) {
	if (!fhe || fhe->events==newevents ) return;
	fhe->events=newevents;
	sio_ctx_t *ctx=sio_fh2sio(fhe); 
	if (fhe->fdsi==-1) {
		ctx->rebuild_fds=1;
	} else {
		ctx->fds[fhe->fdsi].events=fhe->events;
	}
}


int sio_ioloop (sio_ctx_t* ctx, int autobufevents) {
	
	dll_list_iterator_t li;
	sio_fh_t *fhe;
	struct pollfd* fd;
	
	while (ctx->listsz) {
		if (ctx->rebuild_fds){
			ctx->rebuild_fds=0;
			if (ctx->fds_cap<ctx->listsz){
				if (ctx->fds) free(ctx->fds);
				if (ctx->fds_grow<=0) ctx->fds_grow=SIO_FDS_GROW;
				if (ctx->fds_cap+ctx->fds_grow>ctx->listsz) ctx->fds_cap+=ctx->fds_grow;
				else ctx->fds_cap=ctx->fds_grow+ctx->listsz;
				ctx->fds=malloc(sizeof(struct pollfd)*ctx->fds_cap);
				if (!ctx->fds) return SIO_IOLOOP_ERR_OOM;
			}
			ctx->fds_use=0;
			int composedfhs=0;
			int secondrun=0;
			dll_iterate_fwd(&ctx->fhlist,&li);
			while (NULL!=(fhe=sio_bh2fh(dll_next(&li)))) {
				if (fhe->tobecleaned) {
					int bi=0;
					for (; bi<SIO_FH_BUFS; bi++) {
						if (fhe->bufs[bi]) {
							if (fhe->bufs[bi]->reader==fhe) fhe->bufs[bi]->reader=NULL;
							if (fhe->bufs[bi]->writer==fhe) fhe->bufs[bi]->writer=NULL;
							if (fhe->bufs[bi]->reader==NULL && fhe->bufs[bi]->writer==NULL){
								sio_freebuf(fhe->bufs[bi]);
								fhe->bufs[bi]=NULL;
							}
						}
					}
					if (fhe->fd>0) sio_fdclose (&fhe->fd);
					if ((fhe->fhlisth.dll_block_mem_flags & DLL_BLOCK_MEM_FLAG_FREE_HEAD) ==0) { //this is allways safe to unlink
						dll_deinit_block((dll_blockh_t*)fhe,DLL_DEINIT_BLOCK_UNLINK);
					} else { //this will invalidate memory on unlinking
						if (!ctx->composedfhs) { // no composed fhs it's ok to free 
							dll_deinit_block((dll_blockh_t*)fhe,DLL_DEINIT_BLOCK_UNLINK);
						} else { //it is dangerous to unlink as we might have a fh in list that will be invalidated
							secondrun=1;
						}
					}
					continue;
				}
				
				if ((fhe->fhlisth.dll_block_mem_flags & DLL_BLOCK_MEM_FLAG_FREE_HEAD) ==0) composedfhs=1;
				
				if (fhe->fd>0) {
					fd=&ctx->fds[ctx->fds_use];
					fhe->fdsi=ctx->fds_use;
					ctx->fds_use++;
					fd->events=fhe->events;
					fd->fd=fhe->fd;
					fd->revents=0;
				} else {
					fhe->fdsi=-1;
				}
			}
			
			ctx->composedfhs=composedfhs;
			if (secondrun) { //now just go and unlink what's left 
				dll_iterate_fwd(&ctx->fhlist,&li);
				while (NULL!=(fhe=sio_bh2fh(dll_next(&li)))) {
					if (fhe->tobecleaned) {
						dll_deinit_block((dll_blockh_t*)fhe,DLL_DEINIT_BLOCK_UNLINK);
					}
				}
			}
			
		}
		if (ctx->fds_use==0) break;
		
		if (autobufevents) {
			dll_iterate_fwd(&ctx->fhlist,&li);
			while (NULL!=(fhe=sio_bh2fh(dll_next(&li)))) {
				if (fhe->autobufevents && fhe->events!=0 && fhe->fdsi>=0) {
					short cevents=fhe->events; 
					int bi=0;
					int isreder=0;
					int iswriter=0;
					int isread_needed=0;
					int iswrite_needed=0;
					for (; bi<SIO_FH_BUFS; bi++) {
						if (fhe->bufs[bi]!=NULL) {
							if (fhe->bufs[bi]->reader==fhe) {
								isreder=1;
								if (fhe->bufs[bi]->max_readsz>0) isread_needed=1;
							} 
							if (fhe->bufs[bi]->writer==fhe) {
								iswriter=1;
								if (fhe->bufs[bi]->max_writesz>0) iswrite_needed=1;
							}
						}
					}
					if ((cevents&POLLIN) && isreder && isread_needed==0) cevents=cevents & (~POLLIN);
					if ((cevents&POLLOUT) && iswriter && iswrite_needed==0) cevents=cevents & (~POLLOUT);
					if (cevents!=fhe->events) ctx->fds[fhe->fdsi].events=cevents;
				}
			}
		}
		int res=ppoll(ctx->fds,ctx->fds_use,NULL,&ctx->sigtomask);
		int errnolocal=errno;
		if (ctx->breakrq ) break;
		if (res==-1){
			if (errnolocal==EINTR) continue;
			return SIO_IOLOOP_ERR_POOL;
		}
		if (res<1) continue; //wtf?
			
		dll_iterate_fwd(&ctx->fhlist,&li);
		while (NULL!=(fhe=sio_bh2fh(dll_next(&li)))) {
			if (fhe->fdsi!=-1) {
				fd=&ctx->fds[fhe->fdsi];
				if (fd->revents==0) continue;
				if ((fd->revents & (POLLIN|POLLPRI))  && fhe->sio_read_ok) fhe->sio_read_ok (fd->revents,fhe);
				if ((fd->revents & (POLLOUT))  && fhe->sio_write_ok) fhe->sio_write_ok (fd->revents,fhe);
				if ((fd->revents & (POLLERR|POLLHUP|POLLNVAL))) {
					if (fhe->sio_closed) fhe->sio_closed (fd->revents,fhe);
					else {
						sio_fdclose(&fhe->fd);
						sio_delfh (fhe);
					}
				}
				
				fd->revents=0;
			}
		}
		
	}
	
	dll_iterate_fwd(&ctx->fhlist,&li);
	while (NULL!=(fhe=sio_bh2fh(dll_next(&li)))) {
		if (fhe->tobecleaned) {
			dll_deinit_block((dll_blockh_t*)fhe,DLL_DEINIT_BLOCK_UNLINK);
			continue;
		}
	}
	ctx->rebuild_fds=1;
	
	return SIO_IOLOOP_OK;
}


sio_buf_t *sio_allocbuf (int sz, sio_fh_t* areader, sio_fh_t* awriter, int reserve) {
	if (reserve<0) reserve=0;
	if (sz<0) return NULL;
	sio_buf_t *buf=malloc(sizeof(sio_buf_t)+reserve+sz+1);
	if (!buf) return NULL;
	memset(buf,0,sizeof(sio_buf_t)+reserve);
	buf->bufsz=sz;
	buf->ud=(char *)(buf+1);
	buf->udsz=reserve;
	buf->sod=buf->ud+reserve;
	buf->eod=buf->sod+sz;
	buf->readhead=buf->sod;
	buf->writehead=buf->sod;
	if (areader) {
		buf->reader=areader;
		int bi=0;
		for (;bi<SIO_FH_BUFS;bi++) if (areader->bufs[bi]==NULL){
			areader->bufs[bi]=buf;
			break;
		}
	}
	if (awriter){
		buf->writer=awriter;
		int bi=0;
		for (;bi<SIO_FH_BUFS;bi++) if (awriter->bufs[bi]==NULL){
			awriter->bufs[bi]=buf;
			break;
		}
	}
	buf->max_readsz=sz;
	buf->avl_readsz=sz;
	buf->max_writesz=0;
	buf->avl_writesz=0;
	return buf;
	
}

void sio_freebuf (sio_buf_t *buf) {
	if (!buf) return;
	if (buf->reader) {
		int bi=0; for (;bi<SIO_FH_BUFS; bi++){
			if (buf->reader->bufs[bi]==buf) buf->reader->bufs[bi]=NULL;
		}
		buf->reader=NULL;
	}
	if (buf->writer) {
		int bi=0; for (;bi<SIO_FH_BUFS; bi++){
			if (buf->writer->bufs[bi]==buf) buf->writer->bufs[bi]=NULL;
		}
		buf->writer=NULL;
	}
	buf->bufsz=0; 
	buf->readhead=buf->sod;
	buf->writehead=buf->sod;
	buf->eod=buf->sod;
	buf->max_readsz=0;
	buf->max_writesz=0;
	buf->avl_readsz=0;
	buf->avl_writesz=0;
	free (buf);
}

/*
|<-sod...................................................eod->|
case 1:
**************************************************************
|                 |>>______DATA______##|                     |
**************************************************************
..................^wh..................^rh
                  
case 2:
**************************************************************
|>_______DATA______##|                  |>>______DATA_______>|
**************************************************************
.....................^rh................^wh

case 3:
**************************************************************
|                      |                                     |
**************************************************************
.....................wh^rh

case 4:
**************************************************************
|>_______DATA______##|>>______________DATA__________________>|
**************************************************************
...................wh^rh
*/
void sio_buf_realign (sio_buf_t *buf) {
	if (!buf || buf->writehead==buf->sod) return; //nomthing to do
		
	if (buf->avl_writesz==0) { //case 3? how did we got here?
		buf->readhead=buf->sod;
		buf->writehead=buf->sod;
		buf->max_readsz=buf->bufsz;
		buf->max_writesz=0;
		return;
	}
	
	if ((buf->writehead)<(buf->readhead)) { //case 1
		int delta=(buf->writehead)-(buf->sod);
		if (delta>buf->avl_writesz) {//we don't overlap!
			memcpy(buf->sod,buf->writehead,buf->avl_writesz);
		} else {
			memmove(buf->sod,buf->writehead,buf->avl_writesz);
		}
		buf->writehead=buf->sod;
		buf->readhead-=delta;
		buf->max_readsz+=delta;
	} else { //case 2,4 (worst case)
		int evacsz=buf->readhead-buf->sod;
		int movsz=buf->eod-buf->writehead;
		int delta=buf->writehead-buf->sod;
		//this is going to be ugly!
		void *evac=malloc(evacsz);
		if (!evac) return; //no mem to evac the wrapped data 
		memcpy(evac,buf->sod,evacsz);

		if (delta>movsz) {//we don't overlap!
			memcpy(buf->sod,buf->writehead,buf->avl_writesz);
		} else {
			memmove(buf->sod,buf->writehead,buf->avl_writesz);
		}
		buf->writehead=buf->sod;
		buf->readhead=buf->sod+movsz;
		memcpy(buf->readhead,evac,evacsz);
		free (evac);
		buf->max_writesz=movsz+evacsz;
		
	}
}
void sio_buf_post_add_data (sio_buf_t *buf,int sz) {
	buf->readhead+=sz;
	buf->avl_readsz-=sz;
	buf->avl_writesz+=sz;
	if (buf->readhead>=buf->eod) { //wrap 
		buf->readhead=buf->sod;
	}
	if (buf->readhead>buf->writehead) { //case 1
		buf->max_readsz=buf->eod-buf->readhead;
		buf->max_writesz=buf->readhead-buf->writehead;
	} else { //case 2,case 4
		buf->max_readsz=buf->writehead-buf->readhead;
		buf->max_writesz=buf->eod-buf->writehead;
	}
}

void sio_buf_post_used_data (sio_buf_t *buf,int sz) {
	buf->writehead+=sz;
	buf->avl_readsz+=sz;
	buf->avl_writesz-=sz;
	if (buf->writehead>=buf->eod) { //wrap 
		buf->writehead=buf->sod;
	}
	if (buf->readhead==buf->writehead) { //case 3 we can rellocate from begining of memory
		buf->readhead=buf->sod;
		buf->writehead=buf->sod;
		buf->max_readsz=sz;
		buf->max_writesz=0;
		return;
	} 
	if (buf->readhead>buf->writehead) { //case 1
		buf->max_readsz=buf->eod-buf->readhead;
		buf->max_writesz=buf->readhead-buf->writehead;
	} else { //case 2
		buf->max_readsz=buf->writehead-buf->readhead;
		buf->max_writesz=buf->eod-buf->writehead;
	}
}

int sio_buf_perform_fhread (sio_buf_t *buf) {
	if (!buf || !buf->reader) return 0;
	buf->last_readsz=0;
	if(buf->max_readsz==0) return 0;
	buf->last_readsz=read(buf->reader->fd,buf->readhead,buf->max_readsz);
	if (buf->last_readsz>0) sio_buf_post_add_data(buf,buf->last_readsz);
	return 1;
}


int sio_buf_perform_fhwrite (sio_buf_t *buf) {
	if (!buf || !buf->writer) return 0;
	buf->last_writesz=0;
	if(buf->max_writesz==0) return 0;
	buf->last_writesz=write(buf->writer->fd,buf->writehead,buf->max_writesz);
	if (buf->last_writesz>0) {
		sio_buf_post_used_data(buf,buf->last_writesz);
		if (buf->closeonsend && buf->avl_writesz==0) {
			sio_fh_t*reader=buf->reader;
			sio_fh_t*writer=buf->writer;
			if (writer->sio_closed) writer->sio_closed(0,writer);
			else  sio_fdclose(&writer->fd);
			sio_delfh(writer);
			if (reader){
				if (reader->sio_closed) reader->sio_closed(0,reader);
				else sio_fdclose(&reader->fd);
				sio_delfh(reader);
			}
		}
	}
	return 1;

}

void sio_buf_reset (sio_buf_t *buf) {
	if (!buf) return;
	buf->readhead=buf->writehead=buf->sod;	
	buf->avl_readsz=buf->max_readsz=buf->bufsz;
	buf->max_writesz=buf->avl_writesz=0;
	
}

void sio_autobuf_read (short event,sio_fh_t* fh) {
	if (!fh) return;
	int bi=0;
	sio_buf_t *buf=NULL;
	for (;bi<SIO_FH_BUFS;bi++) if (fh->bufs[bi] && fh->bufs[bi]->reader==fh){
		buf=fh->bufs[bi];
		break;
	}
	if (!buf) {
		if (fh->sio_closed) fh->sio_closed(event,fh);
		else sio_fdclose(&fh->fd);
		sio_delfh(fh);
		return;
	}
	int sz=sio_buf_perform_fhread(buf);
	if (sz==0) {
		if (fh->sio_closed) fh->sio_closed(event,fh);
		else sio_fdclose(&fh->fd);
		sio_delfh(fh);
	}
}

void sio_autobuf_write (short event,sio_fh_t* fh) {
	if (!fh) return;
	int bi=0;
	sio_buf_t *buf=NULL;
	for (;bi<SIO_FH_BUFS;bi++) if (fh->bufs[bi] && fh->bufs[bi]->writer==fh){
		buf=fh->bufs[bi];
		break;
	}
	if (!buf) {
		if (fh->sio_closed) fh->sio_closed(event,fh);
		else sio_fdclose(&fh->fd);
		sio_delfh(fh);
		return;
	}
	int sz=sio_buf_perform_fhwrite(buf);
	if (sz==0) {
		if (fh->sio_closed) fh->sio_closed(event,fh);
		else sio_fdclose(&fh->fd);
		sio_delfh(fh);
	}
}

void sio_autobuf_closed (short event,sio_fh_t* fh) {
	if (!fh) return;
	fh->sio_closed=NULL; //prevent infinite indirect recursion
	int bi=0;
	sio_buf_t *buf=NULL;
	sio_fdclose(&fh->fd);
	sio_delfh(fh);
	for (;bi<SIO_FH_BUFS;bi++) if (fh->bufs[bi]){
		buf=fh->bufs[bi];
		if (buf->reader==fh) {//we are the reader here!
			if (buf->writer) {
				if (buf->avl_writesz==0) { //buffer is empty we shall close writer
					if (buf->writer->sio_closed) buf->writer->sio_closed (event,buf->writer);
					else sio_fdclose(&buf->writer->fd);
					sio_delfh(buf->writer);
				} else { //waith the writer to finish then close
					buf->closeonsend=1;
				}
			}
		} else { //we're the writer here! kill the reader as no data is needed any more
			if (buf->reader) {
				if (buf->reader->sio_closed) buf->reader->sio_closed (event,buf->reader);
				else sio_fdclose(&buf->reader->fd);
				sio_delfh(buf->reader);
			}
		}
	}
}

void sio_autoflush_read (short event,sio_fh_t* fh) {
	static char buf [1500];
	if(read(fh->fd,buf,1500)<=0) {
		if (fh->sio_closed) fh->sio_closed(event,fh);
		else sio_fdclose(&fh->fd);
		sio_delfh(fh);
	}
}


int sio_fdclose(int *fd){
	if (!fd) return -1;
	int fdsafe=*fd;
	if (fdsafe==-1) return 0; //ok allready closed
	*fd=-1;//mark as closed
	return close(fdsafe); //do the close
}