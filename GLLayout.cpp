//==============================================================================
// GLLayout.cpp
//	: program file for the layout window
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-08-07 21:06:49 shigeo"
//
//==============================================================================

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <iostream>
#include <string>
using namespace std;

// OpenCV library
#include <opencv2/opencv.hpp>

#include "GLLayout.h"


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Private Functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------
// Function for drawing barcharts 
void GLLayout::_barchart( unsigned int idRow, unsigned int idCol,
			  const double & data, const double & smooth, const double & label )
{
    const double edgeX = 0.5;
    const double titleX = 0.35;
    const double spanX = 0.4;
    const double stepY = 0.1;
    const double roofB = -0.65;
    const double roofD = roofB;
    const double roofS = roofB - stepY;
    const double roofL = roofB - 2.0 * stepY;
    
    // data cost
    glColor3d( 1.0, 0.0, 0.0 );
    glBegin( GL_POLYGON );
    glVertex2d( edgeX, roofD );
    glVertex2d( edgeX, roofD - stepY );
    glVertex2d( edgeX + data*spanX, roofD - stepY );
    glVertex2d( edgeX + data*spanX, roofD );
    glEnd();

    // smooth cost
    glColor3d( 0.0, 1.0, 0.0 );
    glBegin( GL_POLYGON );
    glVertex2d( edgeX, roofS );
    glVertex2d( edgeX, roofS - stepY );
    glVertex2d( edgeX + smooth*spanX, roofS - stepY );
    glVertex2d( edgeX + smooth*spanX, roofS );
    glEnd();

    // label cost
    glColor3d( 0.0, 0.0, 1.0 );
    glBegin( GL_POLYGON );
    glVertex2d( edgeX, roofL );
    glVertex2d( edgeX, roofL - stepY );
    glVertex2d( edgeX + label*spanX, roofL - stepY );
    glVertex2d( edgeX + label*spanX, roofL );
    glEnd();

    // annotate the bars
    if ( _isFilled ) glColor3d( 1.0, 1.0, 1.0 );
    else glColor3d( 0.0, 0.0, 0.0 );
    _string2D( titleX, roofD - stepY, "D:" );
    _string2D( titleX, roofS - stepY, "S:" );
    _string2D( titleX, roofL - stepY, "L:" );
}


// Function for drawing figures as a specific matrix element
void GLLayout::_place_option( unsigned int idRow, unsigned int idCol,
			      vector< Polygon2 > & polys,
			      const double & data, const double & smooth, const double & label )
{
    // #ifdef DEBUG
    cerr << HERE 
	 << " idRow = " << idRow << " idCol = " << idCol << endl;
    // #endif	// DEBUG
    // int quarterW = design_width  / _num_options_in_line;
    // int quarterH = design_height / _num_options_in_line;
    int quarterW = this->w() / _num_options_in_line;
    int quarterH = this->h() / _num_options_in_line;
    int minW = idCol * quarterW;
    int minH = idRow * quarterH;
    
    // #ifdef DEBUG
    cerr << HERE << " minH = " << minH << " minW = " << minW << endl;
    // #endif	// DEBUG
    glViewport( minW, minH, quarterW, quarterH );
    gluOrtho2D( -1.0, 1.0, -1.0, 1.0 );
    
    if ( _isFilled ) glColor3d( 1.0, 1.0, 1.0 );
    else glColor3d( 0.0, 0.0, 0.0 );
    glLineWidth( 1.0 );
    cerr << HERE << " polys.size() = " << polys.size() << endl;
    for ( unsigned int i = 0; i < polys.size(); ++i ) {
	_draw_polygon( polys[ i ] );
    }
    _barchart( idRow, idCol, data, smooth, label );
    
    glViewport( 0, 0, this->w(), this->h() );
    gluOrtho2D( -1.0, 1.0, -1.0, 1.0 );
}


