//==============================================================================
// Contour.cpp
//	: program file for contour shapes
//
//------------------------------------------------------------------------------
//
//				Date: Tue Sep 19 08:50:08 2017
//
//==============================================================================

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>
// for sleep
#include <chrono>
#include <thread>
using namespace std;

#include <CGAL/intersections.h>
#include <CGAL/squared_distance_2.h>

#include "Common.h"
#include "Contour.h"


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------
// #define DEBUG_SIMPLIFICATION


//------------------------------------------------------------------------------
//	Private Functions
//------------------------------------------------------------------------------

//
//  Contour::_innerDiv --	find the inner division point of a segment
//
//  Inputs
//	P, Q	: endpoints of the segment
//	t	: division ratio
//
//  Outputs
//	coordinates of the inner division point
//
Point2 Contour::_innerDiv( const Point2 & P, const Point2 & Q, const double & t )
{
    Point2 R =
	CGAL::ORIGIN +
	( 1.0 - t )*( P - CGAL::ORIGIN ) +
	t*( Q - CGAL::ORIGIN );
    return R;
}

//
//  Contour::_polyVertex --	set the coordinates of the polygon corner vertex
//
//  Inputs
//	id	: id of the corner vertex
//	coord	: coordinates
//
//  Outputs
//	none
//
void Contour::_polyVertex( const unsigned int & id, const Point2 & coord )
{
    unsigned int sz = _polygon.size();
    VertexIterator iter = _polygon.vertices_begin();
    iter = iter + ( id % sz );
    _polygon.set( iter, coord );
}

//
//  Contour::_solveQuadratic --	find the solution of a quadratic equation
//
//  Inputs
//	a, b, c:	the coefficients of the quadratic equation
//
//  Outputs
//	the solution (large solution) 
//
double Contour::_solveQuadratic( const double & a, const double & b, const double & c )
{
    double sol = 0.0;

    if ( abs( a ) < EPSILON ) {
	cerr << HERE << " >>>>>>>>>> Coefficient for the quadratic term vanishes." << endl;
	sol = -c/b;
	if ( abs( sol - 1.0 ) < EPSILON ) sol = 0.999;
    }
    else {
	sol = ( -b + sqrt( b*b - 4*a*c ) )/( 2.0 * a );
    }
    return sol;	
}

//
//  Contour::_Gaussian --	Gaussian function
//
//  Inputs
//	x	:	input argument
//
//  Outputs
//	density
//
double Contour::_Gaussian( double x )
{
    return ( 1.0/sqrt( 2.0 * M_PI ) ) * exp( - (x*x) / 2.0 );
}


//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------

//
//  Contour::_initConfig --	initialize the list of conflict configurations
//
//  Inputs
//	configID :	configuration ID (inner edge ID of the configuration)
//
//  Outputs
//	none
//
void Contour::_initConfig( void )
{
    unsigned int sz	= _polygon.size();
    _config.resize( sz );
    for ( unsigned int i = 0; i < sz; ++i )
	_config[ i ].clear();

    for ( unsigned int i = 0; i < sz; ++i )
	_incrementConfig( i );
}

//
//  Contour::_incrementConfig --	increment the list of conflicting configuration
//
//  Inputs
//	configID :	configuration ID (inner edge ID of the configuration)
//
//  Outputs
//	none
//
void Contour::_incrementConfig( unsigned int configID )
{
    unsigned int sz	= _polygon.size();
    Turn trSource	= _turn[ ( configID + sz - 1 ) % sz ];
    Turn trOrig		= _turn[ configID ];
    Turn trDest		= _turn[ ( configID + 1 ) % sz ];
    Turn trTarget	= _turn[ ( configID + 2 ) % sz ];
    unsigned int cur = configID;
    unsigned int ms1 = ( configID + sz - 1 ) % sz;
    unsigned int ms2 = ( configID + sz - 2 ) % sz;
    unsigned int ps1 = ( configID + 1 ) % sz;
    unsigned int ps2 = ( configID + 2 ) % sz;
    _config[ cur ].push_back( configID );
    _config[ ms1 ].push_back( configID );
    _config[ ps1 ].push_back( configID );
#ifdef CONSERVATIVE_CONFIGURATION_MATCHING
    _config[ ms2 ].push_back( configID );
    _config[ ps2 ].push_back( configID );
#else	// CONSERVATIVE_CONFIGURATION_MATCHING
    if ( trSource == trOrig ) _config[ ms2 ].push_back( configID );
    if ( trTarget == trDest ) _config[ ps2 ].push_back( configID );
#endif	// CONSERVATIVE_CONFIGURATION_MATCHING

#ifdef SKIP
    // print the counter
    cerr << HERE << " [Increment] Count : ";
    for ( unsigned int i = 0; i < sz; ++i ) {
	cerr << "[ " << setw( 3 ) << i << " ]=" << setw( 2 ) << _count[ i ] << " ,";
    }
    cerr << endl;
#endif	// SKIP
}


//
//  Contour::_decrementConfig --	decrement the list of conflicting configurations
//
//  Inputs
//	edgeID :	edge ID
//
//  Outputs
//	none
//
void Contour::_decrementConfig( unsigned int configID )
{
    unsigned int sz	= _polygon.size();
    Turn trSource	= _turn[ ( configID + sz - 1 ) % sz ];
    Turn trOrig		= _turn[ configID ];
    Turn trDest		= _turn[ ( configID + 1 ) % sz ];
    Turn trTarget	= _turn[ ( configID + 2 ) % sz ];
    unsigned int cur = configID;
    unsigned int ms1 = ( configID + sz - 1 ) % sz;
    unsigned int ms2 = ( configID + sz - 2 ) % sz;
    unsigned int ps1 = ( configID + 1 ) % sz;
    unsigned int ps2 = ( configID + 2 ) % sz;
    remove( _config[ cur ].begin(), _config[ cur ].end(), configID );
    remove( _config[ ms1 ].begin(), _config[ ms1 ].end(), configID );
    remove( _config[ ps1 ].begin(), _config[ ps1 ].end(), configID );
#ifdef CONSERVATIVE_CONFIGURATION_MATCHING
    remove( _config[ ms2 ].begin(), _config[ ms2 ].end(), configID );
    remove( _config[ ps2 ].begin(), _config[ ps2 ].end(), configID );
#else	// CONSERVATIVE_CONFIGURATION_MATCHING
    if ( trSource == trOrig )
	remove( _config[ ms2 ].begin(), _config[ ms2 ].end(), configID );
    if ( trTarget == trDest ) 
	remove( _config[ ps2 ].begin(), _config[ ps2 ].end(), configID );
#endif	// CONSERVATIVE_CONFIGURATION_MATCHING

#ifdef SKIP
    // print the counter
    cerr << HERE << " [Decrement] Count : ";
    for ( unsigned int i = 0; i < sz; ++i ) {
	cerr << "[ " << setw( 3 ) << i << " ]=" << setw( 2 ) << _count[ i ] << " ,";
    }
    cerr << endl;
#endif	// SKIP
}


//
//  Contour::_load --	load polygon data
//
//  Inputs
//	filename
//
//  Outputs
//	none
//
void Contour::_load( const char * filename )
{
    ifstream            ifs( filename );
    char                buf[ BUFFER_SIZE ];
    
    // Never call this
    // _label.clear();
    
    if ( !ifs ) {
        cerr << "Cannot open the target file : " << filename << endl;
        return;
    }
    
    //------------------------------------------------------------------------------
    //  Read each site
    //------------------------------------------------------------------------------
    istringstream istr;

    _polygon.clear();
    while ( ifs.getline( buf, sizeof( buf ) ) ) {
        istr.clear();
        istr.str( buf );
	double x, y;
        istr >> x >> y;
#ifdef DEBUG_SIMPLIFICATION
        cerr << HERE << "\t" << x << "\t" << y << endl;
#endif	// DEBUG_SIMPLIFICATION
	_polygon.push_back( Point2( x, y ) );
    }

    ifs.close();

    _normalize();
    _reset();
}



