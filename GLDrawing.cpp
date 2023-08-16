//==============================================================================
// GLDrawing.cpp
//	: program file for the drawing window
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-08-08 14:28:38 shigeo"
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

// #include <FL/filename.H>		// fl_open_uri()

#include "CSVIO.h"
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
// Draw the neighbor graph
void GLDrawing::_draw_vertex_ids( Network & g )
{
    NetVertexIDMap	vertexID	= get( vertex_myid, g );
    NetVertexCntrMap	vertexCntr	= get( vertex_mycntr, g );

    // Annotating vertices of the proximity graph
    BGL_FORALL_VERTICES( vd, g, Network ) {
	unsigned int id = vertexID[ vd ];
	Point2 & point = vertexCntr[ vd ];
	ostringstream strID;
	strID << "#" << setw( 2 ) << id << ends;
	_string2D( point.x(), point.y(), strID.str().c_str() );
    }
}


// Draw the neighbor graph
void GLDrawing::_draw_vertex_ids( Directed & g )
{
    DirVertexAddrMap	vertexAddr	= get( vertex_myaddr, g );
    DirVertexPointMap	vertexPoint	= get( vertex_mypoint, g );

    // Annotating vertices of the proximity graph
    BGL_FORALL_VERTICES( vd, g, Directed ) {
	unsigned int id = vertexAddr[ vd ];
	Point2 & point = vertexPoint[ vd ];
	ostringstream strID;
#ifdef DEBUGGING_PHASE
	if ( ( id == 234 ) || ( id == 235 ) || ( id == 289 ) ) glColor3d( 1.0, 0.5, 0.0 );
	else glColor3d( 0.0, 1.0, 0.5 );
#endif	// DEBUGGING_PHASE
	strID << "#" << setw( 2 ) << id << ends;
	_string2D( point.x(), point.y(), strID.str().c_str() );
    }
}


