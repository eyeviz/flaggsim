//==============================================================================
// GLDrawing.cpp
//	: program file for the drawing window
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-10-15 18:19:50 shigeo"
//
//==============================================================================

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

// OpenCV library
#include <opencv2/opencv.hpp>

// #include <FL/filename.H>		// fl_open_uri()

#include "Common.h"
#include "GLDrawing.h"


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Variables here
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Private Functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------
// Function for drawing all the building polygons with names
void GLDrawing::_draw_polygon_set( void )
{
//------------------------------------------------------------------------------
    glLoadName( NO_NAME );
//------------------------------------------------------------------------------
    for ( unsigned int i = 0; i < _fig->bound().size(); ++i ) {
//------------------------------------------------------------------------------
	glLoadName( i );
//------------------------------------------------------------------------------
	_draw_polygon( _fig->bound()[ i ] );
    }
//------------------------------------------------------------------------------
    glLoadName( NO_NAME );
//------------------------------------------------------------------------------
}


// Draw the neighbor graph
void GLDrawing::_draw_vertex_ids( Network & g )
{
    NetVertexIDMap	vertexID	= get( vertex_myid, g );
    NetVertexCntrMap	vertexCntr	= get( vertex_mycntr, g );
    const double	xdisp		= 0.02;
    
    // Annotating vertices of the proximity graph
    BGL_FORALL_VERTICES( vd, g, Network ) {
	unsigned int id = vertexID[ vd ];
	Point2 & point = vertexCntr[ vd ];
	ostringstream strID;
	strID << '#' << setw( 3 ) << setfill( '0' ) << id << ends;
	_string2D( point.x() - xdisp, point.y(), strID.str().c_str(), 10 );
    }
}


// Draw the neighbor graph
void GLDrawing::_draw_vertex_ids( Directed & g )
{
    DirVertexAddrMap	vertexAddr	= get( vertex_myaddr, g );
    DirVertexPointMap	vertexPoint	= get( vertex_mypoint, g );
    const double	xdisp		= 0.02;

    // Annotating vertices of the proximity graph
    BGL_FORALL_VERTICES( vd, g, Directed ) {
	unsigned int id = vertexAddr[ vd ];
	Point2 & point = vertexPoint[ vd ];
	ostringstream strID;
#ifdef DEBUGGING_PHASE
	if ( ( id == 234 ) || ( id == 235 ) || ( id == 289 ) ) glColor3d( 1.0, 0.5, 0.0 );
	else glColor3d( 0.0, 1.0, 0.5 );
#endif	// DEBUGGING_PHASE
	strID << setw( 3 ) << setfill( '0' ) << id << ends;
	_string2D( point.x() - xdisp, point.y(), strID.str().c_str(), 10 );
    }
}


// Draw the neighbor graph
void GLDrawing::_draw_network( Network & g )
{
    // VertexIDMap	vertexID	= get( vertex_myid, g );
    NetVertexCntrMap	vertexCntr	= get( vertex_mycntr, g );

    // cerr << HERE << " draw_network (for Network) " << endl;

    // Drawing edges of the proximity graph
    glBegin( GL_LINES );
    BGL_FORALL_EDGES( ed, g, Network ) {
	NetVertexDescriptor vdS = source( ed, g );
        NetVertexDescriptor vdT = target( ed, g );
        Point2 & pointS = vertexCntr[ vdS ];
        Point2 & pointT = vertexCntr[ vdT ];
	glVertex2d( pointS.x(), pointS.y() );
	glVertex2d( pointT.x(), pointT.y() );
    }
    glEnd();
}


// Draw the neighbor graph
void GLDrawing::_draw_directed( Directed & g )
{
    DirVertexPointMap	vertexPoint	= get( vertex_mypoint, g );

    // cerr << HERE << " draw_network (for Directed) " << endl;
    // Drawing edges of the proximity graph
    glBegin( GL_LINES );
    BGL_FORALL_EDGES( ed, g, Directed ) {
	NetVertexDescriptor vdS = source( ed, g );
        NetVertexDescriptor vdT = target( ed, g );
        Point2 & pointS = vertexPoint[ vdS ];
        Point2 & pointT = vertexPoint[ vdT ];
	double linkLength = sqrt( ( pointT - pointS ).squared_length() );
	if ( linkLength > Drawing::interval_threshold ) {
	    glColor3d( 0.0, 0.4, 0.8 );
	}
	else {
	    glColor3d( 0.0, 0.8, 0.4 );
	}
	glVertex2d( pointS.x(), pointS.y() );
	glVertex2d( pointT.x(), pointT.y() );
	glColor3d( 0.0, 0.4, 0.8 );
    }
    glEnd();
}