// Function for previewing candidate aggregations in a grid
void GLLayout::_tile( void )
{
    vector< Set > & group = _worksp->cluster();

    glLoadName( NO_NAME );
    unsigned int groupID = 0;
    for ( unsigned int i = 0; i < _num_options_in_line; ++i ) {
	if ( groupID >= group.size() ) break;
	for ( unsigned int j = 0; j < group[ groupID ].size(); ++j ) {
	    unsigned int blockID = group[ groupID ][ j ];

	    // Skip if it is not included in the candidate list
	    if ( blockID >= _worksp->coverBand().size() ) continue;
	    
	    if ( blockID == _worksp->pickID() )
		if ( _isFilled ) glColor3d( 0.5, 0.4, 0.0 );
		else glColor3d( 1.0, 0.9, 0.5 );
	    else {
		if ( _isFilled ) glColor3d( 0.0, 0.0, 0.0 );
		else glColor3d( 1.0, 1.0, 1.0 );
	    }
	    glLoadName( blockID );

	    int idRow = _num_options_in_line - i - 1;
	    int idCol = j;
	    double quarterW = 2.0 / _num_options_in_line;
	    double quarterH = 2.0 / _num_options_in_line;
	    double minW = idCol * quarterW - 1.0;
	    double minH = idRow * quarterH - 1.0;
#ifdef DEBUG
	    cerr << HERE << " minW = " << minW << " minH = " << minH
		 << " quarterW = " << quarterW << " quarterH = " << quarterH << endl;
#endif	// DEBUG
	    glBegin( GL_POLYGON );
	    glVertex2d( minW, minH );
	    glVertex2d( minW + quarterW, minH );
	    glVertex2d( minW + quarterW, minH + quarterH );
	    glVertex2d( minW, minH + quarterH );
	    glEnd();
	    glLoadName( NO_NAME );

	    glLineWidth( 1.0 );
	    if ( _isFilled ) glColor3d( 1.0, 1.0, 1.0 );
	    else glColor3d( 0.0, 0.0, 0.0 );
	    glBegin( GL_LINE_LOOP );
	    glVertex2d( minW, minH );
	    glVertex2d( minW + quarterW, minH );
	    glVertex2d( minW + quarterW, minH + quarterH );
	    glVertex2d( minW, minH + quarterH );
	    glEnd();

	    if ( ( j + 1 != group[ groupID ].size() ) &&
		 ( ( j + 1 ) % _num_options_in_line == 0 ) ) i++;
	}

	groupID++;
    }
}


//------------------------------------------------------------------------------
//	Public Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------

