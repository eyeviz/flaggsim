//==============================================================================
// Sketch.h
//	: header file for line drawings
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-11-24 03:51:02 shigeo"
//
//==============================================================================

#ifndef _Sketch_H		// begining of header file
#define _Sketch_H		// notifying that this file is included

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <vector>
#include <cassert>

using namespace std;

#include "CGAL.h"
#include "Set.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------

// for saving the drawing
#define NORMALIZE_BY_FIXED_SCALE	// (180m x 2 = 360m)

#ifdef NORMALIZE_BY_FIXED_SCALE
// (Average = 74.3) + (STDEV = 45.9) = 120.2
#define BLOCK_SIZE_AVERAGE	(74.3)
#define BLOCK_SIZE_STDEV	(45.9)
#define BLOCK_SIZE_UPPER_LIMIT	(BLOCK_SIZE_AVERAGE+BLOCK_SIZE_STDEV)
#define BLOCK_SIZE_LOWER_LIMIT	(BLOCK_SIZE_AVERAGE-BLOCK_SIZE_STDEV)
//#define BLOCK_SIZE_BASE		(0.5*BLOCK_SIZE_UPPER_LIMIT)
//#define BLOCK_SIZE_BASE		(60.0)
#define BLOCK_SIZE_BASE		(75.0)
#endif	// NORMALIZE_BY_FIXED_SCALE

#define BORDER_RATIO            (0.2)


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class Sketch {

  private:

  protected:

    vector< Polygon2 >		_poly;		// polygons
    Point2			_center;	// center of the line drawing
    
    void	_calcCenter	( void );
    void	_save		( const char * filename );
    
public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    Sketch();			// default constructor
    Sketch( const Sketch & obj );
				// copy constructor

//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
    ~Sketch();			// destructor

//------------------------------------------------------------------------------
//	Referrring to members
//------------------------------------------------------------------------------
    const vector< Polygon2 > & poly( void ) const { return _poly; }
    vector< Polygon2 > & poly( void )		{ return _poly; }
    const Point2 & center( void ) const		{ return _center; }
    
//------------------------------------------------------------------------------
//	File I/O
//------------------------------------------------------------------------------
    void save( const char * filename ) {
	_save( filename );
    }

//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------
    Sketch & operator = ( const Sketch & obj );
				// assignment

//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
    friend ostream & operator << ( ostream & stream, const Sketch & obj );
				// output
    friend istream & operator >> ( istream & stream, Sketch & obj );
				// input

//------------------------------------------------------------------------------
//	Class name
//------------------------------------------------------------------------------
    virtual const char * className( void ) const { return "Sketch"; }
				// class name
};



#endif // _Sketch_H

// end of header file
// Do not add any stuff under this line.
