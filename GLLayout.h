//==============================================================================
// GLLayout.h
//	: header file for the layout window
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-11-23 19:54:10 shigeo"
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

#include "GLMacro.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------
#define	DEFAULT_NUM_OPTIONS_IN_LINE	(3)
// #define	DEFAULT_NUM_OPTIONS_IN_LINE	(4)

#define UNIT_WINDOW_SIZE	(256)

//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------
class GLLayout : public GLMacro {

  private:

    int			_num_options_in_line;
    int			_baseW, _baseH, _baseU;
    
  protected:

    GLMacro *		_glDrawing;
    
    Point2		_cursor;	// mouse position
    int			_left, _middle, _right;
					// mouse buttons

//------------------------------------------------------------------------------
//	Fundamental functions
//------------------------------------------------------------------------------
    void _calcGridSize		( void );

    void _setViewport		( unsigned int idRow, unsigned int idCol );
    void _clearViewport		( void );

    void _annotate		( unsigned int idRow, unsigned int idCol );

    void _barchart		( const double & data,
				  const double & smooth,
				  const double & label );
    void _place_option		( vector< Polygon2 > & polys );
    void _tile			( void );
    
//------------------------------------------------------------------------------
//	Picking & Selection
//------------------------------------------------------------------------------
    bool _select		( int & hitID, int nHits, unsigned int * buf );
    bool _pick			( int & hitID, int x, int y, int button );
    
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
    void setGLDrawing( GLMacro * __glDrawing )	{
	_glDrawing = __glDrawing;
    }
    
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
//	Redrawing function
//------------------------------------------------------------------------------
    virtual void redrawAll	( void );

//------------------------------------------------------------------------------
//	Class name
//------------------------------------------------------------------------------
    virtual const char * className( void ) const { return "GLLayout"; }
				// class name
};



#endif // _GLLayout_H

// end of header file
// Do not add any stuff under this line.
