#include <iostream>
#include <fstream>

#include <chrono>
#include <ctime>

#include "logger.h"
using namespace std;
/*
Write messages to log
Print timestamps
Create log file
*/

string getTimeStamp(){
    time_t now = time(nullptr);
    char buffer[100];

    strftime(buffer, sizeof(buffer),  "%Y-%m-%d %H:%M:%S", localtime(&now));

    return string(buffer);
}

void logMessage(const char* message){
    // cout << "Logger available" << endl;
    // cout << "[LOG] " << message << endl;
    //open the log file and write into it
    ofstream logfile("logs/server.log", ios::app); //ios::app means it does not overwrite, instead appends at end
    logfile << "[" << getTimeStamp() << "] " << message << endl;
    cout << "[LOG] " << message << endl;
    logfile.close();
}