//
//  Contour::_save --	save polygon data
//
//  Inputs
//	filename
//
//  Outputs
//	none
//
void Contour::_save( const char * filename )
{
    ofstream            ofs( filename );
    
    // Never call this
    // _label.clear();
    
    if ( !ofs ) {
        cerr << "Cannot open the target file : " << filename << endl;
        return;
    }
    
    for ( unsigned int i = 0; i < _polygon.size(); ++i ) {
	const Point2 & point	 = _polygon.vertex( i );
	ofs	<< setw( 8 ) << point.x() << "\t"
		<< setw( 8 ) << point.y() << endl;
    }

    ofs.close();
}


//------------------------------------------------------------------------------
//	initialize contours
//------------------------------------------------------------------------------

//
//  Contour::_reset --	reset the contours of the area
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Contour::_reset( void )
{
    while ( _delIdentical() );
    while ( _delCollinear() ) {
#ifdef DEBUG_SIMPLIFICATION
	cerr << HERE << " Deleting collinear samples " << endl;
#endif	// DEBUG_SIMPLIFICATION
    }
    
    _isContractible = _prepare();
}

//
//  Contour::_normalize --	normalize the contours of the area
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Contour::_normalize( void )
{
    Vector2 sum( 0.0, 0.0 );
    for ( unsigned int k = 0; k < _polygon.size(); ++k ) {
	sum += ( _polygon[ k ] - CGAL::ORIGIN ); 
    }
    sum /= ( double )_polygon.size();
    for ( unsigned int k = 0; k <  _polygon.size(); ++k ) {
	_polygon[ k ] -= sum;
    }
    
    double radius = 0.0;
    for ( unsigned int k = 0; k <  _polygon.size(); ++k ) {
	double dist = sqrt( SQUARE( _polygon[ k ].x() ) + SQUARE( _polygon[ k ].y() ) );
	if ( dist > radius ) radius = dist;
    }
    
    for ( unsigned int k = 0; k <  _polygon.size(); ++k ) {
	_polygon[ k ] = CGAL::ORIGIN + ( 1.0/radius ) * ( _polygon[ k ] - CGAL::ORIGIN );
    }

//------------------------------------------------------------------------------
//	Extra handling
//------------------------------------------------------------------------------
    const double th = 1.0e-8;
    if ( _polygon.size() == 0 ) return;

    Polygon2	temp = _polygon;
    _polygon.clear();
    Point2	prev = temp[ 0 ];
    
    _polygon.push_back( prev );
    for ( unsigned int k = 1; k < temp.size(); ++k ) {
	Point2 curr	= temp[ k ];
	// double dist = SQUARE( x - px ) + SQUARE( y - py );
	double dist = ( curr - prev ).squared_length();
	if ( dist >= th ) {
	    _polygon.push_back( curr );
	    prev = curr;
	}
#ifdef DEBUG_SIMPLIFICATION
	else {
	    cerr << HERE << " Skipped the duplicated samples : "
		 << " dist = " << dist
		 << " prev = " << prev << " curr = " << curr << endl;
	}
#endif	// DEBUG_SIMPLIFICATION
    }
    unsigned int sz = _polygon.size();
    if ( ( _polygon[ sz - 1 ] - _polygon[ 0 ] ).squared_length() < th ) {
	_polygon.erase( _polygon.begin() + ( sz - 1 ) );
    }
    
#ifdef DEBUG_SIMPLIFICATION
    for ( unsigned int k = 0; k < _polygon.size(); ++k ) {
	cerr << HERE << "[ " << setw( 3 ) << k << "] : "
	     << _polygon[ k ] << endl;
    }
#endif	// DEBUG_SIMPLIFICATION
}



//------------------------------------------------------------------------------
//	Fundamentals for schematization
//------------------------------------------------------------------------------

