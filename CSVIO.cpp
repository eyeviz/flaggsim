//==============================================================================
// CSVIO.cpp
//	: program file for csv file I/O
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-02-21 02:17:50 shigeo"
//
//==============================================================================

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <regex>
#include <cmath>

using namespace std;

#include "CSVIO.h"


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

namespace funcs {
    bool is_int(std::string s) {
	std::smatch m;
	std::regex re("^\\s*[+-]?\\d+$");
	
	try {
	    if (!std::regex_search(s, m, re))
		return false;
	} catch (std::regex_error& e) {
	    return false;
	}
	
	return true;
    }

    bool is_real( std::string s ) {
	std::smatch m;
	std::regex re("^\\s*[+-]?(\\d+\\.\\d*|\\.\\d+)$");
	try {
	    if ( ! std::regex_search( s, m, re ) ) {
		return false;
	    }
	}
	catch ( std::regex_error & e ) {
	    std::cerr << "[EXCEPTION] Regex error! (" << e.what() << ")" << std::endl;
	    return false;
	}

	return true;
    }
}

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------

//
//  CSVIO::CSVIO --	default constructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
CSVIO::CSVIO() 
{

}


//
//  CSVIO::CSVIO --	copy constructor
//
//  Inputs
//	obj	: object of this class
//
//  Outputs
//	none
//
CSVIO::CSVIO( const CSVIO & obj )
{

}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------

//
//  CSVIO::~CSVIO --	destructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
CSVIO::~CSVIO()
{

}


//------------------------------------------------------------------------------
//	Fundamental member functions
//------------------------------------------------------------------------------
std::vector< string > CSVIO::split( string & input, char delimiter )
{
    istringstream istr( input );
    string field;
    std::vector< string > result;
    while ( getline( istr, field, delimiter ) ) {
        result.push_back( field );
    }
    return result;
}

void CSVIO::csvline( string & line, std::vector< double > & seq )
{
    std::vector< string > strvec = split( line, ',' );

    seq.clear();
    for ( unsigned int k = 0; k < strvec.size(); ++k ) {
	if ( funcs::is_int( strvec[ k ] ) || funcs::is_real( strvec[ k ] )  ) {
	    double value;
	    istringstream istr;
	    istr.str( strvec[ k ] );
	    istr >> value;
	    seq.push_back( value );
	}
	else {
	    seq.push_back( std::nan("") );
	}
    }

    return;
}


void CSVIO::sheet( const char * filename, std::vector< std::vector< double > > & table )
{
    table.clear();

    ifstream ifs( filename );
    if ( ! ifs.is_open() ) {
	cerr << "Cannot open the file: " << filename << endl;
	return;
    }
    
    string line;
    while ( getline( ifs, line ) ) {
	std::vector< double > seq;
	csvline( line, seq );
	table.push_back( seq );
    }

#ifdef DEBUG
    for ( unsigned int i = 0; i < table.size(); ++i ) {
	cerr << "[ " << setw( 3 ) << setfill( '0' ) << i << " ] : " << ends;
	for ( unsigned int j = 0; j < table[ i ].size(); ++j ) {
	    cerr << setw( 4 ) << setfill( ' ' ) << table[ i ][ j ] << ends;
	}
	cerr << endl;
    } 
#endif	// DEBUG

    return;
}




//------------------------------------------------------------------------------
//	Functions for sorting
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------

#ifdef SKIP
//
//  CSVIO::operator = --	assignement
//
//  Inputs
//	obj	: objects of this class
//
//  Outputs
//	this object
//
CSVIO & CSVIO::operator = ( const CSVIO & obj )
{
    if ( this != &obj ) {
	( %%% & )*this = obj;
    }
    return *this;
}
#endif	// SKIP


//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------

#ifdef SKIP
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
ostream & operator << ( ostream & stream, const CSVIO & obj )
{
    stream << ( const %%% & )obj;	
    return stream;
}
#endif	// SKIP


#ifdef SKIP
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
istream & operator >> ( istream & stream, CSVIO & obj )
{
    stream >> ( %%% & )obj;
    return stream;
}
#endif	// SKIP


// end of program file
// Do not add any stuff under this line.
