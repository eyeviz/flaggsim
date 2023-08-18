//==============================================================================
// FLSlinderInput.cpp
//	: program file for the widget consisting of slider and input box together
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-08-18 23:11:42 shigeo"
//
//==============================================================================

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

using namespace std;

#include "Common.h"
#include "FLSliderInput.h"


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Private Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	For intervals

// Function for responding to the input value changes in the interval
void FLSliderInput::_intervalInputHandler( Fl_Float_Input * input )
{
    if ( _adjust == NULL ) {
	cerr << " Pointer to adjuster is NULL!" << endl;
	return;
    }

    static int recurse = 0;
    if ( recurse ) {
	return;
    }
    else {
	recurse = 1;
	istringstream istr;
	istr.clear();
	istr.str( _intervalInput->value() );
	float val = 0.0;
	istr >> val;
	// cerr << HERE << " val = " << val << endl;
	val = min( _intervalSlider->maximum(),
		   max( _intervalSlider->minimum(), ( double )val ) );
	// pass input's value to slider
	_intervalSlider->value( val );
	recurse = 0;
    }

    _adjust->interval() = _intervalSlider->value();
    Drawing::interval_threshold =
	_intervalSlider->value() / _adjust->intervalScale();
    _gl_drawing->Keyboard( 'a', 0, 0 );
    _gl_drawing->redraw();
    _gl_layout->redraw();

    _intervalSliderHandler( _intervalSlider );

#ifdef SKIP
    cerr << HERE << " slider value = " << _intervalSlider->value() << endl;
    cerr << HERE << " Drawing::interval_threshold = "
	 << fixed << setprecision( 4 )
	 << Drawing::interval_threshold << endl;
#endif	// SKIP
}

// Callback for handling input box for the intervals
void FLSliderInput::_intervalInputCallback( Fl_Widget *w, void * userdata )
{
    Fl_Float_Input *input = ( Fl_Float_Input* )w; // Get the menubar widget
    FLSliderInput *appwin = ( FLSliderInput* )userdata;
    appwin->_intervalInputHandler( input );
};

// Function for responding to the slider value changes in the interval
void FLSliderInput::_intervalSliderHandler( Fl_Slider * slider )
{
    if ( _adjust == NULL ) {
	cerr << " Pointer to adjuster is NULL!" << endl;
	return;
    }
    
    static int recurse = 0;
    if ( recurse ) { 
	return;
    }
    else {
	recurse = 1;
	ostringstream ostr;
	ostr << fixed << right << setw( 5 ) << setprecision( 2 )
	     << _intervalSlider->value() << ends;
	// pass slider's value to input
	_intervalInput->value( ostr.str().c_str() );
	recurse = 0;
    }

    _adjust->interval() = _intervalSlider->value();
    Drawing::interval_threshold =
	_intervalSlider->value() / _adjust->intervalScale();
    _gl_drawing->Keyboard( 'a', 0, 0 );
    _gl_drawing->redraw();
    _gl_layout->redraw();

#ifdef SKIP
    cerr << HERE << " slider value = " << _intervalSlider->value() << endl;
    cerr << HERE << " Drawing::interval_threshold = "
	 << fixed << setprecision( 4 )
	 << Drawing::interval_threshold << endl;
#endif	// SKIP
}

// Callback founctoin for handling slider value for the intervals
void FLSliderInput::_intervalSliderCallback( Fl_Widget *w, void * userdata )
{
    Fl_Slider *slider = (Fl_Slider*)w; // Get the menubar widget
    FLSliderInput *appwin = (FLSliderInput*)userdata;
    appwin->_intervalSliderHandler( slider );
}

//------------------------------------------------------------------------------
//	For data costs

