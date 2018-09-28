#ifndef MAIN_UTILS_H
#define MAIN_UTILS_H	1

#include <errno.h>

#define DIE(assertion, call_description)				\
	do {								\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",			\
					__FILE__, __LINE__);		\
			perror(call_description);			\
			exit(errno);					\
		}							\
	} while (0)
#endif

#define	HASHMAP_USE_SIMPLE_HASH

#define BUF_SIZE 	        	(1 << 13)
#define MAX_CLIENTS         	12
#define MAX_TOKENS          	255
#define SERVER_QUEUE_NAME   	"/mqueue"
#define REQUEST_TEXT        	"<REQUEST_TOKEN>"
#define EXISTS_MESSAGE			"<TOKEN_UNAVAILABLE>"
#define MAX_QUEUE_NAME_SIZE     25
#define MAX_TOKEN_MESSAGE_SIZE  100
#define CONNECTION_REFUSED		"-1"
#define CONNECTION_ACCEPTED		"-255"

#define KEY_EXISTS				-1
#define KEY_AVAILABLE			0

#define	TOKEN_DELETED			1023
#define	TOKEN_DELETION_ERROR	1024