#ifdef SKIP
// Draw the set of convex hulls
void GLDrawing::_draw_polygon_ids( const Drawing & fig )
{
    glColor3d( 0.0, 0.0, 0.0 );
    for ( unsigned int k = 0; k < fig.poly().size(); ++k ) {
	Vector2 toCenter( 0.0, 0.0 );
	for ( unsigned int i = 0; i < fig.poly()[ k ].size(); ++i ) {
	    toCenter += fig.poly()[ k ][ i ] - CGAL::ORIGIN;
	    // cerr << HERE
	    // << " toCenter : x = " << toCenter.x() << " y = " << toCenter.y() << endl;
	}
	toCenter /= ( double )fig.poly()[ k ].size();
	ostringstream strID;
	strID << "#" << setw( 2 ) << k << ends;
	_string2D( toCenter.x()-0.05, toCenter.y()-0.02, strID.str().c_str() );
    }
}
#endif	// SKIP


// Draw the set of convex hulls
void GLDrawing::_draw_hulls( vector< Polygon2 > & hull )
{
    // cerr << HERE << " Number of hulls = " << hull.size() << endl;
    for ( unsigned int k = 0; k < hull.size(); ++k ) {
	_draw_polygon( hull[ k ] );
    }
}

// Draw the rubberband for the polygon selection
void GLDrawing::_draw_rubberband( void )
{
    if ( _middle == 1 ) {
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D( 0.5, this->w() - 0.5, this->h() - 0.5, 0.5 );
	glColor4d( 0.9, 0.5, 0.1, 0.5 );
	glLineWidth( 3.0 );
	// cerr << HERE << " _corner = " << _corner;
	// cerr << HERE << " _cursor = " << _cursor;
	glBegin( GL_LINE_LOOP );
	glVertex2d( _corner.x(), _corner.y() );
	glVertex2d( _cursor.x(), _corner.y() );
	glVertex2d( _cursor.x(), _cursor.y() );
	glVertex2d( _corner.x(), _cursor.y() );
	glEnd();
	glPopMatrix();
    }
}

//------------------------------------------------------------------------------
//	Functions for Polygon Selection
//------------------------------------------------------------------------------
// Function for retrieve the set of selected items
void GLDrawing::_retrieve( vector< unsigned int > & ids, int nHits, unsigned int * buffer )
{
    unsigned int	* ptr = NULL; //, names;
    // const float		defaultDepth = 1000.0;
    // float		minDepth = defaultDepth;

    make_current();

    // cerr << HERE << "**** retrieveCluster ****" << endl;
    ptr = buffer;
    ids.clear();

    for ( int i = 0; i < nHits; ++i ) { // for each bit
#ifdef DEBUG
	cerr << " i = " << i
	     << " [0]: " << ptr[ 0 ]
	     << " [1]: " << ptr[ 1 ]
	     << " [2]: " << ptr[ 2 ]
	     << " [3]: " << ptr[ 3 ] << endl;
#endif	// DEBUG
	if ( ptr[ 0 ] != 1 ) {
	    cerr << " Number of names for hit = " << ( int )ptr[ 0 ] << endl;
	    assert( ptr[ 0 ] == 1 );
	}
	//float curDepth = (float)ptr[ 1 ]/0xffffffff;
	unsigned int curID = ( unsigned int )ptr[ 3 ];
	if ( curID != NO_NAME ) {
	    vector< unsigned int >::iterator it =
		std::find( ids.begin(), ids.end(), curID );
	    if ( it == ids.end() ) ids.push_back( curID );
	}
	ptr += 4;
    }

    sort( ids.begin(), ids.end() );

    for ( unsigned int k = 0; k < ids.size(); ++k ) {
	cerr << " set[ " << setw( 2 ) << k << " ] = " << setw( 3 ) << ids[ k ];
	if ( k % 2 == 1 ) cerr << endl;
    }
    if ( ids.size() % 2 == 1 ) cerr << endl;

    // glutPostRedisplay();
    redraw();
}


