//==============================================================================
// GLDrawing.cpp
//	: program file for the drawing window
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-08-06 18:55:32 shigeo"
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

#include <FL/filename.H>		// fl_open_uri()

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
//! 再描画イベント処理関数
void GLDrawing::draw( void )
{
    if(!context_valid()){
	// context_valid() : OpenGLコンテキストが生成された際にfalseとなる．draw関数呼び出し後trueになる．
	InitGL();
    }
    if(!valid()){
	// valid() : OpenGLコンテキストが生成 or リサイズされた際にfalseとなる．draw関数呼び出し後trueになる．
	Resize(w(), h());
    }
    
    // OpenGL描画
    Display();
}


//! リサイズイベント処理関数
void GLDrawing::resize(int x_, int y_, int w_, int h_)
{
    Fl_Gl_Window::resize(x_, y_, w_, h_);
    //Resize(w_, h_);	// リサイズ処理はこちらにおいてもよい．
}


//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------
void GLDrawing::_string2D( double x, double y, const char *str )
{
    double basex = x;
    double basey = y;
    glRasterPos2d( basex, basey );

    for (; *str != 0; str++) {
        if ( *str == '\n' ) {
            glRasterPos2i( basex, basey );
        }
        else {
            // glutBitmapCharacter( FUTL_FONT_TYPE, *str );
            // GLUT_BITMAP_8_BY_13
            // GLUT_BITMAP_9_BY_15
            // GLUT_BITMAP_TIMES_ROMAN_10
            // GLUT_BITMAP_TIMES_ROMAN_24
            // GLUT_BITMAP_HELVETICA_10
            // GLUT_BITMAP_HELVETICA_12
            // GLUT_BITMAP_HELVETICA_18
            // glutBitmapCharacter( GLUT_BITMAP_8_BY_13, *str );
            // glutBitmapCharacter( GLUT_BITMAP_9_BY_15, *str );
            // glutBitmapCharacter( GLUT_BITMAP_HELVETICA_10, *str );
            // glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12, *str );
            glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, *str );
            // glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_10, *str );
            // glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, *str );
        }
    }
}


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
void GLDrawing::_draw_polygon( Polygon2 & poly )
{
    // cerr << HERE << " Number of corners = " << poly.size() << endl;
    glBegin( GL_LINE_LOOP );
    for ( unsigned int i = 0; i < poly.size(); ++i ) {
	glVertex2d( poly[ i ].x(), poly[ i ].y() );
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
// load a drawging from the given file
void GLDrawing::_load_drawing( const char * filename )
{
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

    _fig.clear();
    
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
	    cerr << HERE << "%%%%% Polygon No. " << _fig.poly().size() << " CW " << endl;

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
	_fig.poly().push_back( fine );
	_fig.glID().push_back( glob );
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

    _fig.bound() = _fig.poly();
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

    ofs << _fig.poly().size() << endl;
    for ( unsigned int i = 0; i < _fig.poly().size(); ++i ) {
	ofs << _fig.poly()[ i ].size() << endl;
	for ( unsigned int j = 0; j < _fig.poly()[ i ].size(); ++j ) {
	    ofs << fixed << setprecision( 4 ) << _fig.poly()[ i ][ j ].x();
	    ofs << "\t";
	    ofs << fixed << setprecision( 4 ) << _fig.poly()[ i ][ j ].y();
	    ofs << endl;
	}
    }
    ofs.close();

    cerr << " Finished saving the data!" << endl;  
}

// Capture the window as a image file
void GLDrawing::_capture( const char * name )
{
    static cv::Mat              image;          // Mesh image
    static GLubyte *            pixel   = NULL;

    // glutSetWindow( win_drawing );   

    // unsigned int		wx = glutGet( GLUT_WINDOW_X );
    // unsigned int		wy = glutGet( GLUT_WINDOW_Y );
    // unsigned int		ww = glutGet( GLUT_WINDOW_WIDTH );
    // unsigned int		wh = glutGet( GLUT_WINDOW_HEIGHT );
    unsigned int		ww = this->w();
    unsigned int		wh = this->h();
    // const unsigned int          nChannels = 3;
    const unsigned int          nChannels = 4;

    Display();
    Display();

    cerr << HERE << " Window size : " << ww << " x " << wh << endl;

    if ( pixel == NULL ) pixel = new GLubyte [ ww * wh * nChannels ];
    // glReadPixels( 0, 0, ww, wh, GL_RGB, GL_UNSIGNED_BYTE, pixel );
    glReadPixels( 0, 0, ww, wh, GL_RGBA, GL_UNSIGNED_BYTE, pixel );

    // image = cv::Mat( cv::Size( ww, wh ), CV_8UC3 );
    image = cv::Mat( cv::Size( ww, wh ), CV_8UC4 );
    memcpy( image.data, pixel, ww * wh * nChannels );

    cv::cvtColor( image, image, cv::COLOR_BGR2RGB );
    cv::flip( image, image, 0 );
    cv::imwrite( name, image );

    cerr << "Capturing the drawing window as " << name << " ... done." << endl;
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
    : Fl_Gl_Window( _x, _y, _w, _h, _l )  	
{
    _isConjoined	= false;
    _isFilled		= false;
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
	for ( unsigned int i = 0; i < _fig.tri().size(); ++i ) {
	    for ( unsigned int j = 0; j < _fig.tri()[ i ].size(); ++j ) {
		glBegin( GL_POLYGON );
		for ( unsigned int k = 0; k < 3; ++k ) {
		    glVertex2d( _fig.tri()[i][j][k].x(), _fig.tri()[i][j][k].y() );
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
	cerr << HERE << " _fig.bound().size() = " << _fig.bound().size() << endl;
	for ( unsigned int i = 0; i < _fig.bound().size(); ++i ) {
	    glBegin( GL_LINE_LOOP );
	    for ( unsigned int j = 0; j < _fig.bound()[ i ].size(); j++ ) {
		glVertex2d( _fig.bound()[ i ][ j ].x(), _fig.bound()[ i ][ j ].y() );
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
	for ( unsigned int i = 0; i < _fig.bound().size(); ++i ) {
	    for ( unsigned int j = 0; j < _fig.bound()[ i ].size(); j++ ) {
		glVertex2d( _fig.bound()[ i ][ j ].x(), _fig.bound()[ i ][ j ].y() );
	    }
	}
	glEnd();
    }

#ifdef SKIP
    if ( pickID != NO_NAME ) {
	unsigned int nSegs = coverBand[ pickID ].size();
	for ( unsigned int i = 0; i < nSegs; ++i ) {
	    glColor3d( 1.0, 0.5, 0.0 );
	    glLineWidth( 7.0 );
	    Polygon2 curPoly = coverBand[ pickID ][ i ];
	    draw_polygon( curPoly );
	}
    }
#endif	// SKIP

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
	_draw_directed( _fig.wrapper() );
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
	_draw_vertex_ids( _fig.netNbr() );
	
	// glColor4d( 1.0, 0.5, 0.0, 0.4 );
	glColor4d( 0.3, 0.3, 0.3, 0.4 );
	glLineWidth( 1.0 );
	// draw_network( netN );
	_draw_network( _fig.netNbr() );
	
	glPushMatrix();
	glTranslated( -offset, -offset, 0.0 );
	glColor4d( 1.0, 0.5, 0.0, 0.7 );
	glLineWidth( 5.0 );
	// draw_network( netP );
	_draw_network( _fig.netPrx() );
	glPopMatrix();
    
#ifdef USING_SIMILARITY_CONJOINING
	glPushMatrix();
	glTranslated( offset, offset, 0.0 );
	glColor4d( 0.0, 0.0, 1.0, 0.7 );
	glLineWidth( 5.0 );
	// draw_network( netS );
	_draw_network( _fig.netSim() );
	glPopMatrix();
#endif	// USING_SIMILARITY_CONJOINING
	
	// drawing concave polygons
	glColor4d( 0.0, 0.5, 0.0, 0.8 );
	glLineWidth( 2.0 );
	_draw_hulls( _fig.hullPrx() );
#ifdef USING_SIMILARITY_CONJOINING
	_draw_hulls( _fig.hullSim() );
#endif	// USING_SIMILARITY_CONJOINING
    }

    // for disabling antialiasing
    glDisable( GL_LINE_SMOOTH );
    glDisable( GL_BLEND );
    glEnable( GL_DEPTH_TEST );

    // Skip the swap buffers
    // glutSwapBuffers();
}


// end of header file
// Do not add any stuff under this line.