//
//  Contour::_calc -- compute attributes necessary for schematization
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Contour::_calc( void )
{
#ifdef DEBUG_SIMPLIFICATION
    cerr << HERE << " Now in Contour::_calc" << endl;
#endif	// DEBUG_SIMPLIFICATION
    const double zero = 0.0;
    unsigned int sz = _polygon.size();

    if ( sz < 3 ) {
	_eangle.resize( sz );
	_turn.resize( sz );
	_move.resize( sz );
	_config.resize( sz );

	_distPos.resize( sz );
	_distNeg.resize( sz );
	_segPos.resize( sz );
	_segNeg.resize( sz );
	_areaPos.resize( sz );
	_areaNeg.resize( sz );

	_idPri = _idSec = 0;
	_scheme = NONE;

#ifdef CHECK_CONFLICTS_WITH_OTHERS
	_obstacle.clear();
#endif	// CHECK_CONFLICTS_WITH_OTHERS

	return;
    }

//------------------------------------------------------------------------------
//	external angles	
//------------------------------------------------------------------------------
    _eangle.clear();
    _turn.clear();
    double sumExt = 0.0;
    for ( unsigned int i = 0; i < sz; ++i ) {
	Segment2 edO = _polygon.edge( ( i + sz - 1 ) % sz );
	Segment2 edD = _polygon.edge( i );
	Vector2 tgO = edO.to_vector();
	Vector2 tgD = edD.to_vector();
	double cosAngle = (tgO*tgD)/sqrt( tgO.squared_length()*tgD.squared_length() );
	Vector3 spO( tgO.x(), tgO.y(), 0.0 );
	Vector3 spD( tgD.x(), tgD.y(), 0.0 );
	Vector3 oprd = cross_product( spO, spD );
	double extAngle = acos( cosAngle );
	// Vector2 nmN = tgD.perpendicular( CGAL::COUNTERCLOCKWISE );

	// If edO and edD makes an obtuse angle 
	// if ( edO.supporting_line().has_on_negative_side( edD.target() ) )
	// ext *= (-1.0);
	// if ( edO.supporting_line().has_on_negative_side( edD.target() ) ) {
	if ( oprd.z() < 0.0 ) {
	    extAngle *= (-1.0);
	    _turn.push_back( REFLEX );
	}
	else {
	    _turn.push_back( CONVEX );
	}
#ifdef DEBUG_SIMPLIFICATION
	cerr << HERE << " eangle (in degree) = " << 180.0*extAngle/M_PI << endl;
#endif	// DEBUG_SIMPLIFICATION
	_eangle.push_back( extAngle );
	sumExt += extAngle;
	if ( std::isnan( sumExt ) ) {
	    cerr << HERE << " i = " << setw( 4 ) << i << " sum of angles = " << sumExt << endl;
	    cerr << HERE << " edO = " << edO << endl;
	    cerr << HERE << " edD = " << edD << endl;
	    cerr << HERE << " tgO = " << tgO << endl;
	    cerr << HERE << " tgD = " << tgD << endl;
	    cerr << HERE << " ext = " << extAngle << endl;
	    // cerr << HERE << " nmN = " << nmN << endl;
	    cerr << HERE << " Stop the program to check: (Press enter) " << endl;
	    getchar();
	}
    }
#ifdef DEBUG_SIMPLIFICATION
    cerr << " sum of external angles = " << sumExt << endl;
#endif	// DEBUG_SIMPLIFICATION

    if ( sumExt < 0.0 ) {
	std::reverse( _polygon.begin(), _polygon.end() );
    }

    
//------------------------------------------------------------------------------
//	compute possible edge moves
//------------------------------------------------------------------------------
    _move.clear();
    _distPos.clear();
    _distNeg.clear();
    _segPos.clear();
    _segNeg.clear();
    for ( unsigned int i = 0; i < sz; ++i ) {
#ifdef DEBUG_SIMPLIFICATION
	cerr << HERE << "##############################" << endl;
	cerr << HERE << " i = " << i << endl;
#endif	// DEBUG_SIMPLIFICATION
	// three consecutive segments
	// edO=>edPrev, edC=>edCurr, edD=>edNext
	Segment2 edPrev		= _polygon.edge( ( i + sz - 1 ) % sz );
	Segment2 edCurr		= _polygon.edge( i );
	Segment2 edNext		= _polygon.edge( ( i + 1 ) % sz );
#ifdef DEBUG_SIMPLIFICATION
	cerr << " edPrev = " << edPrev << endl;
	cerr << " edCurr = " << edCurr << endl;
	cerr << " edNext = " << edNext << endl;
#endif	// DEBUG_SIMPLIFICATION
	// Angle turn types
	Turn trOrig		= _turn[ i ];
	Turn trDest		= _turn[ ( i + 1 ) % sz ];
	Point2 ptSource		= edPrev.source();
	Point2 ptTarget		= edNext.target();
	// infinite lines that support the external edges
	Line2 lnPrev		= edPrev.supporting_line();
	Line2 lnCurr		= edCurr.supporting_line();
	Line2 lnNext		= edNext.supporting_line();
	double distSource	= sqrt( squared_distance( ptSource, lnCurr ) );
	double distTarget	= sqrt( squared_distance( ptTarget, lnCurr ) );
	double distPositive	= zero;
	double distNegative	= zero;
	Move moveCurr;	

#ifdef DEBUG_SIMPLIFICATION
	if ( trOrig == CONVEX ) cerr << HERE << " CONVEX ";
	else if ( trOrig == REFLEX ) cerr << HERE << " REFLEX ";
	else cerr << HERE << " UNKNOWN ";
	cerr << "at Orig ; ";
	if ( trDest == CONVEX ) cerr << HERE << " CONVEX ";
	else if ( trDest == REFLEX ) cerr << HERE << " REFLEX ";
	else cerr << HERE << " UNKNOWN ";
	cerr << "at Dest ; " << endl;

	cerr << " LineCurr : " << lnCurr.a() << " x + "
	     << lnCurr.b() << " y + " << lnCurr.c() << " = 0 " << endl;
	cerr << " ptSource : " << ptSource << endl;
	cerr << " ptTarget : " << ptTarget << endl;
	cerr << " Judge source : " 
	     << lnCurr.a() * ptSource.x() + lnCurr.b() * ptSource.y() + lnCurr.c() << endl;
	cerr << " Judge target : "
	     << lnCurr.a() * ptTarget.x() + lnCurr.b() * ptTarget.y() + lnCurr.c() << endl;
#endif	// DEBUG_SIMPLIFICATION
	
	// Intersection between two outer infinite lines
	Object objOuter = CGAL::intersection( lnPrev, lnNext );
	double distCorner	= zero;
	Point2 cornerOuter;
	if ( CGAL::assign( cornerOuter, objOuter ) ) {
	    distCorner = sqrt( CGAL::squared_distance( cornerOuter, lnCurr ) );
	    if ( lnCurr.has_on_positive_side( cornerOuter ) )
		distCorner *= (-1.0);
	}
	
	// Negative move only
	if ( ( trOrig == CONVEX ) && ( trDest == CONVEX ) ) {
	    //assert( lnCurr.has_on_positive_side( ptSource ) );
	    //assert( lnCurr.has_on_positive_side( ptTarget ) );
	    distNegative = (-1.0) * std::min( distSource, distTarget );
	    if ( distCorner < 0 )
		distNegative = std::max( distNegative, distCorner );
	    moveCurr = SHRINK;
	}
	// Positive move only 
	else if ( ( trOrig == REFLEX ) && ( trDest == REFLEX ) ) {
	    //assert( lnCurr.has_on_negative_side( ptSource ) );
	    //assert( lnCurr.has_on_negative_side( ptTarget ) );
	    distPositive = std::min( distSource, distTarget );
	    if ( distCorner > 0 )
		distPositive = std::min( distPositive, distCorner );
	    moveCurr = EXPAND;
	}
	// Both positive and negative
	else if ( ( trOrig == CONVEX ) && ( trDest == REFLEX ) ) {
	    // assert( lnCurr.has_on_positive_side( ptSource ) );
	    // assert( lnCurr.has_on_negative_side( ptTarget ) );
	    distNegative = (-1.0) * distSource;
	    distPositive = distTarget;
	    if ( distCorner > 0 ) {
		distPositive = std::min( distPositive, distCorner );
	    }
	    else {
		distNegative = std::max( distNegative, distCorner );
	    }
	    moveCurr = BOTH;
	}
	else if ( ( trOrig == REFLEX ) && ( trDest == CONVEX ) ) {
	    // assert( lnCurr.has_on_negative_side( ptSource ) );
	    // assert( lnCurr.has_on_positive_side( ptTarget ) );
	    distPositive = distSource;
	    distNegative = (-1.0) * distTarget;
	    if ( distCorner > 0 ) {
		distPositive = std::min( distPositive, distCorner );
	    }
	    else {
		distNegative = std::max( distNegative, distCorner );
	    }
	    moveCurr = BOTH;
	}
	
	_distPos.push_back( distPositive );
	_distNeg.push_back( distNegative );
	_move.push_back( moveCurr );
//------------------------------------------------------------------------------
//	Obtain the unit vector perpendicular to the current edge
	Vector2		perp = lnCurr.to_vector().perpendicular( CGAL::CLOCKWISE );
	perp	= perp / sqrt( perp.squared_length() );
//------------------------------------------------------------------------------
//	positive edge-move case
	Transformation	translatePos( CGAL::TRANSLATION, distPositive*perp );
	Line2		linePositive = translatePos( lnCurr );
#ifdef DEBUG
	cerr << HERE << " possible displacement = " 
	     << distPositive * perp.squared_length() << endl;
	cerr << HERE << " LnCurr : " << lnCurr.a() << " x + "
	     << lnCurr.b() << " y + " << lnCurr.c() << " = 0 " << endl;
	cerr << HERE << " LnPrev : " << lnPrev.a() << " x + "
	     << lnPrev.b() << " y + " << lnPrev.c() << " = 0 " << endl;
	cerr << HERE << " LnNext : " << lnNext.a() << " x + "
	     << lnNext.b() << " y + " << lnNext.c() << " = 0 " << endl;
	cerr << HERE << " LinePositive : " << linePositive.a() << " x + "
	     << linePositive.b() << " y + " << linePositive.c() << " = 0 " << endl;
	Point2 ptOrig = edCurr.source();
	Point2 ptDest = edCurr.target();
	cerr << HERE << " Orig = " << ptOrig << endl;
	cerr << HERE << " Dest = " << ptDest << endl;
	cerr << HERE << " LnCurr( Orig ) = "
	     << lnCurr.a() * ptOrig.x() + lnCurr.b() * ptOrig.y() + lnCurr.c() << endl;
	cerr << HERE << " LnCurr( Dest ) = "
	     << lnCurr.a() * ptDest.x() + lnCurr.b() * ptDest.y() + lnCurr.c() << endl;
	cerr << HERE << " LnPrev( Orig ) = "
	     << lnPrev.a() * ptOrig.x() + lnPrev.b() * ptOrig.y() + lnPrev.c() << endl;
	cerr << HERE << " LnNext( Dest ) = "
	     << lnNext.a() * ptDest.x() + lnNext.b() * ptDest.y() + lnNext.c() << endl;
#endif	// DEBUG

	Object		objPosOrig = CGAL::intersection( linePositive, lnPrev );
	Object		objPosDest = CGAL::intersection( linePositive, lnNext );
	Point2		pntPosOrig( 0.0, 0.0 ), pntPosDest( 0.0, 0.0 );
	assert( CGAL::assign( pntPosOrig, objPosOrig ) && 
		CGAL::assign( pntPosDest, objPosDest ) ); 
	_segPos.push_back( Segment2( pntPosOrig, pntPosDest ) );

//------------------------------------------------------------------------------
//	negative edge-move case
	Transformation	translateNeg( CGAL::TRANSLATION, distNegative*perp );
	Line2		lineNegative = translateNeg( lnCurr );
#ifdef DEBUG
	cerr << " possible displacement = " 
	     << distNegative * perp.squared_length() << endl;
	cerr << " LineNegative : " << lineNegative.a() << " x + "
	     << lineNegative.b() << " y + " << lineNegative.c() << " = 0 " << endl;
#endif	// DEBUG

	Object		objNegOrig = CGAL::intersection( lineNegative, lnPrev );
	Object		objNegDest = CGAL::intersection( lineNegative, lnNext );
	Point2		pntNegOrig( 0.0, 0.0 ), pntNegDest( 0.0, 0.0 );
	assert( CGAL::assign( pntNegOrig, objNegOrig ) && 
		CGAL::assign( pntNegDest, objNegDest ) );
	_segNeg.push_back( Segment2( pntNegOrig, pntNegDest ) );
    }

#ifdef SKIP
    // Numbers of _segPos and _segNeg are equivalent to that of points on the
    // contour
    cerr << HERE << " _segPos.size() = " << _segPos.size() << endl;
    cerr << HERE << " _segNeg.size() = " << _segNeg.size() << endl;
    breakpoint();
#endif	// SKIP


#ifdef CHECK_CONFLICTS_WITH_OTHERS
//------------------------------------------------------------------------------
//	Special treatment: Check intersections with obstacles
//------------------------------------------------------------------------------
    // cerr << HERE << " ****** Number of obstacles = " << _obstacle.size() << endl;
    for ( unsigned int i = 0; i < sz; ++i ) {
	Segment2 edCurr		= _polygon.edge( i );
	Point2 ptOrig = edCurr.source();
	Point2 ptDest = edCurr.target();
	//------------------------------------------------------------------------------
	// Check positive case
	//------------------------------------------------------------------------------
	if ( abs( _distPos[ i ] ) >= EPSILON ) {
	    Point2 mvOrig = _segPos[ i ].source();
	    Point2 mvDest = _segPos[ i ].target();
	    Triangle2 triA( ptOrig, mvOrig, mvDest );
	    Triangle2 triB( ptOrig, mvDest, ptDest );
	    Bbox2 boxA = triA.bbox();
	    Bbox2 boxB = triB.bbox();
	    bool withConflict = false;
	    for ( unsigned int k = 0; k < _obstacle.size(); ++k ) {
		Bbox2 boxS = _obstacle[ k ].bbox();
		if ( ( do_overlap( boxA, boxS ) ) || do_intersect( triA, _obstacle[ k ] ) )
		    withConflict = true;
		if ( withConflict ) break;
		if ( ( do_overlap( boxB, boxS ) ) || do_intersect( triB, _obstacle[ k ] ) )
		    withConflict = true;
		if ( withConflict ) break;
	    }
	    if ( withConflict ) {
		_distPos[ i ] = 0.0;
		_segPos[ i ] = edCurr;
	    }
	}
	//------------------------------------------------------------------------------
	// Check negative case
	//------------------------------------------------------------------------------
	if ( abs( _distNeg[ i ] ) >= EPSILON ) {
	    Point2 mvOrig = _segNeg[ i ].source();
	    Point2 mvDest = _segNeg[ i ].target();
	    Triangle2 triA( ptOrig, mvDest, mvOrig );
	    Triangle2 triB( ptOrig, ptDest, mvDest );
	    Bbox2 boxA = triA.bbox();
	    Bbox2 boxB = triB.bbox();
	    bool withConflict = false;
	    for ( unsigned int k = 0; k < _obstacle.size(); ++k ) {
		Bbox2 boxS = _obstacle[ k ].bbox();
		if ( ( do_overlap( boxA, boxS ) ) || do_intersect( triA, _obstacle[ k ] ) )
		    withConflict = true;
		if ( withConflict ) break;
		if ( ( do_overlap( boxB, boxS ) ) || do_intersect( triB, _obstacle[ k ] ) )
		    withConflict = true;
		if ( withConflict ) break;
	    }
	    if ( withConflict ) {
		_distNeg[ i ] = 0.0;
		_segNeg[ i ] = edCurr;
	    }
	}
    }
#endif	// CHECK_CONFLICTS_WITH_OTHERS
    
//------------------------------------------------------------------------------
//	Count the number of blocking edges
//------------------------------------------------------------------------------
    // count the number of edges that block the current configuration
    _initConfig();
#ifdef DEBUG
    // print the counter
    for ( unsigned int i = 0; i < sz; ++i ) {
	cerr << HERE << " conflict[ " << setw( 3 ) << i << " ] : ";
	for ( unsigned int j = 0; j < _config[ i ].size(); ++j ) {
	    cerr << setw( 3 ) << _config[ i ][ j ] << " ,";
	}
	cerr << endl;
    }
#endif	// DEBUG

//------------------------------------------------------------------------------
//	compute the associated change in the area
//------------------------------------------------------------------------------
    _areaPos.clear();
    _areaNeg.clear();
    for ( unsigned int i = 0; i < sz; ++i ) {
	//	_segPos.target	-	_segPos.source
	//	|		\		|
	//	edC.target	-	edC.source
	//	|		/		|
	//	_segNeg.target	-	_segNeg.source
	Segment2 edCurr = _polygon.edge( i );


#ifdef NO_NEED
	bool posIntersected = false, negIntersected = false;
	//------------------------------------------------------------------------------
	//	Conflict check between the contour and segments
	//	edC.source() - _segPos[ i ].source() &
	//	edC.target() - _segPos[ i ].target() &
	//	edC.source() - _segNeg[ i ].source() &
	//	edC.target() - _segNeg[ i ].target() &
	//------------------------------------------------------------------------------
	//#ifdef SKIP
	Segment2 wingPosSrc( edC.source(), _segPos[ i ].source() );
	Segment2 wingPosTgt( edC.target(), _segPos[ i ].target() );
	Segment2 wingNegSrc( edC.source(), _segNeg[ i ].source() );
	Segment2 wingNegTgt( edC.target(), _segNeg[ i ].target() );
	for ( unsigned int k = 2; k < sz - 1; ++k ) {
	    Segment2 edR = _polygon.edge( ( i + k ) % sz );
	    if ( do_intersect( edR, wingPosSrc ) || do_intersect( edR, wingPosTgt ) ) {
#ifdef DEBUG_SIMPLIFICATION
		cerr << HERE << " k = " << k << " pos intersected " << endl;
#endif	// DEBUG_SIMPLIFICATION
		posIntersected = true;
	    }
	    if ( do_intersect( edR, wingNegSrc ) || do_intersect( edR, wingNegTgt ) ) {
#ifdef DEBUG_SIMPLIFICATION
		cerr << HERE << " k = " << k << " neg intersected " << endl;
#endif	// DEBUG_SIMPLIFICATION
		negIntersected = true;
	    }
	}
	//#endif	// SKIP
#endif	// NO_NEED

	//------------------------------------------------------------------------------
	//	Calculating positive and negative areas
	//------------------------------------------------------------------------------
	// Positive area
	double areaPB = CGAL::area( edCurr.target(), edCurr.source(), 
				    _segPos[ i ].target() );
	double areaPT = CGAL::area( _segPos[ i ].source(), _segPos[ i ].target(),
				    edCurr.source() );
	_areaPos.push_back( areaPB+areaPT );
	// Negative area
	double areaNB = CGAL::area( edCurr.source(), edCurr.target(),
				    _segNeg[ i ].target() );
	double areaNT = CGAL::area( _segNeg[ i ].target(), _segNeg[ i ].source(),
				    edCurr.source() );
	_areaNeg.push_back( areaNB+areaNT );
    }

#ifdef DEBUG
    for ( unsigned int i = 0; i < sz; ++i ) {
	cerr << " i = " << i 
	     << " areaP = " << _areaPos[ i ] 
	     << " areaN = " << _areaNeg[ i ] << endl;
    }
#endif	// DEBUG

#ifdef SKIP
    breakpoint();
#endif	// SKIP
}


