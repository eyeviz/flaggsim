//==============================================================================
// FLControl.cpp
//	: program file for control panel window
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-08-18 22:59:05 shigeo"
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
    chooser.directory( "." );
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

// callback function for handling menu selections
void FLControl::_menuCallback( Fl_Widget *w, void * userdata )
{
    Fl_Menu_Bar *bar = (Fl_Menu_Bar*)w;	      // Get the menubar widget
    FLControl *appwin = (FLControl*)userdata;
    
    appwin->_menuHandler( bar );
}

//------------------------------------------------------------------------------
//	slider for intervals
//------------------------------------------------------------------------------
// response to the slider value changes in interval
void FLControl::_intervalHandler( Fl_Value_Slider * slider )
{
    if ( _adjust == NULL ) {
	cerr << " Pointer to adjuster is NULL!" << endl;
	return;
    }
    
    _adjust->interval() = _intervalSlider->value();
    Drawing::interval_threshold =
	_intervalSlider->value() / _adjust->intervalScale();
    _gl_drawing->Keyboard( 'a', 0, 0 );
    _gl_drawing->redraw();
    _gl_layout->redraw();
    cerr << HERE << " slider value = " << _intervalSlider->value() << endl;
    cerr << HERE << " Drawing::interval_threshold = "
	 << fixed << setprecision( 4 )
	 << Drawing::interval_threshold << endl;
}


// slider callback function for intervals
void FLControl::_intervalCallback( Fl_Widget *w, void * userdata )
{
    Fl_Value_Slider *slider = (Fl_Value_Slider*)w;	      // Get the menubar widget
    FLControl *appwin = (FLControl*)userdata;
    
    appwin->_intervalHandler( slider );
}

//------------------------------------------------------------------------------
//	slider for data costs
//------------------------------------------------------------------------------
// response to the slider value changes in data cost
void FLControl::_dataCostHandler( Fl_Value_Slider * slider )
{
    if ( _adjust == NULL ) {
	cerr << " Pointer to adjuster is NULL!" << endl;
	return;
    }
    
    _adjust->upperDataCost() = _dataCostSlider->value();
    Drawing::data_cost_upper = _dataCostSlider->value();
    _gl_drawing->Keyboard( 'a', 0, 0 );
    _gl_drawing->redraw();
    _gl_layout->redraw();
    cerr << HERE << " slider value = " << _dataCostSlider->value() << endl;
    cerr << HERE << " Drawing::data_cost_upper = "
	 << Drawing::data_cost_upper << endl;
}


// slider callback function for data costs
void FLControl::_dataCostCallback( Fl_Widget *w, void * userdata )
{
    Fl_Value_Slider *slider = (Fl_Value_Slider*)w;	      // Get the menubar widget
    FLControl *appwin = (FLControl*)userdata;
    
    appwin->_dataCostHandler( slider );
}


//------------------------------------------------------------------------------
//	slider for smooth costs
//------------------------------------------------------------------------------
// response to the slider value changes in smooth cost
void FLControl::_smoothCostHandler( Fl_Value_Slider * slider )
{
    if ( _adjust == NULL ) {
	cerr << " Pointer to adjuster is NULL!" << endl;
	return;
    }
    
    _adjust->upperSmoothCost() = _smoothCostSlider->value();
    Drawing::smooth_cost_upper = _smoothCostSlider->value();
    _gl_drawing->Keyboard( 'a', 0, 0 );
    _gl_drawing->redraw();
    _gl_layout->redraw();
    cerr << HERE << " slider value = " << _smoothCostSlider->value() << endl;
    cerr << HERE << " Drawing::smooth_cost_upper = "
	 << Drawing::smooth_cost_upper << endl;
}


// slider callback function for smooth costs
void FLControl::_smoothCostCallback( Fl_Widget *w, void * userdata )
{
    Fl_Value_Slider *slider = (Fl_Value_Slider*)w;	      // Get the menubar widget
    FLControl *appwin = (FLControl*)userdata;
    
    appwin->_smoothCostHandler( slider );
}


//------------------------------------------------------------------------------
//	slider for label costs
//------------------------------------------------------------------------------
// response to the slider value changes in label cost
void FLControl::_labelCostHandler( Fl_Value_Slider * slider )
{
    if ( _adjust == NULL ) {
	cerr << " Pointer to adjuster is NULL!" << endl;
	return;
    }
    
    _adjust->upperLabelCost() = _labelCostSlider->value();
    Drawing::label_cost_upper = _labelCostSlider->value();
    _gl_drawing->Keyboard( 'a', 0, 0 );
    _gl_drawing->redraw();
    _gl_layout->redraw();
    cerr << HERE << " slider value = " << _labelCostSlider->value() << endl;
    cerr << HERE << " Drawing::label_cost_upper = "
	 << Drawing::label_cost_upper << endl;
}


