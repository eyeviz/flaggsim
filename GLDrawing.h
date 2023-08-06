//==============================================================================
// GLDrawing.h
//	: header file for the drawing window
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-08-06 17:17:33 shigeo"
//
//==============================================================================

#ifndef _GLDrawing_H		// begining of header file
#define _GLDrawing_H		// notifying that this file is included

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

#include <FL/gl.h>
#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/Fl_Menu_Bar.H>


#include "Common.h"
#include "Drawing.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------
#define RESAMPLE_BOUNDARY
#ifdef RESAMPLE_BOUNDARY
// #define RESAMPLE_INTERVAL	(0.02) // 120m / 2.0 * 0.02 = 1.2m
#define RESAMPLE_INTERVAL	(0.05)
//#define RESAMPLE_INTERVAL	(0.10) // 120m / 2.0 * 0.10 = 6.0m <== Current selection
// #define RESAMPLE_INTERVAL	(0.20) // <== Previous seleciton
//#define RESAMPLE_INTERVAL	(0.25)
#endif	// RESAMPLE_BOUNDARY


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class GLDrawing : public Fl_Gl_Window {

  private:

    void draw( void );
    void resize( int x_, int y_, int w_, int h_ );

  protected:
    
    bool		_isConjoined;
    bool		_isFilled;
    bool		_isWrapped;
    bool		_isPlotted;

    Drawing		_fig;
    unsigned int	_nPolys;


//------------------------------------------------------------------------------
//	Fundamental functions
//------------------------------------------------------------------------------
    void _string2D		( double x, double y, const char * str );
    void _draw_vertex_ids	( Network & g );
    void _draw_vertex_ids	( Directed & g );
    void _draw_network		( Network & g );
    void _draw_directed		( Directed & g );
    void _draw_polygon		( Polygon2 & poly );
    void _draw_polygon_ids	( const Drawing & fig );
    void _draw_hulls		( vector< Polygon2 > & hull );

//------------------------------------------------------------------------------
//	Functions for File I/O
//------------------------------------------------------------------------------
    void _load_drawing		( const char * filename );

  public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    GLDrawing( int _x, int _y, int _w, int _h, const char *_l ); 
				// default constructor

//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
    ~GLDrawing();			// destructor

//------------------------------------------------------------------------------
//	Referrring to members
//------------------------------------------------------------------------------
    const bool & isConjoined( void ) const 	{ return _isConjoined; }
    void setConjoined( void )			{ _isConjoined = true; }
    void clearConjoined( void )			{ _isConjoined = false; }

    const bool & isFilled( void ) const 	{ return _isFilled; }
    void setFilled( void )			{ _isFilled = true; }
    void clearFilled( void )			{ _isFilled = false; }

    const bool & isWrapped( void ) const 	{ return _isWrapped; }
    void setWrapped( void )			{ _isWrapped = true; }
    void clearWrapped( void )			{ _isWrapped = false; }

    const bool & isPlotted( void ) const 	{ return _isPlotted; }
    void setPlotted( void )			{ _isPlotted = true; }
    void clearPlotted( void )			{ _isPlotted = false; }

    const Drawing & fig( void ) const		{ return _fig; }
    Drawing & fig( void )			{ return _fig; }

//------------------------------------------------------------------------------
//	Fundamental functions for OpenGL Window
//------------------------------------------------------------------------------
    void initGL( void );
    void resize( int w, int h );
    void display( void );

//------------------------------------------------------------------------------
//	Functions for File I/O
//------------------------------------------------------------------------------
    void load_drawing		( const char * filename ) {
	_load_drawing( filename );
    }


#ifdef WITH_MENU
    void initMenus( void );
#endif	// WITH_MENU
    

    
//------------------------------------------------------------------------------
//	Class name
//------------------------------------------------------------------------------
    virtual const char * className( void ) const { return "GLDrawing"; }
				// class name
};


#endif // _GLDrawing_H

// end of header file
// Do not add any stuff under this line.
