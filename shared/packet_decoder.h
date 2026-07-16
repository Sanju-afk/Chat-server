#pragma once

#include <vector>
#include <queue>
#include <string>
#include "protocol.h"

using namespace std;

struct Packet{
    PacketHeader header;
    string payload;
};

struct PacketDecoder{
private:
    vector<char> buffer;
    queue<Packet> packets;

public:
    void feed(const  char* data, size_t bytes);
    
    bool hasPacket() const;
    
    Packet nextpacket();
};  