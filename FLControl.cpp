//==============================================================================
// FLControl.cpp
//	: program file for control panel window
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-08-21 19:06:35 shigeo"
//
//==============================================================================

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------
#include <iostream>

using namespace std;

#include <FL/Fl_Native_File_Chooser.H>

#include "FLControl.h"


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Private Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	For menubars 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	File Load & Save

// handler function for loading data from files
void FLControl::_fileHandler( Fl_Menu_Bar * bar )
{
    const Fl_Menu_Item *item = bar->mvalue(); // Get the menu item that was picked

    // Get full pathname of picked item
    char ipath[ 256 ];
    bar->item_pathname( ipath, sizeof( ipath ) );

    // Create the file chooser, and show it
    Fl_Native_File_Chooser chooser( Fl_Native_File_Chooser::BROWSE_FILE );
    chooser.title( "Select File" );
    chooser.type( Fl_Native_File_Chooser::BROWSE_FILE );
    chooser.directory( "./map/" );
    if ( ( strcmp( ipath, "&File/&Load" ) == 0 ) ||
	 ( strcmp( ipath, "&File/&Save" ) == 0 ) )
	chooser.filter( "*.dat" );
    else
	chooser.filter( "*" );
    
    switch ( chooser.show() ) {
      case -1: // ERROR
	  cerr << HERE << "ERROR: " << chooser.errmsg() << endl;
	  break;
      case 1: // CANCEL
	  cerr << HERE << "CANCEL" << endl;
	  break;
      default: // FILE CHOSEN
	  cerr << HERE << "PICKED: " << chooser.filename() << endl;
	  cerr << HERE << " _gl_drawing = " << FLControl::_gl_drawing << endl;
	  if ( strcmp( ipath, "&File/&Load" ) == 0 ) {
	      FLControl::_gl_drawing->load_drawing( chooser.filename() );
	  }
	  else if ( strcmp( ipath, "&File/&Save" ) == 0 ) {
	      FLControl::_gl_drawing->save_drawing( chooser.filename() );
	  }
	  else {
	      cerr << HERE << " No menu items matched" << endl;
	  }
#ifdef SKIP
	  // Multiple files? Show all of them
	  if ( chooser.count() > 0 ) {
	      for ( int t = 0; t < chooser.count(); t++ ) {
		  fprintf(stderr, " VALUE[%d]: '%s'\n", t, chooser.filename( t ) );
	      }
	  }
#endif	// SKIP
	  break;
    }
}
    
// callback function for loading data from files
void FLControl::_fileCallback( Fl_Widget *w, void * userdata )
{
    Fl_Menu_Bar *bar = (Fl_Menu_Bar*)w;	      // Get the menubar widget
    cerr << HERE << " bar = " << bar << endl;
    FLControl *appwin = (FLControl*)userdata;
    cerr << HERE << " appwin = " << appwin << endl;
    
    appwin->_fileHandler( bar );
}

//------------------------------------------------------------------------------
//	Menu selection

// handler function for handling menu selections
void FLControl::_menuHandler( Fl_Menu_Bar * bar )
{
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

    if ( strcmp( ipath, "&Edit/Clear" ) == 0 ) {
	if ( _gl_drawing->fig() != NULL ) _gl_drawing->fig()->clear();
	if ( _adjust != NULL ) _adjust->clear();
	_gl_drawing->redraw();
	_gl_layout->redraw();
    }

    if ( strcmp( ipath, "&Edit/Unselect" ) == 0 ) {
	if ( _gl_drawing != NULL ) {
	    _gl_drawing->unselect();
	    _gl_drawing->redraw();
	}
    }

    if ( strcmp( ipath, "&Switch/Conjoined" ) == 0 ) {
	if ( _gl_drawing != NULL ) {
	    if ( _gl_drawing->isConjoined() ) _gl_drawing->clearConjoined();
	    else _gl_drawing->setConjoined();
	    _gl_drawing->redraw();
	}
    }

    if ( strcmp( ipath, "&Switch/Wrapped" ) == 0 ) {
	if ( _gl_drawing != NULL ) {
	    if ( _gl_drawing->isWrapped() ) _gl_drawing->clearWrapped();
	    else _gl_drawing->setWrapped();
	    _gl_drawing->redraw();
	}
    }

    if ( strcmp( ipath, "&Capture/&Drawing" ) == 0 ) {
	if ( _gl_drawing != NULL )
	    _gl_drawing->capture( "drawing.png" );
    }

    if ( strcmp( ipath, "&Capture/&Layout" ) == 0 ) {
	if ( _gl_layout != NULL )
	    _gl_layout->capture( "layout.png" );
    }

    if ( strcmp( item->label(), "&Quit" ) == 0 ) { exit(0); }
}

// callback function for handling menu selections
void FLControl::_menuCallback( Fl_Widget *w, void * userdata )
{
    Fl_Menu_Bar *bar = (Fl_Menu_Bar*)w;	      // Get the menubar widget
    FLControl *appwin = (FLControl*)userdata;
    
    appwin->_menuHandler( bar );
}

//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Public Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------

//
//  FLControl::FLControl --	default constructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
FLControl::FLControl( Adjuster * __adjust,
		      GLDrawing * __gl_drawing, GLLayout * __gl_layout,
		      int _x, int _y, int _w, int _h, const char *_l )
    : Fl_Window( _x, _y, _w, _h, _l )  	
{
    int bottom = 0;

    _adjust		= __adjust;
    _gl_drawing		= __gl_drawing;
    _gl_layout		= __gl_layout;
    
    //------------------------------------------------------------------------------
    //	Preparing the menu bar
    //------------------------------------------------------------------------------
    _menubar = new Fl_Menu_Bar( 0, 0, _w, 25 );

    _menubar->add( "&File/&Load",	0,	_fileCallback,	( void* )this );
    _menubar->add( "&File/&Save",	0,	_fileCallback,	( void* )this );
    _menubar->add( "&File/&Clear",	0,	_menuCallback,	( void* )this );
    
    _menubar->add( "&Edit/Unselect",	0,	_menuCallback,	( void* )this );
    _menubar->add( "&Edit/con&Join",	0,	_menuCallback,	( void* )this );
    _menubar->add( "&Edit/&Optimize",	0,	_menuCallback,	( void* )this );
    
    _menubar->add( "&Switch/Conjoined",	0,	_menuCallback,	( void* )this );
    _menubar->add( "&Switch/Wrapped",	0,	_menuCallback,	( void* )this );

    _menubar->add( "&Capture/&Drawing",	0,	_menuCallback,	( void* )this );
    _menubar->add( "&Capture/&Layout",	0,	_menuCallback,	( void* )this );

    _menubar->add( "&Quit",		0,	_menuCallback,	( void* )this );

    //------------------------------------------------------------------------------
    //	
    //------------------------------------------------------------------------------
    bottom += 60;
    int border = 100;
    _panel = new FLSliderInput( __adjust, __gl_drawing, __gl_layout,
				border, bottom, this->w() - border - 40, 200,
				"Parameter panel" );

    //------------------------------------------------------------------------------
    //	
    //------------------------------------------------------------------------------
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------

//
//  FLControl::~FLControl --	destructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
FLControl::~FLControl()
{
    delete _menubar;
    delete _panel;
}


//------------------------------------------------------------------------------
//	Referrring to members
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------


// end of header file
// Do not add any stuff under this line.
