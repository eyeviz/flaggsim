//==============================================================================
// Expansion.cpp
//	: program file for expansion over the polygon elements
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-07-03 23:04:03 shigeo"
//
//==============================================================================

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <iostream>
#include <iomanip>

using namespace std;

#include "Common.h"
#include "Expansion.h"


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Variables
//------------------------------------------------------------------------------
unsigned int Expansion::nLabels = 0;

//------------------------------------------------------------------------------
//	Private Functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------

//
//  Expansion::cluster --	return the array of clusters each of which
//				contains the list of polygon IDs.
//
//  Inputs
//	none
//
//  Outputs
//	array of clsuters
//
vector< Set > Expansion::_cluster( void )
{
    // collect polygon IDs for each label
    vector< Set > team( Expansion::nLabels );
    for ( unsigned int k = 0; k < Expansion::nLabels; ++k )
	team[ k ].clear();

    // cerr << HERE << " Grouping by labels" << endl;
    for ( unsigned int i = 0; i < _group.size(); ++i ) {
	unsigned int labelID = _group[ i ];
	// cerr << " " << labelID << ends;
	team[ labelID ].push_back( i );
    }
    // cerr << endl;

    // delete the empty labels
    unsigned int count = 0;
    while ( count < Expansion::nLabels ) {
	unsigned int index = Expansion::nLabels - 1 - count;
	if ( team[ index ].size() == 0 ) {
#ifdef DEBUG
	    cerr << HERE << " Deleting " << index << "-th team label" << endl;
#endif	// DEBUG
	    team.erase( team.begin() + index );
	}
	count++;
#ifdef DEBUG
	cerr << HERE << " Incrementing the count to " << count << endl;
#endif	// DEBUG
    }
    // cerr << HERE << " Number of valid labels = " << team.size() << endl;

    return team;
}



//------------------------------------------------------------------------------
//	Public Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------

//
//  Expansion::Expansion --	default constructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Expansion::Expansion() 
{
    _totalCost		= 0;
    _dataCost		= 0;
    _smoothCost		= 0;
    _labelCost		= 0;
    _group.clear();
}


//
//  Expansion::Expansion --	copy constructor
//
//  Inputs
//	obj	: object of this class
//
//  Outputs
//	none
//
Expansion::Expansion( const Expansion & obj )
{
    _totalCost		= obj._totalCost;
    _dataCost		= obj._dataCost;
    _smoothCost		= obj._smoothCost;
    _labelCost		= obj._labelCost;
    _group		= obj._group;
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------

//
//  Expansion::~Expansion --	destructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Expansion::~Expansion()
{
    _totalCost		= 0;
    _dataCost		= 0;
    _smoothCost		= 0;
    _labelCost		= 0;
    _group.clear();
}


//------------------------------------------------------------------------------
//	Referrring to members
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Functions for sorting
//------------------------------------------------------------------------------

#ifdef SKIP
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
int compare( const Expansion * a, const Expansion * b )
{
    if ( a->_totalCost < b->_totalCost ) {
	return (-1);
    }
    else {
	if ( a->_totalCost > b->_totalCost ) {
	    return (1);
	}
	else {
	    return 0;
	}
    }
}
#endif	// SKIP


//------------------------------------------------------------------------------
//	equality
//------------------------------------------------------------------------------

#ifdef SKIP
//
//  operator == --	equality
//
//  Inputs
//	a, b	: objects of this class
//
//  Outputs
//	boolean value
//
int operator == ( const Expansion & a, const Expansion & b )
{
    return ( a._totalCost == b._totalCost );
}
#endif	// SKIP


//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------

//
//  Expansion::operator = --	assignement
//
//  Inputs
//	obj	: objects of this class
//
//  Outputs
//	this object
//
Expansion & Expansion::operator = ( const Expansion & obj )
{
    if ( this != &obj ) {
	_totalCost		= obj._totalCost;
	_dataCost		= obj._dataCost;
	_smoothCost		= obj._smoothCost;
	_labelCost		= obj._labelCost;
	_group		= obj._group;
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
ostream & operator << ( ostream & stream, const Expansion & obj )
{
    stream << obj._totalCost << " ( "
	   << obj._dataCost << "+" << obj._smoothCost << "+" << obj._labelCost << " ) : "
	   << endl;
    for ( unsigned int i = 0; i < obj._group.size(); ++i )
	stream << setw( 4 ) << obj._group[ i ];
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
istream & operator >> ( istream & stream, Expansion & obj )
{
    unsigned int num, each;
    stream >> obj._totalCost 
	   >> obj._dataCost >> obj._smoothCost >> obj._labelCost;
    stream >> num;
    obj._group.clear();
    for ( unsigned int i = 0; i < num; ++i ) {
	stream >> each;
	obj._group.push_back( each );
    }
    return stream;
}


// end of header file
// Do not add any stuff under this line.
