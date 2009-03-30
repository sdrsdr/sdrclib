/***************************************************************************
 *   Copyright (C) 2007 by Stoian Ivanov                                   *
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
 /** \file
      \brief String replacement functions (implementation).
	  \author Stoian Ivanov sdr@tera-com.com
  */


#include <string.h>

#include "str_replace.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

phctx_t * str_replace_ph_init (int bucket_names,int bucket_order, char delimiter, malloc_ft*use_malloc/*=NULL*/,free_ft* use_free/*=NULL*/) {
	if (use_malloc==NULL) use_malloc=malloc;
	if (use_free==NULL) use_free=free;
	
	phctx_t * ctx=use_malloc(sizeof(phctx_t));
	if (!ctx) return ctx; /// @return NULL on failure
	
	memset (ctx,0,sizeof(phctx_t));
	ctx->bucket_names=bucket_names;
	ctx->bucket_order=bucket_order;
	ctx->delimiter=delimiter;
	ctx->malloc=use_malloc;
	ctx->free=use_free;
	//oh! the sanity!
	if (ctx->bucket_names<0) ctx->bucket_names=10;
	if (ctx->bucket_order<ctx->bucket_names) ctx->bucket_order=3*ctx->bucket_names;
	return ctx; /// @return newly allocated phctx_t * if all ok 
}

void str_replace_ph_clean_charpp (free_ft *fr,char **arr, int count){
	if (count==0 || !arr) return;
	count --;
	while (count>=0){
		if (arr[count]) {
			fr(arr[count]);
			arr[count]=NULL;
		}
		count--;
	}
}

void str_replace_ph_free (phctx_t *ctx){
	if (!ctx) return;
	free_ft *fr=ctx->free;
	
	if (ctx->pattern && ctx->pattern_free) {
		fr(ctx->pattern);
		ctx->pattern=NULL;
	}
	if (ctx->ph_names) {
		str_replace_ph_clean_charpp (fr,ctx->ph_names,ctx->ph_names_count);
		fr(ctx->ph_names);
	}
	if (ctx->ph_values) {
		str_replace_ph_clean_charpp (fr,ctx->ph_values,ctx->ph_names_count);
		fr(ctx->ph_values);
	}
	if (ctx->ph_names_l) fr(ctx->ph_names_l);
	if (ctx->ph_order) fr(ctx->ph_order);
	if (ctx->ph_offset) fr(ctx->ph_offset);
	fr (ctx);
}

void str_replace_ph_reset_ph( phctx_t *ctx ){
	if (!ctx) return;
	ctx->prepared=0;
	if (ctx->ph_names) {
		str_replace_ph_clean_charpp (ctx->free,ctx->ph_names,ctx->ph_names_count);
	}
	if (ctx->ph_values) {
		str_replace_ph_clean_charpp (ctx->free,ctx->ph_values,ctx->ph_names_count);
	}
	ctx->ph_names_count=0;
	ctx->ph_order_count=0;
}

int  str_replace_ph_config_pattern(phctx_t *ctx,char *pattern,int copy,int plen/*=-1*/){
	if (!ctx||!pattern) return 0; /// @return 0 on failure
	if (plen<0) plen=strlen(pattern);
	ctx->prepared=0;
	if (ctx->pattern && ctx->pattern_free) ctx->free(ctx->pattern);
	ctx->pattern_len=plen;
	if (copy) {
		ctx->pattern_free=1;
		ctx->pattern=ctx->malloc(plen+1); //keep it asciiZ
		if (ctx->pattern) { //got memory
			memcpy (ctx->pattern,pattern,plen); //keep it asciiZ
			ctx->pattern[plen]=0;
		} else { //nt enough memory :(
			ctx->pattern_free=0;
			return 0;
		}
	} else {
		ctx->pattern_free=0;
		ctx->pattern=pattern;
	}
	return 1; /// @return 1 if OK
}


