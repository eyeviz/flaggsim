//==============================================================================
// FLSliderInput.h
//	: header file for the widget consisting of slider and input box together
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-08-18 18:11:12 shigeo"
//
//==============================================================================

#ifndef _FLSliderInput_H		// begining of header file
#define _FLSliderInput_H		// notifying that this file is included

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <FL/Fl.H>
// #include <FL/Fl_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Slider.H>

#include "Adjuster.h"
#include "GLBase.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class FLSliderInput : public Fl_Group {

  private:

    void _intervalInputHandler			( Fl_Float_Input * input );
    static void _intervalInputCallback		( Fl_Widget *w, void * userdata );
    void _intervalSliderHandler			( Fl_Slider * slider );
    static void _intervalSliderCallback		( Fl_Widget *w, void * userdata );

    void _dataCostInputHandler			( Fl_Float_Input * input );
    static void _dataCostInputCallback		( Fl_Widget *w, void * userdata );
    void _dataCostSliderHandler			( Fl_Slider * slider );
    static void _dataCostSliderCallback		( Fl_Widget *w, void * userdata );

    void _smoothCostInputHandler		( Fl_Float_Input * input );
    static void _smoothCostInputCallback	( Fl_Widget *w, void * userdata );
    void _smoothCostSliderHandler		( Fl_Slider * slider );
    static void _smoothCostSliderCallback	( Fl_Widget *w, void * userdata );

    void _labelCostInputHandler			( Fl_Float_Input * input );
    static void _labelCostInputCallback		( Fl_Widget *w, void * userdata );
    void _labelCostSliderHandler		( Fl_Slider * slider );
    static void _labelCostSliderCallback	( Fl_Widget *w, void * userdata );
    
    void _cutThresholdInputHandler		( Fl_Float_Input * input );
    static void _cutThresholdInputCallback	( Fl_Widget *w, void * userdata );
    void _cutThresholdSliderHandler		( Fl_Slider * slider );
    static void _cutThresholdSliderCallback	( Fl_Widget *w, void * userdata );

  protected:
    Adjuster			* _adjust;
    GLBase			* _gl_drawing;
    GLBase			* _gl_layout;
    
    Fl_Float_Input		* _intervalInput;
    Fl_Slider			* _intervalSlider;

    Fl_Float_Input		* _dataCostInput;
    Fl_Slider			* _dataCostSlider;

    Fl_Float_Input		* _smoothCostInput;
    Fl_Slider			* _smoothCostSlider;

    Fl_Float_Input		* _labelCostInput;
    Fl_Slider			* _labelCostSlider;

    Fl_Float_Input		* _cutThresholdInput;
    Fl_Slider			* _cutThresholdSlider;

public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    FLSliderInput( Adjuster * __adjust,
		   GLBase * __gl_drawing, GLBase * __gl_layout,
		   int x, int y, int w, int h,
		   const char *label = NULL );
					// default constructor

//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
    ~FLSliderInput();			// destructor

//------------------------------------------------------------------------------
//	Referrring to members
//------------------------------------------------------------------------------

#ifdef SKIP
    const double value( void ) const		{
	return ( double )_slider->value();
    }
    void value( double val )			{
	_slider->value( ( float )val );
	_slider_handler();
    }
    const double minumum( void ) const		{
	return ( double )_slider->minimum();
    }
    void minumum( double val )			{
	_slider->minimum( ( float )val );
    }
    const double maximum( void ) const		{
	return ( double )_slider->maximum();
    }
    void maximum( double val )			{
	_slider->maximum( ( float )val );
    }
    void bounds( double low, double high )	{
	_slider->bounds( ( float )low, ( float )high );
    }

    void setInputCallback( void (*__callback)( Fl_Widget *, void * ) ) {
	_input->callback( __callback, ( void * )this );
    }
    void setSliderCallback( void (*__callback)( Fl_Widget *, void * ) ) {
	_slider->callback( __callback, ( void * )this );
    }
#endif	// SKIP


//------------------------------------------------------------------------------
//	Class name
//------------------------------------------------------------------------------
    virtual const char * className( void ) const { return "FLSliderInput"; }
				// class name
};


#endif // _FLSliderInput_H

// end of header file
// Do not add any stuff under this line.
