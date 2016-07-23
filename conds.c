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


#include "conds.h"

#include <errno.h>
#include <string.h>

#include <stdio.h>
#ifdef __MACH__
#include <mach/mach_time.h>
#define CLOCK_REALTIME 0
#define CLOCK_MONOTONIC 0
int clock_gettime(int clk_id, struct timespec *t){
    mach_timebase_info_data_t timebase;
    mach_timebase_info(&timebase);
    uint64_t time;
    time = mach_absolute_time();
    double nseconds = ((double)time * (double)timebase.numer)/((double)timebase.denom);
    double seconds = ((double)time * (double)timebase.numer)/((double)timebase.denom * 1e9);
    t->tv_sec = seconds;
    t->tv_nsec = nseconds;
    return 0;
}
#else
#include <time.h>
#endif

void cond_init(cond_t* c){
	memset (c,0,sizeof(*c));
	pthread_mutex_init (&c->m,NULL);
	pthread_cond_init (&c->c,NULL);
}

void cond_deinit(cond_t* c){
	pthread_mutex_destroy (&c->m);
	pthread_cond_destroy (&c->c);
}

int waitcond (cond_t* c, int tm_ms) {
	int r;
	pthread_mutex_lock (&c->m);
	if (c->events) {
		c->events--;
		//printf ("COND-LATEWAIT: events: %d\n",c->events);
		r=0;
	} else {
		if (tm_ms>0) {
			tm_ms=tm_ms*1000;
			struct timespec ts;
			clock_gettime(CLOCK_REALTIME, &ts);
			ts.tv_sec += tm_ms / 1000000;
			ts.tv_nsec += tm_ms % 1000000;
			r=pthread_cond_timedwait (&c->c,&c->m,&ts);
			if (r!=ETIMEDOUT) c->events--;
			//printf ("COND-TIMEWAIT: events: %d\n",c->events);
		} else {
			r=pthread_cond_wait (&c->c,&c->m);
			c->events--;
			//printf ("COND-WAIT: events: %d\n",c->events);
		}
	}
	pthread_mutex_unlock (&c->m);
	if (r==0) return waitcond_condok;
	if (r==ETIMEDOUT) return waitcond_condtm;
	return waitcond_err;
}

int signalcond (cond_t* c) {
	pthread_mutex_lock (&c->m);
	c->events++;
	//printf ("COND-SIGNAL: events: %d\n",c->events);
	int res=pthread_cond_signal (&c->c);
	pthread_mutex_unlock (&c->m);
	return res;
}
