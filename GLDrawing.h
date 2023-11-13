//==============================================================================
// GLDrawing.h
//	: header file for the drawing window
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-11-13 21:44:38 shigeo"
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

#include "GLBase.h"

//------------------------------------------------------------------------------
//	Macro switches
//------------------------------------------------------------------------------
#define SHOW_SAMPLE_IDS

//------------------------------------------------------------------------------
//	Macro variables
//------------------------------------------------------------------------------
#define NORMAL_MODE		(0)
#define AGGREGATION_MODE	(1)
#define SIMPLIFICATION_MODE	(2)




//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class GLDrawing : public GLBase {

  private:

  protected:
    
    GLBase *		_glLayout;
    
    int			_mode;	// 0: Normal
				// 1: Aggregated
				// 2: Simplified

    Point2		_cursor;	// current mouose position
    Point2		_corner;	// latest mouse position
    int			_left, _middle, _right;
					// mouse buttons

    bool		_isConjoined;
    bool		_isWrapped;
    bool		_isPlotted;

//------------------------------------------------------------------------------
//	Fundamental functions
//------------------------------------------------------------------------------
    void _draw_polygon_set	( void );
    void _draw_outline_set	( void );
    void _draw_vertex_ids	( Network & g );
    void _draw_vertex_ids	( Directed & g );
    void _draw_network		( Network & g );
    void _draw_directed		( Directed & g );
    // void _draw_polygon_ids	( const Drawing & fig );
    void _draw_hulls		( vector< Polygon2 > & hull );
    void _draw_polygon_samples	( const Polygon2 & poly );
    void _draw_hull_samples	( vector< Polygon2 > & hull );

    void _draw_rubberband	( void );
    
//------------------------------------------------------------------------------
//	Functions for Polygon Selection
//------------------------------------------------------------------------------
    void _retrieve		( vector< unsigned int > & ids, int nHits,
				  unsigned int * buffer );
    void _bound			( int x, int y, int button = FL_MIDDLE_MOUSE,
				  int modifier = 0 );
    void _release		( void );
//------------------------------------------------------------------------------
    void _placeNames		( void );
    bool _select		( int & hitID, int nHits, unsigned int * buf );
    bool _pick			( int & hitID, int x, int y, int button );

//------------------------------------------------------------------------------
//	Functions for label cost optimization
//------------------------------------------------------------------------------
    void _isometric		( vector< Expansion > & expand );

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
    void setGLLayout( GLBase * __glLayout )	{
	_glLayout = __glLayout;
    }

    void setMode( int __mode )			{ _mode = __mode; }
    
    void release		( void )	{ _release(); }

    const bool & isConjoined( void ) const 	{ return _isConjoined; }
    void setConjoined( void )			{ _isConjoined = true; }
    void clearConjoined( void )			{ _isConjoined = false; }

    const bool & isWrapped( void ) const 	{ return _isWrapped; }
    void setWrapped( void )			{ _isWrapped = true; }
    void clearWrapped( void )			{ _isWrapped = false; }

    const bool & isPlotted( void ) const 	{ return _isPlotted; }
    void setPlotted( void )			{ _isPlotted = true; }
    void clearPlotted( void )			{ _isPlotted = false; }

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
    virtual const char * className( void ) const { return "GLDrawing"; }
				// class name
};


#endif // _GLDrawing_H

// end of header file
// Do not add any stuff under this line.