//
//  Contour::_arrange --	sort the positive and negative moves in terms of
//				the change in area
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Contour::_arrange( void )
{
#ifdef DEBUG_SIMPLIFICATION
    cerr << HERE << " Now in Contour::_arrange" << endl;
#endif	// DEBUG_SIMPLIFICATION
    const unsigned int num_tops = 6;

    _mmapPos.clear();
    _mmapNeg.clear();
    uint sz = _polygon.size();

    // preparing samping points on the contour
    for ( uint i = 0; i < sz; ++i ) {
	if ( _areaPos[ i ] > 1.0e-8 )
	    _mmapPos.insert( std::make_pair( _areaPos[ i ], i ) );
	if ( _mmapPos.size() > num_tops ) {
	    // map< double, unsigned int >::iterator last = _mapPos.end();
	    auto last = _mmapPos.end();
	    last--;
	    _mmapPos.erase( last );
	}
	if ( _areaNeg[ i ] > 1.0e-8 )
	    _mmapNeg.insert( std::make_pair( _areaNeg[ i ], i ) );
	if ( _mmapNeg.size() > num_tops ) {
	    auto last = _mmapNeg.end();
	    last--;
	    _mmapNeg.erase( last );
	}
    }

#ifdef DEBUG_SIMPLIFICATION
    for ( auto itPos = _mmapPos.begin(); itPos != _mmapPos.end(); itPos++ ) {
	cerr << HERE << " Positive move : area = " << itPos->first
	     << " id = " << itPos->second << endl;
    }
    for ( auto itNeg = _mmapNeg.begin(); itNeg != _mmapNeg.end(); itNeg++ ) {
	cerr << HERE << " Negative move : area = " << itNeg->first
	     << " id = " << itNeg->second << endl;
    }
#endif	// DEBUG_SIMPLIFICATION
    
}


