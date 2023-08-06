//==============================================================================
// Expansion.h
//	: header file for expansion over the polygon elements
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-05-16 20:56:09 shigeo"
//
//==============================================================================

#ifndef _Expansion_H		// begining of header file
#define _Expansion_H		// notifying that this file is included

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include "Set.h"
#include "Expansion.h"


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class Expansion {

  private:

  protected:

    int		_totalCost;
    int		_dataCost;
    int		_smoothCost;
    int		_labelCost;
    Set		_group;
    
    vector< Set >	_cluster( void );
    
  public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    Expansion();			// default constructor
    Expansion( const Expansion & obj );
					// copy constructor

//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
    virtual ~Expansion();		// destructor

//------------------------------------------------------------------------------
//	Referrring to members
//------------------------------------------------------------------------------
    static unsigned int nLabels;

    const int & totalCost( void ) const		{ return _totalCost; }
    int & totalCost( void )			{ return _totalCost; }
    void setTotalCost( int __totalCost )	{ _totalCost = __totalCost; }

    const int & dataCost( void ) const		{ return _dataCost; }
    int & dataCost( void )			{ return _dataCost; }
    void setDataCost( int __dataCost ) 		{ _dataCost = __dataCost; }

    const int & smoothCost( void ) const	{ return _smoothCost; }	
    int & smoothCost( void )			{ return _smoothCost; }	
    void setSmoothCost( int __smoothCost ) 	{ _smoothCost = __smoothCost; }	

    const int & labelCost( void ) const		{ return _labelCost; }
    int & labelCost( void )			{ return _labelCost; }
    void setLabelCost( int __labelCost ) 	{ _labelCost = __labelCost; }

    const Set & group( void ) const		{ return _group; }
    Set & group( void )				{ return _group; }

    vector< Set > cluster( void )		{ return _cluster(); }
    
#ifdef SKIP
//------------------------------------------------------------------------------
//	Functions for sorting
//------------------------------------------------------------------------------
    friend int compare ( const Expansion * a, const Expansion * b );
    friend int compare ( const Expansion ** a, const Expansion ** b );
				// comparison
//------------------------------------------------------------------------------
//	equality
//------------------------------------------------------------------------------
    friend int operator == ( const Expansion & a, const Expansion & b );
    friend int operator != ( const Expansion & a, const Expansion & b ) {
	return ( ! ( a == b ) );
    }				// equality
#endif	// SKIP

//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------
    Expansion & operator = ( const Expansion & obj );
				// assignment

//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
    friend ostream & operator << ( ostream & stream, const Expansion & obj );
				// output
    friend istream & operator >> ( istream & stream, Expansion & obj );
				// input

//------------------------------------------------------------------------------
//	Class name
//------------------------------------------------------------------------------
    virtual const char * className( void ) const { return "Expansion"; }
				// class name
};

#ifdef SKIP
inline int arrange( const Expansion * a, const Expansion * b ) {
    return compare( a, b );
}
#endif	// SKIP


#endif // _Expansion_H

// end of header file
// Do not add any stuff under this line.
