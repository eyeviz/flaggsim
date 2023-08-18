//==============================================================================
// GLLayout.cpp
//	: program file for the layout window
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-08-18 18:31:22 shigeo"
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
// Function for computing the grid size
void GLLayout::_calcGridSize( void )
{
    assert( _fig != NULL );
    assert( _worksp != NULL );

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
	// cerr << HERE << " matrix_size = " << matrix_size << endl;
	// cerr << HERE << " _num_options_in_line = " << _num_options_in_line << endl;
    } while ( out_of_limit );

    // cerr << HERE << " _num_options_in_line = " << _num_options_in_line << endl;
}


// Function for limiting the viewpoint for a specific matrix element
void GLLayout::_setViewport( unsigned int idRow, unsigned int idCol )
{
#ifdef DEBUG
    cerr << HERE 
	 << " idRow = " << idRow << " idCol = " << idCol << endl;
#endif	// DEBUG
    // int quarterW = design_width  / _num_options_in_line;
    // int quarterH = design_height / _num_options_in_line;
    int quarterW = this->w() / _num_options_in_line;
    int quarterH = this->h() / _num_options_in_line;
    int minW = idCol * quarterW;
    int minH = idRow * quarterH;
    
#ifdef DEBUG
    cerr << HERE << " minH = " << minH << " minW = " << minW << endl;
#endif	// DEBUG
    glViewport( minW, minH, quarterW, quarterH );
    glMatrixMode( GL_PROJECTION );
    glOrtho( -1.0, 1.0, -1.0, 1.0, -1.0, 1.0 );
    // gluOrtho2D( -1.0, 1.0, -1.0, 1.0 );
}


// Function for reseting the viewpoint to the entire window
void GLLayout::_clearViewport( void )
{
    glViewport( 0, 0, this->w(), this->h() );
    glMatrixMode( GL_PROJECTION );
    glOrtho( -1.0, 1.0, -1.0, 1.0, -1.0, 1.0 );
    // gluOrtho2D( -1.0, 1.0, -1.0, 1.0 );
}


// Function for annotating labels
void GLLayout::_annotate( unsigned int idRow, unsigned int idCol )
{
    double titleX = 0.40 - 0.01 * _num_options_in_line;
    double stepY = 0.10;
    //double roofB = -0.60 - 0.01 * _num_options_in_line;
    double roofB = -0.63;
    double roofD = roofB;
    double roofS = roofB - stepY;
    double roofL = roofB - 2.0 * stepY;
    int fontsize = 36 / _num_options_in_line;
	
    // int quarterW = this->w() / _num_options_in_line;
    // int quarterH = this->h() / _num_options_in_line;
    // int minW = idCol * quarterW;
    // int minH = idRow * quarterH;

    if ( _isFilled ) glColor3d( 1.0, 1.0, 1.0 );
    else glColor3d( 0.0, 0.0, 0.0 );

    double span = 2.0 / (double)_num_options_in_line;
    double transX = span * ( ( double )idCol + ( 1.0 + titleX )/2.0 ) - 1.0;
    double transD = span * ( ( double )idRow + ( 1.0 + roofD - stepY )/2.0 ) - 1.0;
    _string2D( transX, transD, "D:", fontsize );
    double transS = span * ( ( double )idRow + ( 1.0 + roofS - stepY )/2.0 ) - 1.0;
    _string2D( transX, transS, "S:", fontsize );
    double transL = span * ( ( double )idRow + ( 1.0 + roofL - stepY )/2.0 ) - 1.0;
    _string2D( transX, transL, "L:", fontsize );
#ifdef SKIP
    ostringstream ostr;
    ostr << "( " << transX << " , " << transD << " )" << ends;
    _string2D( transX, transD, ostr.str().c_str() );
#endif	// SKIP
}


// Function for drawing barcharts 
void GLLayout::_barchart( const double & data,
			  const double & smooth,
			  const double & label )
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

    // In FLTK, we cannot precisely locate the position of texts
    // after resetting glViewport
    // noted by ST on 2023/08/16
    // annotate the bars
    // if ( _isFilled ) glColor3d( 1.0, 1.0, 1.0 );
    //else glColor3d( 0.0, 0.0, 0.0 );
    //_string2D( titleX, roofD - stepY, "D:" );
    //_string2D( titleX, roofS - stepY, "S:" );
    //_string2D( titleX, roofL - stepY, "L:" );
}


