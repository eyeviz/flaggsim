//==============================================================================
// Votes.cpp
//	: program file for angle votes
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-10-16 15:18:46 shigeo"
//
//==============================================================================

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include "Votes.h"


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
//  Votes::Votes --	default constructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Votes::Votes() 
      : vector< double >()  	
{

}


//
//  Votes::Votes --	copy constructor
//
//  Inputs
//	obj	: object of this class
//
//  Outputs
//	none
//
Votes::Votes( const vector< double > & obj )
      : vector< double >()
{
    clear();
    for ( unsigned int i = 0; i < obj.size(); ++i )
	this->push_back( obj[ i ] );
}


//
//  Votes::Votes --	copy constructor
//
//  Inputs
//	obj	: object of this class
//
//  Outputs
//	none
//
Votes::Votes( const Votes & obj )
      : vector< double >( ( const vector< double > & ) obj )
{

}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------

//
//  Votes::~Votes --	destructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Votes::~Votes()
{

}


//------------------------------------------------------------------------------
//	Referrring to members
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Approximate equality
//------------------------------------------------------------------------------

//
//  approxequal --	approximate equality
//
//  Inputs
//	a, b	: objects of this class
//
//  Outputs
//	boolean value
//
bool Votes::approxequal( const Votes & obj, const double & tolerance )
{
    if ( this->size() != obj.size() ) return false;
    for ( unsigned int i = 0; i < this->size(); ++i ) {
	if ( abs( this->at( i ) - obj[ i ] ) >= tolerance ) return false;
    }
    return true;
}


//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------

//
//  Votes::operator = --	assignement
//
//  Inputs
//	obj	: objects of this class
//
//  Outputs
//	this object
//
Votes & Votes::operator = ( const Votes & obj )
{
    if ( this != &obj ) {
	( vector< double > & )*this = obj;
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
ostream & operator << ( ostream & stream, const Votes & obj )
{
    stream << obj.size() << endl;
    for ( unsigned int i = 0; i < obj.size(); ++i ) {
	cerr << obj[ i ] << " ";
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
istream & operator >> ( istream & stream, Votes & obj )
{
    unsigned int num;
    stream >> num;
    obj.clear();
    double val;
    for ( unsigned int i = 0; i < num; ++i ) {
	stream >> val;
	obj.push_back( val );
    }
    return stream;
}


// end of header file
// Do not add any stuff under this line.
