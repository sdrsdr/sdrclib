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

int str_replace_single (const char *haystack,const char *needle,const char *changeto, char* dst , int dstmaxlen, int hslen, int nlen, int chtolen);
int str_replace_multiple (const char *haystack,const char *needle,const char *changeto, char* dst , int dstmaxlen, int hslen, int nlen, int chtolen);

#endif
