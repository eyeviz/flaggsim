//==============================================================================
// FLControl.cpp
//	: program file for control panel window
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-08-07 22:26:32 shigeo"
//
//==============================================================================

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------
#include <iostream>

using namespace std;

#include "FLControl.h"


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Variables
//------------------------------------------------------------------------------
GLBase * FLControl::_gl_drawing = NULL;
GLBase * FLControl::_gl_layout  = NULL;

//------------------------------------------------------------------------------
//	Private Functions
//------------------------------------------------------------------------------
// response to the selected menu item
void FLControl::_menu_handler( Fl_Menu_Bar * bar )
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

    if ( strcmp( ipath, "&Capture/&Drawing" ) == 0 ) {
	cerr << HERE << " FLControl::_gl_drawing = " << FLControl::_gl_drawing << endl;
	if ( FLControl::_gl_drawing != NULL )
	    FLControl::_gl_drawing->capture( "drawing.png" );
    }

    if ( strcmp( ipath, "&Capture/&Layout" ) == 0 ) {
	if ( FLControl::_gl_layout != NULL )
	    FLControl::_gl_layout->capture( "layout.png" );
    }

    if ( strcmp( item->label(), "&Quit" ) == 0 ) { exit(0); }
}

// menu callback function
void FLControl::_menu_callback( Fl_Widget *w, void * userdata )
{
    Fl_Menu_Bar *bar = (Fl_Menu_Bar*)w;	      // Get the menubar widget
    FLControl *appwin = (FLControl*)userdata;
    
    appwin->_menu_handler( bar );
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
FLControl::FLControl( int _x, int _y, int _w, int _h, const char *_l )
    : Fl_Window( _x, _y, _w, _h, _l )  	
{
    Fl_Menu_Bar *menubar = new Fl_Menu_Bar( 0, 0, _w, 25 );

    menubar->add( "&File/&Clear",	0,	_menu_callback );
    menubar->add( "&File/&Load",	0,	_menu_callback );
    menubar->add( "&File/&Save",	0,	_menu_callback );
    
    menubar->add( "&Edit/con&Join",	0,	_menu_callback );
    menubar->add( "&Edit/&Optimize",	0,	_menu_callback );
    
    menubar->add( "&Switch/Conjoined",	0,	_menu_callback );
    menubar->add( "&Switch/Wrapped",	0,	_menu_callback );

    menubar->add( "&Capture/&Drawing",	0,	_menu_callback );
    menubar->add( "&Capture/&Layout",	0,	_menu_callback );

    menubar->add( "&Quit",		0,	_menu_callback );

    _gl_drawing = NULL;
    _gl_layout = NULL;
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

}


//------------------------------------------------------------------------------
//	Referrring to members
//------------------------------------------------------------------------------




// end of header file
// Do not add any stuff under this line.
