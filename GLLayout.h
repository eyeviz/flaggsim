//==============================================================================
// GLLayout.h
//	: header file for the layout window
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-08-07 17:18:20 shigeo"
//
//==============================================================================

#ifndef _GLLayout_H		// begining of header file
#define _GLLayout_H		// notifying that this file is included

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------
#include <iostream>
#include <string> 
using namespace std;

// OpenGL library
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "GLBase.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------
#define	DEFAULT_NUM_OPTIONS_IN_LINE	(3)
// #define	DEFAULT_NUM_OPTIONS_IN_LINE	(4)


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------
class GLLayout : public GLBase {

  private:

    int			_num_options_in_line;

  protected:

//------------------------------------------------------------------------------
//	Fundamental functions
//------------------------------------------------------------------------------
    void _barchart		( unsigned int idRow, unsigned int idCol,
				  const double & data,
				  const double & smooth,
				  const double & label );
    void _place_option		( unsigned int idRow, unsigned int idCol,
				  vector< Polygon2 > & polys,
				  const double & data,
				  const double & smooth,
				  const double & label );
    void _tile			( void );
    

public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    GLLayout( int _x, int _y, int _w, int _h, const char *_l ); 
					// default constructor

//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
    ~GLLayout();			// destructor

//------------------------------------------------------------------------------
//	Referrring to members
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Fundamental functions for OpenGL Window
//------------------------------------------------------------------------------
    void InitGL		( void );
    void Resize		( int w, int h );
    void Display	( void );
    void Mouse		( int button, int state, int x, int y );
    void Motion		( int x, int y );
    void PassiveMotion	( int x, int y );
    void Keyboard	( int key, int x, int y );

//------------------------------------------------------------------------------
//	Class name
//------------------------------------------------------------------------------
    virtual const char * className( void ) const { return "GLLayout"; }
				// class name
};



#endif // _GLLayout_H

// end of header file
// Do not add any stuff under this line.
