/*******************************************
 * Student:    Billy J Rhoades II
 * Username:   bjrq48
 * Class:      CS5201
 * Assignment: 2 
 * File:       page.cpp
 ******************************************/
#include "page.h"

//PRE: None
//POST: m_owner is null, m_clock is false, and all other member variables are 1, num is unknown
Page::Page( )
{
  m_accessed = m_loaded = m_contents = 1;
  m_owner = NULL;
  m_clock = false;
}

//PRE: num should be within memorysimulator's m_memory array
//POST: m_owner is null, m_clock is false, m_num is num, and all other member variables are 1
Page::Page( unsigned int num ) : m_num( num )
{
  m_loaded = m_accessed = m_contents = 1;
  m_owner = NULL;
  m_clock = false;
}

//PRE: word is a word owned by some program in virtual memory. 
//     PC is the current program counter, p is a valid program (not NULL)
//POST: this page contains the word given, the old program and the passed program's jump table
//      have been appropriately updated. m_clock is true, m_accessed and m_loaded have been updated. 
void Page::update( unsigned int word, unsigned long PC, Program* p )
{
  if( m_owner != NULL )
  {
    // remove ourself from our old owner's jump table
    m_owner->m_jump[m_contents] = -1;
  }

  m_contents = word;
  m_owner = p;

  p->m_jump[m_contents] = m_num;

  m_clock = true;
  m_accessed = PC;
  m_loaded = PC;
}

// This page has just been accessed
//PRE: PC is the current program counter
//POST: m_accessed=PC, m_clock is now true
void Page::update( unsigned long PC )
{
  m_accessed = PC;
  m_clock = true;
}
