#include "config.h"
#include<fstream>
#include<sstream>

using namespace std;


Config loadConfig(const std::string& config_file){
    Config config;

    ifstream file(config_file);
    string line;

    while (getline(file, line)){
        if(line.find("PORT") == 0){
            config.port = stoi(line.substr(5));
        }
        else if (line.find("LOG_FILE") == 0){
            config.log_file = line.substr(9);
        }
        else if (line.find("MAX_CLIENTS") == 0){
            config.max_clients = stoi(line.substr(12));
        }
    }
    return config;
}