int str_replace_ph_set_ph (
	phctx_t *ctx,
	char *ph_name,
	int ph_nlen/*=-1*/,
	char *ph_value/*=NULL*/,
	int ph_vlen/*=-1*/
){
	///both name and value are copied into the ctx
	if (!ctx || !ph_name) return 0; ///@return 0 on failure
	if (ph_nlen<0) ph_nlen=strlen (ph_name);
	
	int x=0,idx=-1;
	for (;x<ctx->ph_names_count;x++) {
		if (
			ctx->ph_names[x]
			&& ph_nlen==ctx->ph_names_l[x] 
			&& memcmp(ctx->ph_names[x],ph_name,ph_nlen)==0
		) {
			idx=x;
			break;
		}
	}
	
	if (idx==-1) { //new placeholder!
		ctx->prepared=0; //need to re-prepare!
		if (ctx->ph_names_count>=ctx->ph_names_size){ //no space left GROW!
			int new_count=ctx->ph_names_size+ctx->bucket_names;
			int new_psize=new_count*sizeof(void *);
			int new_isize=new_count*sizeof(int);
			
			char **new_names=ctx->malloc(new_psize);
			char **new_values=ctx->malloc(new_psize);
			
			int *new_names_l=ctx->malloc(new_isize);
			int *new_values_l=ctx->malloc(new_isize);
			
			if (!new_names || !new_values || !new_names_l || !new_values_l) {
				return 0; //can't malloc!
			}
			
			memset (new_names,0,new_psize);
			memset (new_values,0,new_psize);
			memset (new_names_l,0,new_isize);
			memset (new_values_l,0,new_isize);
			
			if (ctx->ph_names_size>0) {
				int old_psize=ctx->ph_names_size*sizeof(void *);
				int old_isize=ctx->ph_names_size*sizeof(int);
				memcpy (new_names,ctx->ph_names,old_psize);
				memcpy (new_values,ctx->ph_values,old_psize);
				memcpy (new_names_l,ctx->ph_names_l,old_isize);
				memcpy (new_values_l,ctx->ph_values_l,old_isize);
				ctx->free(ctx->ph_names);
				ctx->free(ctx->ph_values);
				ctx->free(ctx->ph_names_l);
				ctx->free(ctx->ph_values_l);
			}
			ctx->ph_names=new_names;
			ctx->ph_values=new_values;
			ctx->ph_names_l=new_names_l;
			ctx->ph_values_l=new_values_l;
			
			idx=ctx->ph_names_size;
			
			ctx->ph_names_size=new_count;
		} else {
			idx=ctx->ph_names_count;
		}
		ctx->ph_names_count++;
		//copy ph_name
		ctx->ph_names[idx]=ctx->malloc(ph_nlen+1);
		if (!ctx->ph_names[idx]) return 0;
		memcpy (ctx->ph_names[idx],ph_name,ph_nlen);
		ctx->ph_names_l[idx]=ph_nlen;
		ctx->ph_names[idx][ph_nlen]=0; //keep asciiZ
		if (ph_nlen>ctx->max_name_l) ctx->max_name_l=ph_nlen;
	}
	//copy ph_value
	if (ph_value) { //got data to copy
		if (ph_vlen<0) ph_vlen=strlen (ph_value);
		if (ctx->ph_values[idx]==NULL || ph_vlen>ctx->ph_values_l[idx]){ //we need to (re)allocate..
			ctx->ph_values_l[idx]=ph_vlen;
			if (ctx->ph_values[idx]) ctx->free (ctx->ph_values[idx]);
			ctx->ph_values[idx]=ctx->malloc (ph_vlen+1);
			if (!ctx->ph_values[idx]) return 0; //can't malloc
			if (ph_vlen>ctx->max_value_l) ctx->max_value_l=ph_vlen;
		}
		memcpy (ctx->ph_values[idx],ph_value,ph_vlen);
		ctx->ph_values[idx][ph_vlen]=0;//keep asciiZ
	} else { //no data to copy just free
		if (ctx->ph_values[idx]) ctx->free(ctx->ph_values[idx]);
		ctx->ph_values[idx]=NULL;
		ctx->ph_values_l[idx]=0;
	}
	return 1; /// @return 1 on all OK
}