// Function for responding to the input value changes in the data cost
void FLSliderInput::_dataCostInputHandler( Fl_Float_Input * input )
{
    if ( _adjust == NULL ) {
	cerr << " Pointer to adjuster is NULL!" << endl;
	return;
    }

    static int recurse = 0;
    if ( recurse ) {
	return;
    }
    else {
	recurse = 1;
	istringstream istr;
	istr.clear();
	istr.str( _dataCostInput->value() );
	float val = 0.0;
	istr >> val;
	// cerr << HERE << " val = " << val << endl;
	val = min( _dataCostSlider->maximum(),
		   max( _dataCostSlider->minimum(), ( double )val ) );
	// pass input's value to slider
	_dataCostSlider->value( val );
	recurse = 0;
    }
    
    _adjust->upperDataCost() = _dataCostSlider->value();
    Drawing::data_cost_upper = _dataCostSlider->value();
    _gl_drawing->Keyboard( 'a', 0, 0 );
    _gl_drawing->redraw();
    _gl_layout->redraw();

    _dataCostSliderHandler( _dataCostSlider );

#ifdef SKIP
    cerr << HERE << " slider value = " << _dataCostSlider->value() << endl;
    cerr << HERE << " Drawing::data_cost_upper = "
	 << Drawing::data_cost_upper << endl;
#endif	// SKIP
}

// Callback for handling input box for the data costs
void FLSliderInput::_dataCostInputCallback( Fl_Widget *w, void * userdata )
{
    Fl_Float_Input *input = ( Fl_Float_Input* )w; // Get the menubar widget
    FLSliderInput *appwin = ( FLSliderInput* )userdata;
    appwin->_dataCostInputHandler( input );
};

// Function for responding to the slider value changes in the data cost
void FLSliderInput::_dataCostSliderHandler( Fl_Slider * slider )
{
    if ( _adjust == NULL ) {
	cerr << " Pointer to adjuster is NULL!" << endl;
	return;
    }
    
    static int recurse = 0;
    if ( recurse ) { 
	return;
    }
    else {
	recurse = 1;
	ostringstream ostr;
	ostr << fixed << right << setw( 5 ) << setprecision( 2 )
	     << _dataCostSlider->value() << ends;
	// pass slider's value to input
	_dataCostInput->value( ostr.str().c_str() );
	recurse = 0;
    }

    _adjust->upperDataCost() = _dataCostSlider->value();
    Drawing::data_cost_upper = _dataCostSlider->value();
    _gl_drawing->Keyboard( 'a', 0, 0 );
    _gl_drawing->redraw();
    _gl_layout->redraw();

#ifdef SKIP
    cerr << HERE << " slider value = " << _dataCostSlider->value() << endl;
    cerr << HERE << " Drawing::data_cost_upper = "
	 << Drawing::data_cost_upper << endl;
#endif	// SKIP
}

// Callback founctoin for handling slider value for data costs
void FLSliderInput::_dataCostSliderCallback( Fl_Widget *w, void * userdata )
{
    Fl_Slider *slider = ( Fl_Slider* )w; // Get the menubar widget
    FLSliderInput *appwin = ( FLSliderInput* )userdata;
    appwin->_dataCostSliderHandler( slider );
}

//------------------------------------------------------------------------------
//	For smoothness costs

// Function for responding to the input value changes in the smoothness cost
void FLSliderInput::_smoothCostInputHandler( Fl_Float_Input * input )
{
    if ( _adjust == NULL ) {
	cerr << " Pointer to adjuster is NULL!" << endl;
	return;
    }

    static int recurse = 0;
    if ( recurse ) {
	return;
    }
    else {
	recurse = 1;
	istringstream istr;
	istr.clear();
	istr.str( _smoothCostInput->value() );
	float val = 0.0;
	istr >> val;
	// cerr << HERE << " val = " << val << endl;
	val = min( _smoothCostSlider->maximum(),
		   max( _smoothCostSlider->minimum(), ( double )val ) );
	// pass input's value to slider
	_smoothCostSlider->value( val );
	recurse = 0;
    }

    _adjust->upperSmoothCost() = _smoothCostSlider->value();
    Drawing::smooth_cost_upper = _smoothCostSlider->value();
    _gl_drawing->Keyboard( 'a', 0, 0 );
    _gl_drawing->redraw();
    _gl_layout->redraw();

    _smoothCostSliderHandler( _smoothCostSlider );

#ifdef SKIP
    cerr << HERE << " slider value = " << _smoothCostSlider->value() << endl;
    cerr << HERE << " Drawing::smooth_cost_upper = "
	 << Drawing::smooth_cost_upper << endl;
#endif	// SKIP
}

