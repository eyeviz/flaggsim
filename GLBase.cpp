//==============================================================================
// GLBase.cpp
//	: program file for GL Window basement
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-10-31 14:50:00 shigeo"
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

#include "CSVIO.h"
#include "GLBase.h"


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Static Variables
//------------------------------------------------------------------------------
string GLBase::_headname;

//------------------------------------------------------------------------------
//	Private Functions
//------------------------------------------------------------------------------
// function for handling redrawing events
void GLBase::draw( void )
{
    if ( !context_valid() ) {
	// context_valid() : OpenGLコンテキストが生成された際にfalseとなる．draw関数呼び出し後trueになる．
	InitGL();
    }
    if ( !valid() ) {
	// valid() : OpenGLコンテキストが生成 or リサイズされた際にfalseとなる．draw関数呼び出し後trueになる．
	Resize(w(), h());
    }
    
    // OpenGL描画
    Display();
}


// function for handling resizing events
void GLBase::resize(int x_, int y_, int w_, int h_)
{
    Fl_Gl_Window::resize(x_, y_, w_, h_);
    //Resize(w_, h_);	// リサイズ処理はこちらにおいてもよい．
}


// Event handler
int GLBase::handle( int ev )
{
    switch ( ev ) {
      case FL_PUSH:
	  cerr << HERE << "[Push] FL event button = " << Fl::event_button() << endl;
	  Mouse( Fl::event_button(), 1, Fl::event_x(), Fl::event_y() );
	  break;
      case FL_RELEASE:
	  cerr << HERE << "[Release] FL event button = " << Fl::event_button() << endl;
	  Mouse( Fl::event_button(), 0, Fl::event_x(), Fl::event_y() );
	  break;
      case FL_DRAG:
	  Motion( Fl::event_x(), Fl::event_y() );
	  break;
#ifdef WITH_PASSIVE_MOTION
      case FL_MOVE:
	  PassiveMotion( Fl::event_x(), Fl::event_y() );
	  break;
#endif	// SKIP_PASSIVE_MOTION
      case FL_KEYDOWN:
	  Keyboard( Fl::event_key(), Fl::event_x(), Fl::event_y() );
	  break;
      case FL_KEYUP:
      case FL_SHORTCUT:
	  break;
      case FL_FOCUS:
      case FL_UNFOCUS:
	  break;
      default:
	  return Fl_Window::handle( ev );
    }

    return 1;
}


//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------
// Function for placing character strings
void GLBase::_string2D( double x, double y, const char *str, int size )
{
    double basex = x;
    double basey = y;
    gl_font( 1, size );
    glRasterPos2d( basex, basey );
    // gl_draw( str, strlen( str ) );
    gl_draw( str );
    
#ifdef SKIP
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
#endif	// SKIP
}


// Draw the set of convex hulls
void GLBase::_draw_polygon( Polygon2 & poly )
{
    // cerr << HERE << " Number of corners = " << poly.size() << endl;

    glBegin( GL_LINE_LOOP );
    for ( unsigned int i = 0; i < poly.size(); ++i ) {
	glVertex2d( poly[ i ].x(), poly[ i ].y() );
    }
    glEnd();
}


//------------------------------------------------------------------------------
//	Functions for File I/O
//------------------------------------------------------------------------------
// retrieve the head name of the input file name
void GLBase::_retrieve_headname( const char * args )
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
void GLBase::_load_drawing( const char * filename )
{
    cerr << HERE << " load_drawing => " << filename << endl;

    _retrieve_headname( filename );
    
    if ( _fig == NULL ) {
	cerr << HERE << " NULL pointer to the line drawing" << endl;
	exit( 0 );
    }
    
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

    // Final post process after loading the data from files
    _fig->triangulate();
}

// save a drawging into the given file
void GLBase::_save_drawing( const char * filename )
{
    if ( _fig == NULL ) {
	cerr << HERE << " NULL pointer to the line drawing" << endl;
	exit( 0 );
    }

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


// Capture the window as a image file
void GLBase::_capture( const char * name )
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

    make_current();

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
//  GLBase::GLBase --	default constructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
GLBase::GLBase( int _x, int _y, int _w, int _h, const char *_l )
    : Fl_Gl_Window( _x, _y, _w, _h, _l )  	
{
    _flwin.clear();
    _isFilled		= false;
    // _isFilled		= true;

    _fig		= NULL;
    _worksp		= NULL;
    _adjust		= NULL;

    resizable( this );
    end();
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------

//
//  GLBase::~GLBase --	destructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
GLBase::~GLBase()
{
    _flwin.clear();
}


//------------------------------------------------------------------------------
//	Referrring to members
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	OpenGL functions
//------------------------------------------------------------------------------
// function for initialized GL setups
void GLBase::InitGL( void )
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
void GLBase::Resize(int w, int h)
{
    cerr << HERE << " Resize " << w << " x " << h << endl;
    glViewport( 0, 0, w, h );
    
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluOrtho2D( -1.0, 1.0, -1.0, 1.0 );
    // glOrtho( -1.0, 1.0, -1.0, 1.0, -1.0, 1.0 );
    
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}


// Function for drawing the window
void GLBase::Display( void )
{
    cerr << HERE << " in GLBase::Display" << endl;

    glClear( GL_COLOR_BUFFER_BIT );
    
    // Skip the swap buffers
    // glutSwapBuffers();
}

// Function for handling mouse events
void GLBase::Mouse( int button, int state, int x, int y )
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
void GLBase::Motion( int x, int y )
{
    cerr << HERE << "GLBase::Motion" << endl;
}

// Function for handling mouse moving events
void GLBase::PassiveMotion( int x, int y )
{
    cerr << HERE << "GLBase::PassiveMotion" << endl;
}

// Function for handling keyboard events
void GLBase::Keyboard( int key, int x, int y )
{
    cerr << HERE << "GLBase::Keyboard" << endl;
    make_current();

    cerr << HERE << " key = " << key << endl;

    switch ( key ) {
      case 'q':
	  exit( 0 );
	  break;
      default:
	  break;
    }

    redraw();
}

// Function for redrawing associative windows as well as this one
void GLBase::redrawAll( void )
{
    redraw();
    // Redrawing other associative windows
    // cerr << HERE << " size of associative windows = " << _flwin.size() << endl;
    for ( unsigned int i = 0; i < _flwin.size(); ++i )
	if ( _flwin[ i ]->valid() ) _flwin[ i ]->redraw();
}

// end of header file
// Do not add any stuff under this line.
