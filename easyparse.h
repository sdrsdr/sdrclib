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
      \brief Easy parser declarations
	  \author Stoian Ivanov sdr@tera-com.com
  */

#ifndef easyparse_h_SDR_3d34mfsdaa_included
#define easyparse_h_SDR_3d34mfsdaa_included

#ifdef  __cplusplus
extern "C" {
#endif
///easy parser callback retun  non 0 to stop parsing with error
typedef int (easyparse_cb_t) (void*userdata,  char *name, int namel, char *value, int valuel);

/**the easy parser 
	line based white space trimmed where apropriate
	accepts grama:
\n
name=value\n
name\<terminator\n
data\n
data\n
terminator\n

*/
int easyparse(
	char *buf, ///<data to parse
	int bufl, ///<length to parse (-1 will use strlen to findout)
	easyparse_cb_t *callback, ///<callback to invoke 
	void*userdata ///<aux data for callback
	);

/**the dumb parser 
	white space delimited name=value pairs. No white space allowed around =
*/
int dumbparse(
	char *buf,///< data to parse
	int bufl,///< length to parse (-1 will use strlen to findout)
	easyparse_cb_t *callback,///<callback to invoke 
	void*userdata ///<aux data for callback
);
#ifdef  __cplusplus
}
#endif

#endif
