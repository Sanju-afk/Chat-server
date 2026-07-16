# Compiler
CXX = g++

# Compiler Flags
CXXFLAGS = -Wall -Wextra -std=c++17

# Source files
# SERVER_SRCS = server/server.cpp \
# 			  server/client_manager.cpp \
# 			  server/logger.cpp \
# 			  server/stats.cpp \
# 			  server/config.cpp

EPOLL_SERVER_SRCS = server/server_epoll.cpp \
					 server/client_manager.cpp \
					 server/logger.cpp \
					 server/stats.cpp \
					 server/config.cpp \
					 shared/protocol.cpp \
					 shared/packet_decoder.cpp

CLIENT_SRCS = client/client.cpp \
			  	server/config.cpp \
				shared/protocol.cpp \
				shared/packet_decoder.cpp


# Output Executables	
#SERVER_TARGET = server_app
EPOLL_SERVER_TARGET = server_app
CLIENT_TARGET = client_app

# Default target
all :$(EPOLL_SERVER_TARGET) $(CLIENT_TARGET)

# Build thread-per-client server
#$(SERVER_TARGET) : $(SERVER_SRCS)
#	$(CXX) $(CXXFLAGS) -o $(SERVER_TARGET) $(SERVER_SRCS)

# Build epoll server
$(EPOLL_SERVER_TARGET) : $(EPOLL_SERVER_SRCS)
	$(CXX) $(CXXFLAGS) -o $(EPOLL_SERVER_TARGET) $(EPOLL_SERVER_SRCS)

# Build client
$(CLIENT_TARGET) : $(CLIENT_SRCS)
	$(CXX) $(CXXFLAGS) -o $(CLIENT_TARGET) $(CLIENT_SRCS)

# Clean generated files
clean:
	rm -f $(EPOLL_SERVER_TARGET) $(CLIENT_TARGET)

# Rebuild everything
rebuild: clean all

.PHONY: all clean rebuild