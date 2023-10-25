//==============================================================================
// FLControl.h
//	: header file for control panel window
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-08-19 22:25:01 shigeo"
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
#include "GLDrawing.h"
#include "GLLayout.h"
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

  protected:
    Adjuster		* _adjust;
    GLDrawing		* _gl_drawing;
    GLLayout		* _gl_layout;

    Fl_Menu_Bar		* _menubar;
    FLSliderInput	* _panel;	
    
  public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    FLControl( Adjuster * __adjust,
	       GLDrawing * __gl_drawing, GLLayout * __gl_layout,
	       int _x, int _y, int _w, int _h, const char *_l );
				// default constructor

    FLControl( const FLControl & obj );
				// copy constructor
//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
    ~FLControl();			// destructor

//------------------------------------------------------------------------------
//	Class name
//------------------------------------------------------------------------------
    virtual const char * className( void ) const { return "FLControl"; }
				// class name
};

#endif // _FLControl_H

// end of header file
// Do not add any stuff under this line.