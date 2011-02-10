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


#ifndef bindaddr_h_SDR_3gg234msd2aa_included
#define bindaddr_h_SDR_3gg234msd2aa_included

#include <netinet/in.h>
#include <arpa/inet.h>

#define udpsocket()  socket(AF_INET, SOCK_DGRAM, 0)
#define tcpsocket()  socket(AF_INET, SOCK_STREAM, 0)

#define bindsok(sok,addr) bind(sok,(struct sockaddr *)addr,sizeof(struct sockaddr_in))

int bindaddr (struct sockaddr_in *sadr,const char *host, int port);
int canreuseaddr (int sock);

#endif
