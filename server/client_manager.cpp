#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <string>

#include "stats.h"
#include "client_manager.h"
#include "logger.h"
using namespace std;
/*
Add client
Remove client
Broadcast message
Count clients   
*/

unordered_map<int, Client> clients;    

string viewMetrics(){
    string metrics = "Server uptime : " + to_string(getServerUptime()) + "\n" +
                    "Total messages : " + to_string(total_messages) + "\n" + 
                    "Total connections : " + to_string(total_connections) + "\n" +
                    "Peak clients : " + to_string(peak_clients) + "\n" +
                    "Active users : " + to_string(clients.size()) + "\n";

    return metrics;            
}

void closeAllClients(){
    for(auto& [fd, client] : clients)
    {
        close(fd);
    }
    clients.clear();
}

void broadcast(const string& message, const int sender){
    for (auto [socket, client]: clients){
        //broadcast to all users
        if (sender == socket) continue;
        send(
            socket,
            message.c_str(),
            message.length(),
            0
        );
    }
}

Client* findClient(int socket){
    auto it = clients.find(socket);
    if (it == clients.end()){
        return nullptr;
    }
    return &it->second; // this returns a pointer to client object stored in it ie address of
}

void printClientCount(){
    cout << "Active clients: " << clients.size() << endl;
}

void addClient(const Client& client){
    clients[client.socket] = client;
    peak_clients = max(peak_clients, (int)clients.size());
    total_connections++;
    //display updated count
    printClientCount();
}

void removeClient(const int socket){
    if (clients.count(socket)) clients.erase(socket);
    //display updated count
    printClientCount();
}





