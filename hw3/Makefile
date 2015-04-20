CXX=/usr/bin/clang
CFLAGS=-lnsl -g -Lcsp -Wall

OBJECTS=client \
				server_utils.o \
				server 

.SUFFIXES: .c

.c.o: $<
	$(CXX) $(CFLAGS) -c $<

default: all

all: $(OBJECTS)

client: client.o
	$(CXX) $(CFLAGS) $? -o client

server: server.o server_utils.o
	$(CXX) $(CFLAGS) $? -pthread -o server

clean: 
	-@rm -f $(OBJECTS)

server_utils.o: server_utils.h server_utils.c
server.o: server.c
client.o: client.c

