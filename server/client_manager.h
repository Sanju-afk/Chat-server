#pragma once

#include <string>

struct Client{
    int socket;
    std::string username;
    bool username_set;
};
void addClient(const Client& client);

void removeClient(int socket);

void broadcast(
    const std::string& message,
    int sender_socket
);

std::string viewMetrics();

Client* findClient(int socket);

void printClientCount();
