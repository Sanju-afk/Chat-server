#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <vector>
#include <algorithm>
#include <mutex>

#include "client_manager.h"
#include "logger.h"
using namespace std;
/*
Add client
Remove client
Broadcast message
Count clients   
*/

//global client storage
mutex clients_mutex;
vector<Client> clients;

void handleClient(int client_socket){
    //ask for username
    const char* user_prompt = "Enter username: ";

    int bytes_sent = send(
        client_socket,
        user_prompt,
        strlen(user_prompt),
        0
    );
    if (bytes_sent < 0){
        logMessage("send() failed (user name)...\n");
        close(client_socket);
        return;
    }
    //recieve username
    char username_buffer[100];
    int username_bytes = recv(
        client_socket,
        username_buffer,
        sizeof(username_buffer) - 1,
        0
    );

    if (username_bytes == 0){
        logMessage("Client connected but then disconnected...\n");
        close(client_socket);
        return;
    }

    if (username_bytes < 0){
        logMessage("Username recieved failed...\n");
        close(client_socket);
        return;
    }
    username_buffer[username_bytes] = '\0';
    
    //create client object
    Client client;
    client.socket = client_socket;
    client.username = username_buffer;

    //add to vector storage
    while (!client.username.empty() &&
      (client.username.back() == '\n' ||
       client.username.back() == '\r'))
    {
        client.username.pop_back();
    }
    addClient(client);
    string join_message = "USER JOINED " + client.username;

    logMessage(join_message.c_str());

    //welcome message sent to user
    string welcome = "Welcome " + client.username + "\n";
    bytes_sent = send(
        client.socket,
        welcome.c_str(),
        welcome.length(),
        0
    );

    //joining notification to everyone else
    string join_notification =
    "*** " +
    client.username +
    " joined the chat ***\n";
    broadcast(join_notification, client.socket);


    if (bytes_sent < 0){
        logMessage("send() failed (welcome)...\n");
        removeClient(client.socket);
        close(client.socket);
        return;
    }

    //start the while loop recieving messages
    while (true){
        
        //recieve data from client
        char buffer[1024];
        int bytes_recieved = recv(
            client.socket,
            buffer,
            sizeof(buffer) - 1,
            0); // source ,dest, max size, default behaviour (0).
            //recv > 0 success, < 0 error, = 0 client disconnected
        
        if (bytes_recieved == 0)
        {
            logMessage("Client disconnected");
            break;
        }

        if (bytes_recieved < 0)
        {
            logMessage("recv() failed");
            break;
        }

        logMessage("Message recieved...");
        buffer[bytes_recieved] = '\0'; // add c string delimiter

        //print the message recieved from user
         std::cout << "[" << client.username << "] : " << buffer;
   
        //send response
        // const char* response = "Server recieved your message\n";

        // int bytes_sent = send(
        //     client.socket,
        //     response,
        //     strlen(response),
        //     0
        // ); //socket, buffer, number of bytes, flags 
        
        //broadcast message
        string message = "[" + client.username + "]: " + buffer;
        broadcast(message, client.socket);
    }
    std::string leave_message =
        "User left: " + client.username;

    logMessage(leave_message.c_str());
    //remove from clients list

    //leaving notification
    string leave_notification =
    "*** " +
    client.username +
    " left the chat ***\n";
    broadcast(leave_notification, client.socket);

    removeClient(client.socket);
    //close socket
    close(client.socket);
    return;
}

void broadcast(const string& message, const int sender){
    lock_guard<mutex> lock(clients_mutex);
    for (const Client& client : clients){
        //broadcast to all users
        if (sender == client.socket) continue;
        send(
            client.socket,
            message.c_str(),
            message.length(),
            0
        );
    }
}

void printClientCount(){
    cout << "Active clients: " << clients.size() << endl;
}

void addClient(const Client& client){

    lock_guard<mutex> lock(clients_mutex); //automatically locks when created and unlocks when destoroyed
    clients.push_back(client);

    //display updated count
    printClientCount();
}

void removeClient(const int socket){
    lock_guard<mutex> lock(clients_mutex);
    clients.erase(
    remove_if(
        clients.begin(),
        clients.end(),
        [&](const Client& c)
        {
            return c.socket == socket;
        }),
        clients.end()
    );

    //display updated count
    printClientCount();
}





