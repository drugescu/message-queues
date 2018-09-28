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
#include <time.h>
#include <stdbool.h>
#include <signal.h>

#include "utils.h"
#include "hashmap.h"

// 8192B
char buf[ BUF_SIZE ] = { 0 };

int rc;
unsigned int prio = 10;

mqd_t server_mqueue;
mqd_t client_mqueue[ MAX_CLIENTS ];
mqd_t temp_mqueue;
char  client_mqueue_name[ MAX_CLIENTS ][ MAX_QUEUE_NAME_SIZE ] = { 0 };
int CLIENT_INDEX = 0;

hashmap_t *client_map;

// Interrupt signal handler - perform cleanup
void sig_handler(int signo)
{
    if (signo == SIGINT) {
        printf("\nReceived SIGINT\n");

        // Cleanup
        // Client connection closing not needed, too fast
        rc = mq_close(server_mqueue);
        DIE(rc == -1, "mq_close");

        rc = mq_unlink(SERVER_QUEUE_NAME);
        DIE(rc == -1, "mq_close");
    }

    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
    // Initialization
    printf("Main pid is %d\n", getpid());
    srand(time(NULL));
    if (signal(SIGINT, sig_handler) == SIG_ERR)
        printf("Could not register sig_handler, can't catch SIGINT\n'");

    // Client client token hash
    client_map = create_hash(MAX_CLIENTS);

    // Start message queue
    server_mqueue = mq_open(SERVER_QUEUE_NAME, O_CREAT | O_RDWR, 0666, NULL);
    DIE(server_mqueue == (mqd_t) - 1, "mq_open");

    // Server doing its stuff
    printf("Server mode on, awaiting clients...\n");

    // Start listening to requests from clients
    while (true) {
        // Clear buffer
        memset(buf, 0, BUF_SIZE);
        
        // Receive message from client in server_mqueue
        rc = mq_receive(server_mqueue, buf, BUF_SIZE, &prio);
        DIE(rc == -1, "mq_receive");

        // Process message
        char *command = NULL;
        char *client = NULL;
        char *stoken = NULL;
        
        // Require command
        command = strtok(buf, " ");
        DIE(command == NULL, "null command in received message (strtok)");

        // Read originating client
        client = strtok(NULL, " ");
        if (client == NULL) { // If Request is for initial connection to client
            // Retain queue name from client
            printf("Received queue name from client: %s\n", buf);

            // Now open client queue if possible
            if (CLIENT_INDEX < MAX_CLIENTS) { // Do connection
                // Associate name with client
                sprintf(client_mqueue_name[ CLIENT_INDEX ], "%s", buf);

                // Open queue
                client_mqueue[ CLIENT_INDEX++ ] = mq_open(buf, O_RDWR, 0666, NULL);
                DIE(client_mqueue[ CLIENT_INDEX - 1] == (mqd_t) - 1, "mq_open");
                printf("Opened queue %s and associated with client %d/%d.\n",
                        client_mqueue_name[ CLIENT_INDEX - 1 ],
                        CLIENT_INDEX - 1, MAX_CLIENTS);

                // Establish connection
                rc = mq_send(client_mqueue[ CLIENT_INDEX - 1], CONNECTION_ACCEPTED,
                             strlen(CONNECTION_ACCEPTED), prio);
                DIE(rc == -1, "mq_send");
                printf("Sending connection request answer.\n");
            }
            else { // Send refusal message
                // Delay request or put it in a queue for later pick-up
                printf("Maximum number of concurrent clients reached, canot open queue.\n");

                // Temporarily open and close connection, sending error message
                temp_mqueue = mq_open(buf, O_RDWR, 0666, NULL);
                DIE(temp_mqueue == (mqd_t) - 1, "mq_open");
                printf("Temporarily opened temp_queue to send error message.\n");

                // Send refusal message to the client
                rc = mq_send(temp_mqueue, CONNECTION_REFUSED, strlen(CONNECTION_REFUSED), prio);
                DIE(rc == -1, "mq_send");

                rc = mq_close(temp_mqueue);
                DIE(rc == -1, "mq_close");
            }
        }
        else { // If Request is for a token
            // Require token
            stoken = strtok(NULL, " ");
            DIE(stoken == NULL, "null token in received token request (strtok)");

            printf("Received request for token %s from client %s.\n", stoken, client);

            // Find the client
            int current_client = 0;
            
            while (strcmp(client_mqueue_name[ current_client++ ], client) != 0) {}

            printf("Found client queue @(%s, I:%d)\n", client_mqueue_name[ current_client - 1 ],
                     current_client - 1 );

            // Check if token is available and serve it if yes
            if (put_token(client_map, stoken, "[TOKEN]") == KEY_EXISTS) {

                // Allocate a token
                int new_token = 0;
                char snew_token[ BUF_SIZE ] = { 0 };

                sprintf(snew_token, "%d", new_token);

                while (put_token(client_map, snew_token, "[TOKEN]") == KEY_EXISTS) {
                    // Watch out for the situation where all tokens are allocated!
                    // Busy-waiting here! Not optimal! Insert a timeout!
                    new_token = rand() % MAX_TOKENS;
                    sprintf(snew_token, "%d", new_token);
                }

                // Send token message to the client
                rc = mq_send(client_mqueue[ current_client - 1 ], snew_token, strlen(snew_token), prio);
                DIE(rc == -1, "mq_send");
            } else {
                printf("Token %s allocated and sent to client.\n", stoken );

                rc = mq_send(client_mqueue[ current_client - 1 ], stoken, strlen(stoken), prio);
                DIE(rc == -1, "mq_send");
            }
            
            // Close the connection with the current client
            rc = mq_close(client_mqueue[ current_client - 1 ]);
            DIE(rc == -1, "mq_close");
            //goto l;
        }
    }

    // Cleanup
    for (int index = 0; index < CLIENT_INDEX; index++) {
        rc = mq_close(client_mqueue[ index ]);
        DIE(rc == -1, "mq_close");
    }
    
    rc = mq_close(server_mqueue);
    DIE(rc == -1, "mq_close");

    rc = mq_unlink(SERVER_QUEUE_NAME);
    DIE(rc == -1, "mq_close");

    return 0;
}
