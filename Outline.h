//==============================================================================
// Outline.h
//	: header file for simplified outlines over the polygon elements
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-10-16 13:46:58 shigeo"
//
//==============================================================================

#ifndef _Outline_H		// begining of header file
#define _Outline_H		// notifying that this file is included

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include "CGAL.h"


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class Outline {

  private:

  protected:

    vector< Polygon2 >		_shape;
    
  public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    Outline();			// default constructor
    Outline( const Outline & obj );
				// copy constructor

//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
    ~Outline();			// destructor

//------------------------------------------------------------------------------
//	Referrring to members
//------------------------------------------------------------------------------
    const vector< Polygon2 > &	shape( void ) const	{ return _shape; }
    vector< Polygon2 > &	shape( void )		{ return _shape; }

//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------
    Outline & operator = ( const Outline & obj );
				// assignment

//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
    friend ostream & operator << ( ostream & stream, const Outline & obj );
				// output
    friend istream & operator >> ( istream & stream, Outline & obj );
				// input

//------------------------------------------------------------------------------
//	Class name
//------------------------------------------------------------------------------
    virtual const char * className( void ) const { return "Outline"; }
				// class name
};


//------------------------------------------------------------------------------
//	Private Functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Public Functions
//------------------------------------------------------------------------------


#endif // _Outline_H

// end of header file
// Do not add any stuff under this line.