// Function for selecting a set of building polygons
// 1 : no modifier, 2: SHIFT, 3: CONTROL
void GLDrawing::_bound( int x, int y, int button, int modifier )
{
    unsigned int	selectBuf[ BUFFER_SIZE ];
    int			nHits;
    int			viewport[ 4 ];

    double center_x =	( _corner.x() + ( double )x ) / 2.0;
    double center_y =	( _corner.y() + ( double )y ) / 2.0;
    double full_x =	max( 1.0, abs( _corner.x() - ( double )x ) );
    double full_y =	max( 1.0, abs( _corner.y() - ( double )y ) );


    cerr << HERE << " center = ( " << center_x << " , "	 << center_y << " ) " << endl;
    cerr << HERE << " full = ( " << full_x << " , "	 << full_y << " ) " << endl;
    getchar();
    
    make_current();

    glGetIntegerv( GL_VIEWPORT, viewport );

    // bounding begins here
    glSelectBuffer( BUFFER_SIZE, selectBuf );
    glRenderMode( GL_SELECT );

    glInitNames();
    glPushName( NO_NAME );

    glMatrixMode( GL_PROJECTION );
    glPushMatrix(); // <====
    glLoadIdentity();

    gluPickMatrix( center_x, ( double )viewport[ 3 ] - center_y,
		   full_x, full_y, viewport );
    glOrtho( -1.0, 1.0, -1.0, 1.0, -1.0, 1.0 );
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix(); // <====
    glLoadIdentity();

    // draw the set of building polygons
    _draw_polygon_set();

    glMatrixMode( GL_PROJECTION );
    glPopMatrix(); // <====
    glMatrixMode( GL_MODELVIEW );
    glPopMatrix(); // <====

    glFlush();

    Set ids;
    nHits = glRenderMode( GL_RENDER );
    _retrieve( ids, nHits, selectBuf );
    if ( ids.size() != 0 ) {
	bool doExist = false;
	for ( unsigned int j = 0; j < _fig->labelDes().size(); ++j ) {
	    if ( _fig->labelDes()[ j ] == ids ) doExist = true;
	    if ( doExist ) break;
	}
	if ( ! doExist ) _fig->labelDes().push_back( ids );
    }
    
    cerr << HERE << " Selected polygons : ";
    for ( unsigned int k = 0; k < _fig->labelDes().size(); ++k ) {
	cerr << HERE << " Design label [ " << setw( 2 ) << k << " ] : ";
	for ( unsigned int m = 0; m < _fig->labelDes()[ k ].size(); ++m )
	    cerr << setw( 4 ) << _fig->labelDes()[ k ][ m ];
        cerr << endl;
    }

    Keyboard( 'a', 0, 0 );
    redrawAll();
}

