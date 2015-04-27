/*******************************************
 * Student:    Billy J Rhoades II
 * Username:   bjrq48
 * Class:      CS5201
 * Assignment: 2 
 * File:       memorysimulator.cpp
 ******************************************/
#include "memorysimulator.h"

//PRE: Argv should be filled with all 6 parameters specified in this header file. They should all match the
//     the exception descriptions below.
//POST: readMemory( ) and preparePrograms( ) are loaded. All internal variables match appropriate passed values. 
MemorySimulator::MemorySimulator( int argc, char* argv[] )
{
  if( argc < 6 )
    throw domain_error( "Simulator must have all six arguments specified" );

  m_progList.open( argv[1] );
  if( !m_progList )
  {
    cerr << "Program list file provided does not appear to exist" << endl;
    throw domain_error( "File not found" );
  }

  m_progTrace.open( argv[2] );
  if( !m_progTrace )
  {
    cerr << "Program trace file provided does not appear to exist" << endl;
    throw domain_error( "File not found" );
  }

  m_pageSize = atoi( argv[3] );

  if( m_pageSize <= 0 || m_pageSize > AVAILABLE_FRAMES )
    throw domain_error( "Page size is not within range ( > 0 but < AVAILABLE_FRAMES )" );
    
  if( pow( 2, (int)log2( m_pageSize ) ) != pow( 2, log2( m_pageSize ) ) )
    throw domain_error( "Page size is not a power of two." );

  if( !strcmp( "clock", argv[4] ) || !strcmp("c", argv[4] ) )
    m_rAlgo = ALGO_CLOCK;
  else if( !strcmp("lru", argv[4] ) || !strcmp("l", argv[4] ) )
    m_rAlgo = ALGO_LRU;
  else if( !strcmp("f", argv[4] ) || !strcmp("fifo", argv[4] ) )
    m_rAlgo = ALGO_FIFO;
  else
    throw domain_error( "Unknown page replacement algorithm specified" );

  m_prepage = bool( atoi( argv[5] ) );
  m_frames = AVAILABLE_FRAMES / m_pageSize;

  m_programs = NULL;
  m_numPrograms = 0;
  m_pageFaults = 0;
  m_clockPointer = 0;
  m_PC = 0;
  m_memory = NULL;

  readPrograms( );
  prepareMemory( );
}

//PRE: None
//POST: both file handles are closed, m_programs and m_memory are deleted.
MemorySimulator::~MemorySimulator( )
{
  m_progList.close( );
  m_progTrace.close( );


  for( unsigned int i=0; i<m_numPrograms; i++ )
    delete m_programs[i];
  delete[] m_programs;

  delete[] m_memory;
}

//PRE: constructor has been run and its requirements applied.
//POST: m_programs are initialized with proper programs.
void MemorySimulator::readPrograms( )
{
  string trash;

  // figure out how many programs we're making
  while( getline( m_progList, trash ) ) 
    m_numPrograms++;

  m_programs = new Program*[m_numPrograms];

  m_progList.clear( );
  m_progList.seekg( 0, ios::beg );

  unsigned int pagecnt = 0;

  for( unsigned int i=0; i<m_numPrograms; i++ )
  {
    unsigned int num, numPages;  
    m_progList >> num >> numPages;

    numPages /= m_pageSize;

    m_programs[i] = new Program( num, pagecnt, numPages );

    pagecnt += numPages;
  }
}

//PRE: the constructor has been run with its checks.
//POST: m_memory is initalized with pages of appropriate values.
void MemorySimulator::prepareMemory( )
{
  m_memory = new Page[m_frames];

  for( unsigned int i=0; i<m_frames; i++ )
    m_memory[i].m_num = i;
  
  unsigned int memEach = m_frames / m_numPrograms; 

  for( unsigned int i=0; i<m_numPrograms; i++ )
  {
    unsigned int size = 0;
    if( m_programs[i]->m_numPages > memEach )
      size = memEach;
    else
      size = m_programs[i]->m_numPages;

    for( unsigned int j=0; j<size; j++ )
    {
      unsigned int mainmem = i*memEach + j;
      unsigned int virt = m_programs[i]->m_firstPage + j;

      m_memory[mainmem].update( virt, m_PC, m_programs[i] );
    }
  }
}

