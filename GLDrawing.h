//==============================================================================
// GLDrawing.h
//	: header file for the drawing window
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-08-08 02:31:19 shigeo"
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

class GLDrawing : public GLBase {

  private:

  protected:
    
    GLBase *		_glLayout;
    
    bool		_isConjoined;
    bool		_isWrapped;
    bool		_isPlotted;

    unsigned int	_nPolys;
    
#ifdef SKIP
    const double	interval_scale		= 100.0;
    float		interval_spinner =
	( float )( interval_scale * Drawing::interval_threshold );
    const float	interval_ceil		=  1.0;
    const float	interval_roof		= 20.0;
    // ------------------------------
    const float	data_cost_gap		=  1.0;
    float		min_data_spinner	= ( float )Drawing::data_cost_lower;
    float		max_data_spinner	= ( float )Drawing::data_cost_upper;
    const float	data_cost_ceil		=  2.0;
    // const float	data_cost_roof		= 12.0;
    const float	data_cost_roof		= 16.0;
    // ------------------------------
    const float	smooth_cost_gap		=  1.0;
    float		min_smooth_spinner	= ( float )Drawing::smooth_cost_lower;
    float		max_smooth_spinner	= ( float )Drawing::smooth_cost_upper;
    const float	smooth_cost_ceil	=  1.0;
    // const float	smooth_cost_roof	=  5.0;
    const float	smooth_cost_roof	= 10.0;
    // ------------------------------
    const float	label_cost_gap		=  1.0;
    float		min_label_spinner	= ( float )Drawing::label_cost_lower;
    float		max_label_spinner	= ( float )Drawing::label_cost_upper;
    const float	label_cost_ceil		=  2.0;
    //const float	label_cost_roof		= 10.0;
    const float	label_cost_roof		= 16.0;
    // ------------------------------
#endif	// SKIP
    double		_cut_threshold		= 20.0;
#ifdef SKIP
    float		cut_threshold_spinner	= ( float )cut_threshold;
    const float	cut_threshold_ceil	=  5.0;
    const float	cut_threshold_roof	= 50.0;
    // ------------------------------
#endif	// SKIP
    
//------------------------------------------------------------------------------
//	Fundamental functions
//------------------------------------------------------------------------------
    void _draw_vertex_ids	( Network & g );
    void _draw_vertex_ids	( Directed & g );
    void _draw_network		( Network & g );
    void _draw_directed		( Directed & g );
    void _draw_polygon_ids	( const Drawing & fig );
    void _draw_hulls		( vector< Polygon2 > & hull );

//------------------------------------------------------------------------------
//	Functions for File I/O
//------------------------------------------------------------------------------
    void _retrieve_headname	( const char * args );
    void _load_drawing		( const char * filename );
    void _save_drawing		( const char * filename );

    
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
//	Functions for File I/O
//------------------------------------------------------------------------------
    void load_drawing		( const char * filename ) {
	_load_drawing( filename );
    }
    void save_drawing		( const char * filename ) {
	_save_drawing( filename );
    }

//------------------------------------------------------------------------------
//	Class name
//------------------------------------------------------------------------------
    virtual const char * className( void ) const { return "GLDrawing"; }
				// class name
};


#endif // _GLDrawing_H

// end of header file
// Do not add any stuff under this line.
