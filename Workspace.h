//==============================================================================
// Workspace.h
//	: header file for maintaining auxiliary variables
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-08-07 17:23:47 shigeo"
//
//==============================================================================

#ifndef _Workspace_H		// begining of header file
#define _Workspace_H		// notifying that this file is included

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------
#include <vector>

using namespace std;

#include "Common.h"
#include "CGAL.h"
#include "Set.h"
#include "Linkage.h"


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Type definitions
//------------------------------------------------------------------------------
typedef enum {
    FREE, SELECTED
} PickMode;


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------
class Workspace {

  private:

  protected:
    
    vector< vector< Polygon2 > >	_coverHull;
    vector< vector< Polygon2 > >	_coverBand;
    vector< vector< Set > >		_coverGlob;

    Linkage				_dendrogram;
    vector< Set >			_cluster;
    
    int					_pickID;
    vector< Point2 >			_pickCoord;
    bool				_isPressed;
    PickMode				_mode;	

public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    Workspace();			// default constructor
    Workspace( const Workspace & obj );
				// copy constructor

//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
    ~Workspace();			// destructor

//------------------------------------------------------------------------------
//	Referrring to members
//------------------------------------------------------------------------------
    const vector< vector< Polygon2 > > &	coverHull( void ) const {
	return _coverHull;
    }
    vector< vector< Polygon2 > > &		coverHull( void ) {
	return _coverHull;
    }

    const vector< vector< Polygon2 > > & 	coverBand( void ) const {
	return _coverBand;
    }
    vector< vector< Polygon2 > > &		coverBand( void ) {
	return _coverBand;
    }

    const vector< vector< Set > > &		coverGlob( void ) const {
	return _coverGlob;
    }
    vector< vector< Set > > &			coverGlob( void ) {
	return _coverGlob;
    }


    const Linkage &				dendrogram( void ) const {
	return _dendrogram;
    }
    Linkage &					dendrogram( void ) {
	return _dendrogram;
    }

    const vector< Set > &			cluster( void ) const {
	return _cluster;
    }
    vector< Set > &				cluster( void ) {
	return _cluster;
    }

    const int &					pickID( void ) const {
	return _pickID;
    }
    int &					pickID( void ) {
	return _pickID;
    }

    const vector< Point2 > &			pickCoord( void ) const {
	return _pickCoord;
    }
    vector< Point2 > &				pickCoord( void ) {
	return _pickCoord;
    }

    const PickMode &				mode( void ) const {
	return _mode;
    }
    PickMode &					mode( void ) {
	return _mode;
    }
    
//------------------------------------------------------------------------------
//	Functions for sorting
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------
    Workspace & operator = ( const Workspace & obj );
				// assignment

//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
    friend ostream & operator << ( ostream & stream, const Workspace & obj );
				// output
    friend istream & operator >> ( istream & stream, Workspace & obj );
				// input

//------------------------------------------------------------------------------
//	Class name
//------------------------------------------------------------------------------
    virtual const char * className( void ) const { return "Workspace"; }
				// class name
};



#endif // _Workspace_H

// end of header file
// Do not add any stuff under this line.
