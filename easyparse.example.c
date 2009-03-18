/***************************************************************************
 *   Copyright (C) 2007 by Stoian Ivanov                                   *
 *   sdr@tera-com.com                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU  General Public License version 2       *
 *   as published by the Free Software Foundation.                         *
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
      \brief Easy parser example
	  \author Stoian Ivanov sdr@tera-com.com
  */


#define TESTCONFG1 "\
test=mest\n\
\n\
opa<tropa\n\
multi\n\
line\n\
tropa\n\
#comment\n\
more=values"


#define TESTCONFG  "\
 test   =  mest  \n\
  #komentirame\n\
 more = values  \n"


#include <stdio.h>

#include "easyparse.h"

int easyparse_cb (void*userdata,  char *name, int namel, char *value, int valuel){
	printf ("name:[%.*s] value:[%.*s]\n",namel,name,valuel,value);
}

int main () {
	printf ("-----parsing-------\n%s\n-------------------------\n",TESTCONFG);
	easyparse (TESTCONFG,-1,easyparse_cb,NULL);
	return 0;
}
