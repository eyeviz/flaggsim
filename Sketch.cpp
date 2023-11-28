//==============================================================================
// Sketch.cpp
//	: program file for line drawings
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-10-11 21:48:30 shigeo"
//
//==============================================================================

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

using namespace std;

#include "Common.h"
#include "Sketch.h"


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Type definitions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Private Functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------

//
//  Sketch::_calcCenter --	calculate the center of the line drawing
//				as the center of the bounding box
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Sketch::_calcCenter( void )
{
    Bbox2	bbox;

    for ( unsigned int i = 0; i < _poly.size(); ++i ) {
	Bbox2 curbox = _poly[ i ].bbox();
	bbox += curbox;
    }
    _center = Point2( 0.5*( bbox.xmin()+bbox.xmax() ),
		      0.5*( bbox.ymin()+bbox.ymax() ) );
}


//------------------------------------------------------------------------------
//	File I/O
//------------------------------------------------------------------------------

//
//  Sketch::_save --	save the list of polygons with normalized coordinates
//
//  Inputs
//	filename	: file name
//
//  Outputs
//	none
//
void Sketch::_save( const char * filename )
{
    // Identify the bouding box of all the polygons
    Bbox2 domain;
    for ( unsigned int i = 0; i < _poly.size(); ++i ) {
	domain += _poly[ i ].bbox();
    }

    double aveX = 0.5 * ( domain.xmin() + domain.xmax() );
    double aveY = 0.5 * ( domain.ymin() + domain.ymax() );
    // cerr << HERE << " domain = " << domain << endl;

    // #ifndef NORMALIZE_BY_FIXED_SCALE
    double sizeX = domain.xmax() - domain.xmin();
    double sizeY = domain.ymax() - domain.ymin();
    double sizeB = max( sizeX, sizeY );
    // #endif	// NORMALIZE_BY_FIXED_SCALE
    
#ifdef SKIP
    cout << HERE << filename << " : " << " block size = "
	 << setw( 8 ) << setprecision( 2 ) << std::right << std::fixed 
	 << sizeB << endl;
#endif	// SKIP

#ifdef NORMALIZE_BY_FIXED_SCALE
    // If the building block overflow the image range, skip this
    if ( ( sizeB < BLOCK_SIZE_LOWER_LIMIT ) ||
	 ( sizeB > BLOCK_SIZE_UPPER_LIMIT ) ) return;
#endif	// NORMALIZE_BY_FIXED_SCALE
    if ( _poly.size() <= 1 ) return;
    
    ofstream ofs( filename );

    if ( ! ofs ) {
        cerr << HERE << " cannot open the file " << filename << endl;
        return;
    }

    ofs << _poly.size() << endl;
    for ( unsigned int i = 0; i < _poly.size(); ++i ) {
	ofs << _poly[ i ].size() << endl;
	for ( unsigned int j = 0; j < _poly[ i ].size(); ++j ) {
	    double x = _poly[ i ][ j ].x() - aveX;
	    double y = _poly[ i ][ j ].y() - aveY;
#ifdef NORMALIZE_BY_FIXED_SCALE
	    x = x / BLOCK_SIZE_BASE;
	    y = y / BLOCK_SIZE_BASE;
#else	// NORMALIZE_BY_FIXED_SCALE
	    x = 2.0 * x / sizeB;
	    y = 2.0 * y / sizeB;
	    double s = ( 1.0 - BORDER_RATIO );
	    x *= s;
	    y *= s;
#endif	// NORMALIZE_BY_FIXED_SCALE
	    ofs << fixed << setprecision( 4 ) << x;
	    ofs << "\t";
	    ofs << fixed << setprecision( 4 ) << y;
	    ofs << endl;
	}
    }
    ofs.close();

    cerr << HERE << " Finished saving the data!" << endl;  
}




//------------------------------------------------------------------------------
//	Public Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------

//
//  Sketch::Sketch --	default constructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Sketch::Sketch() 
{
    _poly.clear();
}


//
//  Sketch::Sketch --	copy constructor
//
//  Inputs
//	obj	: object of this class
//
//  Outputs
//	none
//
Sketch::Sketch( const Sketch & obj )
{
    _poly	= obj._poly;
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------

//
//  Sketch::~Sketch --	destructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Sketch::~Sketch()
{
    _poly.clear();
}


//------------------------------------------------------------------------------
//	Referrring to members
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------

//
//  Sketch::operator = --	assignement
//
//  Inputs
//	obj	: objects of this class
//
//  Outputs
//	this object
//
Sketch & Sketch::operator = ( const Sketch & obj )
{
    if ( this != &obj ) {
	_poly	= obj._poly;
    }
    return *this;
}


//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------

//
//  operator << --	output
//
//  Argument
//	stream	: output stream
//	obj	: object of this class
//
//  Outputs
//	output stream
//
ostream & operator << ( ostream & stream, const Sketch & obj )
{
    stream << obj._poly.size() << endl;
    for ( unsigned int i = 0; i < obj._poly.size(); ++i ) {
	stream << obj._poly[ i ].size() << endl;
	for ( unsigned int j = 0; j < obj._poly[ i ].size(); ++j ) {
	    stream << obj._poly[ i ][ j ] << endl;
	}
    }
    return stream;
}


//
//  operator >> --	input
//
//  Inputs
//	stream	: input stream
//	obj	: object of this class
//
//  Outputs
//	input stream
//
istream & operator >> ( istream & stream, Sketch & obj )
{
    unsigned int nPolys;
    obj._poly.clear();

    stream >> nPolys;
    for ( unsigned int i = 0; i < nPolys; ++i ) {
	unsigned int nCnrs;
	stream >> nCnrs;
	Polygon2 curPoly;
	for ( unsigned int j = 0; j < nCnrs; ++j ) {
	    Point2 cnr;
	    stream >> cnr;
	    curPoly.push_back( cnr );
	}
	obj._poly.push_back( curPoly );
    }

    return stream;
}


// end of header file
// Do not add any stuff under this line.
