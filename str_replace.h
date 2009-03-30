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

#ifndef str_replace_h_SDR_321234msdaa_included
#define str_replace_h_SDR_321234msdaa_included


 /** \file
      \brief String replacement functions.
      \author Stoian Ivanov sdr@tera-com.com
      \example str_replace.example.c
  */


#include <stdlib.h>

#ifdef  __cplusplus
extern "C" {
#endif

///malloc() proto as type
typedef void * (malloc_ft) (size_t SIZE); 

///free() proto as type
typedef void (free_ft) (void *PTR); 

///pattern replace context
typedef struct {
	malloc_ft* malloc; ///<use this malloc implementation
	free_ft* free; ///<use this free implementation
	int prepared; 
	int pattern_free;
	int pattern_len;
	char *pattern;
	int ph_names_count; ///<current element count in ph_names,ph_values,ph_names_l
	int ph_names_size; ///<currrent size of ph_names,ph_values,ph_names_l
	char **ph_names;
	char **ph_values;
	int *ph_names_l; ///<names lengths
	int *ph_values_l; ///<values lengths
	
	int max_name_l;
	int max_value_l;

	int ph_order_count; ///<current element count in ph_order, ph_offset
	int ph_order_size; ///<current size of ph_order, ph_offset
	int *ph_order;
	int *ph_offset;
	
	int bucket_names; ///<incremetntal step for ph_names_size
	int bucket_order; ///<incremetntal step for ph_order_size
	char delimiter; ///<placeholder delimiter
} phctx_t;

///user values holder 
typedef struct {
	char **ph_values;
	int *ph_values_l;
	int max_value_l;
} phctx_user_values_t;

///initialize a replace_ph context
phctx_t * str_replace_ph_init ( 
	int bucket_names, ///<expected placeholders count
	int bucket_order, ///<expected placeholders occurrences in pattern
	char delimiter, ///<place holder delimiter
	malloc_ft*use_malloc/*=NULL*/, ///<use this malloc implementation NULL for default
	free_ft* use_free/*=NULL*/ ///<use this free implementation NULL for default
);


///initialize a phctx_user_values_t from a ctx having space just for the allready set placeholders
phctx_user_values_t * str_replace_ph_init_uv (
	phctx_t *ctx
);

///clear placeholders data
void str_replace_ph_reset_ph( phctx_t *ctx );

///free replace_ph context
void str_replace_ph_free ( phctx_t *ctx);

///free phctx_user_values
void str_replace_ph_free_uv (phctx_t *ctx, phctx_user_values_t *uv);

///add/set a place holder and a value for it
int str_replace_ph_set_ph (
	phctx_t *ctx, ///<context
	char *ph_name, ///<placeholder name
	int ph_nlen/*=-1*/, ///<place holder name length if known else -1 
	char *ph_value/*=NULL*/, ///<place hoder value if known
	int ph_vlen/*=-1*/  ///<place holder value length if known else -1
);

///set (NOT ADD) a place holder user value 
int str_replace_ph_set_ph_uv (
	phctx_t *ctx, ///<context
	phctx_user_values_t *uv,
	char *ph_name, ///<placeholder name
	int ph_nlen/*=-1*/, ///< place holder name length if known else -1 
	char *ph_value/*=NULL*/, ///< place hoder value if known
	int ph_vlen/*=-1*/,  ///< place holder value length if known else -1
	char **oldvalue/*=NULL*/ ///< write return old value here if not NULL
);

///removed a place holder and a value for it
int str_replace_ph_remove_ph (
	phctx_t *ctx, ///< context
	char *ph_name, ///< placeholder name
	int ph_nlen/*=-1*/  ///< place holder name length if known else -1 
);

///add a pattern to context
int  str_replace_ph_config_pattern(
	phctx_t *ctx, ///< context to change
	char *pattern, ///< pattern to add asciiZ
	int copy, ///< copy(1) data or just use(0) the pointer 
	int plen/*=-1*/ ///< data len if known -1 to autofindout
);

///prepare context for susbstitutions. e.g. find placeholders in pattern
int str_replace_ph_prepare (phctx_t *ctx);

///fid maximal destionation size with given pattern and placeholders. This will automaticaly call str_replace_ph_prepare() if needed
int str_replace_ph_subst_maxsize (phctx_t *ctx);

///fid maximal destionation size with given pattern, user values and placeholders. This will automaticaly call str_replace_ph_prepare() if needed
int str_replace_ph_subst_maxsize_uv (phctx_t *ctx,phctx_user_values_t *uv);

///make the substitutions in current pattern with current placeholders. This will automaticaly call str_replace_ph_prepare() if needed
int str_replace_ph_subst (
	phctx_t *ctx,
	char *dst, ///< where to store result plus a terminateig 0
	int dstmaxlen ///< space availible in dst
);

///make the substitutions in current pattern with current placeholders. This will automaticaly call str_replace_ph_prepare() if needed
///this will also use values from provided arrays
int str_replace_ph_subst_user (
	phctx_t *ctx,
	char **user_ph_values, ///< values to use
	int *user_ph_values_l, ///< values lengths to use
	char *dst, ///< where to store result plus a terminateig 0
	int dstmaxlen ///< space availible in dst
);

///directly uses str_replace_ph_subst_user
int str_replace_ph_subst_uv (
	phctx_t *ctx,
	phctx_user_values_t *uv, ///< values lengths to use
	char *dst, ///< where to store result plus a terminateig 0
	int dstmaxlen ///< space availible in dst
);

///simple search-and-replace replace first found
int str_replace_single (const char *haystack,const char *needle,const char *changeto, char* dst , int dstmaxlen, int hslen, int nlen, int chtolen);
///simple search-and-replace replace all found - based on str_replace_single()
int str_replace_multiple (const char *haystack,const char *needle,const char *changeto, char* dst , int dstmaxlen, int hslen, int nlen, int chtolen);

#ifdef  __cplusplus
}
#endif

#endif