// Function for unselecting all the set of building polygons
void GLDrawing::_unselect( void )
{
    cerr << HERE << " Clearing the all selected building polygons" << endl;
    _fig->labelDes().clear();
    Keyboard( 'a', 0, 0 );
    redrawAll();
}
	
    
//------------------------------------------------------------------------------
//	Functions for label cost optimization
//------------------------------------------------------------------------------
// processing the ressults of label cost optimization
void GLDrawing::_isometric( vector< Expansion > & expand )
{
    const double R3P2 = 0.5*sqrt(3.0);

    int maxD = 0, maxS = 0, maxL = 0;
    for ( unsigned int i = 0; i < expand.size(); ++i ) {
	if ( maxD < expand[ i ].dataCost() ) maxD = expand[ i ].dataCost();
	if ( maxS < expand[ i ].smoothCost() ) maxS = expand[ i ].smoothCost();
	if ( maxL < expand[ i ].labelCost() ) maxL = expand[ i ].labelCost();
    }    
    _worksp->pickCoord().clear();
    for ( unsigned int i = 0; i < expand.size(); ++i ) {
	double p = expand[ i ].dataCost()/(double)maxD;
	double q = expand[ i ].smoothCost()/(double)maxS;
	double r = expand[ i ].labelCost()/(double)maxL;
	double x = ( R3P2 * ( -p + r ) );
	double y = -0.5*p + 1.0*q - 0.5*r;
	_worksp->pickCoord().push_back( Point2( x, y ) );
    }
    _worksp->coverHull().clear();
    _worksp->coverBand().clear();
    // Bug fixed: added the follwoing line, which has been skipped previously
    // noted by ST on 2023/08/21
    _worksp->coverGlob().clear();
    for ( unsigned int i = 0; i < expand.size(); ++i ) {
	vector< Polygon2 > hullSet;
	vector< Polygon2 > bandSet;
	vector< Set >	   globSet;
	vector< Set > polySet = expand[ i ].cluster();
	for ( unsigned int j = 0; j < polySet.size(); ++j ) {
	    Polygon2 eachHull;
	    Set eachGlob;
	    _fig->concaveForLabel( _fig->netNbr(), polySet[ j ], eachHull, eachGlob );
#ifdef DEBUG
	    cerr << HERE << " i = " << i << " j = " << j
		 << " each hull = " << eachHull.size()
		 << " each glob = " << eachGlob.size() << endl;
#endif	// DEBUG
	    bandSet.push_back( eachHull );
	    globSet.push_back( eachGlob );
	    hullSet.push_back( eachHull );
	}
#ifdef DEBUG
	cerr << HERE << " i = " << i 
	     << " band size = " << bandSet.size()
	     << " glob size = " << globSet.size() << endl;
#endif	// DEBUG
	_worksp->coverHull().push_back( hullSet );
	_worksp->coverBand().push_back( bandSet );
	_worksp->coverGlob().push_back( globSet );
    }
 
#ifdef DEBUG
    cerr << HERE << " Expand size = " << expand.size() << endl;
    for ( unsigned int i = 0; i < expand.size(); ++i ) {
	vector< Set > polySet = expand[ i ].cluster();
	cerr << HERE << " i = " << i << " band size = "
	     << _worksp->coverBand()[ i ].size()
	     << " glob size = "
	     << _worksp->coverGlob()[ i ].size() << endl;
    }
#endif	// DEBUG
}


//------------------------------------------------------------------------------
//	Public Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------

//
//  GLDrawing::GLDrawing --	default constructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
GLDrawing::GLDrawing( int _x, int _y, int _w, int _h, const char *_l ) 
    : GLBase( _x, _y, _w, _h, _l )  	
{
    _glLayout		= NULL;

    _isConjoined	= false;
    _isWrapped		= false;
    _isPlotted		= false;

    _nPolys		= 0;
    
    resizable( this );
    end();
}



//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------

//
//  GLDrawing::~GLDrawing --	destructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
GLDrawing::~GLDrawing()
{

}