///@warning no memory management is done
int str_replace_ph_set_ph_uv (
	phctx_t *ctx,
	phctx_user_values_t *uv,
	char *ph_name,
	int ph_nlen/*=-1*/,
	char *ph_value/*=NULL*/,
	int ph_vlen/*=-1*/,
	char **oldvalue/*=NULL*/
){
	if (!ctx || !ph_name || !uv) return 0; ///@return -1 on failure
	if (ph_nlen<0) ph_nlen=strlen (ph_name);
	
	int x=0,idx=-1;
	for (;x<ctx->ph_names_count;x++) {
		if (
			ctx->ph_names[x]
			&& ph_nlen==ctx->ph_names_l[x] 
			&& memcmp(ctx->ph_names[x],ph_name,ph_nlen)==0
		) {
			idx=x;
			break;
		}
	}
	
	if (idx==-1) { //new placeholder!
		return 0; ///@return -1 on failure 
	}
	if (oldvalue) *oldvalue=uv->ph_values[idx];

	if (ph_value) { //got data to copy
		uv->ph_values[idx]=ph_value;
		uv->ph_values_l[idx]=ph_vlen;
		if (ph_vlen>uv->max_value_l) uv->max_value_l=ph_vlen;
	} else { //no data to copy mark free
		uv->ph_values[idx]=NULL;
		uv->ph_values_l[idx]=0;
	}
	return idx; /// @return idx of set value on all OK
}

int str_replace_ph_remove_ph (
	phctx_t *ctx,
	char *ph_name,
	int ph_nlen/*=-1*/
){
	if (!ctx || ctx->ph_names_count==0) return 0; ///@return 0 on failure
	
	if (ph_nlen<0) ph_nlen=strlen (ph_name);
	
	int x=0,idx=-1;
	for (;x<ctx->ph_names_count;x++) {
		if (
			ctx->ph_names[x]
			&& ph_nlen==ctx->ph_names_l[x] 
			&& memcmp(ctx->ph_names[x],ph_name,ph_nlen)==0
		) {
			idx=x;
			break;
		}
	}
	if (idx==-1) return 0;
	ctx->free(ctx->ph_names[idx]);
	if (ctx->ph_values[idx]) ctx->free(ctx->ph_values[idx]);
	ctx->ph_names[idx]=NULL;
	ctx->ph_values[idx]=NULL;
	
	ctx->ph_values_l[idx]=0;
	ctx->ph_names_l[idx]=0;
	
	ctx->prepared=0;
	
	ctx->ph_names_count--;
	if (ctx->ph_names_count>0 && idx<ctx->ph_names_count) { //fill the gap
		ctx->ph_names[idx]=ctx->ph_names[ctx->ph_names_count];
		ctx->ph_values_l[idx]=ctx->ph_names_l[ctx->ph_names_count];
		ctx->ph_values[idx]=ctx->ph_values[ctx->ph_names_count];
		ctx->ph_names_l[idx]=ctx->ph_values_l[ctx->ph_names_count];
	}
	return 1; /// @return 1 on all OK
}


int str_replace_ph_prepare (phctx_t *ctx){
	if (!ctx|| !ctx->pattern) return 0; ///@return 0 on failure
	if (ctx->ph_names_count==0){ //nothing to prepare
		ctx->prepared=1;
		return 1;
	}
	ctx->ph_order_count=0;
	ctx->prepared=0;
	
	char *pattern=ctx->pattern;
	int pattern_len=ctx->pattern_len;
	int ci=0;
	char delimiter=ctx->delimiter;
	char cc;
	int in_ph=0;
	int max_name_l=ctx->max_name_l;
	int cph_sz=0;
	int cph_ofs=0;
	int idx,x;
	int ph_names_count=ctx->ph_names_count;
	while (ci<pattern_len){
		cc=pattern[ci];
		if (cc==delimiter){
			if (in_ph){ //detected ph end
				in_ph=0;
				cph_sz++;
				idx=-1;
				for (x=0;x<ph_names_count;x++) if (cph_sz==ctx->ph_names_l[x] && memcmp(&pattern[cph_ofs],ctx->ph_names[x],cph_sz)==0){
					idx=x;
					break;
				}
				if (idx!=-1) { //got valid ph - register it to ph_order/ph_offset
					if (ctx->ph_order_count>=ctx->ph_order_size){ //no space left GROW
						int new_count=ctx->ph_order_size+ctx->bucket_order;
						int new_isize=new_count*sizeof(int);
												
						int *new_order=ctx->malloc(new_isize);
						int *new_offset=ctx->malloc(new_isize);
						
						if (!new_order || !new_offset ) {
							return 0; //can't malloc!
						}
						
						if (ctx->ph_order_count>0) {
							int old_isize=ctx->ph_order_size*sizeof(int);
							memcpy (new_order,ctx->ph_order,old_isize);
							memcpy (new_offset,ctx->ph_offset,old_isize);
							ctx->free(ctx->ph_order);
							ctx->free(ctx->ph_offset);
						}
						ctx->ph_order=new_order;
						ctx->ph_offset=new_offset;
						ctx->ph_order_size=new_count;
					}
					ctx->ph_order[ctx->ph_order_count]=idx;
					ctx->ph_offset[ctx->ph_order_count]=cph_ofs;
					ctx->ph_order_count++;
				} else { //invalid ph try starting a newone from this delimiter
					cph_ofs=ci;
					in_ph=1;
					cph_sz=0;//will become 1 down there..
				}
			} else { //detected ph start
				cph_ofs=ci;
				in_ph=1;
				cph_sz=0;//will become 1 down there..
			}
		}
		if (in_ph) {
			cph_sz++;
			if (cph_sz>=max_name_l){ //abort placeholder - current ph is longer than any declared ph
				in_ph=0;
			}
		}
		ci++;
	}
	ctx->prepared=1;
	return 1; /// @return 1 on all OK
}

