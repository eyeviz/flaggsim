//==============================================================================
// FLControl.cpp
//	: program file for control panel window
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-11-13 21:48:35 shigeo"
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
void FLControl::_IOHandler( Fl_Menu_Bar * bar )
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
	  cerr << HERE << " _gl_drawing = " << _glDrawing << endl;
	  if ( strcmp( ipath, "&File/&Load" ) == 0 ) {
	      _glDrawing->load_drawing( chooser.filename() );
	      _slider->resetValues();
	  }
	  else if ( strcmp( ipath, "&File/&Save" ) == 0 ) {
	      _glDrawing->save_drawing( chooser.filename() );
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
void FLControl::_IOCallback( Fl_Widget *w, void * userdata )
{
    Fl_Menu_Bar *bar = (Fl_Menu_Bar*)w;	      // Get the menubar widget
    cerr << HERE << " bar = " << bar << endl;
    FLControl *appwin = (FLControl*)userdata;
    cerr << HERE << " appwin = " << appwin << endl;
    
    appwin->_IOHandler( bar );
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
	if ( _glDrawing->fig() != NULL ) _glDrawing->fig()->clear();
	if ( _adjust != NULL ) _adjust->clear();
	_glDrawing->redraw();
	_glLayout->redraw();
    }

    if ( strcmp( ipath, "&Edit/Unselect" ) == 0 ) {
	if ( _glDrawing != NULL ) {
	    _glDrawing->release();
	    _glDrawing->redraw();
	}
    }

    if ( strcmp( ipath, "&Switch/Conjoined" ) == 0 ) {
	if ( _glDrawing != NULL ) {
	    if ( _glDrawing->isConjoined() ) _glDrawing->clearConjoined();
	    else _glDrawing->setConjoined();
	    _glDrawing->redraw();
	}
    }

    if ( strcmp( ipath, "&Switch/Wrapped" ) == 0 ) {
	if ( _glDrawing != NULL ) {
	    if ( _glDrawing->isWrapped() ) _glDrawing->clearWrapped();
	    else _glDrawing->setWrapped();
	    _glDrawing->redraw();
	}
    }

    if ( strcmp( ipath, "&Capture/&Drawing" ) == 0 ) {
	if ( _glDrawing != NULL )
	    _glDrawing->capture( "drawing.png" );
    }

    if ( strcmp( ipath, "&Capture/&Layout" ) == 0 ) {
	if ( _glLayout != NULL )
	    _glLayout->capture( "layout.png" );
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
//	For buttons
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	File Load & Save

// handler function for loading data from files
void FLControl::_fileHandler( Fl_Button * b )
{
    // Create the file chooser, and show it
    Fl_Native_File_Chooser chooser( Fl_Native_File_Chooser::BROWSE_FILE );
    chooser.title( "Select File" );
    chooser.type( Fl_Native_File_Chooser::BROWSE_FILE );
    chooser.directory( "./map/" );
    if ( ( strcmp( b->label(), "Load" ) == 0 ) ||
	 ( strcmp( b->label(), "Save" ) == 0 ) )
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
	  cerr << HERE << " _glDrawing = " << _glDrawing << endl;
	  if ( strcmp( b->label(), "Load" ) == 0 ) {
	      if ( _glDrawing == NULL ) {
		  cerr << HERE << " Null pointer to the city block" << endl;
	      }
	      else {
		  _glDrawing->load_drawing( chooser.filename() );
		  _slider->resetValues();
	      }
	      redrawAll();
	  }
#ifdef SKIP
	  if ( strcmp( b->label(), "Save" ) == 0 ) {
	      // _glUrban->loadUrban( chooser.filename() );
	      redrawAll();
	  }
#endif	// SKIP
	  else {
	      cerr << HERE << " No button items matched" << endl;
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
    Fl_Button *b = (Fl_Button*)w;	      // Get the button widget
    FLControl *appwin = (FLControl*)userdata;
    
    appwin->_fileHandler( b );
}


//------------------------------------------------------------------------------
//	Map Edit

// handler function for handling button selections
void FLControl::_editHandler( Fl_Button * b )
{
    if ( strcmp( b->label(), "Aggregate" ) == 0 ) {
	cerr << HERE << " aggregate building polygons" << endl;
	_glDrawing->enableAggregated();
	_glDrawing->Keyboard( 'a', 0, 0 );
	// _urban->restore();
	// _urban->modeOpt() = ORIGINAL;
	// _glUrban->clearPlot();
	redrawAll();
    }

    if ( strcmp( b->label(), "Simplify" ) == 0 ) {
	cerr << HERE << " simplify aggregated polygons " << endl;
	_glDrawing->disableAggregated();
	_glDrawing->Keyboard( 'z', 0, 0 );
	// State currState;
	// _glUrban->record();
	// currState.checkin( _glUrban->layerBak(), _glUrban->scopeBak() );
	// _glUrban->undoStack().push( currState );
	// while ( ! _glUrban->redoStack().empty() ) _glUrban->redoStack().pop();
	// cerr << HERE << " Undo stack size = " << _glUrban->undoStack().size() << endl;
	// cerr << HERE << " Redo Stack size = " << _glUrban->redoStack().size() << endl;
	redrawAll();
    }

    if ( strcmp( b->label(), "Fix" ) == 0 ) {
	cerr << HERE << " fix the selection of aggregated polygons " << endl;
	_glDrawing->disableAggregated();
	_glDrawing->Keyboard( 'x', 0, 0 );
	// _urban->allFix();
	redrawAll();
    }
}

// callback function for handling edit buttons
void FLControl::_editCallback( Fl_Widget *w, void * userdata )
{
    Fl_Button *b = (Fl_Button*)w;	      // Get the button widget
    FLControl *appwin = (FLControl*)userdata;
    
    appwin->_editHandler( b );
}

//------------------------------------------------------------------------------
//	Flag switches

// handler function for handling switch buttons
void FLControl::_switchHandler( Fl_Check_Button * cb )
{
    if ( strcmp( cb->label(), "Conjoin" ) == 0 ) {
	if ( _glDrawing != NULL ) {
	    if ( cb->value() ) {
		_glDrawing->setConjoined(); // on
	    }
	    else {
		_glDrawing->clearConjoined(); // off
	    }
	    redrawAll();
	}
    }
    else if ( strcmp( cb->label(), "Wrap" ) == 0 ) {
	if ( _glDrawing != NULL ) {
	    if ( cb->value() ) {
		_glDrawing->setWrapped(); // on
	    }
	    else {
		_glDrawing->clearWrapped(); // off
	    }
	    redrawAll();
	}
    }
    else if ( strcmp( cb->label(), "Plot" ) == 0 ) {
	if ( _glDrawing != NULL ) {
	    if ( cb->value() ) {
		_glDrawing->setPlotted(); // on
	    }
	    else {
		_glDrawing->clearPlotted(); // off
	    }
	    redrawAll();
	}
    }
}

// callback function for switching flags
void FLControl::_switchCallback( Fl_Widget *w, void * userdata )
{
    Fl_Check_Button *cb = (Fl_Check_Button*)w;	      // Get the button widget
    FLControl *appwin = (FLControl*)userdata;
    
    appwin->_switchHandler( cb );
}

//------------------------------------------------------------------------------
//	Capturing windows

// handler function for capturing windows
void FLControl::_captureHandler( Fl_Button * b )
{
    // Create the file chooser, and show it
    Fl_Native_File_Chooser chooser( Fl_Native_File_Chooser::BROWSE_SAVE_FILE );
    chooser.title( "Select File" );
    chooser.type( Fl_Native_File_Chooser::BROWSE_SAVE_FILE );
    chooser.options( Fl_Native_File_Chooser::SAVEAS_CONFIRM );
    string filename = _glDrawing->headname();
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
    if ( strcmp( b->label(), "Drawing" ) == 0 ) {
	filename += "_drawing";
    }
    else if ( strcmp( b->label(), "Layout" ) == 0 ) {
	filename += "_layout";
    }
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
    if ( strcmp( b->label(), "Drawing" ) == 0 ) {
	chooser.directory( "./snap/" );
	// chooser.preset_file( filename.c_str() );
	//------------------------------------------------------------------------------
	// file name extension
	filename += ".png";
	chooser.preset_file( filename.c_str() );
	chooser.filter( "*.png" );
    }
    else if ( strcmp( b->label(), "Layout" ) == 0 ) {
	chooser.directory( "./snap/" );
	// chooser.preset_file( filename.c_str() );
	//------------------------------------------------------------------------------
	// file name extension
	filename += ".png";
	chooser.preset_file( filename.c_str() );
	chooser.filter( "*.png" );
    }
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
	  if ( strcmp( b->label(), "Drawing" ) == 0 ) {
	      if ( _glDrawing != NULL )
		  _glDrawing->capture( chooser.filename() );
	      else
		  cerr << HERE << " Cannot save the file: " << chooser.filename() << endl;
	  }
	  else if ( strcmp( b->label(), "Layout" ) == 0 ) {
	      if ( _glLayout != NULL )
		  _glLayout->capture( chooser.filename() );
	      else
		  cerr << HERE << " Cannot save the file: " << chooser.filename() << endl;
	  }
	  else {
	      cerr << HERE << " No botton items matched" << endl;
	  }
    }
}

// callback function for switching flags
void FLControl::_captureCallback( Fl_Widget *w, void * userdata )
{
    Fl_Button *b = (Fl_Button*)w;	      // Get the button widget
    FLControl *appwin = (FLControl*)userdata;
    
    appwin->_captureHandler( b );
}

//------------------------------------------------------------------------------
//	Quit the program

// handler function for handling the quit button
void FLControl::_quitHandler( Fl_Button * b )
{
    cerr << " Button " << b->label() << " was ";
    if ( b->value() ) cerr << " Pushed";
    else cerr << " Released";
    cerr << endl;

    if ( strcmp( b->label(), "Quit" ) == 0 ) {
	exit(0);
    }
}


// callback function for handling program quit
void FLControl::_quitCallback( Fl_Widget *w, void * userdata )
{
    Fl_Button *b = (Fl_Button*)w;	      // Get the button widget
    FLControl *appwin = (FLControl*)userdata;
    // fprintf(stderr, "Button '%s' was %s\n", b->label(), b->value() ? "Pushed" : "Released");
    
    appwin->_quitHandler( b );
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
		      GLDrawing * __glDrawing, GLLayout * __glLayout,
		      int _x, int _y, int _w, int _h, const char *_l )
    : Fl_Window( _x, _y, _w, _h, _l )  	
{
    int bottom = 0;
    // int vsize = 18;
    int vsize = 15;
    
    _adjust		= __adjust;
    _glDrawing		= __glDrawing;
    _glLayout		= __glLayout;
    
#ifdef SKIP
    //------------------------------------------------------------------------------
    //	Preparing the menu bar
    //------------------------------------------------------------------------------
    _menubar = new Fl_Menu_Bar( 0, 0, _w, 25 );

    _menubar->add( "&File/&Load",	0,	_IOCallback,	( void* )this );
    _menubar->add( "&File/&Save",	0,	_IOCallback,	( void* )this );
    _menubar->add( "&File/&Clear",	0,	_menuCallback,	( void* )this );
    
    _menubar->add( "&Edit/Unselect",	0,	_menuCallback,	( void* )this );
    _menubar->add( "&Edit/con&Join",	0,	_menuCallback,	( void* )this );
    _menubar->add( "&Edit/&Optimize",	0,	_menuCallback,	( void* )this );
    
    _menubar->add( "&Switch/Conjoined",	0,	_menuCallback,	( void* )this );
    _menubar->add( "&Switch/Wrapped",	0,	_menuCallback,	( void* )this );

    _menubar->add( "&Capture/&Drawing",	0,	_menuCallback,	( void* )this );
    _menubar->add( "&Capture/&Layout",	0,	_menuCallback,	( void* )this );

    _menubar->add( "&Quit",		0,	_menuCallback,	( void* )this );
#endif	// SKIP

//------------------------------------------------------------------------------
//	
//	Preparing the control panel
//	
//------------------------------------------------------------------------------
    Fl_Button			* b;
    Fl_Check_Button		* cb;
    Fl_Radio_Round_Button	* rrb;
    int		groupX, groupY, groupW, groupH;
    int		placeX, placeY, placeW, placeH;
    

    //------------------------------------------------------------------------------
    //	Preparing buttons for the FILE category
    //------------------------------------------------------------------------------
    // groupX = 10; groupW = _w - 20; groupY = 25, groupH = 45;
    groupX = 10; groupW = _w - 20; groupY = 25, groupH = 45;
    Fl_Group	* groupFile = new Fl_Group( groupX, groupY, groupW, groupH,
					    "[File]" );
    
    groupFile->box( FL_ENGRAVED_BOX );
    groupFile->align( FL_ALIGN_TOP );
    groupFile->labelsize( 24 );
    box( FL_BORDER_BOX ); color( 42 );

    placeX = groupX + 10*groupW/25;
    placeY = groupY + 10;
    placeW = 2*groupW/10;
    placeH = 25;
    b = new Fl_Button( placeX, placeY, placeW, placeH, "Load" );
    b->callback( _fileCallback, this );
    
    groupFile->end();
    
    //------------------------------------------------------------------------------
    //  Preparing buttons for the EDIT category
    //------------------------------------------------------------------------------
    groupX = 10; groupW = _w - 20; groupY = 2*_h/vsize + 25, groupH = 45;
    Fl_Group	* groupEdit = new Fl_Group( groupX, groupY, groupW, groupH,
					    "[Edit]" );
    groupEdit->box( FL_ENGRAVED_BOX );
    groupEdit->align( FL_ALIGN_TOP );
    groupEdit->labelsize( 24 );
    box( FL_BORDER_BOX ); color( 44 );

    //------------------------------------------------------------------------------
    //------------------------------------------------------------------------------
    placeX = groupX + 4*groupW/25;
    placeY = groupY + 10;
    placeW = 2*groupW/10;
    placeH = 25;
    b = new Fl_Button( placeX, placeY, placeW, placeH, "Aggregate" );
    b->callback( _editCallback, this );

    placeX += 6*groupW/25;
    b = new Fl_Button( placeX, placeY, placeW, placeH, "Simplify" );
    b->callback( _editCallback, this );

    placeX += 6*groupW/25;
    b = new Fl_Button( placeX, placeY, placeW, placeH, "Fix" );
    b->callback( _editCallback, this );

    groupEdit->end();

    //------------------------------------------------------------------------------
    //  Preparing buttons for the SWITCH category
    //------------------------------------------------------------------------------
    groupX = 10; groupW = _w - 20; groupY = 4*_h/vsize + 25, groupH = 45;
    Fl_Group	* groupSwitch = new Fl_Group( groupX, groupY, groupW, groupH,
					      "[Switch]" );
    groupSwitch->box( FL_ENGRAVED_BOX );
    groupSwitch->align( FL_ALIGN_TOP );
    groupSwitch->labelsize( 24 );
    box( FL_BORDER_BOX ); color( 48 );

    //------------------------------------------------------------------------------
    //------------------------------------------------------------------------------
    placeX = groupX + 4*groupW/25;
    placeY = groupY + 10;
    placeW = 2*groupW/10;
    placeH = 25;

    cb = new Fl_Check_Button( placeX, placeY, placeW, placeH, "Conjoin" );
    cb->callback( _switchCallback, this );

    placeX += 6*groupW/25;
    cb = new Fl_Check_Button( placeX, placeY, placeW, placeH, "Wrap" );
    cb->callback( _switchCallback, this );

    placeX += 6*groupW/25;
    cb = new Fl_Check_Button( placeX, placeY, placeW, placeH, "Plot" );
    cb->callback( _switchCallback, this );

    groupSwitch->end();

    //------------------------------------------------------------------------------
    //	Preparing sliders for parameter tuning
    //------------------------------------------------------------------------------
    groupX = 100; groupW = _w - groupX - 20; groupY = 6*_h/vsize + 25, groupH = 180;
    _slider = new FLSliderInput( __adjust, __glDrawing, __glLayout,
				 groupX, groupY, groupW, groupH,
				 // border, bottom, this->w() - border - 40, 200,
				 "Parameter panel" );
    
    //------------------------------------------------------------------------------
    //  Preparing buttons for the CAPTURE category
    //------------------------------------------------------------------------------
    groupX = 10; groupW = _w - 20; groupY = 11*_h/vsize + 25, groupH = 45;
    Fl_Group	* groupCapture = new Fl_Group( groupX, groupY, groupW, groupH,
					       "[Capture]" );
    groupCapture->box( FL_ENGRAVED_BOX );
    groupCapture->align( FL_ALIGN_TOP );
    groupCapture->labelsize( 24 );
    box( FL_BORDER_BOX ); color( 50 );

    placeX = groupX + 7*groupW/25;
    placeY = groupY + 10;
    placeW = 2*groupW/10;
    placeH = 25;

    b = new Fl_Button( placeX, placeY, placeW, placeH, "Drawing" );
    b->callback( _captureCallback, this );

    placeX += 6*groupW/25;
    b = new Fl_Button( placeX, placeY, placeW, placeH, "Layout" );
    b->callback( _captureCallback, this );

    groupCapture->end();

    //------------------------------------------------------------------------------
    //  Preparing buttons for the QUIT category
    //------------------------------------------------------------------------------
    // Fl_Group	* groupQuit = new Fl_Group( _x, _y + 7*_h/8, _w, _h/8, "Quit" );
    groupX = 10; groupW = _w - 20; groupY = (vsize-2)*_h/vsize + 25, groupH = 45;
    Fl_Group	* groupQuit = new Fl_Group( groupX, groupY, groupW, groupH,
					    "[Quit]" );
    groupQuit->box( FL_ENGRAVED_BOX );
    // groupQuit->box( FL_ROUNDED_BOX );
    // groupQuit->box( FL_SHADOW_BOX );
    // groupQuit->align( FL_ALIGN_INSIDE | FL_ALIGN_TOP );
    groupQuit->align( FL_ALIGN_TOP );
    groupQuit->labelsize( 24 );
    box( FL_BORDER_BOX ); color( 52 );
    
    // b = new Fl_Button( x() + 1*_w/10, y() + 5,	25, 25, "Quit" );
    b = new Fl_Button( groupX + groupW/2 - 40, groupY + 10, 80, 25, "Quit" );
    b->callback( _quitCallback, this );

    groupQuit->end();
			  
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
    delete _slider;
}


//------------------------------------------------------------------------------
//	Referrring to members
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Drawing functions
//------------------------------------------------------------------------------
// Function for redrawing associative windows as well as this one
void FLControl::redrawAll( void )
{
    if ( _glDrawing != NULL ) _glDrawing->redraw();
    if ( _glLayout != NULL ) _glLayout->redraw();
}


//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------


// end of header file
// Do not add any stuff under this line.