// Function for drawing figures as a specific matrix element
void GLLayout::_place_option( vector< Polygon2 > & polys )
{
    // cerr << HERE << " polys.size() = " << polys.size() << endl;
    for ( unsigned int i = 0; i < polys.size(); ++i ) {
	_draw_polygon( polys[ i ] );
    }
}


// Function for previewing candidate aggregations in a grid
void GLLayout::_tile( void )
{
    vector< Set > & group = _worksp->cluster();

//------------------------------------------------------------------------------
    glLoadName( NO_NAME );
//------------------------------------------------------------------------------
    unsigned int groupID = 0;
    for ( unsigned int i = 0; i < _num_options_in_line; ++i ) {
	if ( groupID >= group.size() ) break;
	for ( unsigned int j = 0; j < group[ groupID ].size(); ++j ) {
	    unsigned int blockID = group[ groupID ][ j ];

	    // Skip if it is not included in the candidate list
	    if ( blockID >= _worksp->coverBand().size() ) continue;
	    
	    if ( blockID == _worksp->pickID() ) {
		if ( _isFilled ) glColor3d( 0.5, 0.4, 0.0 );
		else glColor3d( 1.0, 0.9, 0.5 );
	    }
	    else {
		if ( _isFilled ) glColor3d( 0.0, 0.0, 0.0 );
		else glColor3d( 1.0, 1.0, 1.0 );
	    }
//------------------------------------------------------------------------------
	    glLoadName( blockID );
//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
	    glLoadName( NO_NAME );
//------------------------------------------------------------------------------

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
//	Picking & Selection
//------------------------------------------------------------------------------
// Function for selecting the item among the picked ones
bool GLLayout::_select( int & hitID, int nHits, unsigned int * buf )
{
    unsigned int *	ptr		= NULL; //, names;
    const float		defaultDepth	= 1000.0;
    float		minDepth	= defaultDepth;

    make_current();

    ptr = buf;
    hitID = 0;

    for ( int i = 0; i < nHits; ++i ) { // for each bit
#ifdef SKIP
	cerr << HERE << " i = " << i
	     << " [0]: " << ptr[ 0 ]
	     << " [1]: " << ptr[ 1 ]
	     << " [2]: " << ptr[ 2 ]
	     << " [3]: " << ptr[ 3 ] << endl;
#endif	// SKIP
	if ( ptr[ 0 ] != 1 ) {
	    cerr << " Number of names for hit = " << ( int )ptr[ 0 ] << endl;
	    assert( ptr[ 0 ] == 1 );
	}
	float curDepth = (float)ptr[ 1 ]/0xffffffff;
	int curID = ( int )ptr[ 3 ];
	if ( ( curDepth < minDepth ) && ( curID != NO_NAME ) ) {
	    minDepth = curDepth;
	    hitID = ( unsigned int )ptr[ 3 ];
	}
	ptr += 4;
    }

    // cerr << " hitID = " << hitID << " depth = " << minDepth << endl;
    // glutSetWindow( win_design );   
    // glutPostRedisplay();
    redraw();

    if ( minDepth < defaultDepth ) {
	return true;
    }
    return false;
}


// Function for picking iterms with mouse clicks
bool GLLayout::_pick( int & hitID, int x, int y, int button )
{
    unsigned int selectBuf[ BUFFER_SIZE ];
    int nHits;
    int viewport[ 4 ];

    // glutSetWindow( win_design );
    make_current();
    
    hitID = NO_NAME;
    
    glGetIntegerv( GL_VIEWPORT, viewport );

    // Picking begins here
    glSelectBuffer( BUFFER_SIZE, selectBuf );
    glRenderMode( GL_SELECT );

    glInitNames();
    glPushName( NO_NAME );

    glMatrixMode( GL_PROJECTION );
    glPushMatrix(); // <====
    glLoadIdentity();
    const double tolerance = PICKING_ERROR;
#ifdef SKIP
    cerr << HERE
	 << " x = " << x << " y = " << y << " viewport = " 
	 << viewport[ 0 ] << " , " << viewport[ 1 ] << " , "
	 << viewport[ 2 ] << " , " << viewport[ 3 ] << endl;
#endif	// SKIP
    gluPickMatrix( (double)x, (double)(viewport[3]-y), tolerance, tolerance, viewport );
    gluOrtho2D( -1.0, 1.0, -1.0, 1.0 );
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix(); // <====
    glLoadIdentity();

    // draw the set of items
    _tile();

    glMatrixMode( GL_PROJECTION );
    glPopMatrix(); // <====
    glMatrixMode( GL_MODELVIEW );
    glPopMatrix(); // <====

    glFlush();

    nHits = glRenderMode( GL_RENDER );
    if ( button == GLUT_LEFT_BUTTON ) {
	if ( _select( hitID, nHits, selectBuf ) ) {
	    return true;
	}
	else {
	    hitID = NO_NAME;
	    return false;
	}
    }
    else {
	return false;
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
    _glDrawing = NULL;

    _left = _middle = _right = 0;
    
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
    // cerr << HERE << " Resize " << w << " x " << h << endl;
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
    // cerr << HERE << " in GLLayout::Display" << endl;
    // glutSetWindow( win_design );
    glClear( GL_COLOR_BUFFER_BIT );

#ifdef DEBUG
    glColor3d( 1.0, 0.0, 0.0 );
    _string2D(  0.6,  0.2, "( 0.6, 0.2)" );
    _string2D(  0.4,  0.9, "( 0.4, 0.9)" );
#endif	// DEBUG

    if ( _fig == NULL ) return;
    if ( _fig->poly().size() == 0 ) return;

    _calcGridSize();
    
    // for enabling antialiasing
    glEnable( GL_LINE_SMOOTH );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );

    _tile();
   
    vector< Expansion > & expand = _fig->expand();
    int maxD = 0, maxS = 0, maxL = 0;
    for ( unsigned int i = 0; i < expand.size(); ++i ) {
	if ( maxD < expand[ i ].dataCost() ) maxD = expand[ i ].dataCost();
	if ( maxS < expand[ i ].smoothCost() ) maxS = expand[ i ].smoothCost();
	if ( maxL < expand[ i ].labelCost() ) maxL = expand[ i ].labelCost();
    }    

#ifdef DEBUG
    glColor3d( 0.0, 0.0, 1.0 );
    _string2D(  0.2, -0.8, "( 0.2,-0.8)" );
    _string2D(  0.5, -0.3, "( 0.5,-0.3)" );
#endif	// DEBUG

    
    vector< Set > & group = _worksp->cluster();
    unsigned int groupID = 0;
    //------------------------------------------------------------------------------
    for ( unsigned int i = 0; i < _num_options_in_line; ++i ) {
	if ( groupID >= group.size() ) break;
	for ( unsigned int j = 0; j < group[ groupID ].size(); ++j ) {
	    unsigned int blockID = group[ groupID ][ j ];
	    double p = expand[ blockID ].dataCost()/(double)maxD;
	    double q = expand[ blockID ].smoothCost()/(double)maxS;
	    double r = expand[ blockID ].labelCost()/(double)maxL;
	    vector< Polygon2 > & polys = _worksp->coverBand()[ blockID ];
	    //------------------------------------------------------------------------------
	    //glMatrixMode( GL_PROJECTION );
	    //glPushMatrix();
	    //glLoadIdentity();
	    _setViewport( _num_options_in_line - i - 1, ( j % _num_options_in_line ) );
	    //------------------------------------------------------------------------------
	    if ( ( _worksp->pickID() != NO_NAME ) && ( _worksp->pickID() != blockID ) ) {
		vector< Polygon2 > & selected = _worksp->coverBand()[ _worksp->pickID() ];
		glColor3d( 1.0, 0.5, 0.0 );
		glLineWidth( 3.0 );
		_place_option( selected );
	    }
	    glLineWidth( 1.0 );
	    if ( _isFilled ) glColor3d( 1.0, 1.0, 1.0 );
	    else glColor3d( 0.0, 0.0, 0.0 );
	    _place_option( polys );
	    _barchart( p, q, r );
	    //------------------------------------------------------------------------------
	    //glMatrixMode( GL_PROJECTION );
	    //glPopMatrix();
	    _clearViewport();
	    //------------------------------------------------------------------------------
	    _annotate( _num_options_in_line - i - 1, ( j % _num_options_in_line ) );
	    //------------------------------------------------------------------------------

	    if ( ( j + 1 != group[ groupID ].size() ) &&
		 ( ( j + 1 ) % _num_options_in_line == 0 ) ) i++;
	}
	groupID++;
    }

    // for disabling antialiasing
    glDisable( GL_LINE_SMOOTH );
    glDisable( GL_BLEND );
    glEnable( GL_DEPTH_TEST );

    // Skip the swap buffers
    // glutSwapBuffers();
}


// Function for handling mouse events
void GLLayout::Mouse( int button, int state, int x, int y )
{
    make_current();

    switch ( button ) {
	// left mouse event
      case FL_LEFT_MOUSE:
	  if ( state ) {
	      switch ( _worksp->mode() ) {
		case FREE:
		    _cursor = Point2( x, y );
		    if ( _pick( _worksp->pickID(), x, y, button ) ) {
			// cerr << HERE << " select pickID = " << _worksp->pickID() << endl;
			; // do nothing
		    }
		    break;
		case SELECTED:
		    break;
	      }
	      _left = 1;
	      // cerr << HERE << " Left is ON " << endl;
	      if ( _worksp->pickID() != NO_NAME ) {
		  // cerr << HERE << " Button is PRESSED" << endl;
		  _worksp->isPressed() = true;
	      }
#ifdef MODE_DEBUG
	      if ( _worksp->mode() == FREE ) cerr << HERE << " mode is FREE" << endl;
	      else cerr << HERE << " mode is SELECTED" << endl;
#endif	// MODE_DEBUG
	  }
	  else {
	      if ( ( _worksp->pickID() != NO_NAME ) && _worksp->isPressed() ) {
		  switch ( _worksp->mode() ) {
		    case FREE:
			// cerr << HERE << " pickID = " << _worksp->pickID() << endl;
			_worksp->mode() = SELECTED;
			// cerr << HERE << " mode => selected" << endl;
			break;
		    case SELECTED:
			// cerr << HERE << " Selected the choice No. " << _worksp->pickID() << endl;
			_glDrawing->Keyboard( 'f', 0, 0 );
			break;
		  }
	      }
	      _worksp->isPressed() = false;
	      // cerr << HERE << " Button is RELEASED" << endl;
	      _left = 0;  
	      // cerr << HERE << " Left is OFF " << endl;
#ifdef MODE_DEBUG
	      if ( _worksp->mode() == FREE ) cerr << HERE << " mode is FREE" << endl;
	      else cerr << HERE << " mode is SELECTED" << endl;
#endif	// MODE_DEBUG
	  }
	  break;
	  // middle mouse event
      case FL_MIDDLE_MOUSE:
	  if ( state ) {
	      _cursor = Point2( x, y );
	      _middle = 1;
	  }
	  else {
	      _middle = 0;
	  }
	  break;
	  // right mouse event
      case FL_RIGHT_MOUSE:
	  if ( state ) {
	      _cursor = Point2( x, y );
	      _right = 1;
	  }
	  else {
	      _right = 0;
	      _worksp->pickID() = NO_NAME;
	      // reoptBit = false;
	  }
	  break;
      default:
	  cerr << "Unknown button" << endl;
	  break;
    }

    redrawAll();
}


// Function for handling mouse dragging events
void GLLayout::Motion( int x, int y )
{
    // cerr << HERE << "GLLayout::Motion" << endl;

    // cerr << HERE << " in motion_design" << endl;
    if ( _left ) {
	// cerr << HERE << " pointer = ( " << x << " , " << y << " ) " << endl;
	if ( _pick( _worksp->pickID(), x, y, GLUT_LEFT_BUTTON ) ) {
	    ; // do nothing
	    // cerr << HERE << " Pick ID " << setw( 2 ) << _worksp->pickID() << " is picked!!" << endl;
	    // ------------------------------
	    // This line is necessary.
	    // Noted by ST on 2023/08/08 
	    _worksp->mode() = SELECTED;
	    // ------------------------------
	}
#ifdef MODE_DEBUG
	if ( _worksp->mode() == FREE ) cerr << HERE << " mode is FREE" << endl;
	else cerr << HERE << " mode is SELECTED" << endl;
#endif	// MODE_DEBUG
    }
    else if ( _middle ) {
	;
    }
    else if ( _right ) {
	;
    }

    redrawAll();
}


// Function for handling mouse moving events
void GLLayout::PassiveMotion( int x, int y )
{
    cerr << HERE << "GLLayout::PassiveMotion" << endl;
}


// Function for handling keyboard events
void GLLayout::Keyboard( int key, int x, int y )
{
    // cerr << HERE << "GLLayout::Keyboard" << endl;
    make_current();

    switch ( key ) {
      case 'q':
	  exit( 0 );
	  break;
      default:
	  break;
    }

    redrawAll();
}


// Function for redrawing associative windows as well as this one
void GLLayout::redrawAll( void )
{
    redraw();
    if ( _glDrawing != NULL ) _glDrawing->redraw();
}

// end of header file
// Do not add any stuff under this line.
