//==============================================================================
// Set.h
//	: header file for a set of integer labels
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2022-04-25 01:40:13 shigeo"
//
//==============================================================================

#ifndef _Set_H		// begining of header file
#define _Set_H		// notifying that this file is included

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <iostream>
#include <vector>

using namespace std;


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class Set : public vector< unsigned int > {

  private:

  protected:

  public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    Set();			// default constructor
    Set( const Set & obj );
				// copy constructor

//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
    virtual ~Set();		// destructor

//------------------------------------------------------------------------------
//	Referrring to members
//------------------------------------------------------------------------------
    bool isContained( const unsigned int & cf ) const;
    

//------------------------------------------------------------------------------
//	Functions for sorting
//------------------------------------------------------------------------------
    friend int compare ( const Set * a, const Set * b );
				// comparison
#ifdef SKIP
    friend int compare ( const Set ** a, const Set ** b );
#endif	// SKIP

//------------------------------------------------------------------------------
//	equality
//------------------------------------------------------------------------------
    friend int operator == ( const Set & a, const Set & b );
    friend int operator != ( const Set & a, const Set & b ) {
	return ( ! ( a == b ) );
    }				// equality

//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------
    Set & operator = ( const Set & obj );
				// assignment

//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
    friend ostream & operator << ( ostream & stream, const Set & obj );
				// output
    friend istream & operator >> ( istream & stream, Set & obj );
				// input

//------------------------------------------------------------------------------
//	Class name
//------------------------------------------------------------------------------
    virtual const char * className( void ) const { return "Set"; }
				// class name
};

#ifdef SKIP
inline int arrange( const Set * a, const Set * b ) {
    return compare( a, b );
}
#endif	// SKIP


#endif // _Set_H

// end of header file
// Do not add any stuff under this line.
