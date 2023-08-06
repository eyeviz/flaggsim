//==============================================================================
// CSVIO.h
//	: header file for csv file I/O
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2022-04-25 16:28:49 shigeo"
//
//==============================================================================

#ifndef _CSVIO_H		// begining of header file
#define _CSVIO_H		// notifying that this file is included

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------
#include <string>
#include <sstream>
#include <vector>
using namespace std;

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class CSVIO {

  private:

  protected:

  public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    CSVIO();			// default constructor
    CSVIO( const CSVIO & obj );
				// copy constructor

//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
    ~CSVIO();			// destructor

//------------------------------------------------------------------------------
//	Fundamental member functions
//------------------------------------------------------------------------------
    static std::vector< string > split( string & input, char delimiter = ',' );
    
    static void csvline( string & line, std::vector< double > & seq );
    static void sheet( const char * filename, std::vector< std::vector< double > > & data );

//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------
#ifdef SKIP
    CSVIO & operator = ( const CSVIO & obj );
				// assignment
#endif	// SKIP

//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
#ifdef SKIP
    friend ostream & operator << ( ostream & stream, const CSVIO & obj );
				// output
    friend istream & operator >> ( istream & stream, CSVIO & obj );
				// input
#endif	// SKIP

//------------------------------------------------------------------------------
//	Class name
//------------------------------------------------------------------------------
    virtual const char * className( void ) const { return "CSVIO"; }
				// class name
};

#endif // _CSVIO_H

// end of header file
// Do not add any stuff under this line.
