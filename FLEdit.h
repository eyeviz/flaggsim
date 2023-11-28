//==============================================================================
// FLEdit.h
//	: header file for control panel window
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-11-24 00:40:44 shigeo"
//
//==============================================================================

#ifndef _FLEdit_H		// begining of header file
#define _FLEdit_H		// notifying that this file is included

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Radio_Round_Button.H>
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

class FLEdit : public Fl_Window {

  private:

    //------------------------------------------------------------------------------
    void _fileHandler( Fl_Button * b );
    static void _fileCallback( Fl_Widget *w, void * userdata );

    //------------------------------------------------------------------------------
    void _editHandler( Fl_Button * b );
    static void _editCallback( Fl_Widget *w, void * userdata );

    //------------------------------------------------------------------------------
    void _switchHandler( Fl_Check_Button * b );
    static void _switchCallback( Fl_Widget *w, void * userdata );

    //------------------------------------------------------------------------------
    void _captureHandler( Fl_Button * b );
    static void _captureCallback( Fl_Widget *w, void * userdata );

    //------------------------------------------------------------------------------
    void _quitHandler( Fl_Button * bar );
    static void _quitCallback( Fl_Widget *w, void * userdata );

  protected:
    Adjuster		* _adjust;
    GLDrawing		* _glDrawing;
    GLLayout		* _glLayout;

    Fl_Menu_Bar		* _menubar;
    FLSliderInput	* _slider;	
    
  public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    FLEdit( Adjuster * __adjust,
	    GLDrawing * __gl_drawing, GLLayout * __gl_layout,
	    int _x, int _y, int _w, int _h, const char *_l );
				// default constructor

    FLEdit( const FLEdit & obj );
				// copy constructor
//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
    ~FLEdit();			// destructor

//------------------------------------------------------------------------------
//	Redrawing function
//------------------------------------------------------------------------------
    void redrawAll	( void );


//------------------------------------------------------------------------------
//	Class name
//------------------------------------------------------------------------------
    virtual const char * className( void ) const { return "FLEdit"; }
				// class name
};

#endif // _FLEdit_H

// end of header file
// Do not add any stuff under this line.