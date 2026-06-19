# Multi-Client TCP Chat Server (C++)

## Overview

A Linux-based multi-client chat server built in C++ using POSIX sockets. The project supports multiple concurrent clients, real-time message broadcasting, username registration, connection logging, runtime statistics, configuration-driven startup, graceful shutdown handling, and an epoll-based event-driven architecture.

The project was developed to gain hands-on experience with Linux system programming, TCP/IP networking, concurrency, synchronization, and scalable server design.

---

## Features

### Networking

* TCP-based client-server communication
* Multiple concurrent client connections
* Username registration on connection
* Real-time message broadcasting
* Join and leave notifications
* Interactive terminal client

### Server Management

* Graceful shutdown using SIGINT (Ctrl+C)
* Configuration-driven startup
* Runtime statistics tracking
* Connection management subsystem
* Centralized logging

### Architectures Implemented

#### Thread-per-Client Server

* Dedicated thread for each connected client
* Mutex-protected shared resources
* Simple concurrency model

#### Epoll-Based Server

* Event-driven architecture using Linux epoll
* Efficient handling of multiple connections
* Scalable readiness-based I/O processing
* Dynamic epoll event buffer sizing based on configuration

---

## Architecture

```text
+------------------+
|   Client A       |
+------------------+
          |
          |
+------------------+
|   Client B       |
+------------------+
          |
          |
          v
+--------------------------+
|      TCP Server          |
+--------------------------+
            |
            |
    +---------------+
    |   epoll_wait  |
    +---------------+
            |
            |
    +---------------+
    | ClientManager |
    +---------------+
            |
     +------+------+
     |             |
     v             v
 Broadcast      Statistics
     |
     v
 Connected Clients
```

---

## Project Structure

```text
.
├── client/
│   ├── client.cpp
│   └── client.h
│
├── server/
│   ├── server.cpp
│   ├── server_epoll.cpp
│   ├── client_manager.cpp
│   ├── client_manager.h
│   ├── logger.cpp
│   ├── logger.h
│   ├── stats.cpp
│   ├── stats.h
│   ├── config.cpp
│   └── config.h
│
├── config.txt
├── logs/
├── Makefile
└── README.md
```

---

## Technologies Used

* C++
* POSIX Sockets
* TCP/IP
* Linux System Programming
* Threads (std::thread)
* Mutexes (std::mutex)
* Atomics (std::atomic)
* epoll
* File I/O
* Signal Handling

---

## Build

Compile the project using:

```bash
make
```

---

## Run

### Start the Server

```bash
./server_app
```

### Start a Client

```bash
./client_app
```

Open multiple terminal windows and launch multiple clients to test chat functionality.

---

## Example Session

### Client 1

```text
$ ./client_app

Enter username: Alice
Hello everyone!
```

### Client 2

```text
$ ./client_app

Enter username: Bob

*** Alice joined the chat ***
[Alice]: Hello everyone!
```

---

## Available Commands

### Client Commands

```text
/quit
```

Disconnect from the server and close the client.

### Server Commands

```text
/stats
```

Display:

* Active clients
* Total connections
* Total messages
* Server uptime

---

## Configuration

The server reads startup parameters from:

```text
config.txt
```

Example:

```text
PORT=8080
MAX_CLIENTS=100
LOG_FILE=logs/server.log
```

This allows runtime customization without recompiling.

---

## Logging

The server maintains connection and activity logs including:

* Client connections
* Client disconnections
* Join notifications
* Server startup
* Server shutdown
* Important runtime events

Logs are written to the file specified in the configuration.

---

## Statistics Tracking

The server tracks:

* Active connected clients
* Total client connections
* Total messages exchanged
* Server uptime

Statistics can be queried during runtime.

---

## Reliability Improvements

Several networking and system-level improvements were implemented:

* SO_REUSEADDR for immediate server restart after shutdown
* Graceful shutdown using signal handling
* Atomic counters for thread-safe statistics
* Mutex-protected client management
* Dynamic epoll event buffers
* Client-side coordinated shutdown using atomic state and socket shutdown

---

## Key Learnings

Through this project I gained practical experience with:

* TCP socket programming
* Linux networking APIs
* Concurrent programming
* Event-driven architectures
* epoll-based I/O multiplexing
* Thread synchronization
* Signal handling
* Client-server system design
* Resource management and cleanup
* Scalable server development

---

## Future Improvements

* Thread pool implementation
* Unit testing
* Prometheus metrics integration
* Grafana dashboard
* Private messaging
* Chat rooms
* Authentication support
* TLS/SSL encryption
* Benchmarking thread-per-client vs epoll implementations

---

## License

This project was built for learning purposes and portfolio demonstration.

