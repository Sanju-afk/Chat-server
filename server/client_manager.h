#pragma once

#include <string>
#include "../shared/packet_decoder.h"


struct Client{
    int socket;
    std::string username;
    bool username_set;
    PacketDecoder decoder;
};
void addClient(const Client& client);

void removeClient(int socket);

void broadcast(
    PacketType type,
    const string& message,
    int sender = - 1
);

std::string viewMetrics();

Client* findClient(int socket);

void closeAllClients();

void printClientCount();