int str_replace_ph_subst_maxsize_uv (phctx_t *ctx,phctx_user_values_t *uv) {
	if (!ctx || !uv || !ctx->pattern) return 0; ///@return 0 on invalid ctx or aproximate size guaranteed to fit susbstituted data with current placeholders
	int res=1;
	if (!ctx->prepared) res=str_replace_ph_prepare (ctx);
	if (res==0 || !ctx->prepared) return 0;
	return ctx->pattern_len+(ctx->ph_order_count*uv->max_value_l)+1; //this is wide enough even if all replacements are done with longest value
}

int str_replace_ph_subst_maxsize (phctx_t *ctx) {
	if (!ctx || !ctx->pattern) return 0; ///@return 0 on invalid ctx or aproximate size guaranteed to fit susbstituted data with current placeholders
	int res=1;
	if (!ctx->prepared) res=str_replace_ph_prepare (ctx);
	if (res==0 || !ctx->prepared) return 0;
	return ctx->pattern_len+(ctx->ph_order_count*ctx->max_value_l)+1; //this is wide enough even if all replacements are done with longest value
}

///@warning this uses a single memory block for the arrays and the struct itself
phctx_user_values_t * str_replace_ph_init_uv (phctx_t *ctx) {
	if (!ctx || ctx->ph_names_count==0) return NULL;
	int bsize=sizeof(phctx_user_values_t)+ctx->ph_names_count*(sizeof(void *)+sizeof(int));
	phctx_user_values_t *res=ctx->malloc(bsize);
	if (!res) return NULL;
	memset (res,0,bsize);
	char *p=(char *)res;
	p+=sizeof(phctx_user_values_t);
	res->ph_values=(char **)p;
	p+=ctx->ph_names_count*sizeof(void *);
	res->ph_values_l=(int *)p;
	return res;
}

///free phctx_user_values
void str_replace_ph_free_uv (phctx_t *ctx, phctx_user_values_t *uv) {
	if (ctx && uv) ctx->free (uv);
}


/// @warning this temporeraly modifies ctx and calls str_replace_ph_subst making it thread safe is up to you!
int str_replace_ph_subst_user (
	phctx_t *ctx,
	char **user_ph_values, ///values to use
	int *user_ph_values_l, ///values lengths to use
	char *dst, ///where to store result plus a terminateig 0
	int dstmaxlen ///space availible in dst
) {
	if (!ctx || !ctx->pattern || !dst || dstmaxlen==0) return 0; ///@return 0 failure
	char ** old_v=ctx->ph_values;
	int *old_vl=ctx->ph_values_l;
	
	ctx->ph_values=user_ph_values;
	ctx->ph_values_l=user_ph_values_l;
	
	int res=str_replace_ph_subst (ctx,dst,dstmaxlen);
	
	ctx->ph_values=old_v;
	ctx->ph_values_l=old_vl;
	
	return res; ///@return result of str_replace_ph_subst call
}

int str_replace_ph_subst_uv (
	phctx_t *ctx,
	phctx_user_values_t *uv, ///values lengths to use
	char *dst, ///where to store result plus a terminateig 0
	int dstmaxlen ///space availible in dst
) {
	return str_replace_ph_subst_user (ctx,uv->ph_values,uv->ph_values_l,dst,dstmaxlen);
}


