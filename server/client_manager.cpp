#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>

#include "client_manager.h"
#include "logger.h"
using namespace std;
/*
Add client
Remove client
Broadcast message
Count clients   
*/
char buffer[1024];
const char* response = "Server recieved your message\n";
void handleClient(int client_socket){
    while (true){
        //recieve data from client
        int bytes_recieved = recv(
            client_socket,
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

        //print the message
        cout << "Recieved : " << buffer;
   
        //send response
        int bytes_sent = send(
            client_socket,
            response,
            strlen(response),
            0
        ); //socket, buffer, number of bytes, flags 
    
        if (bytes_sent < 0){
            logMessage("send() failed");
        }
        else{
            logMessage("Response sent...\n");
        }
    
    }
   logMessage("Connection Ended...");
   close(client_socket);
   return;
}

void printClientCount(){
    cout << "Active clients : 0" << endl;
}