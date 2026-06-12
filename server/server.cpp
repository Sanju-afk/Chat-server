#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <csignal> // for the while loop (graceful signout)

#include "stats.h"
#include "logger.h"
#include "client_manager.h"
#include "config.h"

using namespace std;
/*
Start server
Initialize modules
Accept clients
Shutdown server
*/

int server_socket = -1;

//while loop
volatile sig_atomic_t running = 1;

void signalHandler(int){
    running = 0;

    if (server_socket >= 0){
        close(server_socket);
    }
}

int main(){
    Config config = loadConfig("config.txt");
    //create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0); // address family ipV4, Sock stream means tcp, 0 means default protocol for SOCK_STREAM => TCP
    
    if (server_socket < 0){
        logMessage("socket creation failed...");
        return 1;
    }
    
    logMessage("Socket created succesfully...");
    
    //create bind
    //sockaddr_in struct to create bind
    sockaddr_in server_address{};
    server_address.sin_family = AF_INET; //same addressing as socket, ipV4
    // server_address.sin_port = htons(8080); // port, htons converts 8000 to the format network expects Big Endian
    server_address.sin_port = htons(config.port);
    server_address.sin_addr.s_addr = INADDR_ANY; //Accept connections on all network interfaces
    
    //now bind, call
    int bind_result = bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address));
    
    if (bind_result < 0){
        logMessage("Bind failed...");
        close(server_socket);
        return 1;
    }
    logMessage("Bind succesful...");

    //listening through port for client connection requests
    // int listen_result = listen(server_socket, 5); // allow upto 5 pending connections
    int listen_result = listen(server_socket, config.max_clients);
    if (listen_result < 0){
        logMessage("Listening failed...");
        close(server_socket);
        return 1;
    }
    logMessage("Listening on port 8080...");
    
    //client connected 
    signal(SIGINT, signalHandler);
    while (running) {
        int client_socket = accept(
            server_socket,
            nullptr,
            nullptr
        );
        if (client_socket < 0){
            if (!running){   // because closing the server will cause an accept failure, so check value of running to see if its a server exit
                logMessage("Shutting down server...");
                break;
            }
            logMessage("Accept failed ...");
            continue;
        }
        logMessage("\nClient connected...");

        //thread the connections to support multiple clients
        thread client_thread(
            handleClient,
            client_socket
        ); 
        client_thread.detach();
    }

    close(server_socket); // close the socket
    logMessage("Server shutdown complete");
    return 0;
}


