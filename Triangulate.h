//==============================================================================
// Triangulate.h
//	: header file for triangulating concave polygons
//
//------------------------------------------------------------------------------
//
//				Date: Wed Dec  5 03:13:00 2018
//
//==============================================================================

#ifndef _Triangulate_H		// begining of header file
#define _Triangulate_H		// notifying that this file is included

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <vector>

using namespace std;

#include "Common.h"
#include "CGAL.h"
#include "Set.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class Triangulate {

  private:

    static bool Snip( const Polygon2 & contour,
		      int u, int v, int w, int n, int *V );

  protected:

  public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    //Triangulate();			// default constructor
    //Triangulate( const Triangulate & obj );
				// copy constructor

//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
    //~Triangulate();			// destructor

//------------------------------------------------------------------------------
//	Referrring to members
//------------------------------------------------------------------------------

    // compute area of a contour/polygon
    static double Area( const Polygon2 & contour );

    // decide if point Px/Py is inside triangle defined by
    // (Ax,Ay) (Bx,By) (Cx,Cy)
    static bool InsideTriangle( double Ax, double Ay,
				double Bx, double By,
				double Cx, double Cy,
				double Px, double Py );

    static bool isContained( const vector< Link > & bag, const Link & item );

    // triangulate a contour/polygon, places results in STL vector
    // as series of triangles.
    static bool process( const Polygon2 & contour,
			 Polygon2 & result, Set & index );

    static void diagonal( const Polygon2 & poly,
			  const Polygon2 & tri,
			  const Set & index,
			  Diag & bridges );

//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------
    //Triangulate & operator = ( const Triangulate & obj );
				// assignment

//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
    //friend ostream & operator << ( ostream & stream, const Triangulate & obj );
				// output
    //friend istream & operator >> ( istream & stream, Triangulate & obj );
				// input

//------------------------------------------------------------------------------
//	Class name
//------------------------------------------------------------------------------
    virtual const char * className( void ) const { return "Triangulate"; }
				// class name
};



#endif // _Triangulate_H

// end of header file
// Do not add any stuff under this line.
