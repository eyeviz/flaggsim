//==============================================================================
// main.cpp
//	: main program file
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-10-15 18:09:34 shigeo"
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
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/Fl_Menu_Bar.H>
// #include <FL/filename.H>		// fl_open_uri()


// OpenCV library
#include <opencv2/opencv.hpp>

#include "Common.h"
#include "GLDrawing.h"
#include "GLLayout.h"
#include "FLControl.h"


//------------------------------------------------------------------------------
//	Macro switches
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Macro definitions
//------------------------------------------------------------------------------
#ifndef HERE
#define HERE            " " << __FILE__ << ":" << __LINE__ << " "
#endif  // HERE

#define WINDOW_LEFT_LIMIT	(50)
#define WINDOW_TOP_LIMIT	(50)

#define HALF_SIZE		(256)
#define FULL_SIZE		(512)
#define LARGE_SIZE		(768)
#define HUGE_SIZE		(1024)

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
Adjuster *	adjust		= NULL;

//int		base_size	= HUGE_SIZE;
int		base_size	= LARGE_SIZE;
int		map_width	= base_size;
int		map_height	= base_size;
int		design_width	= base_size;
int		design_height	= base_size;
int		panel_width	= base_size/2;
int		panel_height	= base_size;

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
    adjust	= new Adjuster();

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
    gl_drawing->setAdjuster	( adjust );

    string inputname;
    if ( argc == 2 ) {
	// gl_drawing->load_drawing( argv[ 1 ] );
	inputname = argv[ 1 ];
    }

    // gl_drawing->fig()->triangulate();
    fig->triangulate();
    
    // gl_drawing->box( FL_FLAT_BOX );
    gl_drawing->begin();

    // gl_drawing->mode( FL_RGB | FL_ALPHA | FL_DOUBLE | FL_DEPTH );
    gl_drawing->mode( FL_RGB | FL_DOUBLE );
    gl_drawing->resizable( win_map );

    gl_drawing->end();

    win_map->end();
    win_map->show( argc, argv );

//------------------------------------------------------------------------------
//	Design window	
//------------------------------------------------------------------------------
    Fl_Window * win_design = new Fl_Double_Window( map_width, 0,
						   design_width, design_height,
						   "Design window" );
    Fl_Scroll * win_scroll = new Fl_Scroll( 0, 0, design_width, design_height, "Scroll window" );
    gl_layout = new GLLayout( 0, 0, design_width, design_height, "Layout" );
    win_scroll->end();

    gl_layout->setFig		( fig );
    gl_layout->setWorkspace	( worksp );
    gl_layout->setAdjuster	( adjust );
    
    gl_layout->begin();

    gl_layout->mode		( FL_RGB | FL_DOUBLE );
    gl_layout->resizable	( win_scroll );

    gl_layout->end();
    
    win_design->end();
    win_design->resizable( gl_layout );
    win_design->show( argc, argv );

//------------------------------------------------------------------------------
//	Set mutual redraw functions
//------------------------------------------------------------------------------
    // Set pointers here to prepare the panel window
    gl_layout->setGLDrawing	( gl_drawing );
    gl_drawing->setGLLayout	( gl_layout );

//------------------------------------------------------------------------------
//	Panel window	
//------------------------------------------------------------------------------
    FLControl * win_panel = new FLControl( adjust,
					   gl_drawing, gl_layout,
					   map_width+design_width, 0,
					   panel_width, panel_height,
					   "Panel window" );
    win_panel->end();
    win_panel->show( argc, argv );
    
#ifdef ACTIVATE_RECORDING_MODE
    gl_drawing->setFilled();
    gl_layout->setFilled();
#endif	// ACTIVATE_RECORDING_MODE

    gl_drawing->load_drawing( inputname.c_str() );

//------------------------------------------------------------------------------
//	Printing out the program usage
    cerr << " Key commands " << endl;
    cerr << " a (097) : list the aggregation choices for building polygons" << endl;
    cerr << " b (098) : switch flag for polygon filling" << endl;
    cerr << " c (099) : clear the map drawings" << endl;
    // d(100), e(101)
    cerr << " f (102) : pick up the preferred aggregation choices" << endl;
    // g(103), h(104), i(105)
    cerr << " j (106) : switch flag for conjoining" << endl;
    // k(107), 
    cerr << " l (108) : load the map drawings from files" << endl;
    cerr << " m (109) : capture the map as image files" << endl;
    // n(110), o(111)
    cerr << " p (112) : switch flag for sample plotting" << endl;
    cerr << " q (113) : quit the program" << endl;
    cerr << " r (114) : squaring building polygons" << endl;
    cerr << " s (115) : save the map drawings into files" << endl;
    // t(116), u(117), v(118)
    cerr << " w (119) : switch flag for polygon wrapping" << endl;
    // x(120), y(121), z(122)
//------------------------------------------------------------------------------

    return Fl::run();
}

