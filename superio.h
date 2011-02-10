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


#ifndef superio_h_SDR_3s21234msddaa_included
#define superio_h_SDR_3s21234msddaa_included

//#define _GNU_SOURCE
#include <poll.h>
#include <signal.h>

#include "dllists.h"

#ifdef  __cplusplus
extern "C" {
#endif

#define SIO_FDS_GROW 100

struct sio_ctx_tag ;
typedef struct sio_ctx_tag sio_ctx_t;

typedef void (*sio_hup_t) (sio_ctx_t* fh);

struct sio_ctx_tag {
	dll_listh_t fhlist;
	int listsz;
	struct pollfd *fds;
	int fds_cap, fds_use;
	int fds_grow;
	int rebuild_fds;
	sigset_t sigtomask;
	sio_hup_t sio_hup;
	int breakrq;
	int composedfhs;
};

#define sio_lh2ctx(lh) ((sio_ctx_t*)(lh))

struct sio_buf_tag ;
typedef struct sio_buf_tag sio_buf_t;

struct sio_fh_tag ;
typedef struct sio_fh_tag sio_fh_t;

typedef void (*sio_read_ok_t) (short event,sio_fh_t* fh);
typedef void (*sio_write_ok_t) (short event,sio_fh_t* fh);
typedef void (*sio_closed_t) (short event,sio_fh_t* fh);

#define SIO_FH_BUFS 4
#define SIO_FH_UD 4

#define SIO_FH_FREEABLE 1
#define SIO_FH_NONFREEABLE 0

struct sio_fh_tag {
	dll_blockh_t fhlisth;
	int fd;
	short events;
	sio_read_ok_t sio_read_ok;
	sio_write_ok_t sio_write_ok;
	sio_closed_t sio_closed;
	int tobecleaned;
	int fdsi;
	int autobufevents;
	sio_buf_t *bufs[SIO_FH_BUFS];
	void *userdata[SIO_FH_UD];
};

#define sio_bh2fh(lh) ((sio_fh_t*)(lh))
#define sio_fh2sio(fh) ((sio_ctx_t*)((fh)->fhlisth.dll_list))

sio_ctx_t *sio_alloc_sio (int start_fds_cap, int blocksize);
sio_fh_t *sio_addfh (sio_ctx_t* ctx, int fd, short int events, sio_read_ok_t sio_read_ok, sio_write_ok_t sio_write_ok, sio_closed_t sio_closed, int blocksize, int initsize);

void sio_initfh (sio_fh_t* fhe, int fd, short int events, sio_read_ok_t sio_read_ok, sio_write_ok_t sio_write_ok, sio_closed_t sio_closed, int initsize, int freeable);
void sio_mutefh (sio_fh_t* fhe, int close);
sio_fh_t *sio_addrdyfh (sio_ctx_t* ctx, sio_fh_t* fhe);

void sio_delfh (sio_fh_t *fhe);
void sio_delfh_ex (sio_fh_t *fhe,int mute,int close);

void sio_update_events (sio_fh_t *fhe,int newevents);

#define SIO_IOLOOP_OK 0
#define SIO_IOLOOP_ERR 1
#define SIO_IOLOOP_ERR_OOM 2
#define SIO_IOLOOP_ERR_POOL 3

int sio_ioloop (sio_ctx_t* ctx, int autobufevents);


/**
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

//================== SIO_BUF ====
struct sio_buf_tag {
	sio_fh_t *reader;
	sio_fh_t *writer;
	int bufsz;
	char *ud;
	int udsz;
	int read_err,write_err;
	int last_readsz,last_writesz;
	int max_readsz,max_writesz;
	int avl_readsz,avl_writesz;
	
	char *readhead,*writehead,*sod,*eod;
	int readsuspended,writesuspended;
	int closeonsend;
};

sio_buf_t *sio_allocbuf (int sz, sio_fh_t *areader,sio_fh_t *awriter,int reserve);
void sio_freebuf (sio_buf_t *buf);

///write to buf and read from associated fh
int sio_buf_perform_fhread (sio_buf_t *buf);

///read from buf and write to associated fh
int sio_buf_perform_fhwrite (sio_buf_t *buf);

///correct read/write head and sizes
void sio_buf_post_add_data (sio_buf_t *buf,int sz);
///correct read/write head and sizes
void sio_buf_post_used_data (sio_buf_t *buf,int sz);

///move data around do data starts at buffer starts
///this might involve some temp memory allocation
void sio_buf_realign (sio_buf_t *buf);

///reset buffer to ampty state loosing all data in it!
void sio_buf_reset (sio_buf_t *buf);


void sio_autobuf_read (short event,sio_fh_t* fh);
void sio_autoflush_read (short event,sio_fh_t* fh);

void sio_autobuf_write (short event,sio_fh_t* fh);
void sio_autobuf_closed (short event,sio_fh_t* fh);

int sio_fdclose(int *fd);
#ifdef  __cplusplus
}
#endif

#endif