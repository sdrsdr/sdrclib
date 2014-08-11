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

#include "bindaddr.h"

#include <netdb.h>
#include <string.h>

#include <sys/types.h>          
#include <sys/socket.h>

int bindaddr (struct sockaddr_in *sadr,const char *host, int port) {

	memset (sadr,0,sizeof(*sadr));
	
	sadr->sin_family = AF_INET;
	sadr->sin_port= htons(port);
	if (host!=NULL) {
		struct hostent *hent = gethostbyname (host);
		if (hent==NULL) return 0;
		char **ip= hent->h_addr_list;
		sadr->sin_addr = *((struct in_addr *)*ip);
	} else {
		sadr->sin_addr.s_addr=INADDR_ANY;
	}
	return 1;
}
int canreuseaddr (int sock) {
	int optval=1;
	return setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval));
}

int connsock(int sock, const char *host, int port){
	struct sockaddr_in connsock_addr;
	if (!bindaddr (&connsock_addr, host, port)) return 0;
	if (connect(sock,(struct sockaddr *)&connsock_addr,sizeof(connsock_addr))==0) return 1;
	else return 0;
}