// Callback for handling input box for the smoothness costs
void FLSliderInput::_smoothCostInputCallback( Fl_Widget *w, void * userdata )
{
    Fl_Float_Input *input = ( Fl_Float_Input* )w; // Get the menubar widget
    FLSliderInput *appwin = ( FLSliderInput* )userdata;
    appwin->_smoothCostInputHandler( input );
};

// Function for responding to the slider value changes in the smoothness cost
void FLSliderInput::_smoothCostSliderHandler( Fl_Slider * slider )
{
    if ( _adjust == NULL ) {
	cerr << " Pointer to adjuster is NULL!" << endl;
	return;
    }
    
    static int recurse = 0;
    if ( recurse ) { 
	return;
    }
    else {
	recurse = 1;
	ostringstream ostr;
	ostr << fixed << right << setw( 5 ) << setprecision( 2 )
	     << _smoothCostSlider->value() << ends;
	// pass slider's value to input
	_smoothCostInput->value( ostr.str().c_str() );
	recurse = 0;
    }

    _adjust->upperSmoothCost() = _smoothCostSlider->value();
    Drawing::smooth_cost_upper = _smoothCostSlider->value();
    _gl_drawing->Keyboard( 'a', 0, 0 );
    _gl_drawing->redraw();
    _gl_layout->redraw();

#ifdef SKIP
    cerr << HERE << " slider value = " << _smoothCostSlider->value() << endl;
    cerr << HERE << " Drawing::smooth_cost_upper = "
	 << Drawing::smooth_cost_upper << endl;
#endif	// SKIP
}

// Callback founctoin for handling slider value for the smoothness costs
void FLSliderInput::_smoothCostSliderCallback( Fl_Widget *w, void * userdata )
{
    Fl_Slider *slider = (Fl_Slider*)w; // Get the menubar widget
    FLSliderInput *appwin = (FLSliderInput*)userdata;
    appwin->_smoothCostSliderHandler( slider );
}

//------------------------------------------------------------------------------
//	For label costs

// Function for responding to the input value changes in the label cost
void FLSliderInput::_labelCostInputHandler( Fl_Float_Input * input )
{
    if ( _adjust == NULL ) {
	cerr << " Pointer to adjuster is NULL!" << endl;
	return;
    }
    
    static int recurse = 0;
    if ( recurse ) {
	return;
    }
    else {
	recurse = 1;
	istringstream istr;
	istr.clear();
	istr.str( _labelCostInput->value() );
	float val = 0.0;
	istr >> val;
	// cerr << HERE << " val = " << val << endl;
	val = min( _labelCostSlider->maximum(),
		   max( _labelCostSlider->minimum(), ( double )val ) );
	// pass input's value to slider
	_labelCostSlider->value( val );
	recurse = 0;
    }

    _adjust->upperLabelCost() = _labelCostSlider->value();
    Drawing::label_cost_upper = _labelCostSlider->value();
    _gl_drawing->Keyboard( 'a', 0, 0 );
    _gl_drawing->redraw();
    _gl_layout->redraw();

    _labelCostSliderHandler( _labelCostSlider );

#ifdef SKIP
    cerr << HERE << " slider value = " << _labelCostSlider->value() << endl;
    cerr << HERE << " Drawing::label_cost_upper = "
	 << Drawing::label_cost_upper << endl;
#endif	// SKIP
}

// Callback for handling input box for the label costs
void FLSliderInput::_labelCostInputCallback( Fl_Widget *w, void * userdata )
{
    Fl_Float_Input *input = ( Fl_Float_Input* )w; // Get the menubar widget
    FLSliderInput *appwin = ( FLSliderInput* )userdata;
    appwin->_labelCostInputHandler( input );
};

