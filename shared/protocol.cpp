#include<iostream>
#include <sys/socket.h>
#include <unistd.h>

#include "protocol.h"
using namespace std;

bool sendAll(int sock, const void* data, size_t length){
    const char* buffer = static_cast<const char*>(data);

    size_t total_sent = 0;

    while (total_sent < length){
        ssize_t bytes = send(sock,buffer+total_sent,length-total_sent,0);
        if (bytes <= 0){
            return false;
        }
        total_sent += bytes;
    }
    return true;
}

bool recvAll(int sock, void* data, size_t length){
    char* buffer = static_cast<char*>(data);

    size_t total_recv = 0;

    while (total_recv < length){    
        ssize_t bytes = recv(sock,buffer + total_recv, length - total_recv, 0);
        if (bytes <= 0){
            return false;
        }
        total_recv += bytes;
    }
    return true;
}


bool sendPacket(int sock, PacketType type, const string& payload){
    PacketHeader header;

    header.type = type;
    header.payload_size = static_cast<uint32_t>(payload.size());

    //send header first
    if (!sendAll(sock, &header, sizeof(header))){
        return false;
    }
    //send payload if present
    if (header.payload_size > 0){
        if (!sendAll(sock,payload.data(), payload.size())){
            return false;
        }
    }
    return true;
}

bool recvPacket(int sock, PacketHeader& header, string& payload){
    //recieve the header
    if (!recvAll(sock, &header, sizeof(header))){
        return false;
    }
    payload.resize(static_cast<size_t>(header.payload_size));

    if (header.payload_size > 0){
        //direct fill no intermediate copies
        if (!recvAll(sock ,payload.data(), payload.size())){
            return false;
        }
    }
    return true;
}

