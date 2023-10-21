//==============================================================================
// Outline.cpp
//	: program file for simplified outlines over the polygon elements
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-10-16 13:47:10 shigeo"
//
//==============================================================================

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include "Outline.h"


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Private Functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Public Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------

//
//  Outline::Outline --	default constructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Outline::Outline() 
{
    _shape.clear();
}


//
//  Outline::Outline --	copy constructor
//
//  Inputs
//	obj	: object of this class
//
//  Outputs
//	none
//
Outline::Outline( const Outline & obj )
{
    _shape	= obj._shape;
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------

//
//  Outline::~Outline --	destructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Outline::~Outline()
{
    _shape.clear();
}


//------------------------------------------------------------------------------
//	Referrring to members
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Functions for sorting
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	equality
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------

//
//  Outline::operator = --	assignement
//
//  Inputs
//	obj	: objects of this class
//
//  Outputs
//	this object
//
Outline & Outline::operator = ( const Outline & obj )
{
    if ( this != &obj ) {
	_shape	= obj._shape;
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
ostream & operator << ( ostream & stream, const Outline & obj )
{
    stream << obj._shape.size() << endl;
    for ( unsigned int i = 0; i < obj._shape.size(); ++i ) {
	stream << obj._shape[ i ].size() << endl;
	for ( unsigned int j = 0; j < obj._shape[ i ].size(); ++j ) {
	    stream << obj._shape[ i ][ j ] << endl;
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
istream & operator >> ( istream & stream, Outline & obj )
{
    unsigned int num;
    stream >> num;
    obj._shape.clear();
    obj._shape.resize( num );
    for ( unsigned int i = 0; i < obj._shape.size(); ++i ) {
	unsigned int nV;
	stream >> nV;
	obj._shape[ i ].clear();
	for ( unsigned int j = 0; j < nV; ++j ) {
	    Point2 p;
	    stream >> p;
	    obj._shape[ i ].push_back( p );
	}
    }
    return stream;
}

// end of header file
// Do not add any stuff under this line.