//------------------------------------------------------------------------------
//	Referrring to members
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	OpenGL functions
//------------------------------------------------------------------------------
// function for initialized GL setups
void GLDrawing::InitGL( void )
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
void GLDrawing::Resize(int w, int h)
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
void GLDrawing::Display( void )
{
    const double offset = 2.0;

    glClear( GL_COLOR_BUFFER_BIT );
    
#ifdef DEBUG
    // Debug codes
    glPointSize( 15.0 );
    glEnable( GL_POINT_SMOOTH );
    glColor3d( 1.0f, 0.5f, 0.0f );
    glBegin( GL_POINTS );
    glVertex2d( 0.0, 0.0 );
    glEnd();
#endif	// DEBUG

//------------------------------------------------------------------------------
//	Drawing the rubberband
//------------------------------------------------------------------------------
    _draw_rubberband();

//------------------------------------------------------------------------------
//	Skip the drawing without building polygons 
//------------------------------------------------------------------------------
    if ( _fig == NULL ) return;
    if ( _fig->poly().size() == 0 ) return;
    
    // for enabling antialiasing
    glEnable( GL_LINE_SMOOTH );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );

    if ( _isFilled ) {
	// Filling polygons
	glLineWidth( 1.0 );
	glColor3d( 1.0, 1.0, 1.0 );
	for ( unsigned int i = 0; i < _fig->tri().size(); ++i ) {
	    for ( unsigned int j = 0; j < _fig->tri()[ i ].size(); ++j ) {
		glBegin( GL_POLYGON );
		for ( unsigned int k = 0; k < 3; ++k ) {
		    glVertex2d( _fig->tri()[i][j][k].x(), _fig->tri()[i][j][k].y() );
		    // cerr << HERE << fig.tri()[i][j][k] << endl;
		}
		glEnd();
	    }
	}
    }
    else {
	// Drawing polygons
	glLineWidth( 1.0 );
	glColor3d( 0.0, 0.0, 0.0 );
	// cerr << HERE << " _fig.bound().size() = " << _fig->bound().size() << endl;
#ifdef SKIP
	for ( unsigned int i = 0; i < _fig->bound().size(); ++i ) {
	    _draw_polygon( _fig->bound()[ i ] );
	}
#endif	// SKIP
	_draw_polygon_set();
    }

    if ( _isPlotted ) {
	// Plotting polygons
	glEnable( GL_POINT_SMOOTH );
	glPointSize( 9.0 );
	glColor3d( 0.4, 0.8, 0.0 );
	glBegin( GL_POINTS );
	for ( unsigned int i = 0; i < _fig->bound().size(); ++i ) {
	    for ( unsigned int j = 0; j < _fig->bound()[ i ].size(); j++ ) {
		glVertex2d( _fig->bound()[ i ][ j ].x(), _fig->bound()[ i ][ j ].y() );
	    }
	}
	glEnd();
    }

    if ( _worksp->pickID() != NO_NAME ) {
	unsigned int nSegs = _worksp->coverBand()[ _worksp->pickID() ].size();
	for ( unsigned int i = 0; i < nSegs; ++i ) {
	    glColor3d( 1.0, 0.5, 0.0 );
	    glLineWidth( 7.0 );
	    Polygon2 curPoly = _worksp->coverBand()[ _worksp->pickID() ][ i ];
	    _draw_polygon( curPoly );
	}
    }

    if ( _isWrapped ) {
	glDepthFunc( GL_ALWAYS );
	// cerr << HERE << " Drawing the beta-skelton" << endl;
#ifdef DEBUGGING_PHASE
	glLineWidth( 2.0 );
	glColor3d( 0.7, 0.7, 0.7 );
#else	// DEBUGGING_PHASE
	glLineWidth( 2.0 );
	// glColor3d( 0.0, 1.0, 0.5 );
#endif	// DEBUGGING_PHASE
	_draw_directed( _fig->wrapper() );
#ifdef SHOW_SAMPLE_IDS
	glColor3d( 1.0, 0.5, 0.0 );
	_draw_vertex_ids( _fig->wrapper() );
#endif	// SHOW_SAMPLE_IDS
	glDepthFunc( GL_LESS );

	// Drawing building polygon IDs
	// draw_polygon_ids( fig );
    }

    if ( _isConjoined ) {
	glColor4d( 0.0, 0.0, 0.0, 1.0 );
	// draw_vertex_ids( netN );
	_draw_vertex_ids( _fig->netNbr() );
	
	// glColor4d( 1.0, 0.5, 0.0, 0.4 );
	//glColor4d( 0.3, 0.3, 0.3, 0.4 );
	glColor4d( 0.8, 0.6, 0.0, 0.6 );
	// glLineWidth( 1.0 );
	glLineWidth( 2.0 );
	_draw_network( _fig->netNbr() );
	
	glPushMatrix();
	glTranslated( -offset, -offset, 0.0 );
	glColor4d( 1.0, 0.5, 0.0, 0.7 );
	glLineWidth( 5.0 );
	_draw_network( _fig->netPrx() );
	glPopMatrix();
    
#ifdef USING_SIMILARITY_CONJOINING
	glPushMatrix();
	glTranslated( offset, offset, 0.0 );
	glColor4d( 0.0, 0.0, 1.0, 0.7 );
	glLineWidth( 5.0 );
	_draw_network( _fig->netSim() );
	glPopMatrix();
#endif	// USING_SIMILARITY_CONJOINING
	
	// drawing concave polygons
	glLineWidth( 3.0 );
	glColor4d( 0.0, 0.5, 0.0, 0.8 );
	_draw_hulls( _fig->hullPrx() );
#ifdef USING_SIMILARITY_CONJOINING
	glColor4d( 0.0, 0.0, 0.5, 0.8 );
	_draw_hulls( _fig->hullSim() );
#endif	// USING_SIMILARITY_CONJOINING
	glColor4d( 0.5, 0.0, 0.0, 0.8 );
	_draw_hulls( _fig->hullDes() );
    }

    // for disabling antialiasing
    glDisable( GL_LINE_SMOOTH );
    glDisable( GL_BLEND );
    glEnable( GL_DEPTH_TEST );

    // Skip the swap buffers
    // glutSwapBuffers();
}