/// @warning this uses memcpy so pattern and destination should not ovrlap
int str_replace_ph_subst (
	phctx_t *ctx,
	char *dst,
	int dstmaxlen
){
	if (!ctx || !ctx->pattern || !dst || dstmaxlen==0) return 0; ///@return 0 failure
	int res=1;
	if (!ctx->prepared) res=str_replace_ph_prepare (ctx);
	if (res==0 || !ctx->prepared) return 0;
	if (ctx->ph_order_count==0) { //no placeholders
		if (dstmaxlen>ctx->pattern_len) { //we need +1 for asciiZ
			memmove (dst,ctx->pattern,ctx->pattern_len+1); //be safe use memmove
			return ctx->pattern_len;
		} else return -1; ///@return -1 on unsufficient dstmaxlen
	}
	
	char *dsti=dst;
	char *pattern=ctx->pattern;
	int pattern_len=ctx->pattern_len;
	int pidx=0;
	int szout=0;
	int oidx=0;
	int next_ofs=ctx->ph_offset[oidx];
	int next_ph=ctx->ph_order[oidx];
	int cpyl;
	while (pidx<pattern_len && dstmaxlen>0) {
		if (pidx==next_ofs){ //ph copy
			cpyl=ctx->ph_values_l[next_ph];
			if (dstmaxlen<=cpyl) return -1;
			memcpy (dsti,ctx->ph_values[next_ph],cpyl);
			dsti+=cpyl; pidx+=ctx->ph_names_l[next_ph]; szout+=cpyl; dstmaxlen-=cpyl;
			oidx++;
			if (oidx>=ctx->ph_order_count) { //this was the last placeholder
				next_ofs=pattern_len;
			} else {
				next_ofs=ctx->ph_offset[oidx];
				next_ph=ctx->ph_order[oidx];
			}
		} else { //pattern copy
			cpyl=next_ofs-pidx;
			if (dstmaxlen<=cpyl) return -1;
			memcpy (dsti,pattern+pidx,cpyl);
			dsti+=cpyl; pidx+=cpyl; szout+=cpyl; dstmaxlen-=cpyl;
		}
	}
	dst[szout]=0;
	return szout;
}


///////////////////////////////////////////////////////////////////////////////
////////////////////// Simple Search-and-replace
/////////////////////////////////////////////////////////////////////////////

int str_replace_single (const char *haystack,const char *needle,const char *changeto, char* dst , int dstmaxlen, int hslen, int nlen, int chtolen) {
	if (hslen==-1) hslen=strlen (haystack);
	if (nlen==-1) nlen=strlen (needle);
	if (chtolen==-1) chtolen=strlen (changeto);
	char *brk=strstr(haystack,needle);
	if (brk==NULL) {
		if (dstmaxlen-1<hslen) return -1; //not enough space in dst
		memcpy(dst,haystack,hslen+1);//+1 so we copy \0 at the end
		return 0;
	} else {
		if (dstmaxlen-1<hslen-nlen+chtolen) return -1; //not enough space in dst
		memcpy(dst,haystack,brk-haystack);//copy starting string
		char *addat=dst+(brk-haystack);
		memcpy(addat,changeto,chtolen);//add changeto
		addat+=chtolen;
		char *left=brk+nlen;
		memcpy(addat,left,hslen-(brk-haystack)-nlen+1);//add left and final \0
		return 1;
	}
}

int str_replace_multiplei (const char *haystack,const char *needle,int changetoi, char* dst , int dstmaxlen, int hslen, int nlen) {
	char changeto[51];
	snprintf(changeto,50,"%d",changetoi);
	return str_replace_multiple (haystack,needle,changeto,dst,dstmaxlen,hslen,nlen,-1);
}

int str_replace_multiple (const char *haystack,const char *needle,const char *changeto, char* dst , int dstmaxlen, int hslen, int nlen, int chtolen) {
	if (nlen==-1) nlen=strlen (needle);
	if (chtolen==-1) chtolen=strlen (changeto);
	char *hbuf=NULL;
	const char *csrc=haystack;
	char *cdst=dst;
	int res=0;
	
	while (str_replace_single(csrc,needle, changeto,cdst , dstmaxlen, -1, nlen, chtolen)==1) { // we got a replace:
		res++;
		csrc=cdst;
		if (cdst==dst) {
			if (hbuf==NULL) {
				hbuf=malloc (dstmaxlen);
				if (hbuf==NULL) {
					res=-1;
					break;
				}
			}
			cdst=hbuf;
		} else cdst=dst;
	}
	if (cdst==hbuf) strcpy (dst,hbuf);
	if (hbuf!=NULL) free (hbuf);
	return res;
}
