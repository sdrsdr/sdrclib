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
	  \author Graham Shaw via http://www.microhowto.info/howto/cause_a_process_to_become_a_daemon_in_c.html
  */

 
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#include "daemonize.h"

void daemonize(const char *newstdin,const char *newstdout,const char *newsterr, const char *pidfile) {
    // Fork, allowing the parent process to terminate.
    pid_t pid = fork();
    if (pid == -1) {
        fprintf(stderr,"failed to fork while daemonizing (errno=%d)",errno);   _exit(-1);
    } else if (pid != 0) {
        _exit(0);
    }

    // Start a new session for the daemon.
    if (setsid()==-1) {
        fprintf(stderr,"failed to become a session leader while daemonizing(errno=%d)",errno);  _exit(-1);
    }

    // Fork again, allowing the parent process to terminate.
    signal(SIGHUP,SIG_IGN);
    pid=fork();
    if (pid == -1) {
        fprintf(stderr,"failed to fork while daemonizing (errno=%d)",errno); _exit(-1);
    } else if (pid != 0) {
        _exit(0);
    }

    // Close then reopen standard file descriptors.
	if (newstdin) {
		int fd=open(newstdin,O_RDONLY);
		if ( fd== -1) {
			fprintf(stderr,"failed to open %s as stdin while daemonising (errno=%d)",newstdin,errno);_exit(-1);
		} else {
			dup2(fd,STDIN_FILENO);
			close(fd);
		}
	}
	if (newstdout){
		int fd=open(newstdout,O_WRONLY);
		if ( fd== -1) {
			fprintf(stderr,"failed to open %s as stdout while daemonising (errno=%d)",newstdout,errno);_exit(-1);
		} else{
			dup2(fd,STDOUT_FILENO);
			close(fd);
		}
	}
	if (newsterr){
		int fd=open(newsterr,O_RDWR);
		if (fd == -1) {
			fprintf(stderr,"failed to open %s as stderr while daemonising (errno=%d)",newsterr,errno);_exit(-1);
		} else {
			dup2(fd,STDERR_FILENO);
			close(fd);
		}
	}
	
	if (pidfile){
		int fd=open(pidfile,O_WRONLY|O_TRUNC|O_CREAT);
		if ( fd== -1) {
			fprintf(stderr,"failed to open %s as pidfile while daemonising (errno=%d)",pidfile,errno);_exit(-1);
		} else{
			char buf[20];
			int len=snprintf(buf,20,"%d",getpid());
			write(fd,buf,len);
			close(fd);
		}
	}
}