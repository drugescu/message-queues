# Practice 2

## Message queues

Implemented a server-client architecture for token requesting using linux message queues.

### Details

Clients connect to a known server message queue, create their own, using their PID, and
send a request for a specific token, which they receive from the server.

* Server queue name is known to clients `\mqueue`.
* Clients queue name is of the form `/mqueue{PID}`.

Token pool is finite and tokens will be reinserted in the pool when recovered from the clients.

The server receives token requests from clients, sends token to client, marks it as unavailable.

Total token pool count is fixed, known to both server & client (255).

A token is an integer number from `0` to `MAX_TOKEN_COUNT`.

Tokens are implemented in a hash table due to fast search/insert/delete O(1) in our case, since
tokens are uniquely allocated to a single client (so no bins). Custom hashmap implementation.

There is a maximum number of concurrent clients allowed by the server, which rejects further
connections if clients will subsequently try to connec.t

### Usage

`./server`

`./client`

### ToDo

- [ ] When done, the client returns the token to the server, to reinsert in the pool.

