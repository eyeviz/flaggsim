//==============================================================================
// Set.cpp
//	: program file for a set of integer labels
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2022-04-25 03:06:13 shigeo"
//
//==============================================================================

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include "Common.h"
#include "Set.h"


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
//  Set::Set --	default constructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Set::Set() 
      : vector< unsigned int >()  	
{

}


//
//  Set::Set --	copy constructor
//
//  Inputs
//	obj	: object of this class
//
//  Outputs
//	none
//
Set::Set( const Set & obj )
      : vector< unsigned int >( ( const vector< unsigned int > & ) obj )
{

}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------

//
//  Set::~Set --	destructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Set::~Set()
{

}


//------------------------------------------------------------------------------
//	Referrring to members
//------------------------------------------------------------------------------

//
//  Set::isContained --	check if the input element is contained in the set
//
//  Inputs
//	cf	: element to be searched for
//
//  Outputs
//	boolean value according to the existence of the input element
//
bool Set::isContained( const unsigned int & cf ) const
{
    Set::const_iterator it = find( this->begin(), this->end(), cf );
    if ( it != this->end() )
	return true;
    else
	return false;
}

//------------------------------------------------------------------------------
//	Functions for sorting
//------------------------------------------------------------------------------

//
//  compare --	comparison function for sorting
//
//  Inputs
//	a, b	: pointers to objects of this class
//
//  Outputs
//	a <  b	-1
//	a == b	 0
//	a >  b	 1
//
int compare ( const Set * a, const Set * b )
{
    unsigned int lower = std::min( a->size(), b->size() );
    for ( unsigned int k = 0; k < lower; ++k ) {
	if ( a->at( k ) < b->at( k ) ) return (-1);
	else if ( a->at( k ) < b->at( k ) ) return (1);
    }

    if ( a->size() < b->size() ) return (-1);
    else if ( a->size() > b->size() ) return 1;
    else return 0;
}


//------------------------------------------------------------------------------
//	equality
//------------------------------------------------------------------------------

//
//  operator == --	equality
//
//  Inputs
//	a, b	: objects of this class
//
//  Outputs
//	boolean value
//
int operator == ( const Set & a, const Set & b )
{
    if ( a.size() != b.size() ) return FALSE;
    unsigned int limit = a.size();
    for ( unsigned int k = 0; k < limit; ++k ) {
	if ( a[ k ] != b[ k ] ) return FALSE;
    }
    return TRUE;
}


//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------

//
//  Set::operator = --	assignement
//
//  Inputs
//	obj	: objects of this class
//
//  Outputs
//	this object
//
Set & Set::operator = ( const Set & obj )
{
    if ( this != &obj ) {
	( vector< unsigned int > & )*this = obj;
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
ostream & operator << ( ostream & stream, const Set & obj )
{
    stream << "[" << obj.size() << "]\t" << endl;
    for ( unsigned int k = 0; k < obj.size(); ++k )
	stream << obj[ k ] << "\t";
    stream << endl;
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
istream & operator >> ( istream & stream, Set & obj )
{
    unsigned int num;
    stream >> num;
    obj.clear();
    for ( unsigned int k = 0; k < num; ++k ) {
	unsigned int v;
	stream >> v;
	obj.push_back( v );
    }
    return stream;
}


// end of header file
// Do not add any stuff under this line.
