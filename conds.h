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

/*
=======================
===== this file require librt (-lrt) 
=======================
*/



#ifndef conds_h_SDR_321234msdaa_included
#define conds_h_SDR_321234msdaa_included

#include <pthread.h>


typedef struct  {
	 pthread_mutex_t m;
	 pthread_cond_t c;
	 int events;
} cond_t;

void cond_init(cond_t* c);

void cond_deinit(cond_t* c);


#define waitcond_err 0
#define waitcond_condok 1
#define waitcond_condtm -1
int waitcond (cond_t* c, int tm_ms);

int signalcond (cond_t* c);

#endif
