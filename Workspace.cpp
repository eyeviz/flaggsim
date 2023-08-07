//==============================================================================
// Workspace.cpp
//	: program file for maintaining auxiliary variables
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-08-07 17:17:55 shigeo"
//
//==============================================================================

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include "Workspace.h"


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
//  Workspace::Workspace --	default constructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Workspace::Workspace() 
{
    _coverHull.clear();
    _coverBand.clear();
    _coverGlob.clear();
    // _dendrogram
    _cluster.clear();

    _pickID		= NO_NAME;
    _pickCoord.clear();
    _isPressed		= false;
    _mode		= FREE;
}


//
//  Workspace::Workspace --	copy constructor
//
//  Inputs
//	obj	: object of this class
//
//  Outputs
//	none
//
Workspace::Workspace( const Workspace & obj )
{
    _coverHull		= obj._coverHull;
    _coverBand		= obj._coverBand;
    _coverGlob		= obj._coverGlob;
    _dendrogram		= obj._dendrogram;
    _cluster		= obj._cluster;

    _pickID		= obj._pickID;
    _pickCoord		= obj._pickCoord;
    _isPressed		= obj._isPressed;
    _mode		= obj._mode;
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------

//
//  Workspace::~Workspace --	destructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Workspace::~Workspace()
{

}


//------------------------------------------------------------------------------
//	Referrring to members
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------

//
//  Workspace::operator = --	assignement
//
//  Inputs
//	obj	: objects of this class
//
//  Outputs
//	this object
//
Workspace & Workspace::operator = ( const Workspace & obj )
{
    if ( this != &obj ) {
	_coverHull		= obj._coverHull;
	_coverBand		= obj._coverBand;
	_coverGlob		= obj._coverGlob;
	_dendrogram		= obj._dendrogram;
	_cluster		= obj._cluster;

        _pickID			= obj._pickID;
	_pickCoord		= obj._pickCoord;
	_isPressed		= obj._isPressed;
	_mode			= obj._mode;
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
ostream & operator << ( ostream & stream, const Workspace & obj )
{
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
istream & operator >> ( istream & stream, Workspace & obj )
{
    return stream;
}


// end of header file
// Do not add any stuff under this line.
