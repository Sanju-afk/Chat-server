---

editor_options: 
  markdown: 
    wrap: 72
---

# Multi-Client TCP Chat Server

> **A high-performance event-driven TCP server framework built in C++ using Linux POSIX sockets, non-blocking I/O, epoll, and a custom binary protocol.**

This project is a systems programming exercise focused on designing a scalable networking architecture rather than simply implementing a chat application. The chat functionality serves as a demonstration of an extensible protocol capable of supporting additional packet types and services.

------------------------------------------------------------------------

## Highlights

- Event-driven server using **epoll**
- Non-blocking sockets
- Custom binary application protocol
- Incremental **PacketDecoder** for TCP stream reconstruction
- Username handshake with duplicate detection
- Public chat
- Private messaging (`/msg`)
- Runtime statistics (`/stats`)
- Graceful shutdown
- Modular architecture
- Thread-safe client management

------------------------------------------------------------------------

# Architecture

``` text
                        Clients
          +---------------+---------------+
          |               |               |
          ▼               ▼               ▼
      TCP Socket      TCP Socket      TCP Socket
              \           |           /
               +----------+----------+
                          │
                 Linux Socket API
                          │
                     epoll_wait()
                          │
                   Ready Socket (EPOLLIN)
                          │
                       recv() bytes
                          │
            PacketDecoder (per connected client)
                          │
               Queue of complete Packet objects
                          │
                  Packet Dispatcher (switch)
                          │
      +-----------+-----------+-----------+
      |           |           |           |
 Username      TEXT       PRIVATE      STATS
 Handler      Handler      Handler     Handler
                          │
                    ClientManager
                          │
                     sendPacket()
                          │
                       Connected Clients
```

------------------------------------------------------------------------

# Repository Structure

``` text
.
├── client/
│   ├── client.cpp
│   └── client.h
├── server/
│   ├── server_epoll.cpp
│   ├── client_manager.*
│   ├── logger.*
│   ├── stats.*
│   └── config.*
├── shared/
│   ├── protocol.*
│   ├── packet_decoder.*
│   └── protocol.h
├── config.txt
├── Makefile
└── README.md
```

------------------------------------------------------------------------

# Features

| Feature                      | Status |
|:-----------------------------|:-------|
| Multi-client support         | ✅     |
| epoll-based event loop       | ✅     |
| Non-blocking sockets         | ✅     |
| Binary protocol              | ✅     |
| Packet decoder               | ✅     |
| Username handshake           | ✅     |
| Duplicate username detection | ✅     |
| Public messaging             | ✅     |
| Private messaging            | ✅     |
| Runtime statistics           | ✅     |
| Graceful shutdown            | ✅     |

------------------------------------------------------------------------

# Protocol

Every message is transmitted using a fixed header followed by a variable-length payload.

``` text
+--------------------------------+
|          PacketHeader          |
+--------------------------------+
| PacketType                     |
| Payload Size (bytes)           |
+--------------------------------+
| Payload                        |
+--------------------------------+
```

## Packet Types

| Type              | Purpose                       |
|:------------------|:------------------------------|
| USERNAME          | Initial username registration |
| USERNAME_ACCEPTED | Username accepted             |
| USERNAME_REJECTED | Username rejected             |
| TEXT              | Public message                |
| PRIVATE           | Private message               |
| STATS_REQUEST     | Request server metrics        |
| STATS_RESPONSE    | Server metrics                |

------------------------------------------------------------------------

# Packet Decoder

TCP is **stream-oriented**, not **message-oriented**.

A single `recv()` call may contain:

- Half a packet
- One complete packet
- Multiple packets
- Half of one packet + half of another

The decoder incrementally reconstructs packets.

``` text
recv()
   │
Append bytes
   │
Internal Buffer
   │
Enough bytes for header?
   │
No ───────────────┐
                  │
                 Wait
                  │
Yes
   │
Enough bytes for payload?
   │
No ───────────────┐
                  │
                 Wait
                  │
Yes
   │
Extract Packet
   │
Push into Queue
   │
Continue parsing remaining bytes
```

This architecture correctly handles: - Fragmentation - Sticky packets - Partial headers - Partial payloads - Multiple packets per receive

------------------------------------------------------------------------

# Build

``` bash
make
```

Produces:

``` text
server_app
client_app
```

------------------------------------------------------------------------

# Run

Server:

``` bash
./server_app
```

Clients (multiple terminals):

``` bash
./client_app
```

------------------------------------------------------------------------

# Available Commands

| Command                   | Description           |
|:--------------------------|:----------------------|
| message                   | Broadcast to everyone |
| /msg <username> <message> | Private message       |
| /stats                    | Runtime statistics    |
| /quit                     | Disconnect            |

------------------------------------------------------------------------

# Major Design Decisions

### epoll over thread-per-client

- Lower memory usage
- Fewer context switches
- Better scalability
- Event-driven architecture

### Binary Protocol

- Efficient serialization
- Explicit packet boundaries
- Extensible PacketType enum

### PacketDecoder

- Handles arbitrary TCP byte streams
- Decouples networking from business logic
- Supports protocol evolution

### Per-client Decoder

Each TCP connection owns an independent parser state.

### Modular Design

Networking, protocol, logging, metrics, and client management remain independent modules.

------------------------------------------------------------------------

# Current Limitations

- No endianness conversion (`htonl()` / `ntohl()`)
- Direct `memcpy()` serialization of `PacketHeader`
- Username lookup is O(n)
- `std::vector::erase()` in PacketDecoder is O(n)
- No authentication
- No TLS
- No protocol versioning
- No asynchronous send queue (`EPOLLOUT`)
- No persistence

------------------------------------------------------------------------

# Future Roadmap

- Authentication
- Chat rooms
- File transfer
- Image transfer
- TLS encryption
- Heartbeats (PING/PONG)
- Ring-buffer decoder
- Protocol versioning
- Checksums
- Async output queue
- Benchmark suite
- Metrics dashboard

------------------------------------------------------------------------

# Lessons Learned

The biggest architectural lesson from this project was understanding that **TCP is a byte stream, not a message protocol**.

The initial implementation assumed one `recv()` corresponded to one complete packet. This worked with blocking sockets but failed after migrating to an event-driven, non-blocking `epoll` architecture.

Redesigning the receive path around an incremental **PacketDecoder** separated transport, protocol parsing, and application logic, resulting in a cleaner and more scalable architecture.

------------------------------------------------------------------------

# Technologies

- C++17
- Linux
- POSIX Sockets
- epoll
- Non-blocking I/O
- std::thread (client)
- STL (`unordered_map`, `vector`, `queue`, `mutex`)
- Make

------------------------------------------------------------------------

# License

This project is intended for educational purposes and demonstrates Linux systems programming, scalable network server design, and custom protocol implementation.
