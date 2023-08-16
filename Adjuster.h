//==============================================================================
// Adjuster.h
//	: header file for a set of parameters to be adjusted
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-08-08 14:12:22 shigeo"
//
//==============================================================================

#ifndef _Adjuster_H		// begining of header file
#define _Adjuster_H		// notifying that this file is included

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------
#include <vector>

using namespace std;

#include "Common.h"


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------
class Adjuster {

  private:

  protected:
     
    double	_interval;	// 
    double	_intervalScale;	//	100.0;
    double	_minInterval;	//       1.0;
    double	_maxInterval;	//	20.0;
    // ------------------------------
    double	_dataCostGap;	//	 1.0;
    double	_lowerDataCost;
    double	_upperDataCost;
    double	_minDataCost;	//	 2.0;
    double	_maxDataCost;	//	16.0;
    // ------------------------------
    double	_lowerSmoothCost;
    double	_upperSmoothCost;
    double	_minSmoothCost;	//	 1.0;
    double	_maxSmoothCost;	// 	10.0;
    // ------------------------------
    double	_labelCostGap;	//	 1.0;
    double	_lowerLabelCost;
    double	_upperLabelCost;
    double	_minLabelCost;	//	 2.0;
    double	_maxLabelCost;	//	16.0;
    // ------------------------------
    double	_cutThreshold;	//	20.0;
    double	_minCutThreshold; //	 5.0;
    double	_maxCutThreshold; //	50.0;

    void	_clear( void );

public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    Adjuster();			// default constructor
    Adjuster( const Adjuster & obj );
				// copy constructor

//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
    ~Adjuster();			// destructor

//------------------------------------------------------------------------------
//	Referrring to members
//------------------------------------------------------------------------------
    const double &	interval( void ) const		{ return _interval; }
    double &		interval( void )		{ return _interval; }
    const double &	intervalScale( void ) const	{ return _intervalScale; }
    const double &	minInterval( void ) const	{ return _minInterval; }
    const double &	maxInterval( void ) const	{ return _maxInterval; }
    
    const double &	dataCostGap( void ) const	{ return _dataCostGap; }
    const double &	lowerDataCost( void ) const	{ return _lowerDataCost; }
    const double &	upperDataCost( void ) const	{ return _upperDataCost; }
    double &		upperDataCost( void )	 	{ return _upperDataCost; }
    const double &	minDataCost( void ) const	{ return _minDataCost; }
    const double &	maxDataCost( void ) const	{ return _maxDataCost; }

    const double &	lowerSmoothCost( void ) const	{ return _lowerSmoothCost; }
    const double &	upperSmoothCost( void ) const	{ return _upperSmoothCost; }
    double &		upperSmoothCost( void )		{ return _upperSmoothCost; }
    const double &	minSmoothCost( void ) const	{ return _minSmoothCost; }
    const double &	maxSmoothCost( void ) const	{ return _maxSmoothCost; }

    const double &	labelCostGap( void ) const	{ return _labelCostGap; }
    const double &	lowerLabelCost( void ) const	{ return _lowerLabelCost; }
    const double &	upperLabelCost( void ) const	{ return _upperLabelCost; }
    double &		upperLabelCost( void )		{ return _upperLabelCost; }
    const double &	minLabelCost( void ) const	{ return _minLabelCost; }
    const double &	maxLabelCost( void ) const	{ return _maxLabelCost; }

    const double &	cutThreshold( void ) const	{ return _cutThreshold; }
    double &		cutThreshold( void )		{ return _cutThreshold; }
    const double &	minCutThreshold( void )		{ return _minCutThreshold; }
    const double &	maxCutThreshold( void )		{ return _maxCutThreshold; }

    void clear( void )				{ _clear(); }
    
//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------
    Adjuster & operator = ( const Adjuster & obj );
				// assignment

//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
    friend ostream & operator << ( ostream & stream, const Adjuster & obj );
				// output
    friend istream & operator >> ( istream & stream, Adjuster & obj );
				// input

//------------------------------------------------------------------------------
//	Class name
//------------------------------------------------------------------------------
    virtual const char * className( void ) const { return "Adjuster"; }
				// class name
};



#endif // _Adjuster_H

// end of header file
// Do not add any stuff under this line.
