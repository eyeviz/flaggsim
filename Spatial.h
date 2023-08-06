//==============================================================================
// Spatial.h
//	: header file for geometric (spatial) neighbor graphs
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-07-24 23:08:00 shigeo"
//
//==============================================================================

#ifndef _Spatial_H		// begining of header file
#define _Spatial_H		// notifying that this file is included

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <iostream>
#include <cstdlib>
#include <ctime>

#include "ngl.h"
//#include "io-utils.h"

#include "Common.h"

typedef enum {
    RNG, Gabriel, BSkeleton
} NeighborType;

using std::string;
using std::vector;
using std::cerr;
using std::ends;
using std::endl;


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Function Declarations
//------------------------------------------------------------------------------

void geometricNeighbors( const int & dims,
			 const vector< vector< double > > & sample,
			 const NeighborType method,
			 const double beta,
			 vector< Link > & link );


#endif // _Spatial_H

// end of header file
// Do not add any stuff under this line.
