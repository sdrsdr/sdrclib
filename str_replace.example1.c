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
      \brief String replacement functions (example@str_replace_add_slashes)
	  \author Stoian Ivanov sdr@tera-com.com
  */


#include "str_replace.h"
#include <stdio.h>
#include <string.h>

#define SRC "this \"  is \\ a ' test ` sz:%d\n"

int main () {
	char buf [200];
	printf (SRC,strlen(SRC));
	int sz=str_replace_add_slashes (SRC,0,buf);
	printf (buf,sz);
	
}
