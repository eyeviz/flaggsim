//==============================================================================
// main.cpp
//	: main program file
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-08-07 22:27:20 shigeo"
//
//==============================================================================

#include <cstdlib>
#include <cmath>
#include <sys/stat.h>

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
// for sleep
#include <chrono>
#include <thread>

using namespace std;

// OpenGL library
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// FLTK library
#include <FL/gl.h>
#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/Fl_Menu_Bar.H>
// #include <FL/filename.H>		// fl_open_uri()


// OpenCV library
#include <opencv2/opencv.hpp>

#include "GLDrawing.h"
#include "GLLayout.h"
#include "FLControl.h"


//------------------------------------------------------------------------------
//	Macro definitions
//------------------------------------------------------------------------------
#ifndef HERE
#define HERE            " " << __FILE__ << ":" << __LINE__ << " "
#endif  // HERE

#define WINDOW_LEFT_LIMIT	(50)
#define WINDOW_TOP_LIMIT	(50)

#define HALF_WIDTH		(256)
#define HALF_HEIGHT		(256)
#define FULL_WIDTH		(512)
#define FULL_HEIGHT		(512)
#define HUGE_WIDTH		(768)
#define HUGE_HEIGHT		(768)
#define TEST_WIDTH		(1024)
#define TEST_HEIGHT		(1024)

#define WINDOW_MARGIN		(0.2)
#define PICKING_ERROR		(20.0)


//------------------------------------------------------------------------------
//	Type definitions
//------------------------------------------------------------------------------
//typedef enum {
//FREE, SELECTED
//} PickMode;


//------------------------------------------------------------------------------
//	GLobal variables
//------------------------------------------------------------------------------
Fl_Window *	win_map		= NULL;
GLDrawing *	gl_drawing	= NULL;
Fl_Window *	win_design	= NULL;
GLLayout *	gl_layout	= NULL;

Drawing *	fig		= NULL;
Workspace *	worksp		= NULL;

int		map_width	= HUGE_WIDTH;
int		map_height	= HUGE_HEIGHT;
int		design_width	= HUGE_WIDTH;
int		design_height	= HUGE_HEIGHT;
int		panel_width	= HUGE_WIDTH/2;
int		panel_height	= HUGE_HEIGHT;

//------------------------------------------------------------------------------
//	SKIPPING FROM HERE
//------------------------------------------------------------------------------
#ifdef SKIP

#include "CGAL.h"
#include "Network.h"
#include "Set.h"
#include "Drawing.h"
#include "Linkage.h"

#include "Config.h"
#include "CSVIO.h"

//------------------------------------------------------------------------------
//	Macro switches
//------------------------------------------------------------------------------
#define USING_MIPMAPS

// #define SHOW_SAMPLE_IDS


#define	NUM_OPTIONS_IN_LINE	(3)
// #define	NUM_OPTIONS_IN_LINE	(4)

// #define DEBUGGING_PHASE
// #define PRINT_COSTS

#ifdef DEBUGGING_PHASE
#define TENTATIVE_COORDINATE_NORMALIZATION
#endif	// DEBUGGING_PHASE

//------------------------------------------------------------------------------
//	Macro definitions
//------------------------------------------------------------------------------
#ifndef HERE
#define HERE            " " << __FILE__ << ":" << __LINE__ << " "
#endif  // HERE

//------------------------------------------------------------------------------
//	variables for GLUT
//------------------------------------------------------------------------------


// mouse position
Point2		drawing_cursor;
Point2		design_cursor;
Point2		chart_cursor;

// mouse buttons
int		design_left = 0, design_middle = 0, design_right = 0;
int		chart_left = 0,  chart_middle = 0,  chart_right = 0;


// pick ID
int				pickID = NO_NAME;
vector< Point2 >		pickCoord;
bool				isPressed = false;
PickMode			mode = FREE;
vector< vector< Polygon2 > >	coverHull;

#ifdef USE_CONCAVE_HULLS
vector< vector< Polygon2 > >	coverBand;
vector< vector< Set > >		coverGlob;
#endif	// USE_CONCAVE_HULLS

string				headname;

//------------------------------------------------------------------------------
//	variables for abstracting line drawings
//------------------------------------------------------------------------------

// Polygon Data
// vector< vector< Point2 > > fig;
Drawing			fig;
Linkage			dendrogram;
vector< Set >		cluster;
// double			cut_threshold = 20.0;		

unsigned int		nPolys = 0;

// window IDs
static int		win_drawing;
static int		win_design;	
static int		win_chart;

// texture attributes
// static GLuint *		textureID	= NULL;
static bool		isLoaded	= false;
static vector< unsigned int > priority;

// retina display handling
static bool		isRetina	= true;

// Gestalt handling
static bool		isConjoined	= false;
static bool		isFilled	= false;
static bool		isWrapped	= false;
//static bool		isWrapped	= true;
static bool		isPlotted	= false;

//int num_options_in_line	= NUM_OPTIONS_IN_LINE;


//#define WITH_MIN_LIMITS
//------------------------------------------------------------------------------
//	GLUI interface
//------------------------------------------------------------------------------
GLUI *		glui = NULL;
const char *	GLUI_TITLE	= { "Panel" };
const int	GLUI_TRUE	= { true  };
const int	GLUI_FALSE	= { false };

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
double		cut_threshold		= 20.0;
float		cut_threshold_spinner	= ( float )cut_threshold;
const float	cut_threshold_ceil	=  5.0;
const float	cut_threshold_roof	= 50.0;
// ------------------------------
int		linkage_switch		= 0;


GLUI_Panel	* rootPanel		= NULL;

GLUI_Panel	* intervalPanel		= NULL;
GLUI_Spinner	* limitInterval		= NULL;

GLUI_Panel	* dataCostPanel		= NULL;
#ifdef WITH_MIN_LIMITS
GLUI_Spinner	* minDataCost		= NULL;
#endif	// WITH_MIN_LIMITS
GLUI_Spinner	* maxDataCost		= NULL;

GLUI_Panel	* smoothCostPanel	= NULL;
#ifdef WITH_MIN_LIMITS
GLUI_Spinner	* minSmoothCost		= NULL;
#endif	// WITH_MIN_LIMITS
GLUI_Spinner	* maxSmoothCost		= NULL;

GLUI_Panel	* labelCostPanel	= NULL;
#ifdef WITH_MIN_LIMITS
GLUI_Spinner	* minLabelCost		= NULL;
#endif	// WITH_MIN_LIMITS
GLUI_Spinner	* maxLabelCost		= NULL;

GLUI_Panel	* cutThresholdPanel	= NULL;
GLUI_Spinner	* cutThresholdValue	= NULL;

GLUI_Panel	* linkagePanel	= NULL;
GLUI_RadioGroup	* linkageRadio	= NULL;

//------------------------------------------------------------------------------
//	Function declarations
//------------------------------------------------------------------------------
void	isometric	( vector< Expansion > & expand );
void	display_drawing	( void );
void	display_design	( void );
void	display_chart	( void );
void	keyboard	( unsigned char, int, int );

void	init_GLUI	( void );



//------------------------------------------------------------------------------
//	redraw function for all windows
//------------------------------------------------------------------------------
void redraw_all_windows( void )
{
    glutSetWindow( win_drawing );
    glutPostRedisplay();
    glutSetWindow( win_design );
    glutPostRedisplay();
    glutSetWindow( win_chart );
    glutPostRedisplay();
}

//------------------------------------------------------------------------------
//	Graph construction
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Label assignment
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//	Common graph computations
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	initializing textures from thumbnail images
//------------------------------------------------------------------------------
void capture_chart( const char * name )
{
    static cv::Mat              image;          // Mesh image
    static GLubyte *            pixel   = NULL;

    glutSetWindow( win_chart );   

    // unsigned int		wx = glutGet( GLUT_WINDOW_X );
    // unsigned int		wy = glutGet( GLUT_WINDOW_Y );
    unsigned int		ww = glutGet( GLUT_WINDOW_WIDTH );
    unsigned int		wh = glutGet( GLUT_WINDOW_HEIGHT );
    // const unsigned int          nChannels = 3;
    const unsigned int          nChannels = 4;

    display_chart();
    display_chart();

    if ( pixel == NULL ) pixel = new GLubyte [ ww * wh * nChannels ];
    // glReadPixels( 0, 0, ww, wh, GL_RGB, GL_UNSIGNED_BYTE, pixel );
    glReadPixels( 0, 0, ww, wh, GL_RGBA, GL_UNSIGNED_BYTE, pixel );

    // image = cv::Mat( cv::Size( ww, wh ), CV_8UC3 );
    image = cv::Mat( cv::Size( ww, wh ), CV_8UC4 );
    memcpy( image.data, pixel, ww * wh * nChannels );

    cv::cvtColor( image, image, cv::COLOR_BGR2RGB );
    cv::flip( image, image, 0 );
    cv::imwrite( name, image );

    cerr << "Capturing the chart window as " << name << " ... done." << endl;
}

//------------------------------------------------------------------------------
//	
//------------------------------------------------------------------------------
// Draw the neighbor graph
void draw_vertex_ids( Network & g )
{
    NetVertexIDMap	vertexID	= get( vertex_myid, g );
    NetVertexCntrMap	vertexCntr	= get( vertex_mycntr, g );

    // Annotating vertices of the proximity graph
    BGL_FORALL_VERTICES( vd, g, Network ) {
	unsigned int id = vertexID[ vd ];
	Point2 & point = vertexCntr[ vd ];
	ostringstream strID;
	strID << "#" << setw( 2 ) << id << ends;
	string2D( point.x(), point.y(), strID.str().c_str() );
    }
}


// Draw the neighbor graph
void draw_vertex_ids( Directed & g )
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
	string2D( point.x(), point.y(), strID.str().c_str() );
    }
}


// Draw the neighbor graph
void draw_network( Network & g )
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
void draw_directed( Directed & g )
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
void draw_polygon( Polygon2 & poly )
{
    // cerr << HERE << " Number of corners = " << poly.size() << endl;
    glBegin( GL_LINE_LOOP );
    for ( unsigned int i = 0; i < poly.size(); ++i ) {
	glVertex2d( poly[ i ].x(), poly[ i ].y() );
    }
    glEnd();
}


