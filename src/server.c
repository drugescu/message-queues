/*
 * server.c - Practice 2
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

#include "utils.h"

// 8192B
char buf[ BUF_SIZE ];

int main(int argc, char **argv)
{
    int rc;
    unsigned int prio = 10;
    mqd_t server_mqueue;
    mqd_t client_mqueue[ MAX_CLIENTS ];
    char  client_mqueue_names[ MAX_CLIENTS ][ MAX_QUEUE_NAME_SIZE ];
    int CLIENT_INDEX = 0;
    
    printf("Main pid is %d\n", getpid());

    // Start message queue
    server_mqueue = mq_open(SERVER_QUEUE_NAME, O_CREAT | O_RDWR, 0666, NULL);
    DIE(server_mqueue == (mqd_t) - 1, "mq_open");

    // Server doing its stuff
    printf("Server mode on, awaiting clients...\n");

    // Start main loop
    while (true) {
        rc = mq_receive(server_mqueue, buf, BUF_SIZE, &prio);
        DIE(rc == -1, "mq_receive");

        // Process message
        char *command, *client, *token;
   
        // Require command
        command = strtok(buf, " ");
        DIE(command == NULL, "null command in received message (strtok)");

        // Read originating client
        client = strtok(NULL, " ");
        if (client == NULL) {
            // Retain queue name from client
            printf("Received queue name from client: %s\n", buf);

            // Now open client queue if possible
            if (CLIENT_INDEX < MAX_CLIENTS) {
                client_mqueue[ CLIENT_INDEX++ ] = mq_open(buf, O_RDWR, 0666, NULL);
                DIE(client_mqueue[ CLIENT_INDEX - 1] == (mqd_t) - 1, "mq_open");
                printf("Opened queue %s.\n", buf);
            }
            else {
                // Delay request or put it in a queue for later pick-up
                printf("Maximum number of concurrent clients reached, canot open queue.\n");
            }
        }
        else {
            // Require token
            token = strtok(NULL, " ");
            DIE(token == NULL, "null token in received token request (strtok)");

            printf("Received request for token %s from client %s.\n", token, client);

            // Check if token is available and serve it
            // If not, serve the next available token to client[correct_index]
            //rc = mq_send(server_mqueue, token_message, strlen(token_message), prio);
            //DIE(rc == -1, "mq_send");
        }
    }

    // Cleanup
    rc = mq_close(server_mqueue);
    DIE(rc == -1, "mq_close");

    return 0;
}
