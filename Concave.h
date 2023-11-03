//==============================================================================
// Concave.h
//	: header file for concave hulls
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-10-22 11:38:04 shigeo"
//
//==============================================================================

#ifndef _Concave_H		// begining of header file
#define _Concave_H		// notifying that this file is included

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <vector>

using namespace std;

#include "CGAL.h"


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------
vector< unsigned int > concave_hull_2( const vector< Point2 > & points, int k );



#endif // _Concave_H

// end of header file
// Do not add any stuff under this line.
