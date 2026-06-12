#pragma once
#include <chrono>

extern std::chrono::steady_clock::time_point server_start_time;

extern int total_messages;
extern int total_connections;
extern int peak_clients;  


long getServerUptime();