// slider callback function for label costs
void FLControl::_labelCostCallback( Fl_Widget *w, void * userdata )
{
    Fl_Value_Slider *slider = (Fl_Value_Slider*)w;	      // Get the menubar widget
    FLControl *appwin = (FLControl*)userdata;
    
    appwin->_labelCostHandler( slider );
}


//------------------------------------------------------------------------------
//	slider for cut thresholds
//------------------------------------------------------------------------------
// response to the slider value changes in cut thresholds
void FLControl::_cutThresholdHandler( Fl_Value_Slider * slider )
{
    if ( _adjust == NULL ) {
	cerr << " Pointer to adjuster is NULL!" << endl;
	return;
    }
    
    _adjust->cutThreshold() = _cutThresholdSlider->value();
    _gl_drawing->Keyboard( 'a', 0, 0 );
    _gl_drawing->redraw();
    _gl_layout->redraw();
    cerr << HERE << " slider value = " << _cutThresholdSlider->value() << endl;
    cerr << HERE << " cut_threshold = " << _adjust->cutThreshold() << endl;
}


// slider callback function for data costs
void FLControl::_cutThresholdCallback( Fl_Widget *w, void * userdata )
{
    Fl_Value_Slider *slider = (Fl_Value_Slider*)w;	      // Get the menubar widget
    FLControl *appwin = (FLControl*)userdata;
    
    appwin->_cutThresholdHandler( slider );
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
		      GLBase * __gl_drawing, GLBase * __gl_layout,
		      int _x, int _y, int _w, int _h, const char *_l )
    : Fl_Window( _x, _y, _w, _h, _l )  	
{
    int bottom = 0;

    _adjust		= __adjust;
    _gl_drawing		= __gl_drawing;
    _gl_layout		= __gl_layout;
    cerr << HERE << " _gl_drawing = " << _gl_drawing << endl;
    cerr << HERE << " _gl_layout = " << _gl_layout << endl;
    
    //------------------------------------------------------------------------------
    //	Preparing the menu bar
    //------------------------------------------------------------------------------
    _menubar = new Fl_Menu_Bar( 0, 0, _w, 25 );

    _menubar->add( "&File/&Clear",	0,	_menuCallback,	( void* )this );
    _menubar->add( "&File/&Load",	0,	_fileCallback,	( void* )this );
    _menubar->add( "&File/&Save",	0,	_fileCallback,	( void* )this );
    
    _menubar->add( "&Edit/con&Join",	0,	_menuCallback,	( void* )this );
    _menubar->add( "&Edit/&Optimize",	0,	_menuCallback,	( void* )this );
    
    _menubar->add( "&Switch/Conjoined",	0,	_menuCallback,	( void* )this );
    _menubar->add( "&Switch/Wrapped",	0,	_menuCallback,	( void* )this );

    _menubar->add( "&Capture/&Drawing",	0,	_menuCallback,	( void* )this );
    _menubar->add( "&Capture/&Layout",	0,	_menuCallback,	( void* )this );

    _menubar->add( "&Quit",		0,	_menuCallback,	( void* )this );

    //------------------------------------------------------------------------------
    //	Preparing the value sliders for intevals
    //------------------------------------------------------------------------------
    bottom = 60;
    _intervalSlider	= new Fl_Value_Slider( 120, bottom, this->w()/2, 20,
					       "limit interval" );
    _intervalSlider->align	( FL_ALIGN_LEFT );
    _intervalSlider->type	( FL_HOR_SLIDER );
    _intervalSlider->bounds	( _adjust->minInterval(), _adjust->maxInterval() );
    _adjust->interval()	=	_adjust->intervalScale() * Drawing::interval_threshold;
    _intervalSlider->value	( _adjust->interval() );

    _intervalSlider->callback	( _intervalCallback, ( void *)this );

    //------------------------------------------------------------------------------
    //	Preparing the value sliders for data costs
    //------------------------------------------------------------------------------
    bottom += 25;
    _dataCostSlider	= new Fl_Value_Slider( 120, bottom, this->w()/2, 20,
					       "data cost" );
    _dataCostSlider->align	( FL_ALIGN_LEFT );
    _dataCostSlider->type	( FL_HOR_SLIDER );
    _dataCostSlider->bounds	( _adjust->minDataCost(), _adjust->maxDataCost() );
    _adjust->upperDataCost() =	Drawing::data_cost_upper;
    _dataCostSlider->value	( _adjust->upperDataCost() );

    _dataCostSlider->callback	( _dataCostCallback, ( void *)this );

    //------------------------------------------------------------------------------
    //	Preparing the value sliders for smooth costs
    //------------------------------------------------------------------------------
    bottom += 25;
    _smoothCostSlider	= new Fl_Value_Slider( 120, bottom, this->w()/2, 20,
					       "smooth cost" );
    _smoothCostSlider->align	( FL_ALIGN_LEFT );
    _smoothCostSlider->type	( FL_HOR_SLIDER );
    _smoothCostSlider->bounds	( _adjust->minSmoothCost(), _adjust->maxSmoothCost() );
    _adjust->upperSmoothCost() =	Drawing::smooth_cost_upper;
    _smoothCostSlider->value	( _adjust->upperSmoothCost() );

    _smoothCostSlider->callback	( _smoothCostCallback, ( void *)this );

    //------------------------------------------------------------------------------
    //	Preparing the value sliders for label costs
    //------------------------------------------------------------------------------
    bottom += 25;
    _labelCostSlider	= new Fl_Value_Slider( 120, bottom, this->w()/2, 20,
					       "label cost" );
    _labelCostSlider->align	( FL_ALIGN_LEFT );
    _labelCostSlider->type	( FL_HOR_SLIDER );
    _labelCostSlider->bounds	( _adjust->minLabelCost(), _adjust->maxLabelCost() );
    _adjust->upperLabelCost() =	Drawing::label_cost_upper;
    _labelCostSlider->value	( _adjust->upperLabelCost() );

    _labelCostSlider->callback	( _labelCostCallback, ( void *)this );

    //------------------------------------------------------------------------------
    //	Preparing the value sliders for cut thresholds
    //------------------------------------------------------------------------------
    bottom += 25;
    _cutThresholdSlider	= new Fl_Value_Slider( 120, bottom, this->w()/2, 20,
					       "cut threshold" );
    _cutThresholdSlider->align	( FL_ALIGN_LEFT );
    _cutThresholdSlider->type	( FL_HOR_SLIDER );
    _cutThresholdSlider->bounds	( _adjust->minCutThreshold(), _adjust->maxCutThreshold() );
    _cutThresholdSlider->value	( _adjust->cutThreshold() );

    _cutThresholdSlider->callback	( _cutThresholdCallback, ( void *)this );

    //------------------------------------------------------------------------------
    //	
    //------------------------------------------------------------------------------
    bottom += 40;
    int border = 100;
    _panel = new FLSliderInput( __adjust, __gl_drawing, __gl_layout,
				border, bottom, this->w() - border - 40, 160,
				"Parameter panel" );

    //------------------------------------------------------------------------------
    //	
    //------------------------------------------------------------------------------
}