// Draw the neighbor graph
void GLDrawing::_draw_network( Network & g )
{
    // VertexIDMap	vertexID	= get( vertex_myid, g );
    NetVertexCntrMap	vertexCntr	= get( vertex_mycntr, g );

    cerr << HERE << " draw_network (for Network) " << endl;

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


// Draw the set of convex hulls
void GLDrawing::_draw_hulls( vector< Polygon2 > & hull )
{
    // cerr << HERE << " Number of hulls = " << hull.size() << endl;
    for ( unsigned int k = 0; k < hull.size(); ++k ) {
	_draw_polygon( hull[ k ] );
    }
}


//------------------------------------------------------------------------------
//	Functions for File I/O
//------------------------------------------------------------------------------
// retrieve the head name of the input file name
void GLDrawing::_retrieve_headname( const char * args )
{
    string inputname = args;
    vector< string > slashlist = CSVIO::split( inputname, '/' );
    string lastblock;
    for ( vector< string >::iterator iterS = slashlist.begin();
	  iterS != slashlist.end(); iterS++ ) {
	lastblock = (*iterS);
    }
    vector< string > dotlist = CSVIO::split( lastblock, '.' );
    vector< string >::iterator iterD = dotlist.begin();
    if ( iterD != dotlist.end() ) {
	_headname = (*iterD);
    }
    cerr << HERE << " headname ======> : " << _headname << endl;
}


// load a drawging from the given file
void GLDrawing::_load_drawing( const char * filename )
{
    _retrieve_headname( filename );

    ifstream ifs( filename );
    istringstream istr;
    string line;
    
    if ( ! ifs ) {
        cerr << HERE << " cannot open the file " << filename << endl;
        return;
    }

    // load the number of points first of all
    getline( ifs, line );
    istr.clear();
    istr.str( line );
    istr >> _nPolys;
    cerr << " Number of polygons = " << _nPolys << endl;

    _fig->clear();
    
    // load the coordinates of the points
    unsigned int countID = 0;
    for ( unsigned int i = 0; i < _nPolys; ++i ) {
	unsigned int nPoints;
	getline( ifs, line );
	istr.clear();
	istr.str( line );
	istr >> nPoints;
#ifdef DEBUG
	cerr << "[ " << setw( 3 ) << i << " ] : Number of points = " << nPoints << endl;
#endif	// DEBUG
	Polygon2 poly;
	for ( unsigned int j = 0; j < nPoints; ++j ) {
	    double px, py;
	    getline( ifs, line );
	    istr.clear();
	    istr.str( line );
	    istr >> px >> py;
#ifdef TENTATIVE_COORDINATE_NORMALIZATION
	    // px = 2.0 * ( double )px/( double )FULL_WIDTH - 1.0;
	    // py = 1.0 - 2.0 * ( double )py/( double )FULL_HEIGHT;
	    px = px * 2.0 - 1.0;
	    py = py * 2.0 - 1.0;
#endif	// TENTATIVE_COORDINATE_NORMALIZATION
	    // cerr << HERE << " px = " << px << " py = " << py << endl;
	    poly.push_back( Point2( px, py ) );
	}

	if ( poly.orientation() != CGAL::COUNTERCLOCKWISE ) {
	    cerr << HERE << "%%%%% Polygon No. " << _fig->poly().size() << " CW " << endl;

	    poly.reverse_orientation();
	}

	assert( poly.orientation() == CGAL::COUNTERCLOCKWISE );
	
#ifdef RESAMPLE_BOUNDARY
	const double div = RESAMPLE_INTERVAL;
	// Resample the polygon boundary;
	Polygon2 fine;
	unsigned int sz = poly.size();
	for ( unsigned int j = 0; j < sz; ++j ) {
	    fine.push_back( poly[ j ] );
	    double length = sqrt( ( poly[ (j+1)%sz ] - poly[ j ] ).squared_length() );
	    int nDiv = ceil( length / div );
	    for ( unsigned int k = 1; k < nDiv; ++k ) {
		double t = ( double )k/( double )nDiv;
		Point2 newP = CGAL::ORIGIN +
		    (1.0 - t)*( poly[ j ] - CGAL::ORIGIN ) +
		    t*(poly[ (j+1)%sz ] - CGAL::ORIGIN );
		fine.push_back( newP );
	    }
	}
	// cerr << HERE << " poly = " << poly << endl;
	// cerr << HERE << " fine = " << fine << endl;
	Set glob;
	for ( unsigned int j = 0; j < fine.size(); ++j ) {
	    glob.push_back( countID++ );
	}
	_fig->poly().push_back( fine );
	_fig->glID().push_back( glob );
#else	// RESAMPLE_BOUNDARY
	Set glob;
	for ( unsigned int j = 0; j < poly.size(); ++j ) {
	    glob.push_back( countID++ );
	}
	_fig.poly().push_back( poly );
	_fig.glID().push_back( glob );
#endif	// RESAMPLE_BOUNDARY
    }
    ifs.close();

    _fig->bound() = _fig->poly();
    cerr << HERE << " Finished loading the data!" << endl;  
}

// save a drawging into the given file
void GLDrawing::_save_drawing( const char * filename )
{
    ofstream ofs( filename );

    if ( ! ofs ) {
        cerr << HERE << " cannot open the file " << filename << endl;
        return;
    }

    ofs << _fig->poly().size() << endl;
    for ( unsigned int i = 0; i < _fig->poly().size(); ++i ) {
	ofs << _fig->poly()[ i ].size() << endl;
	for ( unsigned int j = 0; j < _fig->poly()[ i ].size(); ++j ) {
	    ofs << fixed << setprecision( 4 ) << _fig->poly()[ i ][ j ].x();
	    ofs << "\t";
	    ofs << fixed << setprecision( 4 ) << _fig->poly()[ i ][ j ].y();
	    ofs << endl;
	}
    }
    ofs.close();

    cerr << " Finished saving the data!" << endl;  
}


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
#ifdef USE_CONCAVE_HULLS
    _worksp->coverBand().clear();
#endif	// USE_CONCAVE_HULLS
    for ( unsigned int i = 0; i < expand.size(); ++i ) {
	vector< Polygon2 > hullSet;
#ifdef USE_CONCAVE_HULLS
	vector< Polygon2 > bandSet;
	vector< Set >	   globSet;
#endif	// USE_CONCAVE_HULLS
	vector< Set > polySet = expand[ i ].cluster();
	for ( unsigned int j = 0; j < polySet.size(); ++j ) {
	    Polygon2 eachHull;
#ifdef USE_CONVEX_HULLS
	    Drawing::convexForLabel( fig.netNbr(), polySet[ j ], eachHull );
#endif	// USE_CONVEX_HULLS
#ifdef USE_CONCAVE_HULLS
	    Set eachGlob;
	    _fig->concaveForLabel( _fig->netNbr(), polySet[ j ], eachHull, eachGlob );
	    bandSet.push_back( eachHull );
	    globSet.push_back( eachGlob );
#endif	// USE_CONCAVE_HULLS
	    hullSet.push_back( eachHull );
	}
	_worksp->coverHull().push_back( hullSet );
#ifdef USE_CONCAVE_HULLS
	_worksp->coverBand().push_back( bandSet );
	_worksp->coverGlob().push_back( globSet );
#endif	// USE_CONCAVE_HULLS
    }
#ifndef USE_CONVEX_HULLS
    cerr << HERE << " coverBand.size = " << _worksp->coverBand().size() << endl;
#endif	// USE_CONVEX_HULLS
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
	for ( unsigned int i = 0; i < _fig->bound().size(); ++i ) {
	    glBegin( GL_LINE_LOOP );
	    for ( unsigned int j = 0; j < _fig->bound()[ i ].size(); j++ ) {
		glVertex2d( _fig->bound()[ i ][ j ].x(), _fig->bound()[ i ][ j ].y() );
	    }
	    glEnd();
	}
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
	draw_vertex_ids( _fig.wrapper() );
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
	glColor4d( 0.3, 0.3, 0.3, 0.4 );
	glLineWidth( 1.0 );
	// draw_network( netN );
	_draw_network( _fig->netNbr() );
	
	glPushMatrix();
	glTranslated( -offset, -offset, 0.0 );
	glColor4d( 1.0, 0.5, 0.0, 0.7 );
	glLineWidth( 5.0 );
	// draw_network( netP );
	_draw_network( _fig->netPrx() );
	glPopMatrix();
    
#ifdef USING_SIMILARITY_CONJOINING
	glPushMatrix();
	glTranslated( offset, offset, 0.0 );
	glColor4d( 0.0, 0.0, 1.0, 0.7 );
	glLineWidth( 5.0 );
	// draw_network( netS );
	_draw_network( _fig->netSim() );
	glPopMatrix();
#endif	// USING_SIMILARITY_CONJOINING
	
	// drawing concave polygons
	glColor4d( 0.0, 0.5, 0.0, 0.8 );
	glLineWidth( 2.0 );
	_draw_hulls( _fig->hullPrx() );
#ifdef USING_SIMILARITY_CONJOINING
	_draw_hulls( _fig->hullSim() );
#endif	// USING_SIMILARITY_CONJOINING
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
void GLDrawing::Motion( int x, int y )
{
    cerr << HERE << "GLDrawing::Motion" << endl;
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
    string		inname, outname, imgname, dirname;
    struct stat		statbuf;
    
    cerr << HERE << "GLDrawing::Keyboard" << endl;
    make_current();

    switch ( key ) {
      case 'c':
	  _fig->clear();
	  _nPolys = 0;
	  break;
      // Calls this after loading the line drawing data
      case 'a':
      case 'A':
	  // cerr << HERE << " No. polygons in drawing = " << fig.poly().size() << endl;
	  _fig->conjoin();
	  // cerr << HERE << " No. polygons in drawing = " << fig.poly().size() << endl;
	  _fig->optimize();
	  // cerr << HERE << " No. polygons in drawing = " << fig.poly().size() << endl;
	  _isometric( _fig->expand() );
	  // cerr << HERE << " No. polygons in drawing = " << fig.poly().size() << endl;
#ifdef RECORD_SNAPSHOTS
	  inname = "data/" + headname + "-in.dat";
	  cerr << HERE << " inname data ======> : " << inname << endl;
	  save_drawing( inname.c_str() );
	  dirname = "png/" + headname;
	  if ( ! stat( dirname.c_str(), &statbuf ) ) {
	      cerr << HERE << dirname << " already created" << endl;
	  }
	  else {
	      if ( ! mkdir( dirname.c_str(), 0755 ) ) {
		  cerr << HERE << dirname << " successfully created" << endl;
	      }
	  }
	  inname = dirname + "/in.png";
	  cerr << HERE << " inname png ======> : " << inname << endl;
	  capture_drawing( inname.c_str() );
#endif	// RECORD_SNAPSHOTS
	  // Delete the last aggregation
	  // fig.expand().erase( fig.expand().end() - 1 );
	  // Hierarchical clustering
	  _worksp->dendrogram().set( _fig->expand() );
	  _worksp->dendrogram().merge();
	  _worksp->cluster() = _worksp->dendrogram().retrieve( _adjust->cutThreshold() );
	  // cerr << HERE << " cluster.size = " << cluster.size() << endl;
	  break;
      case 'e':			// exhaustive simplify
	  _fig->simplify();
	  _fig->triangulate();
	  break;
      case 'r':			// squaring building polygons
	  _fig->square();
	  _fig->triangulate();
	  break;
      case 'o':
	  _fig->optimize();
	  _isometric( _fig->expand() );
	  break;
      case 'l':
	  cerr << "Input the file name for loading data : ";
	  getline( cin, line );
	  _load_drawing( line.c_str() );
	  // isLoaded = false;
	  break;      
      case 's':
	  cerr << "Input the file name for saving data : ";
	  getline( cin, line );
	  _save_drawing( line.c_str() );
	  break;
      case 'j':
	  _isConjoined = !_isConjoined;
	  if ( _isConjoined ) cerr << HERE << " isConjoined TRUE" << endl;
	  else cerr << HERE << " isConjoined FALSE" << endl;
	  break;
      case 'F':
	  _isFilled = !_isFilled;
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
      case 'W':
	  _isWrapped = !_isWrapped;
	  if ( _isWrapped ) cerr << HERE << " isWrapped TRUE" << endl;
	  else cerr << HERE << " isWrapped FALSE" << endl;
	  if ( _isWrapped ) _fig->initWrapper();
	  break;
      case 'P':
	  _isPlotted = !_isPlotted;
	  if ( _isPlotted ) cerr << HERE << " isPlotted TRUE" << endl;
	  else cerr << HERE << " isPlotted FALSE" << endl;
	  break;
      case 'D':
	  ostr << setw( 3 ) << setfill( '0' ) << counter++ << ends;
	  imgname = _headname + "-drawing-" + ostr.str().c_str() + ".png";
	  // cerr << " Input file name : " << ends;
	  // getline( cin, line );
	  // capture_drawing( line.c_str() );
	  cerr << " Capture the drawing window and save to " << imgname.c_str() << endl;
	  _capture( imgname.c_str() );
	  return;
      case 'f':
	  cerr << HERE << " pickID = " << _worksp->pickID() << endl;
	  bandSet.clear();
	  globSet.clear();
#ifdef USE_CONVEX_HULLS
	  fig.aggregate( _worksp->pickID() );
	  fig.triangulate();
#else // USE_CONVEX_HULLS
	  // cerr << HERE << " No. polygons in drawing = " << fig.poly().size() << endl;
	  cerr << HERE << " Number of polygons = "
	       << _worksp->coverBand()[ _worksp->pickID() ].size() << endl;
	  for ( unsigned int k = 0;
		k < _worksp->coverBand()[ _worksp->pickID() ].size(); ++k ) {
	      bandSet.push_back( _worksp->coverBand()[ _worksp->pickID() ][ k ] );
	      globSet.push_back( _worksp->coverGlob()[ _worksp->pickID() ][ k ] );
	  }
	  // cerr << HERE << " No. polygons in drawing = " << fig.poly().size() << endl;
	  _fig->aggregate( _worksp->pickID(), bandSet, globSet );
	  _fig->triangulate();
	  // cerr << HERE << " No. polygons in drawing = " << fig.poly().size() << endl;
#endif	// USE_CONVEX_HULLS
	  _isometric( _fig->expand() );
	  // cerr << HERE << " No. polygons in drawing = " << fig.poly().size() << endl;
	  _worksp->clear();
	  cerr << HERE << " mode => free" << endl;
#ifdef RECORED_SNAPSHOTS
	  outname = "data/" + headname + "-out.dat";
	  cerr << HERE << " outname ======> : " << outname << endl;
	  save_drawing( outname.c_str() );
	  dirname = "png/" + headname;
	  outname = dirname + "/out.png";
	  cerr << HERE << " outname ======> : " << outname << endl;
	  capture_drawing( outname.c_str() );
#endif	// RECORED_SNAPSHOTS
	  break;
      case 'q':
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
