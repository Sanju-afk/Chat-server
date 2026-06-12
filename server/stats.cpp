#include "stats.h"

std::chrono::steady_clock::time_point server_start_time = std::chrono::steady_clock::now();

int total_messages = 0;
int total_connections = 0;
int peak_clients = 0;

long getServerUptime(){
    return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - server_start_time).count();
}