// Function for responding to the slider value changes in the label cost
void FLSliderInput::_labelCostSliderHandler( Fl_Slider * slider )
{
    if ( _adjust == NULL ) {
	cerr << " Pointer to adjuster is NULL!" << endl;
	return;
    }
    
    static int recurse = 0;
    if ( recurse ) { 
	return;
    }
    else {
	recurse = 1;
	ostringstream ostr;
	ostr << fixed << right << setw( 5 ) << setprecision( 2 )
	     << _labelCostSlider->value() << ends;
	// pass slider's value to input
	_labelCostInput->value( ostr.str().c_str() );
	recurse = 0;
    }

    _adjust->upperLabelCost() = _labelCostSlider->value();
    Drawing::label_cost_upper = _labelCostSlider->value();
    _gl_drawing->Keyboard( 'a', 0, 0 );
    _gl_drawing->redraw();
    _gl_layout->redraw();

#ifdef SKIP
    cerr << HERE << " slider value = " << _labelCostSlider->value() << endl;
    cerr << HERE << " Drawing::label_cost_upper = "
	 << Drawing::label_cost_upper << endl;
#endif	// SKIP
}

// Callback founctoin for handling slider value for the label costs
void FLSliderInput::_labelCostSliderCallback( Fl_Widget *w, void * userdata )
{
    Fl_Slider *slider = (Fl_Slider*)w; // Get the menubar widget
    FLSliderInput *appwin = (FLSliderInput*)userdata;
    appwin->_labelCostSliderHandler( slider );
}

//------------------------------------------------------------------------------
//	For cut thresholds

// Function for responding to the input value changes in the cut threshold
void FLSliderInput::_cutThresholdInputHandler( Fl_Float_Input * input )
{
    if ( _adjust == NULL ) {
	cerr << " Pointer to adjuster is NULL!" << endl;
	return;
    }
    
    static int recurse = 0;
    if ( recurse ) {
	return;
    }
    else {
	recurse = 1;
	istringstream istr;
	istr.clear();
	istr.str( _cutThresholdInput->value() );
	float val = 0.0;
	istr >> val;
	// cerr << HERE << " val = " << val << endl;
	val = min( _cutThresholdSlider->maximum(),
		   max( _cutThresholdSlider->minimum(), ( double )val ) );
	// pass input's value to slider
	_cutThresholdSlider->value( val );
	recurse = 0;
    }

    _adjust->cutThreshold() = _cutThresholdSlider->value();
    _gl_drawing->Keyboard( 'a', 0, 0 );
    _gl_drawing->redraw();
    _gl_layout->redraw();
    
    _cutThresholdSliderHandler( _cutThresholdSlider );

#ifdef SKIP
    cerr << HERE << " slider value = " << _cutThresholdSlider->value() << endl;
    cerr << HERE << " cut_threshold = " << _adjust->cutThreshold() << endl;
#endif	// SKIP
}

// Callback for handling input box for the cut thresholds
void FLSliderInput::_cutThresholdInputCallback( Fl_Widget *w, void * userdata )
{
    Fl_Float_Input *input = ( Fl_Float_Input* )w; // Get the menubar widget
    FLSliderInput *appwin = ( FLSliderInput* )userdata;
    appwin->_cutThresholdInputHandler( input );
};

// Function for responding to the slider value changes in the cut threshold
void FLSliderInput::_cutThresholdSliderHandler( Fl_Slider * slider )
{
    if ( _adjust == NULL ) {
	cerr << " Pointer to adjuster is NULL!" << endl;
	return;
    }
    
    static int recurse = 0;
    if ( recurse ) { 
	return;
    }
    else {
	recurse = 1;
	ostringstream ostr;
	ostr << fixed << right << setw( 5 ) << setprecision( 2 )
	     << _cutThresholdSlider->value() << ends;
	// pass slider's value to input
	_cutThresholdInput->value( ostr.str().c_str() );
	recurse = 0;
    }

    _adjust->cutThreshold() = _cutThresholdSlider->value();
    _gl_drawing->Keyboard( 'a', 0, 0 );
    _gl_drawing->redraw();
    _gl_layout->redraw();

#ifdef SKIP
    cerr << HERE << " slider value = " << _cutThresholdSlider->value() << endl;
    cerr << HERE << " cut_threshold = " << _adjust->cutThreshold() << endl;
#endif	// SKIP
}

