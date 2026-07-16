#pragma once

#include<cstdint>
#include <string>

using namespace std;

enum class PacketType : uint32_t
{
    USERNAME = 1,
    USERNAME_ACCEPTED,
    USERNAME_REJECTED,
    TEXT,
    STATS_REQUEST,
    STATS_RESPONSE,
    IMAGE,
    FILE,
    PRIVATE
};

struct PacketHeader
{
    PacketType type;
    uint32_t payload_size;
};

/*
Header
------
type = STATS
size = 0

Payload
-------
none

Header
------
TEXT
5

Payload
-------
hello
*/

bool sendAll(
    int sock,
        const void* data,
        size_t length
);

bool recvAll(
    int sock,
    void* data,
    size_t length
);

bool sendPacket(
    int sock,
    PacketType type,
    const string& payload
);

bool recvPacket(
    int sock,
    PacketHeader& header,
    string &payload
);

