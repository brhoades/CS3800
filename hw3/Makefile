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

client: client_utils.o utils.o client.o
	$(CXX) $(CFLAGS) $? -o client

server: server_utils.o utils.o server.o
	$(CXX) $(CFLAGS) $? -pthread -o server

clean: 
	-@rm -f $(OBJECTS)

server_utils.o: server_utils.h server_utils.c
client_utils.o: client_utils.h client_utils.c
server.o: server.c server_utils.h server_utils.c utils.h utils.c
client.o: client.c client_utils.h client_utils.c utils.c utils.h
utils.o: utils.c utils.h
