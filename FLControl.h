//==============================================================================
// FLControl.h
//	: header file for control panel window
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-08-18 22:59:27 shigeo"
//
//==============================================================================

#ifndef _FLControl_H		// begining of header file
#define _FLControl_H		// notifying that this file is included

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Value_Slider.H>

#include "Adjuster.h"
#include "GLBase.h"
#include "FLSliderInput.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class FLControl : public Fl_Window {

  private:
    void _fileHandler( Fl_Menu_Bar * bar );
    static void _fileCallback( Fl_Widget *w, void * userdata );

    void _menuHandler( Fl_Menu_Bar * bar );
    static void _menuCallback( Fl_Widget *w, void * userdata );

    void _intervalHandler( Fl_Value_Slider * slider );
    static void _intervalCallback( Fl_Widget *w, void * userdata );

    void _dataCostHandler( Fl_Value_Slider * slider );
    static void _dataCostCallback( Fl_Widget *w, void * userdata );

    void _smoothCostHandler( Fl_Value_Slider * slider );
    static void _smoothCostCallback( Fl_Widget *w, void * userdata );

    void _labelCostHandler( Fl_Value_Slider * slider );
    static void _labelCostCallback( Fl_Widget *w, void * userdata );
    
    void _cutThresholdHandler( Fl_Value_Slider * slider );
    static void _cutThresholdCallback( Fl_Widget *w, void * userdata );

  protected:
    Adjuster		* _adjust;
    GLBase		* _gl_drawing;
    GLBase		* _gl_layout;


    Fl_Menu_Bar		* _menubar;
    Fl_Value_Slider	* _intervalSlider;
    Fl_Value_Slider	* _dataCostSlider;
    Fl_Value_Slider	* _smoothCostSlider;
    Fl_Value_Slider	* _labelCostSlider;
    Fl_Value_Slider	* _cutThresholdSlider;

    FLSliderInput	* _panel;	
    
  public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    FLControl( Adjuster * __adjust,
	       GLBase * __gl_drawing, GLBase * __gl_layout,
	       int _x, int _y, int _w, int _h, const char *_l );
				// default constructor

    FLControl( const FLControl & obj );
				// copy constructor
//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
    ~FLControl();			// destructor



//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------
    FLControl & operator = ( const FLControl & obj );


//------------------------------------------------------------------------------
//	Class name
//------------------------------------------------------------------------------
    virtual const char * className( void ) const { return "FLControl"; }
				// class name
};

#endif // _FLControl_H

// end of header file
// Do not add any stuff under this line.