//
//  Contour::_match --		determine the pair of minimal edge moves
//
//  Inputs
//	none
//
//  Outputs
//	none
//
bool Contour::_match( void )
{
    uint	sz		= _polygon.size();    

    bool	isFeasible	= false;
    while ( ( _mmapPos.begin() != _mmapPos.end() ) &&
	    ( _mmapNeg.begin() != _mmapNeg.end() ) ) {
//------------------------------------------------------------------------------
//	Check whether the first positive or negative edge-move
//	If the selected edge-move is infeasible, we delete that edge-move from
//	the priority list and then try again
//------------------------------------------------------------------------------
	double	minAreaP	= _mmapPos.begin()->first;
	double	minAreaN	= _mmapNeg.begin()->first;
	uint	idEP		= _mmapPos.begin()->second;
	uint	idEN		= _mmapNeg.begin()->second;
	
	// Positive edge-move case
	if ( minAreaP < minAreaN ) {
	    // idEP is fixed and explore the best mate of negative edge-move
	    _decrementConfig( idEP );
	    uint minDiff	= sz;
	    uint minID		= idEN;
#ifdef DEBUG_SIMPLIFICATION
	    cerr << HERE << "[P] idEP = " << idEP << endl;
#endif	// DEBUG_SIMPLIFICATION
	    for ( auto iter = _mmapNeg.begin(); iter != _mmapNeg.end(); ++iter ) {
		uint idCur = iter->second;
#ifdef DEBUG_SIMPLIFICATION
		cerr << HERE << "[P] idCur = " << idCur;
#endif	// DEBUG_SIMPLIFICATION
		if ( idCur == idEP ) {
#ifdef DEBUG_SIMPLIFICATION
		    cerr << " Illegal identical edge" << endl;
#endif	// DEBUG_SIMPLIFICATION
		    continue;
		}
		// _decrementConfig( idCur );
		auto itEP	= find( _config[ idEP ].begin(), _config[ idEP ].end(),
					idCur );
		auto itCur	= find( _config[ idCur ].begin(), _config[ idCur ].end(),
					idEP );
		if ( ( itEP == _config[ idEP ].end() ) &&
		     ( itCur == _config[ idCur ].end() ) ) {
		    // ---------- This pair of edge-moves must be feasible
		    isFeasible = true;
		    // --------------------------------------------------
		    unsigned int idS = idEP, idL = idCur;
		    if ( idEP < idCur ) {
			idS = idEP; idL = idCur;
		    }
		    else {
			idS = idCur; idL = idEP;
		    }
		    uint curDiff = min( ( int )( idL - idS ), ( int )( idS + sz - idL ) );
		    if ( curDiff < minDiff ) {
			minDiff = curDiff;
			minID = idCur;
#ifdef DEBUG_SIMPLIFICATION
			cerr << " minID = " << minID << " minDiff = " << minDiff;
#endif	// DEBUG_SIMPLIFICATION
		    }
		}
#ifdef DEBUG_SIMPLIFICATION
		else {
		    cerr << HERE << " Not feasible pair of edge-moves ";
		}
		cerr << endl;
#endif	// DEBUG_SIMPLIFICATION
		// _incrementConfig( idCur );
	    }
	    _incrementConfig( idEP );
	    if ( isFeasible ) {
		_scheme = POSITIVE;
		_idPri = idEP;
		_idSec = minID;
#ifdef DEBUG_SIMPLIFICATION
		cerr << HERE << "Positive : idPri = " << _idPri
		     << " idSec = " << _idSec << endl;
#endif	// DEBUG_SIMPLIFICATION
		return true;
	    }
	    // If we cannot find the feasible edge-move pair, we delete the
	    // first element for positive edge-moves
	    else {
		auto iterErase = _mmapPos.begin();
		_mmapPos.erase( iterErase );
	    }
	}
	// Negative edge-move case
	else {
	    // idEP is fixed and explore the best mate of negative edge-move
	    _decrementConfig( idEN );
	    uint minDiff	= sz;
	    uint minID		= idEP;
#ifdef DEBUG_SIMPLIFICATION
	    cerr << HERE << "[N] idEN = " << idEN << endl;
#endif	// DEBUG_SIMPLIFICATION
	    for ( auto iter = _mmapPos.begin(); iter != _mmapPos.end(); ++iter ) {
		uint idCur = iter->second;
#ifdef DEBUG_SIMPLIFICATION
		cerr << HERE << "[N] idCurr = " << idCur;
#endif	// DEBUG_SIMPLIFICATION
		if ( idCur == idEN ) {
#ifdef DEBUG_SIMPLIFICATION
		    cerr << " Illegal identical edge" << endl;
#endif	// DEBUG_SIMPLIFICATION
		    continue;
		}
		// _decrementConfig( idCur );
		auto itEN	= find( _config[ idEN ].begin(), _config[ idEN ].end(),
					idCur );
		auto itCur	= find( _config[ idCur ].begin(), _config[ idCur ].end(),
					idEN );
		if ( ( itEN == _config[ idEN ].end() ) &&
		     ( itCur == _config[ idCur ].end() ) ) {
		    // ---------- This pair of edge-moves must be feasible
		    isFeasible = true;
		    // --------------------------------------------------
		    unsigned int idS = idEN, idL = idCur;
		    if ( idEN < idCur ) {
			idS = idEN; idL = idCur;
		    }
		    else {
			idS = idCur; idL = idEN;
		    }
		    uint curDiff = min( ( int )( idL - idS ), ( int )( idS + sz - idL ) );
		    if ( curDiff < minDiff ) {
			minDiff = curDiff;
			minID = idCur;
#ifdef DEBUG_SIMPLIFICATION
			cerr << " minID = " << minID << " minDiff = " << minDiff;
#endif	// DEBUG_SIMPLIFICATION
		    }
		}
#ifdef DEBUG_SIMPLIFICATION
		else {
		    cerr << HERE << " Not feasible pair of edge-moves ";
		}
		cerr << endl;
#endif	// DEBUG_SIMPLIFICATION
		// _incrementConfig( idCur );
	    }
	    _incrementConfig( idEN );
	    if ( isFeasible ) {
		_scheme = NEGATIVE;
		_idPri = idEN;
		_idSec = minID;
#ifdef DEBUG_SIMPLIFICATION
		cerr << HERE << "Negative : idPri = " << _idPri
		     << " idSec = " << _idSec << endl;
#endif	// DEBUG_SIMPLIFICATION
		return true;
	    }
	    // If we cannot find the feasible edge-move pair, we delete the
	    // first element for negative edge-moves
	    else {
		auto iterErase = _mmapNeg.begin();
		_mmapNeg.erase( iterErase );
	    }
	}
    } // the end of the while loop
    
    _scheme = NONE;
    return false;
}
    
//
//  Contour::_delIdentical --	delete identical sample points
//
//  Inputs
//	none
//
//  Outputs
//	none
//
bool Contour::_delIdentical( void )
{
    uint	sz		= _polygon.size();

    // Remove the identical vertex
    bool doExist = false;
    if ( ( _scheme == POSITIVE ) || ( _scheme == NEGATIVE ) 
	 //  || ( _scheme == MUTUAL )
	 )
	{
	unsigned int idV = 0;
	for ( unsigned int i = 0; i < sz; ++i ) {
	    Segment2 ed = _polygon.edge( i );
	    if ( ed.squared_length() < EPSILON ) {
		idV = i;
		doExist = true;
		break;
	    }
	}
	if ( doExist ) {
#ifdef DEBUG_SIMPLIFICATION
	    cerr << HERE << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl;
	    cerr << HERE << " Deleting the vertex of ID : " << idV << endl;
	    cerr << HERE << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl;
#endif	// DEBUG_SIMPLIFICATION
	    // Sleep for 1 seconds
	    // std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
	    VertexIterator iterV = _polygon.vertices_begin();
	    iterV = iterV + idV;
	    _polygon.erase( iterV ); 
	}
#ifdef DEBUG_SIMPLIFICATION
	else {
	    cerr << HERE << ">>>>> No identical vertices" << endl;
	}
#endif	// DEBUG_SIMPLIFICATION
    }

    return doExist;
}

