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
      \brief String replacement functions (example)
	  \author Stoian Ivanov sdr@tera-com.com
  */


#include "str_replace.h"
#include <stdio.h>


void dump_ctx(phctx_t* ctx, char *msg){
	int x=0;
	if (msg) printf ("%s (ctx:%p)\n",msg,ctx);
	printf ("Placeholders:\n");
	for (x=0;x<ctx->ph_names_count; x++) {
		printf (" #%d n:%*s, v:%*s\n",x,ctx->ph_names_l[x],ctx->ph_names[x],ctx->ph_values_l[x],ctx->ph_values[x]);
	}

	printf ("  max_name_l:%d, max_value_l:%d\n",ctx->max_name_l,ctx->max_value_l);
	
	printf ("Pattern:\n");
	printf ("0         1         2         3         4         5         6         7         \n");
	printf ("01234567890123456789012345678901234567890123456789012345678901234567890123456789\n");
	printf ("%*s\n",ctx->pattern_len,ctx->pattern);
	for (x=0;x<ctx->ph_order_count; x++) {
		printf (" p#:%d o:%d\n",ctx->ph_order[x],ctx->ph_offset[x]);
	}
	printf ("  allocation: n:%d ns:%d nb:%d o:%d os:%d ob:%d\n",
		ctx->ph_names_count,ctx->ph_names_size,ctx->bucket_names,
		ctx->ph_order_count,ctx->ph_order_size,ctx->bucket_order
	);
	
	printf ("=====================================================\n");
}



#define PATTERN "ttt$deco$this $is$ a $inv$test$ that $$is$ testing a pattern replacement$deco$ttt"
#define PATTERN2 "111$deco$2222"
#define PATTERN3 "$deco$2222"
#define PATTERN4 "111$deco$"
#define PATTERN5 "$deco$"
#define PATTERN6 "test"

int main () {
	phctx_t* ctx=str_replace_ph_init(3,5,'$',NULL,NULL);//3 place holders 5 occurances
	
	if (!ctx) {
		perror ("str_replace_ph_init");
		return 1;
	}
	
	str_replace_ph_config_pattern (ctx,PATTERN,0,-1);
	
	str_replace_ph_set_ph (ctx,"$deco$",-1,"@",1);
	str_replace_ph_set_ph (ctx,"$is$",4,"was",-1);
	str_replace_ph_set_ph (ctx,"$test$",-1,"substitution",-1);
	
	str_replace_ph_prepare (ctx);
	
	dump_ctx(ctx,"Post prepare");
	
	int sz;
	printf ("Maximal size for susbtituton: %d\n",sz=str_replace_ph_subst_maxsize (ctx));
	char *subst=malloc (sz);
	str_replace_ph_subst (ctx,subst,sz);
	printf ("substituted string: %s\n",subst);
	
	
	return 0;
}
