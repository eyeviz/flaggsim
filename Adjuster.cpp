//==============================================================================
// Adjuster.cpp
//	: program file for maintaining auxiliary variables
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-08-08 14:08:02 shigeo"
//
//==============================================================================

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include "Drawing.h"
#include "Adjuster.h"


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Private Functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------
// Function for clearing the data
void Adjuster::_clear( void )
{
    _minInterval	=   1.0;
    _maxInterval	=  20.0;
    _intervalScale	= 100.0;
    _interval		= _intervalScale * Drawing::interval_threshold;

    _dataCostGap	=   1.0;
    _lowerDataCost	= Drawing::data_cost_lower;
    _upperDataCost	= Drawing::data_cost_upper;
    _minDataCost	=   2.0;
    _maxDataCost	=  16.0;

    _lowerSmoothCost	= Drawing::smooth_cost_lower;
    _upperSmoothCost	= Drawing::smooth_cost_upper;
    _minSmoothCost	=   1.0;
    _maxSmoothCost	=  10.0;

    _labelCostGap	=   1.0;
    _lowerLabelCost	= Drawing::label_cost_lower;
    _upperLabelCost	= Drawing::label_cost_upper;
    _minLabelCost	=   2.0;
    _maxLabelCost	=  16.0;

    _minCutThreshold	=   5.0;
    _maxCutThreshold	=  50.0;
    _cutThreshold	=  20.0;
}    



//------------------------------------------------------------------------------
//	Public Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------

//
//  Adjuster::Adjuster --	default constructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Adjuster::Adjuster() 
{
    _clear();
}


//
//  Adjuster::Adjuster --	copy constructor
//
//  Inputs
//	obj	: object of this class
//
//  Outputs
//	none
//
Adjuster::Adjuster( const Adjuster & obj )
{
    _interval		= obj._interval;
    _minInterval	= obj._minInterval;
    _maxInterval	= obj._maxInterval;
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------

//
//  Adjuster::~Adjuster --	destructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Adjuster::~Adjuster()
{
    _clear();
}


//------------------------------------------------------------------------------
//	Referrring to members
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------

//
//  Adjuster::operator = --	assignement
//
//  Inputs
//	obj	: objects of this class
//
//  Outputs
//	this object
//
Adjuster & Adjuster::operator = ( const Adjuster & obj )
{
    if ( this != &obj ) {
	_interval	= obj._interval;
	_minInterval	= obj._minInterval;
	_maxInterval	= obj._maxInterval;
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
ostream & operator << ( ostream & stream, const Adjuster & obj )
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
istream & operator >> ( istream & stream, Adjuster & obj )
{
    return stream;
}


// end of header file
// Do not add any stuff under this line.