// Callback founctoin for handling slider value for the cut thresholds
void FLSliderInput::_cutThresholdSliderCallback( Fl_Widget *w, void * userdata )
{
    Fl_Slider *slider = (Fl_Slider*)w; // Get the menubar widget
    FLSliderInput *appwin = (FLSliderInput*)userdata;
    appwin->_cutThresholdSliderHandler( slider );
}


//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------
#ifdef SKIP
// Function for handling events for the slider
void FLSliderInput::_slider_handler( void )
{
    static int recurse = 0;
    if ( recurse ) { 
	return;
    }
    else {
	recurse = 1;
	ostringstream ostr;
	ostr << fixed << right << setw( 5 ) << setprecision( 2 )
	     << _slider->value() << ends;
	// pass slider's value to input
	_input->value( ostr.str().c_str() );
	recurse = 0;
    }
}

// Function for handling events for the input box
void FLSliderInput::_input_handler( void )
{
    static int recurse = 0;
    if ( recurse ) {
	return;
    }
    else {
	recurse = 1;
	istringstream istr;
	istr.clear();
	istr.str( _input->value() );
	float val = 0.0;
	istr >> val;
	cerr << HERE << " val = " << val << endl;
	val = min( _slider->maximum(), max( _slider->minimum(), ( double )val ) );
	// pass input's value to slider
	_slider->value( val );
	recurse = 0;
    }
}
#endif	// SKIP


//------------------------------------------------------------------------------
//	Public Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------

