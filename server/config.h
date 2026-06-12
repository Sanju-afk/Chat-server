#pragma once
#include <string>

struct Config{
    int port;
    int max_clients;
    std::string log_file;
};

Config loadConfig(const std::string& config_file);