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


#include <string.h>

#include "str_replace.h"
#include <stdlib.h>
#include <stdio.h>
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
