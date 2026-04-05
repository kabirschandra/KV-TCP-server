# KV-TCP-Server
A key-value store that communicates over TCP. The server maintains a hashmap in memory. Clients connect, send commands, and get responses.

What you need:
- GCC compiler

- Linux or WSL (socket code uses POSIX)

Make sure port 9000 is free

## Quick start
Open two terminals.

#### Terminal 1 - start the server:
````
./run.sh run-server
````
#### Terminal 2 - connect a client:
````
./run.sh run-client
````
## Commands
At the client prompt, type:

````
connect 127.0.0.1
insert username alice
lookup username
delete username
rehash 32
terminate
````
The server will log what it did.

## Build
From the project root:

````
./run.sh compile
````
This creates binaries in the bin/ folder.

You can also compile separately:

````
./run.sh compile-server
./run.sh compile-client
````
## Project structure
* src/client.c - Client app. Sends packets to server.

* src/server.c - Server app. Listens for connections and processes requests.

* src/hashmap.c / src/hashmap.h - Hashmap implementation. Handles insert, delete, lookup, rehash.

* src/packet.c / src/packet.h - Packet format. Defines how client and server talk.

* run.sh - Build script.

## How packets work
The client builds a packet with an operation and fields. It sends metadata first (operation type, field lengths), then sends the actual field data. The server receives, processes, and sends back responses for lookups.

The protocol handles variable-length fields. Rehash includes a size field. Terminate closes the connection.

## Limitations
* Only one active server connection per client

* No persistent storage (data lives only while server runs)

* Max input size is 1000 characters

* Hardcoded port 9000

## Known issues
* Hashmap warnings about const return types. They don't affect functionality.

## Clean up
````
./run.sh clean
````
Removes the bin/ folder.

## Why this exists
Built to understand TCP sockets and network protocols. Commands go over the wire to the server. The server sends a response back to the client.