//PRE: The constructor and its checks have been run.
//POST: the simulation has been run with m_pageFaults and m_PC set to proper values. all instructions
//      in the program trace file are executed.
void MemorySimulator::run( ) 
{

  cout << "========================" << endl;
  cout << "Page Size: " << m_pageSize << endl;
  cout << "Replacement Algorithm: ";
  switch( m_rAlgo )
  {
    case ALGO_CLOCK:
      cout << "Clock" << endl; 
      break;
    case ALGO_FIFO:
      cout << "FIFO" << endl;
      break;
    case ALGO_LRU:
      cout << "LRU" << endl;
      break;
  }

  cout << "Paging Policy: " << ( m_prepage ? "Prepaging" : "Ondemand" ) << endl;
  // Read in instructions and execute

  while( !m_progTrace.eof( ) )
  {
    unsigned int progNum, word;

    m_PC++;
    m_progTrace >> progNum >> word;

    access( progNum, word );
  }

  cout << "Total Page Faults: " << pageFaults( ) << endl;
  cout << "========================" << endl;

}

//PRE: num is a num in m_programs or domain error is thrown. word belongs to m_program[num]
//POST: page faults are handled if needed, otherwise the page has its values updated.
void MemorySimulator::access( unsigned int num, unsigned int word )
{
  if( num >= m_numPrograms )
  {
    stringstream ss;
    ss << "Program #" << num << " requesting word #" << word << " does not exist";
    throw domain_error( ss.str( ) );
  }
  
  // Convert relative to absolute page number
  word = word / m_pageSize + m_programs[num]->m_firstPage;

  if( word < m_programs[num]->m_firstPage || word > m_programs[num]->m_firstPage + m_programs[num]->m_numPages )
  {
    stringstream ss;
    ss << "Page #" << word << " for program #" << num << " does not exist (only has " << m_programs[num]->m_numPages << " pages)" 
        << " (" << m_programs[num]->m_numPages << "-" << m_programs[num]->m_firstPage+m_programs[num]->m_numPages << ")";
    throw domain_error( ss.str( ) ); 
  }


  // Check if this page is in memory 
  if( m_programs[num]->m_jump[word] == -1 )
  {
    m_pageFaults++;
    handleFault( m_programs[num], word );
  }
  else
    m_memory[m_programs[num]->m_jump[word]].update( m_PC );
}

//PRE: p is a valid program, word is a word of p, prepage is set if first call, otherwise false so we don't infinitely 
//     prepage.
//POST: a new page is loaded into memory and jump tables updated.
void MemorySimulator::handleFault( Program* p, unsigned int word, bool prepage )
{
  unsigned int sel=0;

  switch( m_rAlgo )
  {
    // Rotate through memory and remove infrequently used memory first.
    case ALGO_CLOCK:
    {
      while( true )
      {
        if( m_clockPointer >= m_frames )
          m_clockPointer = 0;  

        if( m_memory[m_clockPointer].m_clock )
          m_memory[m_clockPointer].m_clock = false;
        else
        {
          sel = m_clockPointer;
          break;
        }

        m_clockPointer++;
      }

      m_clockPointer++;
      break; 
    }

    // Swap out the first page with the lowest timestamp
    case ALGO_LRU:
    {
      unsigned int min=m_memory[sel].m_accessed;
      for( unsigned int i=0; i<m_frames; i++ )
      {
        if( min == 0 )
          break;

        if( m_memory[i].m_accessed < min )
        {
          min = m_memory[i].m_accessed;
          sel = i;
        }
      }
      break; 
    }

    // Choose the oldest loaded page and replace it
    case ALGO_FIFO:
    {
      unsigned int min = m_memory[sel].m_loaded;
      for( unsigned int i=0; i<m_frames; i++ )
      {
        if( min == 0 )
          break;

        if( m_memory[i].m_loaded < min )
        {
          min = m_memory[i].m_loaded;
          sel = i;
        }
      }
      break; 
    }
  }

  m_memory[sel].update( word, m_PC, p );

  // Choose a page based on this algo again if prepaging
  // We flip this to false for a second call so we don't prepage forever
  if( m_prepage && prepage )
  {
    if( word == p->m_numPages + p->m_firstPage )
      word = p->m_firstPage;
    else
      word += 1;

    // Is it in memory already?
    if( p->m_jump[word] != -1 )
      return;
    handleFault( p, word, false );
  }
}

//PRE: None
//POST: returns the number of page faults found in the simulation.
unsigned int MemorySimulator::pageFaults( ) const
{
  return m_pageFaults;
}

