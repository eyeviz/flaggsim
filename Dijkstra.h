//==============================================================================
// Dijkstra.h
//	: header file for dijkstra shortest path search
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-12-23 04:12:24 shigeo"
//
//==============================================================================

#ifndef _Dijkstra_H		// begining of header file
#define _Dijkstra_H		// notifying that this file is included

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <stack>

using namespace std;

#include "Directed.h"
#include "Set.h"


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Functions
//------------------------------------------------------------------------------
Set shortestPath( unsigned int idS, unsigned int idT, Directed & net );


#endif // _Dijkstra_H

// end of header file
// Do not add any stuff under this line.
