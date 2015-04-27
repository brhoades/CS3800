/*******************************************
 * Student:    Billy J Rhoades II
 * Username:   bjrq48
 * Class:      CS5201
 * Assignment: 2 
 * File:       main.cpp
   Program arguments:
     ./driver programlist programtrace pagesize pagealgo prepaging
       * programlist - file containing list of programs and their page requirements.
       * programtrace - file containing list of programs and their page accesses.
       * pagesize - size of pages in memory simulation.
       * pagealgo - which page replacement algorithm to use, choose from:
          lru - least recently used
          fifo - first in first out
          clock - clock replacement algorithm
       * prepaging - 0 for ondemand page access, 1 for prepaging
   Example:
    ./driver programlist.dat programtrace.dat 2 lru 0
 ******************************************/
#include <iostream>
#include "memorysimulator.h"

using namespace std;

int main( int argc, char* argv[] )
{
  MemorySimulator sim( argc, argv );

  sim.run( );

  return 0;
}