// Function for handling mouse events
void GLDrawing::Mouse( int button, int state, int x, int y )
{
    make_current();

#ifdef SKIP
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
#endif	// SKIP
    
    switch ( button ) {
	// left mouse event
      case FL_LEFT_MOUSE:
	  if ( state ) {
	      // cerr << HERE << " Left mouse pressed " << endl;
	      _left = 1;
	  }
	  else{
	      // cerr << HERE << " Left mouse released " << endl;
	      _left = 0;
	  }
	  break;
      case FL_MIDDLE_MOUSE:
	  if ( state ) {
	      // cerr << HERE << " Middle mouse pressed " << endl;
	      _corner = _cursor = Point2( x, y );
	      _middle = 1;
	  }
	  else{
	      // cerr << HERE << " Middle mouse released " << endl;
	      _cursor = Point2( x, y );
	      _bound( x, y );
	      _middle = 0;
	  }
	  break;
      case FL_RIGHT_MOUSE:
	  if ( state ) {
	      // cerr << HERE << " Right mouse pressed " << endl;
	      _right = 1;
	  }
	  else{
	      // cerr << HERE << " Right mouse released " << endl;
	      _right = 0;
	  }
	  break;
      default:
	  cerr << "Unknown button" << endl;
	  break;
    }

    redrawAll();
}


