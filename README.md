=Practice 2=

==Message queues==

Server queue name is known to clients "\mqueue".
Clients queue name is of the form "/mqueue{PID}".

Token pool is finite and tokens will be reinserted in the pool when recovered from the clients.

The server receives token requests from clients, sends token to client, marks it as unavailable.

When done, the client returns the token to the server, to reinsert in the pool.

Total token pool count is fixed, known to both server & client (255).

A token is an integer number from 0 to MAX_TOKEN_COUNT

