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
 /** \file str_replace.example.c
      \brief String replacement functions (example)
	  \author Stoian Ivanov sdr@tera-com.com
  */



#include "str_replace.h"
#include <stdio.h>
#define PATTERN "$deco$this $is$ a $test$ that $is$ testing a pattern replacement$deco$"

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
	
	return 0;
}