//
//  Contour::_mycollinear --	my collinearity check
//
//  Inputs
//	boolean value according to the collinearity
//
//  Outputs
//	true if the three points are approximately collinear
//
bool Contour::_mycollinear( const Point2 & p, const Point2 & q, const Point2 & r )
{
    // const double threshold	= 1.0e-7; // 
    // const double threshold	= 1.0e-5; // 
    const double threshold	= 1.0e-4; // default
    // const double threshold	= 2.0e-4;
    // Calculation the area of triangle. We have skipped
    // multiplication with 0.5 to avoid floating point
    // computations
    double cf =
	p.x() * ( q.y() - r.y() ) +
	q.x() * ( r.y() - p.y() ) +
	r.x() * ( p.y() - q.y() );

#ifdef DEBUG_SIMPLIFICATION
    cerr << HERE << " : " << cf << "\t";
#endif	// DEBUG_SIMPLIFICATION
    if ( abs( cf ) < threshold ) return true;
    else return false;
}

//
//  Contour::_delCollinear --	delete sample points colinear with the adjacent
//				points
//
//  Inputs
//	none
//
//  Outputs
//	none
//
bool Contour::_delCollinear( void )
{
    uint	sz		= _polygon.size();
    // Remove the identical vertex
    bool doExist = false;

    if ( ( _scheme == POSITIVE ) || ( _scheme == NEGATIVE ) || ( _scheme == NONE ) ) {
	unsigned int idV = 0;
	for ( unsigned int i = 0; i < sz; ++i ) {
	    Point2 prev = _polygon[ ( i + sz - 1 ) % sz ];
	    Point2 curr = _polygon[ i ];
	    Point2 next = _polygon[ ( i + 1 ) % sz ];
	    if ( _mycollinear( prev, curr, next ) ) {
		idV = i;
		doExist = true;
		break;
	    }
#ifdef DEBUG_SIMPLIFICATION
	    else {
		cerr << HERE << " Vertex " << i << " is not collinear" << endl;
	    }
#endif	// DEBUG_SIMPLIFICATION
	}
	if ( doExist ) {
#ifdef DEBUG_SIMPLIFICATION
	    cerr << " Deleting the vertex of ID : " << idV << endl;
#endif	// DEBUG_SIMPLIFICATION
	    VertexIterator iterV = _polygon.vertices_begin();
	    iterV = iterV + idV;
	    _polygon.erase( iterV ); 
	}
	else {
	    // cerr << HERE << ">>>>> No colinear vertices" << endl;
	}
    }

    return doExist;
}

//
//  Contour::_contract --	contract the polygon by reduce the number of
//				vertices by one
//
//  Inputs
//	none
//	Assume that ( _idPri, _idSec, _scheme ) will be set before being called.
//
//  Outputs
//	boolean value according to whether contracted area is under the limit
//
bool Contour::_contract( double areaUpperLimit, int numLowerLimit )
{
    bool isFeasible = true;
    double minArea = 1.0e+8;

    // ------------------------------------------------------------
    // This is important to stop the simplification when we do not
    // have both positive and negative edge moves.
    // Noted by S.T on 2023/06/02
    if ( _scheme == NONE ) {
	cerr << HERE << "No compative pair of edge-moves" << endl;
	_isContractible = false;
	return false;
    }
    // ------------------------------------------------------------

//------------------------------------------------------------------------------
//	Preprocess
//------------------------------------------------------------------------------
#ifdef DEBUG_SIMPLIFICATION
    cerr << HERE << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << endl;
    cerr << HERE << " Now in Contour::_contract " << endl;
#endif	// DEBUG_SIMPLIFICATION
    switch ( _scheme ) {
      case POSITIVE:
#ifdef DEBUG_SIMPLIFICATION
	  cerr << HERE << " Positive "
	       << " minAreaP = " << _areaPos[ _idPri ]
	       << " refAreaN = " << _areaNeg[ _idSec ]  << endl;
#endif	// DEBUG_SIMPLIFICATION
	  minArea = _areaPos[ _idPri ];
	  break;
      case NEGATIVE:
#ifdef DEBUG_SIMPLIFICATION
	  cerr << HERE << " Negative "
	       << " minAreaN = " << _areaNeg[ _idPri ]
	       << " refAreaP = " << _areaPos[ _idSec ]  << endl;
#endif	// DEBUG_SIMPLIFICATION
	  minArea = _areaNeg[ _idPri ];
	  break;
      default:
	  cerr << " Unknown " << endl;
	  minArea = 0.0;
	  break;
    }
#ifdef DEBUG_SIMPLIFICATION
    cerr << HERE << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << endl;
#endif	// DEBUG_SIMPLIFICATION

    if ( minArea > areaUpperLimit ) {
	cerr << HERE << " contraction area exceets the upper limit " << endl;
	isFeasible = false;
    }

    uint	sz		= _polygon.size();
    if ( sz < numLowerLimit ) {
	cerr << HERE << " contraction polygon size exceets the lower limit " << endl;
	isFeasible = false;
    }

#ifdef DEBUG_SIMPLIFICATION
    cerr << HERE << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
    cerr << HERE << "%%%%%%%%% Start to contract the polygon %%%%%%%%%%" << endl;
    cerr << HERE << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
#endif	// DEBUG_SIMPLIFICATION
    // Sleep for 2 seconds
    // std::this_thread::sleep_for( std::chrono::milliseconds( 2000 ) );
    if ( ! isFeasible ) return false;

//------------------------------------------------------------------------------
//	conducting actual edge-moves 
//------------------------------------------------------------------------------
    switch ( _scheme ) {
      case POSITIVE:
	  {
#ifdef DEBUG_SIMPLIFICATION
	      cerr << HERE << "[Normal] Positive edge move selected" << endl;
#endif	// DEBUG_SIMPLIFICATION
	      double minArea = _areaPos[ _idPri ];
	      // double refArea = _areaNeg[ _idSec ];
	      Point2 vertexNS = _polygon.vertex( _idSec );
	      Point2 vertexNT = _polygon.vertex( (_idSec+1)%sz );

	      // original base = a, new base = b;
	      // original height H
	      // new area = m
	      // new height ratio = t
	      //
	      // ( a + (a + (b-a)t ) * tH = 2m
	      // ( 2a + (b-a)t )*t = 2m/H
	      // (b-a)t^2 + 2at - 2m/H = 0
	      // t = ( -a - sqrt( a*a + 2m(b-a)/H ) ) /  ( b - a )
	      double a = sqrt( _polygon.edge( _idSec ).squared_length() );
	      double b = sqrt( _segNeg[ _idSec ].squared_length() );
	      // double H = ( 2.0 * refArea ) / ( a+b );
	      double H = abs( _distNeg[ _idSec ] );
	      double t = _solveQuadratic( b-a, 2.0*a, -2.0*minArea/H );

#ifdef DEBUG_SIMPLIFICATION
	      double refArea = _areaNeg[ _idSec ];
	      cerr << HERE << " t = " << t << endl;
	      if ( abs( a - b ) >= EPSILON ) {
		  double tt = ( -a + sqrt( a*a + 2.0*minArea*(b-a)/H ) ) / ( b-a );
		  cerr << HERE << " tt = " << tt << endl;
		  assert( abs( t - tt ) < EPSILON );
	      }
	      cerr << HERE << " idPri = " << _idPri << " idSec = " << _idSec << endl;
	      cerr << HERE << " minP = " << minArea << " minN = " << refArea << endl;
	      double e = a + t * ( b - a );
	      double h = t * H;
	      double S = 0.5 * ( a + e ) * h;
	      double SS = 0.5 * ( a + b ) * H;
	      cerr << HERE << "***** refArea = " << refArea << " vs. " << SS << endl;
	      cerr << HERE << " e = " << e << " h = " << h << endl;
	      cerr << HERE << "***** minArea = " << minArea << " vs. " << S << endl;
	      if ( std::abs( refArea - SS ) >= EPSILON )
		  assert( std::abs( refArea - SS ) < EPSILON );
	      if ( std::abs( minArea - S ) >= EPSILON )
		  assert( std::abs( minArea - S ) < EPSILON );
#endif	// DEBUG_SIMPLIFICATION

	      _polyVertex( _idPri, _segPos[ _idPri ].source() );
	      _polyVertex( _idPri+1, _segPos[ _idPri ].target() );

	      Point2 divNS = _innerDiv( vertexNS, _segNeg[ _idSec ].source(), t );
	      Point2 divNT = _innerDiv( vertexNT, _segNeg[ _idSec ].target(), t );
	      _polyVertex( _idSec, divNS );
	      _polyVertex( _idSec+1, divNT );
	  }
	  break;
      case NEGATIVE:
	  {
#ifdef DEBUG_SIMPLIFICATION
	      cerr << HERE << "[Normal] Negative edge move selected" << endl;
#endif	// DEBUG_SIMPLIFICATION
	      double minArea = _areaNeg[ _idPri ];
	      // double refArea = _areaPos[ _idSec ];
	      Point2 vertexPS = _polygon.vertex( _idSec );
	      Point2 vertexPT = _polygon.vertex( (_idSec+1)%sz );

	      // original base = a, new base = b;
	      // original height H
	      // new area = m
	      // new height ratio = t
	      //
	      // ( a + (a + (b-a)t ) * t*H = 2*m
	      // ( 2a + (b-a)t )*t = 2*m/H
	      // (b-a)t*t + 2*a*t - 2*m/H = 0
	      // t = ( -a - sqrt( a*a + 2*m*(b-a)/H ) ) /  ( b - a )
	      double a = sqrt( _polygon.edge( _idSec ).squared_length() );
	      double b = sqrt( _segPos[ _idSec ].squared_length() );
	      // double H = ( 2.0 * refArea ) / ( a + b );
	      double H = abs( _distPos[ _idSec ] );
	      double t = _solveQuadratic( b-a, 2.0*a, -2.0*minArea/H );
#ifdef DEBUG_SIMPLIFICATION
	      double refArea = _areaPos[ _idSec ];
	      cerr << HERE << " t = " << t << endl;
	      if ( abs( a - b ) >= EPSILON ) {
		  double tt = ( -a + sqrt( a*a + 2.0*minArea*(b-a)/H ) ) / ( b-a );
		  cerr << HERE << " tt = " << tt << endl;
		  assert( abs( t - tt ) < EPSILON );
	      }
	      cerr << HERE << " idPri = " << _idPri << " idSec = " << _idSec << endl;
	      cerr << HERE << " minN = " << minArea << " minP = " << refArea << endl;
	      cerr << HERE << " H = " << H << " vs. "
		   << ( 2.0 * refArea ) / ( a + b ) << endl;
	      cerr << HERE << " b - a = " << b - a << endl;
	      double e = a + t * ( b - a );
	      double h = t * H;
	      double S = 0.5 * ( a + e ) * h;
	      double SS = 0.5 * ( a + b ) * H;
	      cerr << HERE << "***** refArea = " << refArea << " vs. " << SS << endl;
	      cerr << HERE << " a = " << a << " b = " << b << endl;
	      cerr << HERE << " e = " << e << " h = " << h << endl;
	      cerr << HERE << "***** minArea = " << minArea << " vs. " << S << endl;
	      if ( std::abs( refArea - SS ) >= EPSILON )
		  assert( std::abs( refArea - SS ) < EPSILON );
	      if ( std::abs( minArea - S ) >= EPSILON )
		  assert( std::abs( minArea - S ) < EPSILON );
#endif	// DEBUG_SIMPLIFICATION

	      _polyVertex( _idPri, _segNeg[ _idPri ].source() );
	      _polyVertex( _idPri+1, _segNeg[ _idPri ].target() );

	      Point2 divPS = _innerDiv( vertexPS, _segPos[ _idSec ].source(), t );
	      Point2 divPT = _innerDiv( vertexPT, _segPos[ _idSec ].target(), t );
	      _polyVertex( _idSec, divPS );
	      _polyVertex( _idSec+1, divPT );
	  }
	  break;
      case NONE:
	  ;			// do nothing
	  break;
      default:
	  cerr << HERE << " This cannot be allowed" << endl;
	  assert( false );
    }

    while ( _delIdentical() );
    while ( _delCollinear() );

    return true;
}