// Function for handling mouse dragging events
void GLDrawing::Motion( int x, int y )
{
    // cerr << HERE << "GLDrawing::Motion" << endl;
    // cerr << HERE << "_left = " << _left << endl;

    if ( _left ) {
	_cursor = Point2( x, y );
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
void GLDrawing::PassiveMotion( int x, int y )
{
    cerr << HERE << "GLDrawing::PassiveMotion" << endl;
}

// Function for handling keyboard events
void GLDrawing::Keyboard( int key, int x, int y )
{
    static unsigned int	counter = 0;		
    ostringstream	ostr;
    istringstream	istr;
    string		line;
    vector< Polygon2 >  bandSet;
    vector< Set >	globSet;
    string		imgname;
    string		dirname, befname, aftname, inname, outname;
    struct stat		statbuf;
    
    // cerr << HERE << "GLDrawing::Keyboard : key " << key << endl;
    
    make_current();

    switch ( key ) {
      // clear the map drawings
      case 'c': // == 99
      case 'C':
	  _fig->clear();
	  _nPolys = 0;
	  break;
      // list the aggregation choices for building polygons
      case 'a':	// == 97
      case 'A':
	  // cerr << HERE << " No. polygons in drawing = " << fig.poly().size() << endl;
	  _fig->conjoin();
	  // cerr << HERE << " No. polygons in drawing = " << fig.poly().size() << endl;
	  _fig->optimize();
	  // cerr << HERE << " No. polygons in drawing = " << fig.poly().size() << endl;
	  _isometric( _fig->expand() );
	  // cerr << HERE << " No. polygons in drawing = " << fig.poly().size() << endl;
#ifdef ACTIVATE_RECORDING_MODE
	  if ( _headname.length() > 0 ) {
	      //------------------------------------------------------------------------------
	      // Prepare the directory name for saving snapshot images
	      dirname = "raster/" + _headname;
	      //------------------------------------------------------------------------------
	      // Specify the file name for the input polygon data before generalization
	      befname = "vector/" + _headname + "-in.dat";
	      cerr << HERE << " saved polygon before genralization ======> : "
		   << befname << endl;
	      save_drawing( befname.c_str() );
	      //------------------------------------------------------------------------------
	      // Prepare the directory
	      if ( ! stat( dirname.c_str(), &statbuf ) ) {
		  cerr << HERE << dirname << " already created" << endl;
	      }
	      else {
		  if ( ! mkdir( dirname.c_str(), 0755 ) ) {
		      cerr << HERE << dirname << " successfully created" << endl;
		  }
	      }
	      //------------------------------------------------------------------------------
	      // Start to save the input image data
	      inname = dirname + "/in.png";
	      cerr << HERE << " input image (png) ======> : " << inname << endl;
	      _capture( inname.c_str() );
	  }
#endif	// ACTIVATE_RECORDING_MODE
	  // Delete the last aggregation
	  // fig.expand().erase( fig.expand().end() - 1 );
	  // Hierarchical clustering
	  _worksp->dendrogram().set( _fig->expand() );
	  _worksp->dendrogram().merge();
	  _worksp->cluster() = _worksp->dendrogram().retrieve( _adjust->cutThreshold() );
	  // cerr << HERE << " cluster.size = " << cluster.size() << endl;
	  break;
      // exhaustive simplify
      case 'e': // == 101
      case 'E':
	  _fig->simplify();
	  _fig->triangulate();
#ifdef ACTIVATE_RECORDING_MODE
	  //------------------------------------------------------------------------------
	  // Prepare the directory name for saving snapshot images
	  dirname = "raster/" + _headname;
	  //------------------------------------------------------------------------------
	  // Start to save the output image data
	  outname = dirname + "/out.png";
	  cerr << HERE << " outpu image (png) ======> : " << outname << endl;
	  _capture( outname.c_str() );
#endif	// ACTIVATE_RECORDING_MODE
	  break;
      // squaring building polygons
      case 'r': // == 114
      case 'R':
	  _fig->square();
	  _fig->triangulate();
#ifdef ACTIVATE_RECORDING_MODE
	  //------------------------------------------------------------------------------
	  // Prepare the directory name for saving snapshot images
	  dirname = "raster/" + _headname;
	  //------------------------------------------------------------------------------
	  // Start to save the output image data
	  outname = dirname + "/out.png";
	  cerr << HERE << " outpu image (png) ======> : " << outname << endl;
	  _capture( outname.c_str() );
#endif	// ACTIVATE_RECORDING_MODE
	  break;
      // load the map drawings from files
      case 'l': // == 108
      case 'L':
	  cerr << "Input the file name for loading data : ";
	  getline( cin, line );
	  _load_drawing( line.c_str() );
	  // isLoaded = false;
	  break;      
      // save the map drawings into files
      case 's': // == 115
      case 'S':
	  cerr << "Input the file name for saving data : ";
	  getline( cin, line );
	  _save_drawing( line.c_str() );
	  break;
      case 'j': // == 106
      case 'J':
	  _isConjoined = !_isConjoined;
	  if ( _isConjoined ) cerr << HERE << " isConjoined TRUE" << endl;
	  else cerr << HERE << " isConjoined FALSE" << endl;
	  break;
      case 'b': // == 98
      case 'B':
	  _isFilled = !_isFilled;
	  if ( _isFilled ) cerr << HERE << " Fill flag ON " << endl;
	  else cerr << HERE << " Fill flag OFF " << endl;
	  InitGL();
	  _glLayout->InitGL();
	  redrawAll();
#ifdef SKIP
	  if ( _isFilled ) {
	      cerr << HERE << " isFilled TRUE" << endl;
	      glutSetWindow( win_drawing );
	      glClearColor( 0.0, 0.0, 0.0, 1.0 );
	      glutSetWindow( win_design );
	      glClearColor( 0.0, 0.0, 0.0, 1.0 );
	      glutSetWindow( win_chart );
	      glClearColor( 0.0, 0.0, 0.0, 1.0 );
	  }
	  else {
	      cerr << HERE << " isFilled FALSE" << endl;
	      glutSetWindow( win_drawing );
	      glClearColor( 1.0, 1.0, 1.0, 1.0 );
	      glutSetWindow( win_design );
	      glClearColor( 1.0, 1.0, 1.0, 1.0 );
	      glutSetWindow( win_chart );
	      glClearColor( 1.0, 1.0, 1.0, 1.0 );
	  }
#endif	// SKIP
	  break;
      // switch flag for polygon wrapping
      case 'w': // == 119
      case 'W':
	  _isWrapped = !_isWrapped;
	  if ( _isWrapped ) cerr << HERE << " isWrapped TRUE" << endl;
	  else cerr << HERE << " isWrapped FALSE" << endl;
	  if ( _isWrapped ) _fig->initWrapper();
	  break;
      // switch flag for sample plotting
      case 'p': // == 112
      case 'P':
	  _isPlotted = !_isPlotted;
	  if ( _isPlotted ) cerr << HERE << " isPlotted TRUE" << endl;
	  else cerr << HERE << " isPlotted FALSE" << endl;
	  break;
      // capture the map drawing as image files
      case 'm': // == 109
      case 'M':
	  ostr << setw( 3 ) << setfill( '0' ) << counter++ << ends;
	  imgname = _headname + "-drawing-" + ostr.str().c_str() + ".png";
	  // cerr << " Input file name : " << ends;
	  // getline( cin, line );
	  // capture_drawing( line.c_str() );
	  cerr << " Capture the drawing window and save to " << imgname.c_str() << endl;
	  _capture( imgname.c_str() );
	  return;
      // select the preferred aggregation choices
      case 'f': // == 102
      case 'F':
	  cerr << HERE << " pickID = " << _worksp->pickID() << endl;
	  bandSet.clear();
	  globSet.clear();
#ifdef DEBUG
	  // cerr << HERE << " No. polygons in drawing = " << fig.poly().size() << endl;
	  cerr << HERE << " Number of polygons = "
	       << _worksp->coverBand()[ _worksp->pickID() ].size() << endl;
	  cerr << HERE << " Number of global sets = "
	       << _worksp->coverGlob()[ _worksp->pickID() ].size() << endl;
	  assert( _worksp->coverBand()[ _worksp->pickID() ].size() ==
		  _worksp->coverGlob()[ _worksp->pickID() ].size() );
#endif	// DEBUG
	  for ( unsigned int k = 0;
		k < _worksp->coverBand()[ _worksp->pickID() ].size(); ++k ) {
	      bandSet.push_back( _worksp->coverBand()[ _worksp->pickID() ][ k ] );
	      globSet.push_back( _worksp->coverGlob()[ _worksp->pickID() ][ k ] );
	  }
	  // cerr << HERE << " No. polygons in drawing = " << fig.poly().size() << endl;
	  _fig->aggregate( _worksp->pickID(), bandSet, globSet );
	  _fig->triangulate();
	  // cerr << HERE << " No. polygons in drawing = " << fig.poly().size() << endl;
	  _isometric( _fig->expand() );
	  // cerr << HERE << " No. polygons in drawing = " << fig.poly().size() << endl;
	  _worksp->clear();
	  cerr << HERE << " mode => free" << endl;
#ifdef ACTIVATE_RECORDING_MODE
	  //------------------------------------------------------------------------------
	  // Prepare the directory name for saving snapshot images
	  dirname = "raster/" + _headname;
	  //------------------------------------------------------------------------------
	  // Specify the file name for the input polygon data after generalization
	  aftname = "vector/" + _headname + "-out.dat";
	  cerr << HERE << " saved polygon after genralization ======> : "
	       << aftname << endl;
	  save_drawing( aftname.c_str() );
	  //------------------------------------------------------------------------------
	  // Start to save the output image data
	  outname = dirname + "/out.png";
	  cerr << HERE << " outpu image (png) ======> : " << outname << endl;
	  capture( outname.c_str() );
#endif	// ACTIVATE_RECORDING_MODE
	  break;
      // quit the program
      case 'q': // == 113
      case 'Q':
      case FL_Escape: // == 65307
	  exit( 0 );
	  break;
      default:
	  break;
    }

    redrawAll();
}


// Function for redrawing associative windows as well as this one
void GLDrawing::redrawAll( void )
{
    redraw();
    if ( _glLayout != NULL ) _glLayout->redraw();
}


// end of header file
// Do not add any stuff under this line.
