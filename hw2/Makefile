####################################################
# Student:    Billy J Rhoades II
# Username:   bjrq48
# Class:      CS
# Assignment: 2 
# File: 			Makefile 
####################################################

CXX=/usr/bin/clang++
CFLAGS=-g -Wall -W -pedantic-errors

OBJECTS=memorysimulator.o \
        program.o \
        page.o \
        main.o

.SUFFIXES: .cpp

.cpp.o: $<
	$(CXX) -c $(CFLAGS) -o $@ $<

driver: $(OBJECTS)
	$(CXX) $(CFLAGS) -o $@ $?

default: all

all: driver

clean: 
	-@rm -rf core > /dev/null 2>&1
	-@rm -f driver > /dev/null 2>&1
	-@rm -f $(OBJECTS) > /dev/null 2>&1

main.o: main.cpp
memorysimulator.o: memorysimulator.cpp memorysimulator.h
program.o: program.cpp program.h
page.o: page.h page.cpp
