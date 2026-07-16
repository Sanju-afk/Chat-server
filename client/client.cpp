#include <iostream>
#include "client.h"
#include <thread>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <atomic>
#include <signal.h>
#include <sys/socket.h>
#include "../server/config.h"
#include "../shared/protocol.h"

using namespace std;

atomic<bool> running(true);
/*
Connect to server
Send messages
Receive messages
*/

void receiveMessages(int sock){
    cout << "[Receiver started]" << endl;

    while (running){
        PacketHeader header;
        string payload;

        if (!recvPacket(sock, header, payload)){
            cout << "Disconnected from server\n";
            break;
        }

        switch(header.type){
            case PacketType :: TEXT:
            {
                cout << payload;
                break;
            }
            case PacketType :: STATS_RESPONSE:
            {
                cout << payload;
                break;
            }
            default:
            {
                cout << "Unknown packet type received\n";
                break;
            }
        }
    }
    cout << "[Reciever exiting]...\n";
}

void sendMessages(int sock){
    string message;

    while (running && getline(cin, message)){
        if (message == "/quit"){
            running = false;
            shutdown(sock , SHUT_RDWR);
            break;
        }

        PacketType type = PacketType::TEXT;
        string payload = message;
        
        //stats command
        if (message == "/stats"){
            type = PacketType :: STATS_REQUEST;
            payload.clear();
        }

        //private messaging command
        else if (message.rfind("/msg", 0) == 0){
            type = PacketType::PRIVATE;
            //remove /msg
            payload = message.substr(5);

            if (payload.find(' ') == string::npos){
                cerr << "Usage: /msg <username> <message>\n";
                continue;
            }
        }

        if (!sendPacket(sock, type, payload)){
            cerr << "Failed to send packet\n";
            shutdown(sock, SHUT_RDWR);
            running = false;
            break;
        }
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

    //recieve username prompt from server (loop based)
    PacketHeader header;
    string payload;

    while (true){
        cout << "Enter username : ";
        string username;
        getline(cin, username);

        if (!sendPacket(
            sock,
            PacketType::USERNAME,
            username
        )){
            cerr << "Failed to send username\n";
            close(sock);
            return 1;
        }
        if (!recvPacket(
            sock,
            header,
            payload
        )){
            cerr << "Server disonnceted\n";
            close(sock);
            return 1;
        }

        if (header.type == PacketType::USERNAME_ACCEPTED){
            break;
        }
        if (header.type == PacketType::USERNAME_REJECTED){
            cout << payload << '\n';
        }
    }

    //start chat threads
    thread recieverThread(receiveMessages,sock);
    thread senderThread(sendMessages, sock);

    recieverThread.join();
    senderThread.join();

    close(sock);

    return 0;
}