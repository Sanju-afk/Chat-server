#Compiler
CXX = g++

#Compiler Flags
CXXFLAGS = -Wall -Wextra -std=c++17

#Source files
SERVER_SRCS = server/server.cpp \
			  server/client_manager.cpp \
			  server/logger.cpp 

CLIENT_SRCS = client/client.cpp

#Output Excecutables
SERVER_TARGET = server_app
CLIENT_TARGET = client_app

#Default target
all : $(SERVER_TARGET) $(CLIENT_TARGET)

#build server
$(SERVER_TARGET) : $(SERVER_SRCS)
	$(CXX) $(CXXFLAGS) -o $(SERVER_TARGET) $(SERVER_SRCS)

#build client
$(CLIENT_TARGET) : $(CLIENT_SRCS)
	$(CXX) $(CXXFLAGS) -o $(CLIENT_TARGET) $(CLIENT_SRCS)

#clean generated files
clean:
	rm -f $(SERVER_TARGET) $(CLIENT_TARGET)

#rebuild everything
rebuild: clean all

.PHONY: all clean rebuild

