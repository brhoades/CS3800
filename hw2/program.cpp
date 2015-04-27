/*******************************************
 * Student:    Billy J Rhoades II
 * Username:   bjrq48
 * Class:      CS5201
 * Assignment: 2 
 * File:       program.cpp
 ******************************************/
#include "program.h"

//PRE: None
//POST: This object is constructed with blank members.
Program::Program( ) { }

//PRE: num is within m_program's bounds.
//POST: This program is constructed with m_num = num, m_numPages = numPages, and m_firstPage = pageNum.
//      m_jump is initialized to contain pages from [pageNum,pageNum+numPages) pointing to -1.
Program::Program( const unsigned int num, const unsigned pageNum, const unsigned int numPages )
{
  m_num = num;
  m_numPages = numPages;
  m_firstPage = pageNum;

  for( unsigned int i=pageNum; i<pageNum+numPages; i++ )
    m_jump[i] = -1;
}

//PRE: None
//POST: This object is a mirror of rhs.
Program& Program::operator=( const Program& rhs )
{
  m_firstPage = rhs.m_firstPage;
  m_numPages = rhs.m_numPages;
  m_num = rhs.m_num; 

  m_jump = rhs.m_jump;

  return *this;
}

//PRE: None
//POST: This object is deallocated.
Program::~Program( )
{
}
