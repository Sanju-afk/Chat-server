#include <iostream>
#include "client.h"
#include <thread>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <atomic>
#include <sys/socket.h>
#include "../server/config.h"
using namespace std;

atomic<bool> running(true);
/*
Connect to server
Send messages
Receive messages
*/

//reciever thread
// void receiveMessages(int sock){
//     char buffer[1024];
//     while (true){
//         int bytes = recv(
//             sock,
//             buffer,
//             sizeof(buffer) - 1,
//             0
//         );
        
//         if (bytes <= 0){
//             cout << "Disconnected from server\n";
//             break;
//         }
//         buffer[bytes] = '\0';
//         cout << buffer;
//         cout.flush();
//     }
// }

void receiveMessages(int sock)
{
    cout << "[Receiver started]\n";

    char buffer[1024];

    while (running)
    {
        int bytes = recv(
            sock,
            buffer,
            sizeof(buffer)-1,
            0
        );

        cout << "[recv returned " << bytes << "]\n";

        if (bytes <= 0)
        {
            cout << "Disconnected from server\n";
            break;
        }

        buffer[bytes] = '\0';
        cout << buffer;
    }

    cout << "[Receiver exiting]\n";
}

//sender thread
void sendMessages(int sock){
    string message;
    while (getline(cin, message)){
        if (message == "/quit"){
            running = false;
            shutdown(sock, SHUT_RDWR);
            close(sock);
            break;
        }
        send(
            sock,
            message.c_str(),
            message.size(),
            0
        );
    }
}

int main(){
    Config config = loadConfig("config.txt");
    int sock = socket(AF_INET, SOCK_STREAM,0); //AF_INET - ipv4, AF_INET6 - ipv6

    if (sock < 0){
        cerr << "Socket creation failed\n";
        return 1;
    }

    sockaddr_in server_addr{};

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(config.port);

    inet_pton(
        AF_INET,
        "127.0.0.1",
        &server_addr.sin_addr
    );

    if (connect(
        sock,
        (sockaddr*)&server_addr,
        sizeof(server_addr)
    ) < 0 ) {
        cerr << "Connection failed\n";
        close(sock);
        return 2;
    }
    cout << "Connected to server\n";

    //recieve username prompt from server
    char buffer[1024];
    int bytes = recv(
        sock,
        buffer,
        sizeof(buffer) - 1,
        0
    );
    if (bytes <= 0){
        cerr << "Recieve failed\n";
        close(sock);
        return 3;
    }
    buffer[bytes] = '\0';
    cout << buffer;

    //Send username
    string username;
    getline(cin, username);

    send(
        sock,
        username.c_str(),
        username.size(),
        0
    );

    //start chat threads
    thread recieverThread(receiveMessages,sock);
    thread senderThread(sendMessages, sock);

    recieverThread.join();
    senderThread.join();

    close(sock);

    return 0;
}