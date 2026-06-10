#pragma once

#include <string>

struct Client{
    int socket;
    std::string username;
};

void handleClient(int client_socket);

void printClientCount();

void broadcast(const std::string& message, const int sender);

//to manage threads + mutexes
void addClient(const Client& client);

void removeClient(const int socket);
