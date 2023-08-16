//==============================================================================
// FLControl.h
//	: header file for control panel window
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-08-08 14:22:34 shigeo"
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


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class FLControl : public Fl_Window {

  private:
    void _menu_handler( Fl_Menu_Bar * bar );
    static void _menu_callback( Fl_Widget *w, void * userdata );

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
    Adjuster *		_adjust;

    Fl_Menu_Bar	*	_menubar;
    Fl_Value_Slider *	_intervalSlider;
    Fl_Value_Slider *	_dataCostSlider;
    Fl_Value_Slider *	_smoothCostSlider;
    Fl_Value_Slider *	_labelCostSlider;
    Fl_Value_Slider *	_cutThresholdSlider;
    
    static GLBase *	_gl_drawing;
    static GLBase *	_gl_layout;

  public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    FLControl( Adjuster * __adjust,
	       int _x, int _y, int _w, int _h, const char *_l );
				// default constructor

//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
    ~FLControl();			// destructor

//------------------------------------------------------------------------------
//	Referrring to members
//------------------------------------------------------------------------------
    void setGLDrawing( GLBase * ptr )	{ _gl_drawing = ptr; }
    void setGLLayout ( GLBase * ptr )	{ _gl_layout = ptr; }
    
//------------------------------------------------------------------------------
//	Class name
//------------------------------------------------------------------------------
    virtual const char * className( void ) const { return "FLControl"; }
				// class name
};

#endif // _FLControl_H

// end of header file
// Do not add any stuff under this line.
