//==============================================================================
// Triangulate.cpp
//	: program file for triangulating concave polygons
//
//------------------------------------------------------------------------------
//
//				Date: Wed Dec  5 03:23:13 2018
//
//==============================================================================

#include "Triangulate.h"

//------------------------------------------------------------------------------
//	Private Functions
//------------------------------------------------------------------------------

bool Triangulate::Snip( const Polygon2 & contour,
			int u, int v, int w, int n, int *V )
{
    double Ax = contour[ V[u] ].x();
    double Ay = contour[ V[u] ].y();

    double Bx = contour[ V[v] ].x();
    double By = contour[ V[v] ].y();

    double Cx = contour[ V[w] ].x();
    double Cy = contour[ V[w] ].y();

    if ( EPSILON > ( ( (Bx-Ax)*(Cy-Ay) ) - ( (By-Ay)*(Cx-Ax) ) ) )
	return false;

    for ( int p = 0 ; p < n; p++ ) {
	if ( ( p == u ) || ( p == v ) || ( p == w ) ) continue;
	double Px = contour[ V[p] ].x();
	double Py = contour[ V[p] ].y();
	if ( InsideTriangle( Ax, Ay, Bx, By, Cx, Cy, Px, Py ) )
	    return false;
    }

    return true;
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

#ifdef SKIP
//
//  Triangulate::Triangulate --	default constructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Triangulate::Triangulate() 
      : %%%()  	
{

}
#endif	// SKIP


#ifdef SKIP
//
//  Triangulate::Triangulate --	copy constructor
//
//  Inputs
//	obj	: object of this class
//
//  Outputs
//	none
//
Triangulate::Triangulate( const Triangulate & obj )
      : %%%( ( const %%% & ) obj )
{

}
#endif	// SKIP


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------

#ifdef SKIP
//
//  Triangulate::~Triangulate --	destructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Triangulate::~Triangulate()
{

}
#endif	// SKIP


//------------------------------------------------------------------------------
//	Referrring to members
//------------------------------------------------------------------------------


double Triangulate::Area( const Polygon2 & contour )
{
    int n = contour.size();

    double A = 0.0f;

    for ( int p = n - 1, q = 0; q < n; p = q++ ) {
	A +=
	    contour[ p ].x()*contour[ q ].y() - 
	    contour[ q ].x()*contour[ p ].y();
    }
    return ( A*0.5f );
}

/*
  InsideTriangle decides if a point P is Inside of the triangle
  defined by A, B, C.
*/
bool Triangulate::InsideTriangle( double Ax, double Ay,
				  double Bx, double By,
				  double Cx, double Cy,
				  double Px, double Py )

{
    double ax = Cx - Bx;  double ay = Cy - By;
    double bx = Ax - Cx;  double by = Ay - Cy;
    double cx = Bx - Ax;  double cy = By - Ay;
    double apx= Px - Ax;  double apy= Py - Ay;
    double bpx= Px - Bx;  double bpy= Py - By;
    double cpx= Px - Cx;  double cpy= Py - Cy;

    double aCROSSbp = ax*bpy - ay*bpx;
    double cCROSSap = cx*apy - cy*apx;
    double bCROSScp = bx*cpy - by*cpx;

    return ( (aCROSSbp >= 0.0f) && (bCROSScp >= 0.0f) && (cCROSSap >= 0.0f) );
};

bool Triangulate::isContained( const vector< Link > & bag, const Link & item )
{
    for ( unsigned int k = 0; k < bag.size(); ++k ) {
	if ( ( bag[ k ].first == item.first ) &&
	     ( bag[ k ].second == item.second ) ) return true;
    }
    return false;
}

bool Triangulate::process( const Polygon2 & contour, 
			   Polygon2 & result, Set & index )
{
    /* allocate and initialize list of Vertices in polygon */
    
    int n = contour.size();
    if ( n < 3 ) return false;

    int *V = new int[ n ];

    /* we want a counter-clockwise polygon in V */

    if ( 0.0f < Area( contour ) )
	for ( int v = 0; v < n; v++ )
	    V[v] = v;
    else
	for ( int v = 0; v < n; v++ )
	    V[v] = (n-1)-v;

    int nv = n;

    /*  remove nv-2 Vertices, creating 1 triangle every time */
    int count = 2*nv;   /* error detection */

    for ( int m = 0, v = nv-1; nv > 2; ) {
	/* if we loop, it is probably a non-simple polygon */
	if ( 0 >= (count--) ) {
	    //** Triangulate: ERROR - probable bad polygon!
	    return false;
	}

	/* three consecutive vertices in current polygon, <u,v,w> */
	int u = v; 
	if ( nv <= u ) u = 0;     /* previous */
	v = u+1;
	if ( nv <= v ) v = 0;     /* new v    */
	int w = v+1;
	if ( nv <= w ) w = 0;     /* next     */

	if ( Snip( contour, u, v, w, nv, V ) ) {
	    int a, b, c, s, t;

	    /* true names of the vertices */
	    a = V[ u ]; b = V[ v ]; c = V[ w ];

	    /* output Triangle */
	    result.push_back( contour[ a ] );
	    index.push_back( a );
	    result.push_back( contour[ b ] );
	    index.push_back( b );
	    result.push_back( contour[ c ] );
	    index.push_back( c );

	    m++;

	    /* remove v from remaining polygon */
	    for ( s = v,t = v+1; t < nv; s++,t++ )
		V[ s ] = V[ t ];
	    nv--;

	    /* resest error detection counter */
	    count = 2*nv;
	}
    }

    delete [] V;

    return true;
}


void Triangulate::diagonal( const Polygon2 & poly,
			    const Polygon2 & tri, 
			    const Set & index,
			    Diag & bridge )
{
    bridge.clear();

    // print out the results.
    unsigned int tcount = tri.size()/3;
    unsigned int n = poly.size();

    for ( unsigned int i = 0; i < tcount; ++i ) {
	//const Point2 &p1 = tri[i*3+0];
	//const Point2 &p2 = tri[i*3+1];
	//const Point2 &p3 = tri[i*3+2];
	const unsigned int &i1 = index[i*3+0];
	const unsigned int &i2 = index[i*3+1];
	const unsigned int &i3 = index[i*3+2];
#ifdef SKIP
	cerr << boost::format( "Triangle %d => [%3d](%4.2f,%4.2f) [%3d](%4.2f,%4.2f) [%3d](%4.2f,%4.2f)" )
	    % (i+1) 
	    % i1 % p1.x() % p1.y() 
	    % i2 % p2.x() % p2.y()
	    % i3 % p3.x() % p3.y()
	     << endl;
#endif	// SKIP

	// retrieving new interior segments
	Link up1 = Link( std::min( i1, i2 ), std::max( i1, i2 ) );
	if ( i1 < i2 ) {
	    if ( i2 - i1 > 1 ) {
		if ( ! Triangulate::isContained( bridge, up1 ) ) 
		    bridge.push_back( up1 );
	    }
	}
	else {
	    if ( i2 + n - i1 > 1 ) {
		if ( ! Triangulate::isContained( bridge, up1 ) ) 
		    bridge.push_back( up1 );
	    }
	}

	Link up2 = Link( std::min( i2, i3 ), std::max( i2, i3 ) );
	if ( i2 < i3 ) {
	    if ( i3 - i2 > 1 ) {
		if ( ! Triangulate::isContained( bridge, up2 ) ) 
		    bridge.push_back( up2 );
	    }
	}
	else {
	    if ( i3 + n - i2 > 1 ) {
		if ( ! Triangulate::isContained( bridge, up2 ) ) 
		    bridge.push_back( up2 );
	    }
	}

	Link up3 = Link( std::min( i3, i1 ), std::max( i3, i1 ) );
	if ( i3 < i1 ) {
	    if ( i1 - i3 > 1 ) {
		if ( ! Triangulate::isContained( bridge, up3 ) ) 
		    bridge.push_back( up3 );
	    }
	}
	else {
	    if ( i1 + n - i3 > 1 ) {
		if ( ! Triangulate::isContained( bridge, up3 ) ) 
		    bridge.push_back( up3 );
	    }
	}
    }

    return;
}


//------------------------------------------------------------------------------
//	Functions for sorting
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------




// end of header file
// Do not add any stuff under this line.
