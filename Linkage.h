//==============================================================================
// Linkage.h
//	: header file for data likeage for hierarhical clustring
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-12-23 04:12:48 shigeo"
//
//==============================================================================

#ifndef _Linkage_H		// begining of header file
#define _Linkage_H		// notifying that this file is included

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <cassert>

using namespace std;

#include "Set.h"
#include "Expansion.h"


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------
typedef enum { MINIMUM, MAXIMUM, CENTROID } LinkageType;

class Join {
public:
    unsigned int	idS;
    unsigned int	idL;
    unsigned int	idM;
    double		threshold;
};


class Linkage {

  private:

    vector< unsigned int >	_matID;
    vector< vector< double > >	_distMat; 

  protected:

    vector< Expansion >		_expand;
    LinkageType			_type;
    vector< Join >		_merge;

    vector< Set >		_cluster;
    vector< double >		_lower;
    vector< double >		_upper;
    
    double	_centroid	( const Set & clusterS, const Set & clusterL );
    double	_maximum	( const Set & clusterS, const Set & clusterL );
    double	_minimum	( const Set & clusterS, const Set & clusterL );

    double	_dissim		( LinkageType type,
				  unsigned int idS, unsigned int idL );

    void	_initVars	( void );
    void	_initMat	( void );

    void	_onestep	( void );

    vector< Set >	_retrieve	( double threshold );	

  public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    Linkage();			// default constructor
    Linkage( const Linkage & obj );
				// copy constructor

//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
    ~Linkage();			// destructor

//------------------------------------------------------------------------------
//	Referrring to members
//------------------------------------------------------------------------------
    const LinkageType & type( void ) const 	{ return _type; }
    LinkageType & type( void )			{ return _type; }
    
    void set( vector< Expansion > & __expand ) {
	_expand = __expand;
	_initVars();
	_initMat();
    }
    void clear( void ) {
	_initVars();
	_initMat();
    }
    void merge( void ) {
	while ( _matID.size() > 1 ) {
	    _onestep();
	}
    }
    vector< Set > retrieve( double threshold ) {
	return _retrieve( threshold );
    }
    
//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------
    Linkage & operator = ( const Linkage & obj );
				// assignment

//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
    friend ostream & operator << ( ostream & stream, const Linkage & obj );
				// output
    friend istream & operator >> ( istream & stream, Linkage & obj );
				// input

//------------------------------------------------------------------------------
//	Class name
//------------------------------------------------------------------------------
    virtual const char * className( void ) const { return "Linkage"; }
				// class name
};


#endif // _Linkage_H

// end of header file
// Do not add any stuff under this line.
