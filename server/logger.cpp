#include <iostream>
#include "logger.h"
using namespace std;
/*
Write messages to log
Print timestamps
Create log file
*/
void logMessage(const char* message){
    cout << "Logger available" << endl;
    cout << "[LOG] " << message << endl;
}