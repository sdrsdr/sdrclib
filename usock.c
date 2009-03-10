
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "usock.h"

int make_named_socket_connect (const char *filename) {
	struct sockaddr_un name;
	int sock;
	size_t size;


	/* Create the socket. */
	sock = socket (PF_LOCAL, SOCK_STREAM, 0);
	if (sock < 0){
		perror ("socket");
		exit (EXIT_FAILURE);
	}


	/* Bind a name to the socket. */
	name.sun_family = AF_LOCAL;
	strncpy (name.sun_path, filename, sizeof (name.sun_path));
	name.sun_path[sizeof (name.sun_path) - 1] = '\0';


	/* The size of the address is
		the offset of the start of the filename,
		plus its length,
		plus one for the terminating null byte.
		Alternatively you can just do:
		size = SUN_LEN (&name);
	*/
	size = SUN_LEN (&name); //(offsetof (struct sockaddr_un, sun_path)	+ strlen (name.sun_path) + 1);


	if (connect (sock, (struct sockaddr *) &name, size) < 0) {
		perror ("connect");
		return -1;
	}


	return sock;
}
int make_named_socket_listen (const char *filename) {
	struct sockaddr_un name;
	int sock;
	size_t size;


	/* Create the socket. */
	sock = socket (PF_LOCAL, SOCK_STREAM, 0);
	if (sock < 0){
		perror ("socket");
		exit (EXIT_FAILURE);
	}


	/* Bind a name to the socket. */
	name.sun_family = AF_LOCAL;
	strncpy (name.sun_path, filename, sizeof (name.sun_path));
	name.sun_path[sizeof (name.sun_path) - 1] = '\0';


	/* The size of the address is
		the offset of the start of the filename,
		plus its length,
		plus one for the terminating null byte.
		Alternatively you can just do:
		size = SUN_LEN (&name);
	*/
	size = SUN_LEN (&name); //(offsetof (struct sockaddr_un, sun_path)	+ strlen (name.sun_path) + 1);


	if (bind (sock, (struct sockaddr *) &name, size) < 0) {
		perror ("bind");
		return -1;
	}

	if (listen (sock,5)<0) {
		perror ("listen");
		return -2;
	}
	return sock;
}