//
//  GLLayout::GLLayout --	default constructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
GLLayout::GLLayout( int _x, int _y, int _w, int _h, const char *_l ) 
    : GLBase( _x, _y, _w, _h, _l )
{
    _num_options_in_line
			= DEFAULT_NUM_OPTIONS_IN_LINE;

    resizable( this );
    end();
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------

//
//  GLLayout::~GLLayout --	destructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
GLLayout::~GLLayout()
{

}


//------------------------------------------------------------------------------
//	Referrring to members
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	OpenGL functions
//------------------------------------------------------------------------------
// function for initialized GL setups
void GLLayout::InitGL( void )
{
    cout << "OpenGL Ver. " << glGetString(GL_VERSION) << endl;
    
    if ( _isFilled ) {
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    }
    else {
	glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
    }
}

// function fo resizing the window
void GLLayout::Resize( int w, int h )
{
    cerr << HERE << " Resize " << w << " x " << h << endl;
    glViewport( 0, 0, w, h );
    
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    // gluOrtho2D( -1.0, 1.0, -1.0, 1.0 );
    glOrtho( -1.0, 1.0, -1.0, 1.0, -1.0, 1.0 );
    
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}


// Function for drawing the window
void GLLayout::Display( void )
{
    cerr << HERE << " in GLLayout::Display" << endl;

    vector< Set > & group = _worksp->cluster();
    vector< Expansion > & expand = _fig->expand();
    unsigned int matrix_size = group.size();
    // cerr << HERE << " matrix_size = " << matrix_size << endl;
    _num_options_in_line = matrix_size;
    // cerr << HERE << " num_options_in_line = " << num_options_in_line << endl;
    bool out_of_limit = false;
    do {
	out_of_limit = false;
	for ( unsigned int k = 0; k < group.size(); ++k ) {
	    _num_options_in_line += ( ( (int)group[ k ].size() - 1 ) / matrix_size );
	}
	if ( _num_options_in_line > matrix_size ) {
	    out_of_limit = true;
	    matrix_size++;
	    _num_options_in_line = matrix_size;
	}
	cerr << HERE << " matrix_size = " << matrix_size << endl;
	cerr << HERE << " _num_options_in_line = " << _num_options_in_line << endl;
    } while ( out_of_limit );

    cerr << HERE << " _num_options_in_line = " << _num_options_in_line << endl;
    
#ifdef SKIP
    if ( coverBand.size() > 9 ) _num_options_in_line = 4;
    else if ( coverBand.size() > 4 ) _num_options_in_line = 3;
    else if ( coverBand.size() > 1 ) _num_options_in_line = 2;
    else _num_options_in_line = 1;
#endif	// SKIP
    
    // glutSetWindow( win_design );
    glClear( GL_COLOR_BUFFER_BIT );

    _tile();
    
    int maxD = 0, maxS = 0, maxL = 0;
    for ( unsigned int i = 0; i < expand.size(); ++i ) {
	if ( maxD < expand[ i ].dataCost() ) maxD = expand[ i ].dataCost();
	if ( maxS < expand[ i ].smoothCost() ) maxS = expand[ i ].smoothCost();
	if ( maxL < expand[ i ].labelCost() ) maxL = expand[ i ].labelCost();
    }    

    unsigned int groupID = 0;
    for ( unsigned int i = 0; i < _num_options_in_line; ++i ) {
	if ( groupID >= group.size() ) break;
	for ( unsigned int j = 0; j < group[ groupID ].size(); ++j ) {
	    unsigned int blockID = group[ groupID ][ j ];
	    double p = expand[ blockID ].dataCost()/(double)maxD;
	    double q = expand[ blockID ].smoothCost()/(double)maxS;
	    double r = expand[ blockID ].labelCost()/(double)maxL;
	    vector< Polygon2 > & polys = _worksp->coverBand()[ blockID ];
	    _place_option( _num_options_in_line - i - 1, ( j % _num_options_in_line ),
			   polys, p, q, r );
	    if ( ( j + 1 != group[ groupID ].size() ) &&
		 ( ( j + 1 ) % _num_options_in_line == 0 ) ) i++;
	}
	groupID++;
    }

    // Skip the swap buffers
    // glutSwapBuffers();
}


// Function for handling mouse events
void GLLayout::Mouse( int button, int state, int x, int y )
{
    make_current();

    int keymod = ( Fl::event_state(FL_SHIFT) ? 2 : (Fl::event_state(FL_CTRL) ? 3 : 1 ) );
    switch ( keymod ) {
      case 2:
	  cerr << HERE << " SHIFT button pressed" << endl;
	  break;
      case 3:
	  cerr << HERE << " CTRL button pressed" << endl;
	  break;
      case 1:
	  cerr << HERE << " No modifier button pressed" << endl;
	  break;
      default:
	  cerr << HERE << " This case cannot be expected" << endl;
	  break;
    }
    
    if ( button == FL_LEFT_MOUSE ) {
        if ( state ) {
	    cerr << HERE << " Left mouse pressed " << endl;
        }
        else{
	    cerr << HERE << " Left mouse released " << endl;
        }
    }
    else if ( button == FL_MIDDLE_MOUSE ) {
        if ( state ) {
	    cerr << HERE << " Middle mouse pressed " << endl;
        }
        else{
	    cerr << HERE << " Middle mouse released " << endl;
        }
    }
    else if ( button == FL_RIGHT_MOUSE ) {
        if ( state ) {
	    cerr << HERE << " Right mouse pressed " << endl;
        }
        else{
	    cerr << HERE << " Right mouse released " << endl;
        }
    }

    redraw();
}


// Function for handling mouse dragging events
void GLLayout::Motion( int x, int y )
{
    cerr << HERE << "GLLayout::Motion" << endl;
}

// Function for handling mouse moving events
void GLLayout::PassiveMotion( int x, int y )
{
    cerr << HERE << "GLLayout::PassiveMotion" << endl;
}

// Function for handling keyboard events
void GLLayout::Keyboard( int key, int x, int y )
{
    cerr << HERE << "GLLayout::Keyboard" << endl;
    make_current();

    switch ( key ) {
      case 'q':
	  exit( 0 );
	  break;
      default:
	  break;
    }

    redraw();
    // Redrawing other associative windows
    // cerr << HERE << " size of associative windows = " << _flwin.size() << endl;
    for ( unsigned int i = 0; i < _flwin.size(); ++i )
	if ( _flwin[ i ]->valid() ) _flwin[ i ]->redraw();
}


// end of header file
// Do not add any stuff under this line.
