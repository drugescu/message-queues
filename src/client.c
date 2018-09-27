/*
 * main.c - Practice 2
 *
 * 2018 drugescu <drugescu@drugescu-VirtualBox>
 *
 */

#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <assert.h>

#include <fcntl.h>
#include <mqueue.h>

#include "utils.h"

// 8192B
#define BUF_SIZE 	        (1 << 13)
#define MAX_CLIENTS         12
#define SERVER_QUEUE_NAME  "/mqueue"

char buf[BUF_SIZE];

// Server queue name is known to clients
// Clients queue name is of the form /mqueue{PID}

int main(int argc, char **argv)
{
    int rc;
    unsigned int prio = 10;
    mqd_t server_mqueue;
    mqd_t client_mqueue[ MAX_CLIENTS ];
    
    printf("Main pid is %d\n", getpid());

    // Start message queue
    mqueue = mq_open(SERVER_QUEUE_NAME, (argc > 1 ? O_CREAT : 0) | O_RDWR, 0666, NULL);
    DIE(mqueue == (mqd_t) - 1, "mq_open");

    // Server or client depending on arguments to process
    if (argc > 1) {
        // Server doing its stuff
        printf("Server mode on, argc > 1, argument is: %s\n", argv[1]);
        
        //rc = mq_send(mqueue, argv[1], strlen(argv[1]), prio);
        //DIE(rc == -1, "mq_send");

        rc = mq_close(mqueue);
        DIE(rc == -1, "mq_close");
    }
    else {
        // Client doing its stuff
        printf("Client mode on, argc = 1, waiting for message...\n");

        rc = mq_receive(mqueue, buf, BUF_SIZE, &prio);
		DIE(rc == -1, "mq_receive");
 
		printf("received: %s\n", buf);
 
		rc = mq_close(mqueue);
		DIE(rc == -1, "mq_close");
 
		rc = mq_unlink(QUEUE_NAME);
		DIE(rc == -1, "mq_unlink");
    }

    /* create */
    // mqd_t mq_open(const char *name, int oflag, mode_t mode, struct mq_attr *attr);
    /* create */
    // mqd_t mq_open(const char *name, int oflag, mode_t mode, struct mq_attr *attr);
    /* send */
    // int mq_send(mqd_t mqdes, const char *buffer, size_t length, unsigned priority);
    /* recv */
    // ssize_t mq_receive(mqd_t mqdes, char *buffer, size_t length, unsigned *priority);
    /* close */
    // int mq_close(mqd_t mqdes);
    /* unlink */
    // int mq_unlink(const char *name);
    

    return 0;
}