// Draw the set of convex hulls
void draw_polygon_ids( const Drawing & fig )
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
	string2D( toCenter.x()-0.05, toCenter.y()-0.02, strID.str().c_str() );
    }
}


// Draw the set of convex hulls
void draw_hulls( vector< Polygon2 > & hull )
{
    // cerr << HERE << " Number of hulls = " << hull.size() << endl;
    for ( unsigned int k = 0; k < hull.size(); ++k ) {
	draw_polygon( hull[ k ] );
    }
}

// Display function for the drawing window
void display_drawing( void )
{
    const double offset = 2.0;

    glutSetWindow( win_drawing );
    glClear( GL_COLOR_BUFFER_BIT );

    // for enabling antialiasing
    glEnable( GL_LINE_SMOOTH );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );

    if ( isFilled ) {
	// Filling polygons
	glLineWidth( 1.0 );
	glColor3d( 1.0, 1.0, 1.0 );
	for ( unsigned int i = 0; i < fig.tri().size(); ++i ) {
	    for ( unsigned int j = 0; j < fig.tri()[ i ].size(); ++j ) {
		glBegin( GL_POLYGON );
		for ( unsigned int k = 0; k < 3; ++k ) {
		    glVertex2d( fig.tri()[i][j][k].x(), fig.tri()[i][j][k].y() );
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
	for ( unsigned int i = 0; i < fig.bound().size(); ++i ) {
	    glBegin( GL_LINE_LOOP );
	    for ( unsigned int j = 0; j < fig.bound()[ i ].size(); j++ ) {
		glVertex2d( fig.bound()[ i ][ j ].x(), fig.bound()[ i ][ j ].y() );
	    }
	    glEnd();
	}
    }

    if ( isPlotted ) {
	// Plotting polygons
	glEnable( GL_POINT_SMOOTH );
	glPointSize( 9.0 );
	glColor3d( 0.4, 0.8, 0.0 );
	glBegin( GL_POINTS );
	for ( unsigned int i = 0; i < fig.bound().size(); ++i ) {
	    for ( unsigned int j = 0; j < fig.bound()[ i ].size(); j++ ) {
		glVertex2d( fig.bound()[ i ][ j ].x(), fig.bound()[ i ][ j ].y() );
	    }
	}
	glEnd();
    }

    
    if ( pickID != NO_NAME ) {
#ifdef USE_CONVEX_HULLS
	glColor3d( 1.0, 0.5, 0.0 );
	glLineWidth( 9.0 );
	draw_hulls( coverHull[ pickID ] );
#else	// USE_CONVEX_HULLS
	unsigned int nSegs = coverBand[ pickID ].size();
	for ( unsigned int i = 0; i < nSegs; ++i ) {
	    glColor3d( 1.0, 0.5, 0.0 );
	    glLineWidth( 7.0 );
	    Polygon2 curPoly = coverBand[ pickID ][ i ];
	    draw_polygon( curPoly );
	}
#endif	// USE_CONVEX_HULLS
    }

    if ( isWrapped ) {
	glDepthFunc( GL_ALWAYS );
	// cerr << HERE << " Drawing the beta-skelton" << endl;
#ifdef DEBUGGING_PHASE
	glLineWidth( 2.0 );
	glColor3d( 0.7, 0.7, 0.7 );
#else	// DEBUGGING_PHASE
	glLineWidth( 2.0 );
	// glColor3d( 0.0, 1.0, 0.5 );
#endif	// DEBUGGING_PHASE
	draw_directed( fig.wrapper() );
#ifdef SHOW_SAMPLE_IDS
	glColor3d( 1.0, 0.5, 0.0 );
	draw_vertex_ids( fig.wrapper() );
#endif	// SHOW_SAMPLE_IDS
	glDepthFunc( GL_LESS );

	// Drawing building polygon IDs
	// draw_polygon_ids( fig );
    }

    if ( isConjoined ) {
	glColor4d( 0.0, 0.0, 0.0, 1.0 );
	// draw_vertex_ids( netN );
	draw_vertex_ids( fig.netNbr() );
	
	// glColor4d( 1.0, 0.5, 0.0, 0.4 );
	glColor4d( 0.3, 0.3, 0.3, 0.4 );
	glLineWidth( 1.0 );
	// draw_network( netN );
	draw_network( fig.netNbr() );
	
	glPushMatrix();
	glTranslated( -offset, -offset, 0.0 );
	glColor4d( 1.0, 0.5, 0.0, 0.7 );
	glLineWidth( 5.0 );
	// draw_network( netP );
	draw_network( fig.netPrx() );
	glPopMatrix();
    
#ifdef USING_SIMILARITY_CONJOINING
	glPushMatrix();
	glTranslated( offset, offset, 0.0 );
	glColor4d( 0.0, 0.0, 1.0, 0.7 );
	glLineWidth( 5.0 );
	// draw_network( netS );
	draw_network( fig.netSim() );
	glPopMatrix();
#endif	// USING_SIMILARITY_CONJOINING
	
	// drawing concave polygons
	glColor4d( 0.0, 0.5, 0.0, 0.8 );
	glLineWidth( 2.0 );
	draw_hulls( fig.hullPrx() );
#ifdef USING_SIMILARITY_CONJOINING
	draw_hulls( fig.hullSim() );
#endif	// USING_SIMILARITY_CONJOINING
    }

    // for disabling antialiasing
    glDisable( GL_LINE_SMOOTH );
    glDisable( GL_BLEND );
    glEnable( GL_DEPTH_TEST );

    glutSwapBuffers();
}



void resize_drawing( int w, int h )
{
    // retina display handling
    int originalViewport[ 4 ];
    glGetIntegerv( GL_VIEWPORT, originalViewport );
    if ( ( originalViewport[ 2 ] == drawing_width * 2 ) &&
	 ( originalViewport[ 3 ] == drawing_height * 2 ) ) {
	cerr << HERE << " Retina display!!" << endl;
	cerr << HERE << " w = " << w << " h = " << h << endl;
	isRetina = true;
    }
    else {
	cerr << HERE << " NOT Retina display!!" << endl;
	cerr << HERE << " w = " << w << " h = " << h << endl;
	isRetina = false;
    }

    static int width, height;
    if ( isRetina ) {
	width = w*2;
	height = h*2;
    }
    else {
	width = w;
	height = h;
    }
    glViewport( 0, 0, width, height ); 


    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    // スクリーン上の座標系をマウスの座標系に一致させる
    // glOrtho(-0.5, (GLdouble)w - 0.5, (GLdouble)h - 0.5, -0.5, -1.0, 1.0);
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}



// Operation with mouse
void mouse_drawing( int button, int state, int x, int y )
{
    int m = glutGetModifiers();
    double xx = 2.0 * ( double )x / ( double )drawing_width - 1.0;
    double yy = 1.0 - 2.0 * ( double )y / ( double )drawing_height;

    switch ( button ) {
	// Points
      case GLUT_LEFT_BUTTON:
	  if ( ( state == GLUT_DOWN ) && ( m == GLUT_ACTIVE_SHIFT ) ) {
	      drawing_cursor = Point2( xx, yy );
	      cerr << HERE << " nPolys = " << nPolys << endl;
	      if ( fig.poly().size() == nPolys )
		  fig.poly().push_back( Polygon2() );
	      fig.poly()[ nPolys ].push_back( drawing_cursor );
	      cerr << " ( " << xx << " , " << yy << " ) " << endl;
	  }
#ifdef SKIP
	  glPointSize( 5.0 );
	  glColor3d( 1.0, 0.5, 0.0 );
	  glBegin( GL_POINTS );
	  glVertex2d( xx, yy );
	  glEnd();
	  glColor3d( 0.0, 1.0, 1.0 );
	  glLineWidth( 3.0 );
	  for ( unsigned int i = 0; i < fig.poly().size(); ++i ) {
	      glBegin( GL_LINE_STRIP );
	      for ( unsigned int j = 0; j < fig.poly()[ i ].size(); ++j ) {
		  glVertex2d( fig.poly()[ i ][ j ].x(), fig.poly()[ i ][ j ].y() );
	      }
	      glEnd();
	  }
	  glutSetWindow( win_drawing );   
	  glutPostRedisplay();
#endif	// SKIP
	  break;
	  // PolyLine  
      case GLUT_MIDDLE_BUTTON:
	  if ( ( state == GLUT_DOWN ) && ( m == GLUT_ACTIVE_SHIFT ) ){
	      keyboard( 'n', 0, 0 );
	  }
	  glutSetWindow( win_drawing );   
	  glutPostRedisplay();
	  break;
      case GLUT_RIGHT_BUTTON:
	  break;
      default:
	  break;
    }
    glutSetWindow( win_drawing );   
    glutPostRedisplay();
}


//------------------------------------------------------------------------------
//	Staff for design window
//------------------------------------------------------------------------------
//
//  barchart --	draw bar charts for the three costs
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void barchart( unsigned int idRow, unsigned int idCol,
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
    if ( isFilled ) glColor3d( 1.0, 1.0, 1.0 );
    else glColor3d( 0.0, 0.0, 0.0 );
    string2D( titleX, roofD - stepY, "D:" );
    string2D( titleX, roofS - stepY, "S:" );
    string2D( titleX, roofL - stepY, "L:" );
}

void place_option( unsigned int idRow, unsigned int idCol,
		   vector< Polygon2 > & polys,
		   const double & data, const double & smooth, const double & label )
{
#ifdef DEBUG
    cerr << HERE << " idBlock = " << idBlock
	 << " idRow = " << idRow << " idCol = " << idCol << endl;
#endif	// DEBUG
    int quarterW = design_width  / num_options_in_line;
    int quarterH = design_height / num_options_in_line;
    int minW = idCol * quarterW;
    int minH = idRow * quarterH;
    
#ifdef DEBUG
    cerr << HERE << " minH = " << minH << " minW = " << minW << endl;
#endif	// DEBUG
    glViewport( minW, minH, quarterW, quarterH );
    gluOrtho2D( -1.0, 1.0, -1.0, 1.0 );
    
    if ( isFilled ) glColor3d( 1.0, 1.0, 1.0 );
    else glColor3d( 0.0, 0.0, 0.0 );
    glLineWidth( 1.0 );
    for ( unsigned int i = 0; i < polys.size(); ++i ) {
	draw_polygon( polys[ i ] );
    }
    barchart( idRow, idCol, data, smooth, label );
    
    glViewport( 0, 0, design_width, design_height );
    gluOrtho2D( -1.0, 1.0, -1.0, 1.0 );
}
    

//
//  tile --	place the background grid tiles in the design window
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void tile( void )
{
    glLoadName( NO_NAME );
    unsigned int clusterID = 0;
    for ( unsigned int i = 0; i < num_options_in_line; ++i ) {
	if ( clusterID >= cluster.size() ) break;
	for ( unsigned int j = 0; j < cluster[ clusterID ].size(); ++j ) {
	    unsigned int blockID = cluster[ clusterID ][ j ];

	    // Skip if it is not included in the candidate list
	    if ( blockID >= coverBand.size() ) continue;
	    
	    if ( blockID == pickID )
		if ( isFilled ) glColor3d( 0.5, 0.4, 0.0 );
		else glColor3d( 1.0, 0.9, 0.5 );
	    else {
		if ( isFilled ) glColor3d( 0.0, 0.0, 0.0 );
		else glColor3d( 1.0, 1.0, 1.0 );
	    }
	    glLoadName( blockID );

	    int idRow = num_options_in_line - i - 1;
	    int idCol = j;
	    double quarterW = 2.0 / num_options_in_line;
	    double quarterH = 2.0 / num_options_in_line;
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
	    if ( isFilled ) glColor3d( 1.0, 1.0, 1.0 );
	    else glColor3d( 0.0, 0.0, 0.0 );
	    glBegin( GL_LINE_LOOP );
	    glVertex2d( minW, minH );
	    glVertex2d( minW + quarterW, minH );
	    glVertex2d( minW + quarterW, minH + quarterH );
	    glVertex2d( minW, minH + quarterH );
	    glEnd();

	    if ( ( j + 1 != cluster[ clusterID ].size() ) &&
		 ( ( j + 1 ) % num_options_in_line == 0 ) ) i++;
	}

	clusterID++;
    }
}


//
//  select_design --	select the item among the picked ones
//
bool select_design( int & hitID, int nHits, unsigned int * buf )
{
    unsigned int *	ptr		= NULL; //, names;
    const float		defaultDepth	= 1000.0;
    float		minDepth	= defaultDepth;

    glutSetWindow( win_design );

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
    glutSetWindow( win_design );   
    glutPostRedisplay();

    if ( minDepth < defaultDepth ) {
	return true;
    }
    return false;
}


//
//  pick_design --	pick the item with mouse click
//
bool pick_design( int & hitID, int x, int y, int button )
{
    unsigned int selectBuf[ BUFFER_SIZE ];
    int nHits;
    int viewport[ 4 ];

    glutSetWindow( win_design );
    
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
    tile();

    glMatrixMode( GL_PROJECTION );
    glPopMatrix(); // <====
    glMatrixMode( GL_MODELVIEW );
    glPopMatrix(); // <====

    glFlush();

    nHits = glRenderMode( GL_RENDER );
    if ( button == GLUT_LEFT_BUTTON ) {
	if ( select_design( hitID, nHits, selectBuf ) ) {
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


// Display function for the design window 
void display_design( void )
{
    vector< Expansion > & expand = fig.expand();
    unsigned int matrix_size = cluster.size();
    // cerr << HERE << " matrix_size = " << matrix_size << endl;
    num_options_in_line = matrix_size;
    // cerr << HERE << " num_options_in_line = " << num_options_in_line << endl;
    bool out_of_limit = false;
    do {
	out_of_limit = false;
	for ( unsigned int k = 0; k < cluster.size(); ++k ) {
	    num_options_in_line += ( ( (int)cluster[ k ].size() - 1 ) / matrix_size );
	}
	if ( num_options_in_line > matrix_size ) {
	    out_of_limit = true;
	    matrix_size++;
	    num_options_in_line = matrix_size;
	}
	// cerr << HERE << " matrix_size = " << matrix_size << endl;
	// cerr << HERE << " num_options_in_line = " << num_options_in_line << endl;
    } while ( out_of_limit );

    // cerr << HERE << " num_options_in_line = " << num_options_in_line << endl;
    
#ifdef SKIP
    if ( coverBand.size() > 9 ) num_options_in_line = 4;
    else if ( coverBand.size() > 4 ) num_options_in_line = 3;
    else if ( coverBand.size() > 1 ) num_options_in_line = 2;
    else num_options_in_line = 1;
#endif	// SKIP
    
    glutSetWindow( win_design );
    glClear( GL_COLOR_BUFFER_BIT );

    tile();
    
    int maxD = 0, maxS = 0, maxL = 0;
    for ( unsigned int i = 0; i < expand.size(); ++i ) {
	if ( maxD < expand[ i ].dataCost() ) maxD = expand[ i ].dataCost();
	if ( maxS < expand[ i ].smoothCost() ) maxS = expand[ i ].smoothCost();
	if ( maxL < expand[ i ].labelCost() ) maxL = expand[ i ].labelCost();
    }    

    unsigned int clusterID = 0;
    for ( unsigned int i = 0; i < num_options_in_line; ++i ) {
	if ( clusterID >= cluster.size() ) break;
	for ( unsigned int j = 0; j < cluster[ clusterID ].size(); ++j ) {
	    unsigned int blockID = cluster[ clusterID ][ j ];
	    double p = expand[ blockID ].dataCost()/(double)maxD;
	    double q = expand[ blockID ].smoothCost()/(double)maxS;
	    double r = expand[ blockID ].labelCost()/(double)maxL;
	    vector< Polygon2 > & polys = coverBand[ blockID ];
	    place_option( num_options_in_line - i - 1, ( j % num_options_in_line ),
			  polys, p, q, r );
	    if ( ( j + 1 != cluster[ clusterID ].size() ) &&
		 ( ( j + 1 ) % num_options_in_line == 0 ) ) i++;
	}
	clusterID++;
    }
    
    glutSwapBuffers();
}


void resize_design( int w, int h )
{
    // retina display handling
    int originalViewport[ 4 ];
    glGetIntegerv( GL_VIEWPORT, originalViewport );
    if ( ( originalViewport[ 2 ] == design_width * 2 ) &&
	 ( originalViewport[ 3 ] == design_height * 2 ) ) {
	cerr << HERE << " Retina display!!" << endl;
	cerr << HERE << " w = " << w << " h = " << h << endl;
	isRetina = true;
    }
    else {
	cerr << HERE << " NOT Retina display!!" << endl;
	cerr << HERE << " w = " << w << " h = " << h << endl;
	isRetina = false;
    }

    static int width, height;
    if ( isRetina ) {
	width = w*2;
	height = h*2;
    }
    else {
	width = w;
	height = h;
    }

    glViewport ( 0, 0, width, height );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluOrtho2D( -1.0, 1.0, -1.0, 1.0 );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}

// callback function for handling mouse clicking
void mouse_design( int button, int state, int x, int y )
{
    // vector< Polygon2 >  bandSet;
    // vector< Set >	globSet;
    // cerr << HERE << " mouse pointer : ( " << x << " , " << y << " ) " << endl;
    // switch according to the mouse event
    switch ( button ) {
	// left mouse event
      case GLUT_LEFT_BUTTON:
	  if ( state == GLUT_DOWN ) {
	      switch( mode ) {
		  case FREE:
		      design_cursor = Point2( x, y );
		      if ( pick_design( pickID, x, y, button ) ) {
			  ; // do nothing
		      }
		      break;
		  case SELECTED:
		      break;
	      }
	      design_left = 1;
	      if ( pickID != NO_NAME ) {
		  isPressed = true;
	      }
	  }
	  else if ( state == GLUT_UP ) {
	      if ( ( pickID != NO_NAME ) && isPressed ) {
		  switch ( mode ) {
		    case FREE:
			cerr << HERE << " pickID = " << pickID << endl;
			mode = SELECTED;
			cerr << HERE << " mode => selected" << endl;
			break;
		    case SELECTED:
			keyboard( 'f', 0, 0 );
			break;
		  }
	      }
	      isPressed = false;
	      design_left = 0;  
	  }
	  redraw_all_windows();
	  break;
	  // middle mouse event
      case GLUT_MIDDLE_BUTTON:
	  if ( state == GLUT_DOWN ) {
	      design_cursor = Point2( x, y );
	      design_middle = 1;
	  }
	  else {
	      design_middle = 0;
	  }
	  break;
	  // right mouse event
      case GLUT_RIGHT_BUTTON:
	  if ( state == GLUT_DOWN ) {
	      design_cursor = Point2( x, y );
	      design_right = 1;
	  }
	  else if ( state == GLUT_UP ) {
	      design_right = 0;
	      pickID = NO_NAME;
	      // reoptBit = false;
	  }
	  break;
      default:
	  cerr << "Unknown button" << endl;
	  break;
    }
    redraw_all_windows();
}


// callback founction for handling mouse dragging
void motion_design( int x, int y )
{
    // cerr << HERE << " in motion_design" << endl;
    if ( design_left ) {
	// cerr << HERE << " pointer = ( " << x << " , " << y << " ) " << endl;
	if ( pick_design( pickID, x, y, GLUT_LEFT_BUTTON ) ) {
	    ; // do nothing
	    // cerr << HERE << " Point ID " << setw( 2 ) << pickID << " is picked!!" << endl;
	}
    }
    else if ( design_middle ) {
	;
    }
    else if ( design_right ) {
	;
    }
    redraw_all_windows();
}



//------------------------------------------------------------------------------
//	Staff for chart window
//------------------------------------------------------------------------------

//
//  isometric --	project 3D coordintes onto isometric 2D plane
//
//  Inputs
//	x, y, z: 3D coordinates
//
//  Outputs
//	2D projection
//
void isometric( vector< Expansion > & expand )
{
    const double R3P2 = 0.5*sqrt(3.0);

    int maxD = 0, maxS = 0, maxL = 0;
    for ( unsigned int i = 0; i < expand.size(); ++i ) {
	if ( maxD < expand[ i ].dataCost() ) maxD = expand[ i ].dataCost();
	if ( maxS < expand[ i ].smoothCost() ) maxS = expand[ i ].smoothCost();
	if ( maxL < expand[ i ].labelCost() ) maxL = expand[ i ].labelCost();
    }    
    pickCoord.clear();
    for ( unsigned int i = 0; i < expand.size(); ++i ) {
	double p = expand[ i ].dataCost()/(double)maxD;
	double q = expand[ i ].smoothCost()/(double)maxS;
	double r = expand[ i ].labelCost()/(double)maxL;
	double x = ( R3P2 * ( -p + r ) );
	double y = -0.5*p + 1.0*q - 0.5*r;
	pickCoord.push_back( Point2( x, y ) );
    }
    coverHull.clear();
#ifdef USE_CONCAVE_HULLS
    coverBand.clear();
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
	    fig.concaveForLabel( fig.netNbr(), polySet[ j ], eachHull, eachGlob );
	    bandSet.push_back( eachHull );
	    globSet.push_back( eachGlob );
#endif	// USE_CONCAVE_HULLS
	    hullSet.push_back( eachHull );
	}
	coverHull.push_back( hullSet );
#ifdef USE_CONCAVE_HULLS
	coverBand.push_back( bandSet );
	coverGlob.push_back( globSet );
#endif	// USE_CONCAVE_HULLS
    }
#ifndef USE_CONVEX_HULLS
    cerr << HERE << " coverBand.size = " << coverBand.size() << endl;
#endif	// USE_CONVEX_HULLS
}


//
//  plot --	plot the label cost optimizations
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void plot( void )
{
    glEnable( GL_POINT_SMOOTH );
    glPointSize( 9.0 );
    if ( isFilled ) glColor3d( 1.0, 1.0, 1.0 );
    else glColor3d( 0.0, 0.0, 0.0 );
    

    glLoadName( NO_NAME );
    for ( unsigned int i = 0; i < pickCoord.size(); ++i ) {
#ifdef SKIP
	cerr << HERE << " " << i
	     << " : " << pickCoord[ i ].x() << " , " << pickCoord[ i ].y() << endl;
#endif	// SKIP
	// glPushName( i );
	if ( i == pickID ) glColor3d( 1.0, 0.5, 0.0 );
	else {
	    if ( isFilled ) glColor3d( 1.0, 1.0, 1.0 );
	    else glColor3d( 0.0, 0.0, 0.0 );
	}
	glLoadName( i );
	glBegin( GL_POINTS );
	glVertex2d( pickCoord[ i ].x(), pickCoord[ i ].y() );
	glEnd();
	// glPopName();
	glLoadName( NO_NAME );
    }
    for ( unsigned int i = 0; i < pickCoord.size(); ++i ) {
	ostringstream strID;
	if ( i == pickID ) glColor3d( 1.0, 0.5, 0.0 );
	else {
	    if ( isFilled ) glColor3d( 1.0, 1.0, 1.0 );
	    else glColor3d( 0.0, 0.0, 0.0 );
	}
	strID << "#" << i << ends;
	string2D( pickCoord[ i ].x(), pickCoord[ i ].y(), strID.str().c_str() );
    }
}


//
//  select_chart --	select the item among the picked ones
//
bool select_chart( int & hitID, int nHits, unsigned int * buf )
{
    unsigned int *	ptr		= NULL; //, names;
    const float		defaultDepth	= 1000.0;
    float		minDepth	= defaultDepth;

    glutSetWindow( win_chart );

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
    glutSetWindow( win_chart );   
    glutPostRedisplay();

    if ( minDepth < defaultDepth ) {
	return true;
    }
    return false;
}


//
//  pick_chart --	pick the item with mouse click
//
bool pick_chart( int & hitID, int x, int y, int button )
{
    unsigned int selectBuf[ BUFFER_SIZE ];
    int nHits;
    int viewport[ 4 ];

    glutSetWindow( win_chart );
    
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
    gluOrtho2D( -(1.0 + WINDOW_MARGIN), 1.0 + WINDOW_MARGIN,
		-(1.0 + WINDOW_MARGIN), 1.0 + WINDOW_MARGIN );
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix(); // <====
    glLoadIdentity();

    // draw the set of items
    plot();

    glMatrixMode( GL_PROJECTION );
    glPopMatrix(); // <====
    glMatrixMode( GL_MODELVIEW );
    glPopMatrix(); // <====

    glFlush();

    nHits = glRenderMode( GL_RENDER );
    if ( button == GLUT_LEFT_BUTTON ) {
	// cerr << " Left button pressed" << endl;
	if ( select_chart( hitID, nHits, selectBuf ) ) {
	    // cerr << "Point successfully selected for displacement" << endl;
	    return true;
	}
	else {
	    // cerr << "Point not selected" << endl;
	    hitID = NO_NAME;
	    return false;
	}
    }
    else {
	// do nothing
	return false;
    }

    // glutPostRedisplay();
}


#ifdef TO_BE_DELETED
void annotate( double cx, double cy, double side )
{
    glEnable( GL_TEXTURE_2D );                    // Enable 2D Texture Mapping
    glBegin( GL_QUADS );
    glTexCoord2f( 0.0f, 0.0f );
    glVertex2d( cx - side, cy - side );
    glTexCoord2f( 1.0f, 0.0f );
    glVertex2d( cx + side, cy - side );
    glTexCoord2f( 1.0f, 1.0f );
    glVertex2d( cx + side, cy + side );
    glTexCoord2f( 0.0f, 1.0f );
    glVertex2d( cx - side, cy + side );
    glEnd();    
    glDisable( GL_TEXTURE_2D ); // Enable 2D Texture Mapping

    glLineWidth( 3.0 );
    // glColor3d( 0.5, 1.0, 0.5 );
    glColor3d( 0.5, 0.5, 0.9 );
    glBegin( GL_LINE_LOOP );
    glVertex2d( cx - side, cy - side );
    glVertex2d( cx + side, cy - side );
    glVertex2d( cx + side, cy + side );
    glVertex2d( cx - side, cy + side );
    glEnd();    
}
#endif	// TO_BE_DELETED


// Display function for the chart window 
void display_chart( void )
{
    const double R3P2 = 0.5*sqrt(3.0);

    glutSetWindow( win_chart );

    glClear( GL_COLOR_BUFFER_BIT );

    // Axis
    glLineWidth( 2.0 );

    if ( isFilled ) glColor3d( 0.8, 0.9, 1.0 );
    else glColor3d( 0.0, 0.1, 0.2 );
    
    glBegin( GL_LINES );
    glVertex2d( 0.0, 0.0 );
    glVertex2d( 0.0, 1.0 );
    glVertex2d( 0.0, 0.0 );
    glVertex2d(-R3P2, -0.5 );
    glVertex2d( 0.0, 0.0 );
    glVertex2d( R3P2, -0.5 );
    glEnd();

    // Text
    string strD="Data cost";
    string2D(-R3P2-0.3, -0.5, strD.c_str() );
    string strS="Smooth cost";
    string2D(-0.2,   1.0, strS.c_str() );
    string strL="Label cost";
    string2D( R3P2, -0.5, strL.c_str() );

    plot();
  
#ifdef SKIP
    glColor3d( 0.0, 1.0, 1.0 );
    glDisable( GL_LIGHTING );
    glDisable( GL_DEPTH_TEST );
    if ( isLoaded ) {
	// place thumbnail images as icons
	for ( unsigned int i = 0; i < priority.size(); ++i ) {
	    unsigned int id = priority[ i ];

	    if ( ! isAnnotated ) continue;

	    // tentatively set
	    double half = 0.1;

	    glBindTexture( GL_TEXTURE_2D, textureID[ id ] );
	    annotate( phl[ id ], shl[ id ], half );
	}

	if ( pickID != NO_NAME ) {
	    double emph = 0.2;
	    glBindTexture( GL_TEXTURE_2D, textureID[ pickID ] );
	    annotate( phl[ pickID ], shl[ pickID ], emph );
	}
    }
#endif	// SKIP
  
    glutSwapBuffers();
}


void resize_chart( int w, int h )
{
    // retina display handling
    int originalViewport[ 4 ];
    glGetIntegerv( GL_VIEWPORT, originalViewport );
    if ( ( originalViewport[ 2 ] == chart_width * 2 ) &&
	 ( originalViewport[ 3 ] == chart_height * 2 ) ) {
	cerr << HERE << " Retina display!!" << endl;
	cerr << HERE << " w = " << w << " h = " << h << endl;
	isRetina = true;
    }
    else {
	cerr << HERE << " NOT Retina display!!" << endl;
	cerr << HERE << " w = " << w << " h = " << h << endl;
	isRetina = false;
    }

    static int width, height;
    if ( isRetina ) {
	width = w*2;
	height = h*2;
    }
    else {
	width = w;
	height = h;
    }

    glViewport ( 0, 0, width, height );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluOrtho2D( -(1.0 + WINDOW_MARGIN), 1.0 + WINDOW_MARGIN,
		-(1.0 + WINDOW_MARGIN), 1.0 + WINDOW_MARGIN );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}


// callback function for handling mouse clicking
void mouse_chart( int button, int state, int x, int y )
{
    vector< Polygon2 >  bandSet;
    vector< Set >	globSet;
    // cerr << HERE << " mouse pointer : ( " << x << " , " << y << " ) " << endl;
    // switch according to the mouse event
    switch ( button ) {
	// left mouse event
      case GLUT_LEFT_BUTTON:
	  if ( state == GLUT_DOWN ) {
	      switch( mode ) {
		  case FREE:
		      chart_cursor = Point2( x, y );
		      if ( pick_chart( pickID, x, y, button ) ) {
			  // bandID = 0;
#ifdef SKIP
			  cerr << HERE << " Point ID " << setw( 2 )
			       << pickID << " is picked!!" << endl;
#endif	// SKIP
		      }
		      break;
		  case SELECTED:
		      break;
	      }
	      chart_left = 1;
	      if ( pickID != NO_NAME ) {
		  isPressed = true;
	      }
	  }
	  else if ( state == GLUT_UP ) {
	      if ( ( pickID != NO_NAME ) && isPressed ) {
		  switch ( mode ) {
		    case FREE:
			cerr << HERE << " pickID = " << pickID << endl;
			mode = SELECTED;
			cerr << HERE << " mode => selected" << endl;
			break;
		    case SELECTED:
			keyboard( 'f', 0, 0 );
			break;
		  }
	      }
	      isPressed = false;
	      chart_left = 0;  
	  }
	  redraw_all_windows();
	  break;
	  // middle mouse event
      case GLUT_MIDDLE_BUTTON:
	  if ( state == GLUT_DOWN ) {
	      chart_cursor = Point2( x, y );
	      chart_middle = 1;
	  }
	  else {
	      chart_middle = 0;
	  }
	  break;
	  // right mouse event
      case GLUT_RIGHT_BUTTON:
	  if ( state == GLUT_DOWN ) {
	      chart_cursor = Point2( x, y );
	      chart_right = 1;
	  }
	  else if ( state == GLUT_UP ) {
	      chart_right = 0;
	      pickID = NO_NAME;
	      // reoptBit = false;
	  }
	  break;
      default:
	  cerr << "Unknown button" << endl;
	  break;
    }
    redraw_all_windows();
}


// callback founction for handling mouse dragging
void motion_chart( int x, int y )
{
    if ( chart_left ) {
	// cerr << HERE << " pointer = ( " << x << " , " << y << " ) " << endl;
	if ( pick_chart( pickID, x, y, GLUT_LEFT_BUTTON ) ) {
	    // cerr << HERE << " Point ID " << setw( 2 ) << pickID << " is picked!!" << endl;
	    // bandID = 0;
	}
    }
    else if ( chart_middle ) {
	;
    }
    else if ( chart_right ) {
	;
    }
    redraw_all_windows();
}


// Operation with keyboard
void keyboard( unsigned char key, int x, int y )
{
    static unsigned int	counter = 0;		
    ostringstream	ostr;
    istringstream	istr;
    string		line;
    vector< Polygon2 >  bandSet;
    vector< Set >	globSet;
    string		inname, outname, imgname, dirname;
    struct stat		statbuf;
    
    switch ( key ) {
      case 'c':
	  clear_data();
	  break;
	  // Calls this after loading the line drawing data
      case 'a':
      case 'A':
	  // cerr << HERE << " No. polygons in drawing = " << fig.poly().size() << endl;
	  fig.conjoin();
	  // cerr << HERE << " No. polygons in drawing = " << fig.poly().size() << endl;
	  fig.optimize();
	  // cerr << HERE << " No. polygons in drawing = " << fig.poly().size() << endl;
	  isometric( fig.expand() );
	  // cerr << HERE << " No. polygons in drawing = " << fig.poly().size() << endl;
#ifdef SKIP_IO
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
#endif	// SKIP_IO
	  // Delete the last aggregation
	  // fig.expand().erase( fig.expand().end() - 1 );
	  // Hierarchical clustering
	  dendrogram.set( fig.expand() );
	  dendrogram.merge();
	  cluster = dendrogram.retrieve( cut_threshold );
	  // cerr << HERE << " cluster.size = " << cluster.size() << endl;
	  break;
      case 'e':			// exhaustive simplify
	  fig.simplify();
	  fig.triangulate();
	  break;
      case 'r':			// squaring building polygons
	  fig.square();
	  fig.triangulate();
	  break;
      case 'g':
	  fig.conjoin();
	  break;
      case 'o':
	  fig.optimize();
	  isometric( fig.expand() );
	  break;
      case 'l':
	  cerr << "Input the file name for loading data : ";
	  getline( cin, line );
	  load_drawing( line.c_str() );
	  isLoaded = false;
	  break;      
      case 's':
	  cerr << "Input the file name for saving data : ";
	  getline( cin, line );
	  save_drawing( line.c_str() );
	  break;
      case 'j':
	  isConjoined = !isConjoined;
	  if ( isConjoined ) cerr << HERE << " isConjoined TRUE" << endl;
	  else cerr << HERE << " isConjoined FALSE" << endl;
	  glutSetWindow( win_drawing );
	  glutPostRedisplay();
	  break;
      case 'F':
	  isFilled = !isFilled;
	  if ( isFilled ) {
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
	  glutSetWindow( win_drawing );
	  glutPostRedisplay();
	  break;
      case 'W':
	  isWrapped = !isWrapped;
	  if ( isWrapped ) cerr << HERE << " isWrapped TRUE" << endl;
	  else cerr << HERE << " isWrapped FALSE" << endl;
	  if ( isWrapped ) fig.initWrapper();
	  glutSetWindow( win_drawing );
	  glutPostRedisplay();
	  break;
      case 'P':
	  isPlotted = !isPlotted;
	  if ( isPlotted ) cerr << HERE << " isPlotted TRUE" << endl;
	  else cerr << HERE << " isPlotted FALSE" << endl;
	  glutSetWindow( win_drawing );
	  glutPostRedisplay();
	  break;
      case 'n':
	  cerr << HERE
	       << " Closing the current polygon : poly id = " << nPolys << endl;
	  fig.poly().push_back( Polygon2() );
	  nPolys++;
	  glutSetWindow( win_drawing );
	  glutPostRedisplay();
	  break;
      case 'p':
	  cerr << HERE << " Deleting the last polygon " << endl;
	  if ( fig.poly().size() > 0 ) {
	      fig.poly().erase( fig.poly().end() - 1 );
	  }
	  glutSetWindow( win_drawing );
	  glutPostRedisplay();
	  break;
      case 'd':
	  cerr << HERE << " Deleting the last vertex" << endl;
	  {
	      Polygon2 & refPoly = fig.poly()[ nPolys ];
	      if ( refPoly.size() > 0 ) {
		  refPoly.erase( refPoly.end() - 1 );
	      }
	  }
	  glutSetWindow( win_drawing );
	  glutPostRedisplay();
	  break;
      case 'D':
	  ostr << setw( 3 ) << setfill( '0' ) << counter++ << ends;
	  imgname = headname + "-drawing-" + ostr.str().c_str() + ".png";
	  // cerr << " Input file name : " << ends;
	  // getline( cin, line );
	  // capture_drawing( line.c_str() );
	  cerr << " Capture the drawing window and save to " << imgname.c_str() << endl;
	  capture_drawing( imgname.c_str() );
	  return;
      case 'C':
	  ostr << setw( 3 ) << setfill( '0' ) << counter++ << ends;
	  imgname = headname + "-chart-" + ostr.str().c_str() + ".png";
	  // cerr << " Input file name : " << ends;
	  // getline( cin, line );
	  // capture_drawing( line.c_str() );
	  cerr << " Capture the chart window and save to " << imgname.c_str() << endl;
	  capture_chart( imgname.c_str() );
	  return;
      case 'f':
	  cerr << HERE << " pickID = " << pickID << endl;
	  bandSet.clear();
	  globSet.clear();
#ifdef USE_CONVEX_HULLS
	  fig.aggregate( pickID );
	  fig.triangulate();
#else // USE_CONVEX_HULLS
	  // cerr << HERE << " No. polygons in drawing = " << fig.poly().size() << endl;
	  cerr << HERE << " Number of polygons = " << coverBand[ pickID ].size() << endl;
	  for ( unsigned int k = 0; k < coverBand[ pickID ].size(); ++k ) {
	      bandSet.push_back( coverBand[ pickID ][ k ] );
	      globSet.push_back( coverGlob[ pickID ][ k ] );
	  }
	  // cerr << HERE << " No. polygons in drawing = " << fig.poly().size() << endl;
	  fig.aggregate( pickID, bandSet, globSet );
	  fig.triangulate();
	  // cerr << HERE << " No. polygons in drawing = " << fig.poly().size() << endl;
#endif	// USE_CONVEX_HULLS
	  isometric( fig.expand() );
	  // cerr << HERE << " No. polygons in drawing = " << fig.poly().size() << endl;
	  pickID = NO_NAME;
	  // bandID = NO_NAME;
	  mode = FREE;
	  cerr << HERE << " mode => free" << endl;
	  outname = "data/" + headname + "-out.dat";
	  cerr << HERE << " outname ======> : " << outname << endl;
	  save_drawing( outname.c_str() );
	  dirname = "png/" + headname;
	  outname = dirname + "/out.png";
	  cerr << HERE << " outname ======> : " << outname << endl;
	  capture_drawing( outname.c_str() );
	  break;
      case 'q':
	  exit(0);
	  break;
      default:
	  break;
    }
    redraw_all_windows();
}


#ifdef SKIP
#ifndef USE_CONVEX_HULLS
// Operation with special keys
void special( int key, int x, int y )
{
    // cerr << HERE << " pickID = " << pickID << " bandID = " << bandID << endl;
    // unsigned int alphaID;
    // unsigned int nAlphas;

    if ( mode != SELECTED ) return;

    Alpha & curBand = coverBand[ pickID ][ bandID ]; 
    unsigned int nPolys = coverBand[ pickID ].size();
    // cerr << HERE << " nPolys = " << nPolys << endl;
    // cerr << HERE << " coverBand.polygon = " << curBand.bound();
    cerr << HERE << "[Before] curband.inquiry => " << ends;
    curBand.inquiry();
    
    switch ( key ) {
      case GLUT_KEY_UP:
	  cerr << HERE << "[Before] layerID = " << curBand.layerID() << endl;
	  curBand.raise();
	  cerr << HERE << "[After]  layerID = " << curBand.layerID() << endl;
	  break;
      case GLUT_KEY_DOWN:
	  cerr << HERE << "[Before] layerID = " << curBand.layerID() << endl;
	  curBand.lower();
	  cerr << HERE << "[After]  layerID = " << curBand.layerID() << endl;
	  break;
      case GLUT_KEY_LEFT:
	  cerr << HERE << "[Before] bandID = " << bandID << endl;
	  bandID = ( bandID - 1 + nPolys ) % nPolys;
	  cerr << HERE << "[After]  bandID = " << bandID << endl;
	  break;
      case GLUT_KEY_RIGHT:
	  cerr << HERE << "[Before] bandID = " << bandID << endl;
	  bandID = ( bandID + 1 ) % nPolys;
	  cerr << HERE << "[After]  bandID = " << bandID << endl;
	  break;
    }

    cerr << HERE << "[After]  curband.inquiry => " << ends;
    curBand.inquiry();

    redraw_all_windows();
}
#endif	// USE_CONVEX_HULLS
#endif	// SKIP


// Popup menu
void perform_task( int task )
{
    // int id;
    // char fname[32];
    // char buf[BUFFER_SIZE];  

    switch ( task ) {
      case CLEAR_DRAWING:	// clear the line drawing
	  keyboard( 'c', 0, 0 );
	  break;
      case JOIN_GESTALTS:	// conjoing the gestalts of polygon
	  keyboard( 'g', 0, 0 );
	  break;
      case OPT_DRAWING:		// optimize the line drawing
	  keyboard( 'o', 0, 0 );
	  break;
      case LOAD_DRAWING:	// load a line drawing from the data file
	  keyboard( 'l', 0, 0 );
	  break;
      case SAVE_DRAWING:	// save the line drawing into the data file
	  keyboard( 's', 0, 0 );
	  break;
      case FLIP_CONJOINED:
	  keyboard( 'j', 0, 0 );
	  break;
      case FLIP_WRAPPED:
	  keyboard( 'w', 0, 0 );
	  break;
      case ADJUST_INTERVAL:
	  Drawing::interval_threshold = ( double )interval_spinner/interval_scale;
	  keyboard( 'a', 0, 0 );
	  break;
#ifdef WITH_MIN_LIMITS
      case ADJUST_MIN_DATA:
	  max_data_spinner = max( min_data_spinner + data_cost_gap,
				  max_data_spinner );
	  maxDataCost->set_float_val( max_data_spinner );
	  Drawing::data_cost_lower = ( double )min_data_spinner;
	  Drawing::data_cost_upper = ( double )max_data_spinner;
	  keyboard( 'a', 0, 0 );
	  break;
#endif	// WITH_MIN_LIMITS
      case ADJUST_MAX_DATA:
	  min_data_spinner = min( min_data_spinner,
				  max_data_spinner - data_cost_gap );
#ifdef WITH_MIN_LIMITS
	  minDataCost->set_float_val( min_data_spinner );
#endif	// WITH_MIN_LIMITS
	  Drawing::data_cost_upper = ( double )max_data_spinner;
	  Drawing::data_cost_lower = ( double )min_data_spinner;
	  keyboard( 'a', 0, 0 );
	  break;
#ifdef WITH_MIN_LIMITS
      case ADJUST_MIN_SMOOTH:
	  max_smooth_spinner = max( min_smooth_spinner + smooth_cost_gap,
				    max_smooth_spinner );
	  maxSmoothCost->set_float_val( max_smooth_spinner );
	  Drawing::smooth_cost_lower = ( double )min_smooth_spinner;
	  Drawing::smooth_cost_upper = ( double )max_smooth_spinner;
	  keyboard( 'a', 0, 0 );
	  break;
#endif	// WITH_MIN_LIMITS
      case ADJUST_MAX_SMOOTH:
	  min_smooth_spinner = min( min_smooth_spinner,
				    max_smooth_spinner - smooth_cost_gap );
#ifdef WITH_MIN_LIMITS
	  minSmoothCost->set_float_val( min_smooth_spinner );
#endif	// WITH_MIN_LIMITS
	  Drawing::smooth_cost_upper = ( double )max_smooth_spinner;
	  Drawing::smooth_cost_lower = ( double )min_smooth_spinner;
	  keyboard( 'a', 0, 0 );
	  break;
#ifdef WITH_MIN_LIMITS
      case ADJUST_MIN_LABEL:
	  max_label_spinner = max( min_label_spinner + label_cost_gap,
				   max_label_spinner );
	  maxLabelCost->set_float_val( max_label_spinner );
	  Drawing::label_cost_lower = ( double )min_label_spinner;
	  Drawing::label_cost_upper = ( double )max_label_spinner;
	  keyboard( 'a', 0, 0 );
	  break;
#endif	// WITH_MIN_LIMITS
      case ADJUST_MAX_LABEL:
	  min_label_spinner = min( min_label_spinner,
				   max_label_spinner - label_cost_gap );
#ifdef WITH_MIN_LIMITS
	  minLabelCost->set_float_val( min_label_spinner );
#endif	// WITH_MIN_LIMITS
	  Drawing::label_cost_upper = ( double )max_label_spinner;
	  Drawing::label_cost_lower = ( double )min_label_spinner;
	  keyboard( 'a', 0, 0 );
	  break;
      case ADJUST_CUT_THRESHOLD:
	  cut_threshold = ( double )cut_threshold_spinner;
	  keyboard( 'a', 0, 0 );
	  break;
      case LINKAGE_TYPE_UPDATE:
	  {
	      int valueID = linkage_switch;
	      cerr << HERE << " valueID = " << valueID << endl;
	      std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
	      switch ( linkage_switch ) {
		case 0:
		    dendrogram.type() = CENTROID;
		    break;
		case 1:
		    dendrogram.type() = MINIMUM;
		    break;
		case 2:
		    dendrogram.type() = MAXIMUM;
		    break;
		default:
		    cerr << HERE << "Illegal case" << endl;
		    break;
	      }
	  }
	  keyboard( 'a', 0, 0 );
	  break;
      case CAPTURE_DRAWING:
	  keyboard( 'D', 0, 0 );
	  return;
      case CAPTURE_CHART:
	  keyboard( 'C', 0, 0 );
	  return;
      case INIT_GLUI:
	  init_GLUI();
	  break;
      case QUIT_PROGRAM:	// instantly quit the program
	  exit( 0 );
    }
    redraw_all_windows();
}


void init(void)
{
    if ( isFilled ) {
	glClearColor( 0.0, 0.0, 0.0, 1.0 );
    }
    else {
	glClearColor( 1.0, 1.0, 1.0, 1.0 );
    }
}



//------------------------------------------------------------------------------
//	Initialize GLUI interface
//------------------------------------------------------------------------------
void init_GLUI( void )
{
    const float spinner_speed = 2.0;

    //------------------------------------------------------------------------------
    // setup the GLUI root window
    //------------------------------------------------------------------------------
    glutInitWindowPosition( WINDOW_LEFT_LIMIT + drawing_width + design_width,
			    WINDOW_TOP_LIMIT );
    if ( glui != NULL ) glui->close();
    glui = GLUI_Master.create_glui( (char *)GLUI_TITLE );

    // glui->add_separator();
    rootPanel = glui->add_panel( "Panel" );

    intervalPanel
	= glui->add_panel_to_panel	( rootPanel, "Metric" );

    limitInterval
	= glui->add_spinner_to_panel	( intervalPanel, "limit interval: ",
					  GLUI_SPINNER_FLOAT,
					  &interval_spinner,
					  ADJUST_INTERVAL, (GLUI_Update_CB)perform_task );
    limitInterval->set_float_limits	( interval_ceil, interval_roof, GLUI_LIMIT_CLAMP );
    limitInterval->set_speed( spinner_speed );

// ------------------------------------------------------------
    dataCostPanel
	= glui->add_panel_to_panel	( rootPanel, "Data cost" );

#ifdef WITH_MIN_LIMITS
    minDataCost
	= glui->add_spinner_to_panel	( dataCostPanel, "minimum: ",
					  GLUI_SPINNER_FLOAT,
					  &min_data_spinner,
					  ADJUST_MIN_DATA, (GLUI_Update_CB)perform_task );
    minDataCost->set_float_limits	( data_cost_ceil,
					  data_cost_roof - data_cost_gap,
					  GLUI_LIMIT_CLAMP );
    minDataCost->set_speed( spinner_speed );
#endif	// WITH_MIN_LIMITS

    maxDataCost
	= glui->add_spinner_to_panel	( dataCostPanel, "maximum: ",
					  GLUI_SPINNER_FLOAT,
					  &max_data_spinner,
					  ADJUST_MAX_DATA, (GLUI_Update_CB)perform_task );
    maxDataCost->set_float_limits	( data_cost_ceil + data_cost_gap,
					  data_cost_roof,
					  GLUI_LIMIT_CLAMP );
    maxDataCost->set_speed( spinner_speed );

    // ------------------------------------------------------------
    smoothCostPanel
	= glui->add_panel_to_panel	( rootPanel, "Smooth cost" );

#ifdef WITH_MIN_LIMITS
    minSmoothCost
	= glui->add_spinner_to_panel	( smoothCostPanel, "minimum: ",
					  GLUI_SPINNER_FLOAT,
					  &min_smooth_spinner,
					  ADJUST_MIN_SMOOTH, (GLUI_Update_CB)perform_task );
    minSmoothCost->set_float_limits	( smooth_cost_ceil,
					  smooth_cost_roof - smooth_cost_gap,
					  GLUI_LIMIT_CLAMP );
    minSmoothCost->set_speed( spinner_speed );
#endif	// WITH_MIN_LIMITS
    // glui->add_separator();


    maxSmoothCost
	= glui->add_spinner_to_panel	( smoothCostPanel, "maximum: ",
					  GLUI_SPINNER_FLOAT,
					  &max_smooth_spinner,
					  ADJUST_MAX_SMOOTH, (GLUI_Update_CB)perform_task );
    maxSmoothCost->set_float_limits	( smooth_cost_ceil + smooth_cost_gap,
					  smooth_cost_roof,
					  GLUI_LIMIT_CLAMP );
    maxSmoothCost->set_speed( spinner_speed );
    // glui->add_separator();
    
    // ------------------------------------------------------------
    labelCostPanel
	= glui->add_panel_to_panel	( rootPanel, "Label cost" );

#ifdef WITH_MIN_LIMITS
    minLabelCost
	= glui->add_spinner_to_panel	( labelCostPanel, "minimum: ",
					  GLUI_SPINNER_FLOAT,
					  &min_label_spinner,
					  ADJUST_MIN_LABEL, (GLUI_Update_CB)perform_task );
    minLabelCost->set_float_limits	( label_cost_ceil,
					  label_cost_roof - label_cost_gap,
					  GLUI_LIMIT_CLAMP );
    minLabelCost->set_speed( spinner_speed );
    // glui->add_separator();
#endif	// WITH_MIN_LIMITS
    
    maxLabelCost
	= glui->add_spinner_to_panel	( labelCostPanel, "maximum: ",
					  GLUI_SPINNER_FLOAT,
					  &max_label_spinner,
					  ADJUST_MAX_LABEL, (GLUI_Update_CB)perform_task );
    maxLabelCost->set_float_limits	( label_cost_ceil + label_cost_gap,
					  label_cost_roof,
					  GLUI_LIMIT_CLAMP );
    maxLabelCost->set_speed( spinner_speed );
    // glui->add_separator();
    
    // ------------------------------------------------------------
    cutThresholdPanel
	= glui->add_panel_to_panel	( rootPanel, "Cut threshold" );

    cutThresholdValue
	= glui->add_spinner_to_panel	( cutThresholdPanel, "cut threshold: ",
					  GLUI_SPINNER_FLOAT,
					  &cut_threshold_spinner,
					  ADJUST_CUT_THRESHOLD,
					  (GLUI_Update_CB)perform_task );
    cutThresholdValue->set_float_limits	( cut_threshold_ceil,
					  cut_threshold_roof,
					  GLUI_LIMIT_CLAMP );
    cutThresholdValue->set_speed( spinner_speed );
    // glui->add_separator();

    // ------------------------------------------------------------
    linkagePanel
	= glui->add_panel_to_panel	( rootPanel, "Linkage type" );

    linkageRadio
	= glui->add_radiogroup_to_panel	( linkagePanel, &linkage_switch,
					  0, (GLUI_Update_CB)perform_task );
    glui->add_radiobutton_to_group	( linkageRadio, "Centroid" );
    glui->add_radiobutton_to_group	( linkageRadio, "Minimum" );
    glui->add_radiobutton_to_group	( linkageRadio, "Maximum" );
    glui->add_button_to_panel		( linkagePanel, "Update",
					  LINKAGE_TYPE_UPDATE,
					  (GLUI_Update_CB)perform_task );

//------------------------------------------------------------------------------
//	Quit button
//------------------------------------------------------------------------------
    glui->add_button( "Quit",
		      QUIT_PROGRAM, (GLUI_Update_CB)perform_task );

    // tell glui what graphics window it needs to post a redisplay to:
    glui->set_main_gfx_window( win_drawing );
    // glui->set_main_gfx_window( win_design );

    // set the graphics window's idle function:
    GLUI_Master.set_glutIdleFunc( NULL );
}


int main( int argc, char *argv[] )
{
    string path	=	"configs/default.conf";
    int option;

    //------------------------------------------------------------------------------
    //	Command line handling
    // "c:"	allow -c with one argument for the option
    // reference
    // "c"	no argument
    // "c::"	argument is optional (allow both no arguments and one argument)
    while ( ( option = getopt( argc, argv, "c:" ) ) != -1 ) {
	switch ( option ) {
	  case 'c':		// option: -c <config file name>
	      path = optarg;
	      break;
	  case '?':		// unknown option
	      cerr << argv[ 0 ] << ": Unknown option: default setup will be used" << endl;
	      break;
	}
    }
    cerr << "loading configuration from " << path << endl;

    //------------------------------------------------------------------------------
    //	read the configuration file
    OZ::Option::Config conf( path );
    //------------------------------------------------------------------------------
    if ( conf.has( "num_trials" ) )
	Drawing::num_trials = ( unsigned int )stoi( conf.gets( "num_trials" ) );
    cerr << HERE << " num_trials = " << Drawing::num_trials << endl;
    //------------------------------------------------------------------------------
    if ( conf.has( "interval_threshold" ) ) {
	Drawing::interval_threshold = stod( conf.gets( "interval_threshold" ) );
	interval_spinner = ( float )( interval_scale * Drawing::interval_threshold );
    }
    cerr << HERE << " interval_threshold = " << Drawing::interval_threshold << endl;
    //------------------------------------------------------------------------------
    if ( conf.has( "proximity_upper" ) )
	Drawing::proximity_upper = stod( conf.gets( "proximity_upper" ) );
    cerr << HERE << " proximity_upper = " << Drawing::proximity_upper << endl;
#ifdef USING_SIMILARITY_CONJOINING
    //------------------------------------------------------------------------------
    if ( conf.has( "similarity_lower" ) )
	Drawing::similarity_lower = stod( conf.gets( "similarity_lower" ) );
    cerr << HERE << " similarity_lower = " << Drawing::similarity_lower << endl;
#endif	// USING_SIMILARITY_CONJOINING
    //------------------------------------------------------------------------------
    if ( conf.has( "proximity_ratio" ) )
	Drawing::proximity_ratio = stod( conf.gets( "proximity_ratio" ) );
    cerr << HERE << " proximity_ratio = " << Drawing::proximity_ratio << endl;
#ifdef USING_SIMILARITY_CONJOINING
    //------------------------------------------------------------------------------
    if ( conf.has( "similarity_ratio" ) )
	Drawing::similarity_ratio = stod( conf.gets( "similarity_ratio" ) );
    cerr << HERE << " similarity_ratio = " << Drawing::similarity_ratio << endl;
#endif	// USING_SIMILARITY_CONJOINING
    //------------------------------------------------------------------------------
    if ( conf.has( "data_cost_lower" ) ) {
	Drawing::data_cost_lower = stod( conf.gets( "data_cost_lower" ) );
	min_data_spinner = ( float )Drawing::data_cost_lower;
    }
    cerr << HERE << " data_cost_lower = " << Drawing::data_cost_lower << endl;
    if ( conf.has( "data_cost_upper" ) ) {
	Drawing::data_cost_upper = stod( conf.gets( "data_cost_upper" ) );
	max_data_spinner = ( float )Drawing::data_cost_upper;
    }
    cerr << HERE << " data_cost_upper = " << Drawing::data_cost_upper << endl;
    if ( conf.has( "data_cost_inside" ) )
	Drawing::data_cost_inside = stod( conf.gets( "data_cost_inside" ) );
    cerr << HERE << " data_cost_inside = " << Drawing::data_cost_inside << endl;
    //------------------------------------------------------------------------------
    if ( conf.has( "smooth_cost_lower" ) ) {
	Drawing::smooth_cost_lower = stod( conf.gets( "smooth_cost_lower" ) );
	min_data_spinner = ( float )Drawing::smooth_cost_lower;
    }
    cerr << HERE << " smooth_cost_lower = " << Drawing::smooth_cost_lower << endl;
    if ( conf.has( "smooth_cost_upper" ) ) {
	Drawing::smooth_cost_upper = stod( conf.gets( "smooth_cost_upper" ) );
	max_data_spinner = ( float )Drawing::smooth_cost_upper;
    }
    cerr << HERE << " smooth_cost_upper = " << Drawing::smooth_cost_upper << endl;
    //------------------------------------------------------------------------------
    if ( conf.has( "label_cost_lower" ) ) {
	Drawing::label_cost_lower = stod( conf.gets( "label_cost_lower" ) );
	min_label_spinner = ( float )Drawing::label_cost_lower;
    }
    cerr << HERE << " label_cost_lower = " << Drawing::label_cost_lower << endl;
    if ( conf.has( "label_cost_upper" ) ) {
	Drawing::label_cost_upper = stod( conf.gets( "label_cost_upper" ) );
	max_label_spinner = ( float )Drawing::label_cost_upper;
    }
    cerr << HERE << " label_cost_upper = " << Drawing::label_cost_upper << endl;
    if ( conf.has( "label_cost_inside" ) )
	Drawing::label_cost_single = stod( conf.gets( "label_cost_single" ) );
    cerr << HERE << " label_cost_single = " << Drawing::label_cost_single << endl;
    //------------------------------------------------------------------------------
    glutInit( &argc, argv );

    fig.redraw	=    glutPostRedisplay;
    
    if ( argc >= 2 ) {
	string inputname =  argv[ argc - 1 ];
	vector< string > slashlist = CSVIO::split( inputname, '/' );
        string lastblock;
        for ( vector< string >::iterator iterS = slashlist.begin();
              iterS != slashlist.end(); iterS++ ) {
            lastblock = (*iterS);
        }
        vector< string > dotlist = CSVIO::split( lastblock, '.' );
        vector< string >::iterator iterD = dotlist.begin();
        if ( iterD != dotlist.end() ) {
            headname = (*iterD);
        }
	cerr << HERE << " headname ======> : " << headname << endl;
	load_drawing( argv[ argc - 1 ] );
	fig.triangulate();
    }

//------------------------------------------------------------------------------
//	Drawing window
//------------------------------------------------------------------------------
    glutInitWindowPosition
			( WINDOW_LEFT_LIMIT, WINDOW_TOP_LIMIT );
    glutInitWindowSize	( drawing_width, drawing_height );
    glutInitDisplayMode	( GLUT_RGBA | GLUT_DOUBLE );
    win_drawing		= glutCreateWindow( "Drawing" );
    if ( nPolys == 0 ) {
	fig.poly().push_back( Polygon2() );
    }
    glutDisplayFunc	( display_drawing );
    glutReshapeFunc	( resize_drawing );
    glutMouseFunc	( mouse_drawing );
    glutKeyboardFunc	( keyboard );
#ifndef USE_CONVEX_HULLS
#ifdef SKIP
    glutSpecialFunc	( special );
#endif	// SKIP
#endif	// USE_CONVEX_HULLS

    // int menu_capture	= glutCreateMenu( perform_task );
    // glutAddMenuEntry    ( "DRAWING",	CAPTURE_DRAWING );

    int menu_flip	= glutCreateMenu( perform_task );
    glutAddMenuEntry	( "Conjoined", FLIP_CONJOINED );
    glutAddMenuEntry	( "Wrapped",   FLIP_WRAPPED );

    int menu_capture	= glutCreateMenu( perform_task );
    glutAddMenuEntry	( "Drawing window",	CAPTURE_DRAWING );
    glutAddMenuEntry	( "Chart window",	CAPTURE_CHART );

    glutCreateMenu	( perform_task );
    glutAddMenuEntry	( "Clear",	CLEAR_DRAWING );
    glutAddMenuEntry	( "Load",	LOAD_DRAWING );
    glutAddMenuEntry	( "Save",	SAVE_DRAWING );
    glutAddMenuEntry	( "Conjoin",	JOIN_GESTALTS );
    glutAddMenuEntry	( "Optimize",	OPT_DRAWING );
    glutAddSubMenu	( "Switch",	menu_flip );
    glutAddMenuEntry	( "Capture",	menu_capture );
    glutAddMenuEntry	( "Quit",	QUIT_PROGRAM );
    glutAttachMenu	( GLUT_RIGHT_BUTTON );

    init();

    // initialize GLUI here
    init_GLUI();

//------------------------------------------------------------------------------
//	Design window
//------------------------------------------------------------------------------
    glutInitWindowPosition
			( WINDOW_LEFT_LIMIT + drawing_width, WINDOW_TOP_LIMIT );
    glutInitWindowSize	( design_width, design_height );
    glutInitDisplayMode	( GLUT_RGBA | GLUT_DOUBLE );
    win_design		= glutCreateWindow( "Design" );
    glutDisplayFunc	( display_design );
    glutReshapeFunc	( resize_design );
    glutMouseFunc	( mouse_design );
    glutMotionFunc	( motion_design );
    glutKeyboardFunc	( keyboard );

    init();

//------------------------------------------------------------------------------
//	Chart window
//------------------------------------------------------------------------------
    glutInitWindowPosition
			( WINDOW_LEFT_LIMIT + drawing_width + design_width,
			  WINDOW_TOP_LIMIT + 200 );
    glutInitWindowSize	( chart_width, chart_height );
    glutInitDisplayMode	( GLUT_RGBA | GLUT_DOUBLE );
    win_chart		= glutCreateWindow( "Chart" );
    glutDisplayFunc	( display_chart );
    glutReshapeFunc	( resize_chart );
    glutMouseFunc	( mouse_chart );
    glutMotionFunc	( motion_chart );
    glutKeyboardFunc	( keyboard );

    init();
  
    glutMainLoop();
    return 0;
}
#endif	// SKIP

#ifdef SKIP
static void system_conf( const string & path )
{
    //------------------------------------------------------------------------------
    //	read the configuration file
    OZ::Option::Config conf( path );
    //------------------------------------------------------------------------------
    if ( conf.has( "num_trials" ) )
	Drawing::num_trials = ( unsigned int )stoi( conf.gets( "num_trials" ) );
    cerr << HERE << " num_trials = " << Drawing::num_trials << endl;
    //------------------------------------------------------------------------------
    if ( conf.has( "interval_threshold" ) ) {
	Drawing::interval_threshold = stod( conf.gets( "interval_threshold" ) );
	interval_spinner = ( float )( interval_scale * Drawing::interval_threshold );
    }
    cerr << HERE << " interval_threshold = " << Drawing::interval_threshold << endl;
    //------------------------------------------------------------------------------
    if ( conf.has( "proximity_upper" ) )
	Drawing::proximity_upper = stod( conf.gets( "proximity_upper" ) );
    cerr << HERE << " proximity_upper = " << Drawing::proximity_upper << endl;
#ifdef USING_SIMILARITY_CONJOINING
    //------------------------------------------------------------------------------
    if ( conf.has( "similarity_lower" ) )
	Drawing::similarity_lower = stod( conf.gets( "similarity_lower" ) );
    cerr << HERE << " similarity_lower = " << Drawing::similarity_lower << endl;
#endif	// USING_SIMILARITY_CONJOINING
    //------------------------------------------------------------------------------
    if ( conf.has( "proximity_ratio" ) )
	Drawing::proximity_ratio = stod( conf.gets( "proximity_ratio" ) );
    cerr << HERE << " proximity_ratio = " << Drawing::proximity_ratio << endl;
#ifdef USING_SIMILARITY_CONJOINING
    //------------------------------------------------------------------------------
    if ( conf.has( "similarity_ratio" ) )
	Drawing::similarity_ratio = stod( conf.gets( "similarity_ratio" ) );
    cerr << HERE << " similarity_ratio = " << Drawing::similarity_ratio << endl;
#endif	// USING_SIMILARITY_CONJOINING
    //------------------------------------------------------------------------------
    if ( conf.has( "data_cost_lower" ) ) {
	Drawing::data_cost_lower = stod( conf.gets( "data_cost_lower" ) );
	min_data_spinner = ( float )Drawing::data_cost_lower;
    }
    cerr << HERE << " data_cost_lower = " << Drawing::data_cost_lower << endl;
    if ( conf.has( "data_cost_upper" ) ) {
	Drawing::data_cost_upper = stod( conf.gets( "data_cost_upper" ) );
	max_data_spinner = ( float )Drawing::data_cost_upper;
    }
    cerr << HERE << " data_cost_upper = " << Drawing::data_cost_upper << endl;
    if ( conf.has( "data_cost_inside" ) )
	Drawing::data_cost_inside = stod( conf.gets( "data_cost_inside" ) );
    cerr << HERE << " data_cost_inside = " << Drawing::data_cost_inside << endl;
    //------------------------------------------------------------------------------
    if ( conf.has( "smooth_cost_lower" ) ) {
	Drawing::smooth_cost_lower = stod( conf.gets( "smooth_cost_lower" ) );
	min_data_spinner = ( float )Drawing::smooth_cost_lower;
    }
    cerr << HERE << " smooth_cost_lower = " << Drawing::smooth_cost_lower << endl;
    if ( conf.has( "smooth_cost_upper" ) ) {
	Drawing::smooth_cost_upper = stod( conf.gets( "smooth_cost_upper" ) );
	max_data_spinner = ( float )Drawing::smooth_cost_upper;
    }
    cerr << HERE << " smooth_cost_upper = " << Drawing::smooth_cost_upper << endl;
    //------------------------------------------------------------------------------
    if ( conf.has( "label_cost_lower" ) ) {
	Drawing::label_cost_lower = stod( conf.gets( "label_cost_lower" ) );
	min_label_spinner = ( float )Drawing::label_cost_lower;
    }
    cerr << HERE << " label_cost_lower = " << Drawing::label_cost_lower << endl;
    if ( conf.has( "label_cost_upper" ) ) {
	Drawing::label_cost_upper = stod( conf.gets( "label_cost_upper" ) );
	max_label_spinner = ( float )Drawing::label_cost_upper;
    }
    cerr << HERE << " label_cost_upper = " << Drawing::label_cost_upper << endl;
    if ( conf.has( "label_cost_inside" ) )
	Drawing::label_cost_single = stod( conf.gets( "label_cost_single" ) );
    cerr << HERE << " label_cost_single = " << Drawing::label_cost_single << endl;
    //------------------------------------------------------------------------------
}
#endif	// SKIP

// Menu callback
static void menu_callback( Fl_Widget *w, void * )
{
    Fl_Menu_Bar *bar = (Fl_Menu_Bar*)w;	      // Get the menubar widget
    const Fl_Menu_Item *item = bar->mvalue(); // Get the menu item that was picked

    // Get full pathname of picked item
    char ipath[ 256 ];
    bar->item_pathname( ipath, sizeof( ipath ) );
    
    cerr << "callback: You picked " << item->label(); // Print item picked
    cerr << ", item_pathname() is " << ipath;	      // ..and full pathname

    // Toggle or radio item?
    if ( item->flags & (FL_MENU_RADIO|FL_MENU_TOGGLE) )	{
	cerr << ", value is " << item->value()?"on":"off";
    }
    cerr << endl;

    if ( strcmp( ipath, "&Capture/&Drawing" ) == 0 ) {
	gl_drawing->capture( "drawing.png" );
    }

    if ( strcmp( item->label(), "&Quit" ) == 0 ) { exit(0); }
}


// main function
int main( int argc, char *argv[] )
{
#ifdef SKIP
    //------------------------------------------------------------------------------
    //	Command line handling
    // "c:"	allow -c with one argument for the option
    // reference
    // "c"	no argument
    // "c::"	argument is optional (allow both no arguments and one argument)
    string	path		= "configs/default.conf";
    int		option;
    while ( ( option = getopt( argc, argv, "c:" ) ) != -1 ) {
	switch ( option ) {
	  case 'c':		// option: -c <config file name>
	      path = optarg;
	      break;
	  case '?':		// unknown option
	      cerr << argv[ 0 ] << ": Unknown option: default setup will be used" << endl;
	      break;
	}
    }
    cerr << "loading configuration from " << path << endl;
#endif	// SKIP

    //------------------------------------------------------------------------------
    //	Allocating memory for variables
    //------------------------------------------------------------------------------
    fig		= new Drawing();
    worksp	= new Workspace();

    //------------------------------------------------------------------------------
    //	Read the configuration file
    //------------------------------------------------------------------------------
    // sytem_config( path );
    
    Fl::scheme( "flaggsim" );
    Fl::visual( FL_DOUBLE | FL_RGB );
    Fl::get_system_colors();

//------------------------------------------------------------------------------
//	Map window
//------------------------------------------------------------------------------
    Fl_Window * win_map = new Fl_Window( 0, 0,
					 map_width, map_height,
					 "Map window" );

    //------------------------------------------------------------------------------
    //------------------------------------------------------------------------------
    gl_drawing = new GLDrawing( 0, 0, map_width, map_height, "Drawing" );

    gl_drawing->setFig		( fig );
    gl_drawing->setWorkspace	( worksp );

    gl_drawing->load_drawing( "akehara-008-case.dat" );
    // gl_drawing->fig()->triangulate();
    fig->triangulate();
    
    // gl_drawing->box( FL_FLAT_BOX );
    gl_drawing->begin();

    // gl_drawing->mode( FL_RGB | FL_ALPHA | FL_DOUBLE | FL_DEPTH );
    gl_drawing->mode( FL_RGB | FL_DOUBLE );
    gl_drawing->resizable( win_map );

    win_map->end();
    win_map->show( argc, argv );

//------------------------------------------------------------------------------
//	Design window	
//------------------------------------------------------------------------------
    Fl_Window * win_design = new Fl_Window( map_width, 0,
					    design_width, design_height,
					    "Design window" );

    gl_layout = new GLLayout( 0, 0, design_width, design_height, "Layout" );

    gl_layout->setFig		( fig );
    gl_layout->setWorkspace	( worksp );

    gl_layout->begin();

    gl_layout->mode		( FL_RGB | FL_DOUBLE );
    gl_layout->resizable	( win_design );

    win_design->end();
    win_design->show( argc, argv );

//------------------------------------------------------------------------------
//	Panel window	
//------------------------------------------------------------------------------
    FLControl * win_panel = new FLControl( map_width+design_width, 0,
					   panel_width, panel_height,
					   "Panel window" );
    win_panel->setGLDrawing( gl_drawing );
    win_panel->setGLLayout ( gl_layout );
    win_panel->end();
    win_panel->show( argc, argv );
    
//------------------------------------------------------------------------------
//	Set mutual redraw functions
//------------------------------------------------------------------------------
    gl_drawing->addFlWin	( gl_layout );
    gl_layout->addFlWin		( gl_drawing );
    
    return Fl::run();
}

