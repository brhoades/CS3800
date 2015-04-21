CXX=/usr/bin/clang
CFLAGS=-lcurses -lform -lnsl -g -Wall

OBJECTS=client \
				server_utils.o \
        utils.o \
				server 

.SUFFIXES: .c

.c.o: $<
	$(CXX) $(CFLAGS) -c $<

default: all

all: $(OBJECTS)

client: utils.o client.o
	$(CXX) $(CFLAGS) $? -o client

server: server_utils.o utils.o server.o
	$(CXX) $(CFLAGS) $? -pthread -o server

clean: 
	-@rm -f $(OBJECTS)

server_utils.o: server_utils.h server_utils.c
server.o: server.c
client.o: client.c
utils.o: utils.c utils.h
