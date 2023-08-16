//==============================================================================
// FLControl.cpp
//	: program file for control panel window
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-08-08 14:33:14 shigeo"
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

//------------------------------------------------------------------------------
//	For menubars 
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
		      int _x, int _y, int _w, int _h, const char *_l )
    : Fl_Window( _x, _y, _w, _h, _l )  	
{
    _adjust	= __adjust;

    //------------------------------------------------------------------------------
    //	Preparing the menu bar
    //------------------------------------------------------------------------------
    _menubar = new Fl_Menu_Bar( 0, 0, _w, 25 );

    _menubar->add( "&File/&Clear",	0,	_menu_callback );
    _menubar->add( "&File/&Load",	0,	_menu_callback );
    _menubar->add( "&File/&Save",	0,	_menu_callback );
    
    _menubar->add( "&Edit/con&Join",	0,	_menu_callback );
    _menubar->add( "&Edit/&Optimize",	0,	_menu_callback );
    
    _menubar->add( "&Switch/Conjoined",	0,	_menu_callback );
    _menubar->add( "&Switch/Wrapped",	0,	_menu_callback );

    _menubar->add( "&Capture/&Drawing",	0,	_menu_callback );
    _menubar->add( "&Capture/&Layout",	0,	_menu_callback );

    _menubar->add( "&Quit",		0,	_menu_callback );

    //------------------------------------------------------------------------------
    //	Preparing the value sliders for intevals
    //------------------------------------------------------------------------------
    _intervalSlider	= new Fl_Value_Slider( 120, 40, this->w()/2, 20,
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
    _dataCostSlider	= new Fl_Value_Slider( 120, 65, this->w()/2, 20,
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
    _smoothCostSlider	= new Fl_Value_Slider( 120, 90, this->w()/2, 20,
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
    _labelCostSlider	= new Fl_Value_Slider( 120, 115, this->w()/2, 20,
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
    _cutThresholdSlider	= new Fl_Value_Slider( 120, 140, this->w()/2, 20,
					       "cut threshold" );
    _cutThresholdSlider->align	( FL_ALIGN_LEFT );
    _cutThresholdSlider->type	( FL_HOR_SLIDER );
    _cutThresholdSlider->bounds	( _adjust->minCutThreshold(), _adjust->maxCutThreshold() );
    _cutThresholdSlider->value	( _adjust->cutThreshold() );

    _cutThresholdSlider->callback	( _cutThresholdCallback, ( void *)this );

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
    delete _menubar;
    delete _intervalSlider;
    delete _dataCostSlider;
    delete _smoothCostSlider;
    delete _labelCostSlider;
    delete _cutThresholdSlider;
}


//------------------------------------------------------------------------------
//	Referrring to members
//------------------------------------------------------------------------------




// end of header file
// Do not add any stuff under this line.