//
//  FLSlinderInput::FLSlinderInput --	default constructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
FLSliderInput::FLSliderInput( Adjuster * __adjust,
			      GLBase * __gl_drawing, GLBase * __gl_layout,
			      int x, int y, int w, int h,
			      const char * label ) 
    : Fl_Group( x, y, w, h, label )  	
{
    int in_w = 40;
    int	bottom	= y;
    int thickness = h / 5;

    cerr << " x = " << x << " y = " << y << " w = " << w << " h = " << h << endl;
    cerr << " label = " << label << endl;
    
    _adjust	= __adjust;
    _gl_drawing	= __gl_drawing;
    _gl_layout	= __gl_layout;
    
//------------------------------------------------------------------------------
//	For intervals

    _intervalInput		= new Fl_Float_Input( x, bottom, in_w, thickness,
						      "limit interval" );
    _intervalInput->callback( _intervalInputCallback, ( void* )this );
    // _intervalInput->align( FL_ALIGN_RIGHT );
    _intervalInput->when( FL_WHEN_ENTER_KEY | FL_WHEN_NOT_CHANGED );

    _intervalSlider		= new Fl_Slider( x+in_w, bottom, w-in_w, thickness );
    _intervalSlider->type( 1 );
    _intervalSlider->bounds( _adjust->minInterval(), _adjust->maxInterval() );
    _adjust->interval()	=	_adjust->intervalScale() * Drawing::interval_threshold;
    _intervalSlider->value	( _adjust->interval() );
    _intervalSlider->callback( _intervalSliderCallback, ( void* )this );

    // to redraw the input box for the interval limit
    _intervalSliderHandler( _intervalSlider );
    
    bottom	+= thickness;

//------------------------------------------------------------------------------
//	For data costs
    _dataCostInput		= new Fl_Float_Input( x, bottom, in_w, thickness,
						      "data cost" );
    _dataCostInput->callback( _dataCostInputCallback, ( void* )this );
    _dataCostInput->when( FL_WHEN_ENTER_KEY | FL_WHEN_NOT_CHANGED );

    _dataCostSlider		= new Fl_Slider( x+in_w, bottom, w-in_w, thickness );
    _dataCostSlider->type( 1 );
    _dataCostSlider->bounds( _adjust->minDataCost(), _adjust->maxDataCost() );
    _adjust->upperDataCost()	= Drawing::data_cost_upper;
    _dataCostSlider->value	( _adjust->upperDataCost() );
    _dataCostSlider->callback( _dataCostSliderCallback, ( void* )this );

    // to redraw the input box for the data cost
    _dataCostSliderHandler( _dataCostSlider );
    
    bottom	+= thickness;

//------------------------------------------------------------------------------
//	For smoothness costs
    _smoothCostInput		= new Fl_Float_Input( x, bottom, in_w, thickness,
						      "smooth cost" );
    _smoothCostInput->callback( _smoothCostInputCallback, ( void* )this );
    _smoothCostInput->when( FL_WHEN_ENTER_KEY | FL_WHEN_NOT_CHANGED );

    _smoothCostSlider		= new Fl_Slider( x+in_w, bottom, w-in_w, thickness );
    _smoothCostSlider->type( 1 );
    _smoothCostSlider->bounds( _adjust->minSmoothCost(), _adjust->maxSmoothCost() );
    _adjust->upperSmoothCost()	= Drawing::smooth_cost_upper;
    _smoothCostSlider->value	( _adjust->upperSmoothCost() );
    _smoothCostSlider->callback( _smoothCostSliderCallback, ( void* )this );

    // to redraw the input box for the data cost
    _smoothCostSliderHandler( _smoothCostSlider );

    bottom	+= thickness;

//------------------------------------------------------------------------------
//	For label costs
    _labelCostInput		= new Fl_Float_Input( x, bottom, in_w, thickness,
						      "label cost" );
    _labelCostInput->callback( _labelCostInputCallback, ( void* )this );
    _labelCostInput->when( FL_WHEN_ENTER_KEY | FL_WHEN_NOT_CHANGED );

    _labelCostSlider		= new Fl_Slider( x+in_w, bottom, w-in_w, thickness );
    _labelCostSlider->type( 1 );
    _labelCostSlider->bounds( _adjust->minLabelCost(), _adjust->maxLabelCost() );
    _adjust->upperLabelCost()	= Drawing::label_cost_upper;
    _labelCostSlider->value	( _adjust->upperLabelCost() );
    _labelCostSlider->callback( _labelCostSliderCallback, ( void* )this );

    // to redraw the input box for the data cost
    _labelCostSliderHandler( _labelCostSlider );

    bottom	+= thickness;
//------------------------------------------------------------------------------
//	For cut thresholds
    _cutThresholdInput		= new Fl_Float_Input( x, bottom, in_w, thickness,
						      "cut threshold" );
    _cutThresholdInput->callback( _cutThresholdInputCallback, ( void* )this );
    _cutThresholdInput->when( FL_WHEN_ENTER_KEY | FL_WHEN_NOT_CHANGED );

    _cutThresholdSlider		= new Fl_Slider( x+in_w, bottom, w-in_w, thickness );
    _cutThresholdSlider->type( 1 );
    _cutThresholdSlider->bounds( _adjust->minCutThreshold(), _adjust->maxCutThreshold() );
    // _adjust->maxCutThreshold()	= Drawing::max_cut_threshold;
    _cutThresholdSlider->value	( _adjust->upperLabelCost() );
    _cutThresholdSlider->callback( _cutThresholdSliderCallback, ( void* )this );

    // to redraw the input box for the data cost
    _cutThresholdSliderHandler( _cutThresholdSlider );

    bottom	+= thickness;
    
    end();             // close the group
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------

//
//  FLSlinderInput::~FLSlinderInput --	destructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
FLSliderInput::~FLSliderInput()
{

}


//------------------------------------------------------------------------------
//	Referrring to members
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Functions for sorting
//------------------------------------------------------------------------------


// end of header file
// Do not add any stuff under this line.
