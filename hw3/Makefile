CXX=/usr/bin/clang
CFLAGS=-lnsl -g

default: all

all: client server

client: client.c
	$(CXX) $(CFLAGS) $< -o client

server: server.c
	$(CXX) $(CFLAGS) $< -pthread -o server

clean: 
	-@rm -f server
	-@rm -f client

