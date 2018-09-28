/*
 * client.c - Practice 2
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

#include <stdbool.h>

#include <time.h>

#include "utils.h"

char buf[BUF_SIZE] = { 0 };

int main(int argc, char **argv)
{
    int rc;
    unsigned int prio = 10;
    
    mqd_t server_mqueue;
    mqd_t client_mqueue;
    
    char client_mqueue_name[ MAX_QUEUE_NAME_SIZE ] = { 0 };
    char token_message[ MAX_TOKEN_MESSAGE_SIZE ] = { 0 };
    int req_token;

    // Set current client mqueue name
    sprintf(client_mqueue_name, "/mqueue%d", getpid());

    // From REQUEST TOKEN message
    sprintf(token_message, "%s ", REQUEST_TEXT);
    
    printf("Main pid is %d\n", getpid());
    srand(time(NULL));

    // Open server message queue
    server_mqueue = mq_open(SERVER_QUEUE_NAME, O_RDWR, 0666, NULL);
    DIE(server_mqueue == (mqd_t) - 1, "mq_open");

    printf("Client on, sending queue name <<%s>> to server...\n", client_mqueue_name);

    // Send message to server with queue name
    rc = mq_send(server_mqueue, client_mqueue_name, strlen(client_mqueue_name), prio);
    DIE(rc == -1, "mq_send");

    // Now open client queue
    client_mqueue = mq_open(client_mqueue_name, O_RDWR | O_CREAT, 0666, NULL);
    DIE(client_mqueue == (mqd_t) - 1, "mq_open");

    // Wait for the server to respond with clearance
    rc = mq_receive(client_mqueue, buf, BUF_SIZE, &prio);
    DIE(rc == -1, "mq_receive");

    if (strcmp(buf, CONNECTION_ACCEPTED) == 0) {
        printf("Connection accepted.\n");

        // Reset buffer
        memset(buf, 0, BUF_SIZE);
        
        // Pick a token the client requires
        req_token = rand() % MAX_TOKENS;

        // Append it to the request
        sprintf(token_message, "%s%s %d", token_message, client_mqueue_name, req_token);

        printf("Asking the server for a token with request \"%s\".\n", token_message);

        // Send request
        rc = mq_send(server_mqueue, token_message, strlen(token_message), prio);
        DIE(rc == -1, "mq_send");

        // Get reply
        rc = mq_receive(client_mqueue, buf, BUF_SIZE, &prio);
        DIE(rc == -1, "mq_receive");

        printf("Received token \"%s\" from server\n", buf);
    }
    else {
        printf("Connection refused, quitting.\n");
    }

    // Cleanup
    rc = mq_close(server_mqueue);
    DIE(rc == -1, "mq_close");

    rc = mq_close(client_mqueue);
    DIE(rc == -1, "mq_close");

    rc = mq_unlink(client_mqueue_name);
    DIE(rc == -1, "mq_close");

    return 0;
}