//
//  Contour::_simplifyOnce --	schematize the polygon only once
//
//  Inputs
//	none
//
//  Outputs
//	none
//
bool Contour::_simplifyOnce( void )
{
    if ( ! _isContractible ) 
	cerr << HERE << " Polygon simplification already done" << endl;

    bool isPossible = _contract( INFINITY, 0 );
    if ( ! isPossible )
	cerr << HERE << " Cannot contract the polygon " << endl;

    cerr << HERE << " polygon size = " << _polygon.size() << endl;
    
    _isContractible = _prepare();
    // isPossible = isPossible && _isContractible;
    
    return isPossible;
}

//
//  Contour::_simplifyByArea --	iteratively schematize the polygon until it converges
//
//  Inputs
//	Arealimit	: limit of the area for contraction
//
//  Outputs
//	none
//
bool Contour::_simplifyByArea( double areaUpperLimit )
{
    if ( ! _isContractible ) 
	cerr << HERE << " Polygon simplification already done" << endl;

    bool isPossible = _contract( areaUpperLimit, 0 );
    if ( ! isPossible )
	cerr << HERE << " Cannot contract the polygon " << endl;

    _isContractible = _prepare();
    // isPossible = isPossible && _isContractible;
    
    return isPossible;
}

//
//  Contour::_simplifyByNum --	iteratively schematize the polygon until it converges
//
//  Inputs
//	numLimit	: limit of the number of polygon corners
//
//  Outputs
//	none
//
bool Contour::_simplifyByNum( int numLowerLimit )
{
    if ( ! _isContractible ) 
	cerr << HERE << " Polygon simplification already done" << endl;

    bool isPossible = _contract( INFINITY, numLowerLimit );
    if ( ! isPossible )
	cerr << HERE << " Cannot contract the polygon " << endl;

    _isContractible = _prepare();
    // isPossible = isPossible && _isContractible;
    
    return isPossible;
}

//
//  Contour::_registerObstacles --	register obstacles
//
//  Inputs
//	id	:	ID in the obstacle list
//	conflict:	obstacles including itself
//
//  Outputs
//	none
//
void Contour::_registerConflicts( const unsigned int & id,
				  const vector< Polygon2 > & conflict )
{
    Bbox2 thisBox = _polygon.bbox();
    
    _obstacle.clear();
    for ( unsigned int i = 0; i < conflict.size(); ++i ) {
	if ( i != id ) {
	    Bbox2 thatBox = conflict[ i ].bbox();
	    if ( do_overlap( thisBox, thatBox ) ) {
		for ( unsigned int j = 0; j < conflict[ i ].size(); ++j ) {
		    _obstacle.push_back( conflict[ i ].edge( j ) );
		}
	    }
	}
    }
}

//
//  Contour::_simplifyFully --	fully schematize the polygon until it converges
//				(Call this before registering obstacles!!)
//  Inputs
//	none
//
//  Outputs
//	none
//
void Contour::_fullySimplify( void )
{
    _reset();
	
    // contour.setContractible();
    if ( ! _prepare() ) _isContractible = false;
	
    bool isPossible = true;
    do {
	isPossible = _simplifyByArea( INFINITY );
    } while ( isPossible );
}