#ifdef SKIP
//
//  FLControl::FLControl --	copy constructor
//
//  Inputs
//	obj	: object of this class
//
//  Outputs
//	none
//
FLControl::FLControl( const FLControl & obj )
{
    _adjust		= obj._adjust;
    _gl_drawing		= obj._gl_drawing;
    _gl_layout		= obj._gl_layout;

    _menubar		= obj._menubar;
    _intervalSlider	= obj._intervalSlider;
    _dataCostSlider	= obj._dataCostSlider;
    _smoothCostSlider	= obj._smoothCostSlider;
    _labelCostSlider	= obj._labelCostSlider;
    _cutThresholdSlider	= obj._cutThresholdSlider;

    _panel		= obj._panel;
}
#endif	// SKIP


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
    
    // Sliders;
    delete _intervalSlider;
    delete _dataCostSlider;
    delete _smoothCostSlider;
    delete _labelCostSlider;
    delete _cutThresholdSlider;
}


//------------------------------------------------------------------------------
//	Referrring to members
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------

//
//  FLControl::operator = --	assignement
//
//  Inputs
//	obj	: objects of this class
//
//  Outputs
//	this object
//
FLControl & FLControl::operator = ( const FLControl & obj )
{
    if ( this != &obj ) {
	// ( Fl_Window & )*this = obj;

	_adjust			= obj._adjust;
	_gl_drawing		= obj._gl_drawing;
	_gl_layout		= obj._gl_layout;
	
	_menubar		= obj._menubar;
	_intervalSlider		= obj._intervalSlider;
	_dataCostSlider		= obj._dataCostSlider;
	_smoothCostSlider	= obj._smoothCostSlider;
	_labelCostSlider	= obj._labelCostSlider;
	_cutThresholdSlider	= obj._cutThresholdSlider;
	
	_panel			= obj._panel;
    }
    return *this;
}



// end of header file
// Do not add any stuff under this line.
