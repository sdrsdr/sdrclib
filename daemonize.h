/***************************************************************************
 *   Copyright (C) 2015 by Stoian Ivanov                                   *
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
      \brief Daemonize declarations
	  \author Stoian Ivanov sdr@mail.bg
  */

#ifndef daemonize_h_SDR_jla2s7m7sj_included
#define daemonize_h_SDR_jla2s7m7sj_included

#ifdef  __cplusplus
extern "C" {
#endif
	
void daemonize(const char *newstdin,const char *newstdout,const char *newsterr, const char *pidfile);

#ifdef  __cplusplus
}
#endif
#endif