//
//  Contour::_voteAngles--	vote the Gaussian distribution of edge
//				orientations as angles
//
//  Inputs
//	none
//
//  Outputs
//	none
//
vector< double > Contour::_voteAngles( const double bandwidth )
{
    unsigned int sz		= _polygon.size();
    const unsigned int nSamples	= 180;
    // const double bandwidth	= 180.0 / 40.0;
    // const double bandwidth	= 180.0 / 25.0;
    // const double bandwidth	= 180.0 / 20.0;
    // const double bandwidth	= 180.0 / 16.0; // <==
    // const double bandwidth	= 180.0 / 10.0; // This strictly squarifies
    _density.resize( nSamples );
    for ( unsigned int k = 0; k < nSamples; ++k ) _density[ k ] = 0.0;
    
    for ( unsigned int i = 0; i < sz; ++i ) {
	Segment2	edge = _polygon.edge( i ) ;
	Vector2		vec = edge.to_vector();
	double		weight = sqrt( vec.squared_length() );
	// double		weight = 1.0;
	double		slant = atan2( vec.y(), vec.x() ) ;
	slant		= 180.0 * slant / M_PI; // radian to degree
	while ( slant < 0.0 ) { slant += 180.0; }
	while ( slant > 180.0 ) { slant -= 180.0; }
	// cerr << HERE << " slanting angle[ " << setw( 2 ) << i << " ] = " << slant << endl;

	for ( unsigned int k = 0; k < nSamples; ++k ) {
	    double x = ( double )k;
	    // Due to the cyclic domain of the oritentation angle
	    _density[ k ] += weight * _Gaussian( ( x - ( slant - 180.0 ) )/bandwidth );
	    _density[ k ] += weight * _Gaussian( ( x - slant )/bandwidth );
	    _density[ k ] += weight * _Gaussian( ( x - ( slant + 180.0 ) )/bandwidth );
	}
    }

    // Unnecessary normalization
    for ( unsigned int k = 0; k < nSamples; ++k ) {
	_density[ k ] /= ((double)sz*bandwidth);
    }

    // Find the representative angles 
    vector< double > proxy;
    proxy.clear();
    for ( unsigned int k = 0; k < nSamples; ++k ) {
	double prev = _density[ (k-1+nSamples)%nSamples ];
	double curr = _density[ k ];
	double next = _density[ (k+1)%nSamples ];
	if ( ( prev < curr ) && ( curr > next ) ) {
	    cerr << HERE << "[ " << setw( 3 ) << k << " ] = " << _density[ k ] << endl;
	    proxy.push_back( ( double )k );
	}
    }

//------------------------------------------------------------------------------
//	Special squaring
//------------------------------------------------------------------------------
    if ( proxy.size() == 2 ) {
	double small = proxy[ 0 ];
	double large = proxy[ 1 ];
	assert( small < large );
	if ( large - small < 90.0 ) {
	    double offset = 90.0 - ( large - small );
	    proxy[ 0 ] -= 0.5 * offset;
	    proxy[ 1 ] += 0.5 * offset;
	}
	else if ( large - small > 90.0 ) {
	    double offset = ( large - small ) - 90.0;
	    proxy[ 0 ] += 0.5 * offset;
	    proxy[ 1 ] -= 0.5 * offset;
	}

	cerr << HERE << " Adjusted angles = " << proxy[ 0 ] << " , " << proxy[ 1 ] << endl;
    }
//------------------------------------------------------------------------------

    return proxy;
}


//
//  Contour::_alignEdges--	align edges to the closest representative orientation
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Contour::_alignEdges( const vector< double > & proxy )
{
    unsigned int	sz	= _polygon.size();
    Polygon2		step;

    // Transform the representative angles into orientation vectors
    vector< Vector2 > orient;
    orient.clear();
    for ( unsigned int j = 0; j < proxy.size(); ++j ) {
	double radian = M_PI * proxy[ j ] / 180.0;
	orient.push_back( Vector2( cos( radian ), sin( radian ) ) );
    }

    assert( proxy.size() == orient.size() );
    for ( unsigned int i = 0; i < sz; ++i ) {
	Segment2	edge = _polygon.edge( i );
	Vector2		vec = edge.to_vector();
	double		angle = atan2( vec.y(), vec.x() ) ;
	angle		= 180.0 * angle / M_PI; // radian to degree
	// search for the closest orientation angle
	assert( proxy.size() > 0 );
	double minSpan	= INFINITY;
	unsigned int idProj = 0; 
	for ( unsigned int j = 0; j < proxy.size(); ++j ) {
	    double curSpan = abs( angle - proxy[ j ] );
	    if ( curSpan < minSpan ) {
		minSpan = curSpan;
		idProj = j;
	    }
	}
#ifdef SKIP
	cerr << HERE << " Closest orientation for edge [ "
	     << setw( 3 ) << i << " ] => angle[ " << idProj
	     << " ] difference = " << minSpan << endl;
#endif	// SKIP

	// project the edge
	Vector2 T = orient[ idProj ];
	Vector2 N = Vector2( -T.y(), T.x() );
	Vector2 projT = ( vec * T ) * T;
	Vector2 projN = ( vec * N ) * N;
	Point2 Ps = edge.source();
	// Point2 Pt = edge.target();
	step.push_back( Ps );
	step.push_back( Ps + 0.5*projT );
	step.push_back( Ps + 0.5*projT + projN );
	// Skip the last point for the next segment
	// step.push_back( Ps + projT + projN );
    }
    _polygon = step;
}
	
//------------------------------------------------------------------------------
//	Public Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------

//
//  Contour::Contour --	default constructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Contour::Contour() 
{
    _mmapPos.clear();
    _mmapNeg.clear();

    _isContractible = true;

    _polygon.clear();
    _eangle.clear();
    _distPos.clear();
    _distNeg.clear();
    _segPos.clear();
    _segNeg.clear();
    _areaPos.clear();
    _areaNeg.clear();

    _idPri = _idSec = 0;
    _scheme = NONE;

#ifdef CHECK_CONFLICTS_WITH_OTHERS
    _obstacle.clear();
#endif	// CHECK_CONFLICTS_WITH_OTHERS
}


//
//  Contour::Contour --	copy constructor
//
//  Inputs
//	obj	: object of this class
//
//  Outputs
//	none
//
Contour::Contour( const Contour & obj )
{
    _mmapPos	= obj._mmapPos;
    _mmapNeg	= obj._mmapNeg;
    
    _isContractible	= obj._isContractible;

    _polygon	= obj._polygon;
    _eangle	= obj._eangle;
    _turn	= obj._turn;
    _move	= obj._move;
    _config	= obj._config;

    _distPos	= obj._distPos;
    _distNeg	= obj._distNeg;
    _segPos	= obj._segPos;
    _segNeg	= obj._segNeg;
    _areaPos	= obj._areaPos;
    _areaNeg	= obj._areaNeg;

    _idPri	= obj._idPri;
    _idSec	= obj._idSec;
    _scheme	= obj._scheme;

#ifdef CHECK_CONFLICTS_WITH_OTHERS
    _obstacle	= obj._obstacle;
#endif	// CHECK_CONFLICTS_WITH_OTHERS
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------

//
//  Contour::~Contour --	destructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Contour::~Contour()
{

}


//------------------------------------------------------------------------------
//	Referrring to members
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Functions for sorting
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	equality
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------

//
//  Contour::operator = --	assignement
//
//  Inputs
//	obj	: objects of this class
//
//  Outputs
//	this object
//
Contour & Contour::operator = ( const Contour & obj )
{
    if ( this != &obj ) {
	_mmapPos	= obj._mmapPos;
	_mmapNeg	= obj._mmapNeg;

	_isContractible	= obj._isContractible;

	_polygon	= obj._polygon;
	_eangle		= obj._eangle;
	_turn		= obj._turn;
	_move		= obj._move;
	_config		= obj._config;
	
	_distPos	= obj._distPos;
	_distNeg	= obj._distNeg;
	_segPos		= obj._segPos;
	_segNeg		= obj._segNeg;
	_areaPos	= obj._areaPos;
	_areaNeg	= obj._areaNeg;

	_idPri		= obj._idPri;
	_idSec		= obj._idSec;
	_scheme		= obj._scheme;

#ifdef CHECK_CONFLICTS_WITH_OTHERS
	_obstacle	= obj._obstacle;
#endif	// CHECK_CONFLICTS_WITH_OTHERS
    }
    return *this;
}


//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------

//
//  operator << --	output
//
//  Argument
//	stream	: output stream
//	obj	: object of this class
//
//  Outputs
//	output stream
//
ostream & operator << ( ostream & stream, const Contour & obj )
{
    return stream;
}


//
//  operator >> --	input
//
//  Inputs
//	stream	: input stream
//	obj	: object of this class
//
//  Outputs
//	input stream
//
istream & operator >> ( istream & stream, Contour & obj )
{
    return stream;
}


// end of header file
// Do not add any stuff under this line.
