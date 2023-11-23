//==============================================================================
// GLMacro.h
//	: header file for GL Window basement 
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-11-23 19:56:11 shigeo"
//
//==============================================================================

#ifndef _GLMacro_H		// begining of header file
#define _GLMacro_H		// notifying that this file is included

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------
#include <iostream>
#include <string> 
#include <vector> 
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
#include "CGAL.h"
#include "Drawing.h"
#include "Workspace.h"
#include "Adjuster.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------

#define PICKING_ERROR		(20.0)


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class GLMacro : public Fl_Gl_Window {

  private:

    virtual void draw	( void );
    virtual int  handle	( int ev );

  protected:

    vector< Fl_Gl_Window * >	_flwin;

    Point2			_origin;
    double			_side;

    unsigned int		_nPolys;

    Drawing			* _fig;
    Workspace			* _worksp;
    Adjuster			* _adjust;

    bool			_isFilled;

    bool			_flagAggregated;

    static string		_headname;

//------------------------------------------------------------------------------
//	Fundamental UI functions
//------------------------------------------------------------------------------
    virtual void resize	( int x_, int y_, int w_, int h_ );

//------------------------------------------------------------------------------
//	Fundamental functions
//------------------------------------------------------------------------------
    void _string2D		( double x, double y, const char * str, int size = 12 );
    void _drawPolygon		( Polygon2 & poly );


//------------------------------------------------------------------------------
//	Functions for File I/O
//------------------------------------------------------------------------------
    void _retrieveHeadname	( const char * args );
    void _loadDrawing		( const char * filename );
    void _saveDrawing		( const char * filename );
    void _capture		( const char * filename );

  public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    GLMacro( int _x, int _y, int _w, int _h, const char *_l );
				// default constructor

//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
    ~GLMacro();			// destructor

//------------------------------------------------------------------------------
//	Referrring to members
//------------------------------------------------------------------------------
    void addFlWin	( Fl_Gl_Window * ptr )  { _flwin.push_back( ptr ); }
    void clearFlWin	( void )		{ _flwin.clear(); }

    void setOrigin	( const Point2 & __origin ) { _origin = __origin; }
    void setSide	( const double & __side ) { _side = __side; }	
    
    void setFig		( Drawing * __fig )	{ _fig = __fig; }
    void setWorkspace	( Workspace * __worksp )
						{ _worksp = __worksp; }
    void setAdjuster	( Adjuster * __adjust )	{ _adjust = __adjust; }

    Drawing * fig	( void )		{ return _fig; }

    const Linkage & dendrogram( void ) const	{ return _worksp->dendrogram(); }
    Linkage & dendrogram( void )		{ return _worksp->dendrogram(); }

    const vector< Set > & cluster( void ) const	{ return _worksp->cluster(); }
    vector< Set > & cluster( void )		{ return _worksp->cluster(); }

    const bool & isFilled( void ) const 	{ return _isFilled; }
    void setFilled( void )			{ _isFilled = true; }
    void clearFilled( void )			{ _isFilled = false; }

    const bool & flagAggregated( void ) const	{ return _flagAggregated; }
    void enableAggregated( void )	 	{ _flagAggregated = true; }
    void disableAggregated( void )		{ _flagAggregated = false; }
    
    static const string & headname( void ) {
	return GLMacro::_headname;
    }
    
//------------------------------------------------------------------------------
//	Fundamental functions for OpenGL Window
//------------------------------------------------------------------------------
    virtual void InitGL		( void );
    virtual void Resize		( int w, int h );
    virtual void Display	( void );
    virtual void Mouse		( int button, int state, int x, int y );
    virtual void Motion		( int x, int y );
    virtual void PassiveMotion	( int x, int y );
    virtual void Keyboard	( int key, int x, int y );
    
//------------------------------------------------------------------------------
//	Redrawing function
//------------------------------------------------------------------------------
    virtual void redrawAll	( void );

//------------------------------------------------------------------------------
//	Functions for File I/O
//------------------------------------------------------------------------------
    void loadDrawing		( const char * filename ) {
	_loadDrawing( filename );
	_adjust->clear();
	disableAggregated();
	if ( _flagAggregated ) Keyboard( 'a', 0, 0 );
	redrawAll();
    }
    void saveDrawing		( const char * filename ) {
	_saveDrawing( filename );
    }
    void capture		( const char * filename ) {
	_capture( filename );
	redrawAll();
    }

//------------------------------------------------------------------------------
//	Class name
//------------------------------------------------------------------------------
    virtual const char * className( void ) const { return "GLMacro"; }
				// class name
};



#endif // _GLMacro_H

// end of header file
// Do not add any stuff under this line.
