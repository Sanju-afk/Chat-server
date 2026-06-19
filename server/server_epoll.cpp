#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <cstring>
#include <csignal> // graceful shutdown
#include<fcntl.h> // for non blocking epoll
#include <cerrno>
#include <vector>

#include "client_manager.h"
#include "logger.h"
#include "stats.h"
#include "config.h"


using namespace std;

int server_socket = -1;

//graceful shutdown
volatile sig_atomic_t running = 1;

void signalHandler(int){
    running = 0;
}

int main(){
    //PORT CREATION
    Config config = loadConfig("config.txt");
    server_socket = socket(AF_INET, SOCK_STREAM, 0); //uses IPV4 family and TCP handshake

    if (server_socket < 0){
        logMessage("Server socket creation failed...");
        return 1;
    }
    logMessage("Server socket created succesfully...");

     //To fix TCP timeout issues
    int opt = 1;
    if (
        setsockopt(
        server_socket,
        SOL_SOCKET,
        SO_REUSEADDR,
        &opt,
        sizeof(opt)  //"Without SO_REUSEADDR, restarting the server quickly can fail because the previous TCP endpoint remains in TIME_WAIT.
    ) < 0 ){
        perror("setsockopt");
        return 1;
    }

    //BIND
    sockaddr_in server_address{};
    server_address.sin_family = AF_INET; //same as port
    server_address.sin_port = htons(config.port); //to convert to supported format
    server_address.sin_addr.s_addr = INADDR_ANY;

    int bind_result = bind(
        server_socket,
        (struct sockaddr*)&server_address,
        sizeof(server_address)
    );

    if (bind_result < 0){
        logMessage("Socket binding failed...");
        close(server_socket);
        return 1;
    }
    logMessage("Bind succesful...");

    //now listen
    int listen_result = listen(
        server_socket,
        config.max_clients
    );

    if (listen_result < 0){
        logMessage("Listening failed...");
        close(server_socket);
        return 1;
    }
    logMessage(("Listening on port " + to_string(config.port) + "...").c_str());

    //create epoll (epoll watchlist : linux maintains a list of sockets)
    int epoll_fd = epoll_create1(0);
    

    if (epoll_fd < 0){
        logMessage("epoll creation failed...");
        cout
        << strerror(errno)
        << endl;
        close(server_socket);
        return 1;
    }

    //register server socket
    epoll_event event{};
    event.events = EPOLLIN;
    event.data.fd = server_socket;  //tell me when some one is trying to connect

    //register
    int ctl_result = epoll_ctl(
        epoll_fd,
        EPOLL_CTL_ADD,
        server_socket,
        &event
    ); //now epoll watches server_socket

    if (ctl_result < 0){
        logMessage("epoll ctl failed...");
        return 1;
    }

    //event buffer
    vector<epoll_event> events(config.max_clients+1); //stores sockets returned by linux
     
    //event loop
    signal(SIGINT, signalHandler);
    while (running){
        int ready = epoll_wait(
            epoll_fd,
            events.data(),
            events.size(),
            1000
        ); //every 1000ms ,wake up check running continue or exit

        if (ready < 0){
            if (errno == EINTR) continue; // to handle ctrl+C
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < ready; i++){
            if (events[i].data.fd == server_socket){
                //accept the connection
                int client_socket = accept(
                    server_socket,
                    nullptr,
                    nullptr
                );
                if (client_socket < 0){
                    logMessage("Accept failed...");
                    continue;
                }
                //make client non blocking during recv()
                int flags = fcntl(
                            client_socket,
                            F_GETFL,
                            0
                            );
                //ensures current flags are not overwritten
                fcntl(
                    client_socket,
                    F_SETFL,
                    flags | O_NONBLOCK
                );
                logMessage(("Client connected FD = " + to_string(client_socket)+"\n").c_str());

                //now register the client socket ie make epoll watch it
                epoll_event client_event{};
                client_event.events = EPOLLIN;
                client_event.data.fd = client_socket;

                //register
                int client_register = epoll_ctl(
                    epoll_fd,
                    EPOLL_CTL_ADD,
                    client_socket,
                    &client_event
                );  

                if (client_register < 0){
                    logMessage("Client connection failed...");
                    continue;
                }
                //initialise client object
                Client client;
                client.socket = client_socket;
                client.username = "";
                client.username_set = false;

                addClient(client);
                //send prompt for username
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
                    continue;
                }
            }
            else {
                //Client has sent a data (event is not occuring on server socke, so its a client socket)
                int client_socket = events[i].data.fd;
                char buffer[1024];

                int bytes_recieved = recv(
                    client_socket,
                    buffer,
                    sizeof(buffer)-1,
                    0
                );

                //Client diconnects
                if (bytes_recieved == 0){
                    Client* client = findClient(client_socket);
                    
                    if (client == nullptr) continue;
                    logMessage((client->username + " disconnected...").c_str());
                    broadcast(("****" + client->username + " left the chat ****\n").c_str(), client->socket);
                    //disconnect from epoll then close
                    epoll_ctl(
                        epoll_fd,
                        EPOLL_CTL_DEL,
                        client_socket,
                        nullptr
                    );
                    removeClient(client_socket);
                    close(client_socket);
                    continue;
                }
                if (bytes_recieved < 0){

                    //check if empty message (non blocking)
                    if (errno == EAGAIN || errno == EWOULDBLOCK) continue;

                    logMessage("Recieved failed...");
                    //disconnect from epoll then close
                    epoll_ctl(
                        epoll_fd,
                        EPOLL_CTL_DEL,
                        client_socket,
                        nullptr
                    );
                    removeClient(client_socket);
                    close(client_socket);
                    continue;
                }
        
                Client* client = findClient(client_socket);
                if(client == nullptr)
                {
                    logMessage("Empty client found...");
                    continue;
                }
                buffer[bytes_recieved] = '\0';
                string message = buffer;
                
                while (!message.empty() && (message.back() == '\n' || message.back() == '\r')){
                    message.pop_back();
                }
                //set username
                if (!client->username_set){
                    client->username = message;
                    client->username_set = true;
                    cout
                    << "Username set for "
                    << client->username
                    << endl;
                    string welcome = "Welcome " + client->username + "\n";
                    send(
                        client_socket,
                        welcome.c_str(),
                        welcome.size(),
                        0
                    );
                    //join notification
                    string join_messaage = "*** " + client->username + " joined the chat ***\n";
                    broadcast(join_messaage, client_socket); 
                    continue;
                }
                //handle statistics dashboard
                if (message == "/stats"){
                    string metrics = viewMetrics();
                    send(
                        client->socket,
                        metrics.c_str(),
                        metrics.length(),
                        0
                    );
                }
                //broadcast normal message
                else{
                    broadcast("[" + client->username + "]: " + message + "\n", client_socket);
                    total_messages++;
                }
            }
        }
    }
    logMessage("Server shutting down...");
    closeAllClients();
    printClientCount();
    close(server_socket);
}