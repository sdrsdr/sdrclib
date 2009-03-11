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
 /** \file str_replace.c
      \brief String replacement functions (implementation).
	  \author Stoian Ivanov sdr@tera-com.com
  */


#include <string.h>

#include "str_replace.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

phctx_t * str_replace_ph_init (int bucket_size,int bucket_size_order, char delimiter, malloc_ft*use_malloc/*=NULL*/,free_ft* use_free/*=NULL*/) {
	if (use_malloc==NULL) use_malloc=malloc;
	if (use_free==NULL) use_free=free;
	
	phctx_t * ctx=use_malloc(sizeof(phctx_t));
	if (!ctx) return ctx; /// @return NULL on failure
	
	memset (ctx,0,sizeof(phctx_t));
	ctx->bucket_size_order=bucket_size_order;
	ctx->bucket_size=bucket_size;
	ctx->delimiter=delimiter;
	ctx->malloc=use_malloc;
	ctx->free=use_free;
	if (ctx->bucket_size<0) ctx->bucket_size_order=10;
	if (ctx->bucket_size_order<0) ctx->bucket_size_order=30;
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
		str_replace_ph_clean_charpp (fr,ctx->ph_names,ctx->ph_count);
		fr(ctx->ph_names);
	}
	if (ctx->ph_values) {
		str_replace_ph_clean_charpp (fr,ctx->ph_values,ctx->ph_count);
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
	ctx->ph_count=0;
	ctx->ph_order_count=0;
	if (ctx->ph_names) {
		str_replace_ph_clean_charpp (ctx->free,ctx->ph_names,ctx->ph_count);
	}
	if (ctx->ph_values) {
		str_replace_ph_clean_charpp (ctx->free,ctx->ph_values,ctx->ph_count);
	}

}

int  str_replace_ph_config_pattern(phctx_t *ctx,char *pattern,int copy,int plen/*=-1*/){
	if (!ctx) return 0; /// @return 0 on failure
	if (plen<0) plen=strlen(pattern);
	ctx->prepared=0;
	if (ctx->pattern && ctx->pattern_free) ctx->free(ctx->pattern);
	ctx->pattern_len=plen;
	if (copy) {
		ctx->pattern_free=1;
		ctx->pattern=ctx->malloc(plen+1); //keep it asciiZ
		if (ctx->pattern) { //got memory
			memcpy (ctx->pattern,pattern,plen+1); //keep it asciiZ
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
	
	int x=0,idx=-1,first_free=-1;
	for (;x<ctx->ph_count;x++) {
		if (first_free==-1 && ctx->ph_names[x]==NULL) first_free=x;
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
		if (first_free==-1){ //no space left GROW!
			int new_count=ctx->ph_bucket_count+ctx->bucket_size;
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
			
			if (ctx->ph_bucket_count>0) {
				int old_psize=ctx->ph_bucket_count*sizeof(void *);
				int old_isize=ctx->ph_bucket_count*sizeof(int);
				memcpy (new_names,ctx->ph_names,old_psize);
				memcpy (new_values,ctx->ph_values,old_psize);
				memcpy (new_names_l,ctx->ph_names_l,old_isize);
				memcpy (new_values_l,ctx->ph_values_l,old_isize);
			}
			if (ctx->ph_names) ctx->free(ctx->ph_names);
			if (ctx->ph_values) ctx->free(ctx->ph_values);
			if (ctx->ph_names_l) ctx->free(ctx->ph_names_l);
			if (ctx->ph_values_l) ctx->free(ctx->ph_values_l);
			ctx->ph_names=new_names;
			ctx->ph_values=new_values;
			ctx->ph_names_l=new_names_l;
			ctx->ph_values_l=new_values_l;
			
			idx=ctx->ph_bucket_count;
			
			ctx->ph_bucket_count=new_count;
		} else idx=first_free;
		//copy ph_name
		ctx->ph_values[idx]=ctx->malloc(ph_nlen+1);
		if (!ctx->ph_values[idx]) return 0;
		memcpy (ctx->ph_values[idx],ph_name,ph_nlen+1);//copy asciiz
		ctx->ph_values_l[idx]=ph_nlen;
		if (ph_nlen>ctx->max_name_l) ctx->max_name_l=ph_nlen;
	}
	//copy ph_value
	if (ph_value) { //got data to copy
		if (ph_vlen<0) ph_vlen=strlen (ph_value);
		if (ph_vlen>ctx->ph_values_l[idx]){ //we need to reallocate..
			if (ctx->ph_values[idx]) ctx->free (ctx->ph_values[idx]);
			ctx->ph_values[idx]=ctx->malloc (ph_vlen+1);
			if (ph_vlen>ctx->max_value_l) ctx->max_value_l=ph_vlen;
		}
		memcpy (ctx->ph_values[idx],ph_value,ph_vlen+1);
		ctx->ph_values_l[idx]=ph_vlen;
	} else { //no data to copy just free
		if (ctx->ph_values[idx]) ctx->free(ctx->ph_values[idx]);
		ctx->ph_values[idx]=NULL;
		ctx->ph_values_l[idx]=0;
	}
	return 1; /// @return 1 on all OK
}

int str_replace_ph_remove_ph (
	phctx_t *ctx,
	char *ph_name,
	int ph_nlen/*=-1*/
){
	if (!ctx || ctx->ph_count==0) return 0; ///@return 0 on failure
	
	if (ph_nlen<0) ph_nlen=strlen (ph_name);
	
	int x=0,idx=-1;
	for (;x<ctx->ph_count;x++) {
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
	ctx->ph_count--;
	return 1; /// @return 1 on all OK
}


int str_replace_ph_prepare (phctx_t *ctx){
	if (!ctx) return 0; ///@return 0 on failure
	
	return 1; /// @return 1 on all OK
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
