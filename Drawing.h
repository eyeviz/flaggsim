//==============================================================================
// Drawing.h
//	: header file for line drawings
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-07-03 20:45:29 shigeo"
//
//==============================================================================

#ifndef _Drawing_H		// begining of header file
#define _Drawing_H		// notifying that this file is included

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <vector>
#include <cassert>

using namespace std;

#include "CGAL.h"
#include "Network.h"
#include "Directed.h"
#include "Set.h"
#include "Expansion.h"

//------------------------------------------------------------------------------
//	Macro Switches
//------------------------------------------------------------------------------
// ************************************************************
// If you like to automatical compute the conjoining of polygons, enable this!!
// ************************************************************
// #define AUTOMATIC_CONJOINING


#define SQUARED_SMOOTH_COST
// #define DATA_COST_PANALTY_WITH_TOPOLOGY
#define PROXIMITY_RATIO_COST

// #define USE_CONVEX_HULLS
#ifndef USE_CONVEX_HULLS
#define USE_CONCAVE_HULLS
#endif	// USE_CONVEX_HULLS

// This is defined in Network.h
// #define USING_SIMILARITY_CONJOINING



//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------
// The height and width of the window is 2.0, which is equivalent to 120mm
// The limit distance range from 0.5 to 3.0, for example
// 4.0/120.0 * 2.0 = 
#define LIMIT_BUILDING_INTERVAL		(0.050)

// // for saving the drawing
// #define NORMALIZE_BY_FIXED_SCALE        // (180m x 2 = 360m)

// #ifdef NORMALIZE_BY_FIXED_SCALE
// // (Average = 74.3) + (STDEV = 45.9) = 120.2
// #define BLOCK_SIZE_AVERAGE      (74.3)
// #define BLOCK_SIZE_STDEV        (45.9)
// #define BLOCK_SIZE_UPPER_LIMIT  (BLOCK_SIZE_AVERAGE+BLOCK_SIZE_STDEV)
// #define BLOCK_SIZE_LOWER_LIMIT  (BLOCK_SIZE_AVERAGE-BLOCK_SIZE_STDEV)
// #define BLOCK_SIZE_BASE         (0.5*BLOCK_SIZE_UPPER_LIMIT)
// #endif  // NORMALIZE_BY_FIXED_SCALE

// [-1.0, 1.0] => 120m

//#define K_NEIGHBOR		(3)
#define K_NEIGHBOR		(5)
// #define K_NEIGHBOR		(8)

//#define NUM_TRIALS		(1)
#define NUM_TRIALS		(3)
//#define NUM_TRIALS		(501)

// #define MAX_QUANTIZATION_LEVEL	(7)
// #define MAX_QUANTIZATION_LEVEL	(15)
// #define MAX_QUANTIZATION_LEVEL	(63)
// #define MAX_QUANTIZATION_LEVEL	(255)


// #define PROXIMITY_RATIO_UPPER	(0.70)
// #define PROXIMITY_RATIO_UPPER	(0.40)
#define PROXIMITY_UPPER	(0.30) // <-- OK
// #define PROXIMITY_RATIO_UPPER	(0.20)
// #define PROXIMITY_RATIO_UPPER	(0.15) // <==
// #define PROXIMITY_RATIO_UPPER	(0.10) // <--
// #define PROXIMITY_RATIO_UPPER	(0.05)
// #define PROXIMITY_RATIO_UPPER	(0.01)

#ifdef USING_SIMILARITY_CONJOINING
// #define SIMILARITY_RATIO_LOWER	(0.50)
// #define SIMILARITY_RATIO_LOWER	(0.70) // <==
// #define SIMILARITY_RATIO_LOWER	(0.80)
#define SIMILARITY_LOWER	(0.90) // <-- OK
// #define SIMILARITY_RATIO_LOWER	(0.95)
#endif	// USING_SIMILARITY_CONJOINING

#define PROXIMITY_RATIO		(0.20)

#ifdef USING_SIMILARITY_CONJOINING
#define SIMILARITY_RATIO	(0.20)
#endif	// USING_SIMILARITY_CONJOINING

//#define CLOSENESS_LIMIT		(2)
#define CLOSENESS_LIMIT		(3)
// #define CLOSENESS_LIMIT		(4)


#define COST_SCALE		(10.0)

// Larger lower limit leads more isolated clusters
// ------------------------------
#define DATA_COST_LOWER		(2.0) // <-- OK
// #define DATA_COST_LOWER		(2.5)
// #define DATA_COST_LOWER		(4.0)
// #define DATA_COST_LOWER		(5.0)
// #define DATA_COST_LOWER		(10.0)
// ------------------------------
// #define DATA_COST_UPPER		(10.0)
#define DATA_COST_UPPER		(12.0) // <-- OK
// #define DATA_COST_UPPER		(20.0)
// #define DATA_COST_UPPER		(50.0)
// #define DATA_COST_UPPER		(100.0)
// #define DATA_COST_UPPER		(200.0)
#define DATA_COST_INSIDE	(1.0)
#define DATA_COST_OUTSIDE	(DATA_COST_UPPER)
#define DATA_COST_PENALTY_SCALE	(5.0)

// Smaller upper limit leads more isolated clusters
// ------------------------------
#define SMOOTH_COST_LOWER	(1.0) // <-- OK
// ------------------------------
// #define SMOOTH_COST_UPPER	(2.0)
// #define SMOOTH_COST_UPPER	(4.0)
#define SMOOTH_COST_UPPER	(5.0) // <-- OK
// #define SMOOTH_COST_UPPER	(10.0)
// #define SMOOTH_COST_UPPER	(20.0)

// Larger lower limit leads more isolated clusters
// ------------------------------
#define LABEL_COST_LOWER	(2.0)
// #define LABEL_COST_LOWER	(4.0)
// #define LABEL_COST_LOWER	(5.0) // <-- OK
// #define LABEL_COST_LOWER	(10.0)
// #define LABEL_COST_LOWER	(50.0)
// ------------------------------
#define LABEL_COST_UPPER	(10.0) // <-- OK
// #define LABEL_COST_UPPER	(20.0)
// #define LABEL_COST_UPPER	(50.0)
// #define LABEL_COST_UPPER	(100.0)
// #define LABEL_COST_UPPER	(200.0)
// ------------------------------
#define LABEL_COST_SINGLE	(1.0)






//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class Drawing {

  private:

    vector< Tuple >		_assoc;		// associateion (PolygonID, CornerID)
    vector< Link >		_web;	
    vector< NetVertexDescriptor >
				_onelayer;
    vector< vector< NetVertexDescriptor > >
				_twolayer;
    Directed			_wrapper;	// network that wrap the corner vertices

  protected:

    vector< Polygon2 >		_poly;		// polygons
    vector< Set >		_glID;		// global IDs of points
    Point2			_center;	// center of the line drawing
    
    vector< Polygon2 >		_bound;		// simplified boundary
    vector< vector<Triangle2> >	_tri;		// triangulated boundary

    Network			_netNbr;	// k-neighbor graph
    Network			_netPrx;	// graph for proximity labels
#ifdef USING_SIMILARITY_CONJOINING
    Network			_netSim;	// graph for similarity labels
#endif	// USING_SIMILARITY_CONJOINING

    vector< Set >		_labelSgl;
    vector< Set >		_labelPrx;
#ifdef USING_SIMILARITY_CONJOINING
    vector< Set >		_labelSim;
#endif	// USING_SIMILARITY_CONJOINING
    vector< Set >		_labelAll;


    vector< Polygon2 >		_hullSgl;
    vector< Polygon2 >		_hullPrx;
#ifdef USING_SIMILARITY_CONJOINING
    vector< Polygon2 >		_hullSim;
#endif	// USING_SIMILARITY_CONJOINING
    vector< Polygon2 >		_hullAll;
#ifdef USE_CONCAVE_HULLS
    vector< Polygon2 >		_boundSgl;
    vector< Polygon2 >		_boundPrx;
#ifdef USING_SIMILARITY_CONJOINING
    vector< Polygon2 >		_boundSim;
#endif	// USING_SIMILARITY_CONJOINING
    vector< Polygon2 >		_boundAll;
#endif	// USE_CONCAVE_HULLS
    
    vector< vector< double > >	_dataCost;
    vector< vector< double > >	_smoothCost;
    vector< double >		_labelCost;

    vector< Expansion >		_expand;
    
    static double		_maxMinVSets( const Polygon2 & p, const Polygon2 & q );
				// distance between two polygons
    static double		_minMinVSets( const Polygon2 & p, const Polygon2 & q );
				// distance between two polygons

    static double		_aspectRatio( const double & a, const double & b );
#ifdef USING_SIMILARITY_CONJOINING
    static double		_simBbox( const Bbox2 & p, const Bbox2 & q );
#endif	// USING_SIMILARITY_CONJOINING

    double	_maxPolys( const unsigned int & i, const unsigned int & j ) {
	// cerr << " i = " << i << " j = " << j << " size = " << this->size() << endl;
	// assert( i < _poly.size() );
	// assert( j < _poly.size() );
	double ret = std::max( _maxMinVSets( _poly[ i ], _poly[ j ] ),
			       _maxMinVSets( _poly[ j ], _poly[ i ] ) );
	return ret;
    } // distance between polygons indexed by p and q
    double	_minPolys( const unsigned int & i, const unsigned int & j ) {
	// cerr << " i = " << i << " j = " << j << " size = " << this->size() << endl;
	// assert( i < _poly.size() );
	// assert( j < _poly.size() );
	double ret = std::min( _minMinVSets( _poly[ i ], _poly[ j ] ),
			       _minMinVSets( _poly[ j ], _poly[ i ] ) );
	return ret;
    } // distance between polygons indexed by p and q
    double	_distPolys( const unsigned int & i, const unsigned int & j ) {
	double ret = _maxPolys( i, j );
	return ret;
    } // distance between polygons indexed by p and q

#ifdef USING_SIMILARITY_CONJOINING
    double	_simPolys( const unsigned int & i, const unsigned int & j ) {
	assert( i < _poly.size() );
	assert( j < _poly.size() );
	return _simBbox( _poly[ i ].bbox(), _poly[ j ].bbox() );
    }
#endif	// USING_SIMILARITY_CONJOINING
    
    static void		_normalizeVector( vector< double > & cost,
					  const double lower = 0.0,
					  const double upper = 1.0 );
    static void		_normalizeMatrix( vector< vector< double > > & cost,
					  const double lower = 0.0,
					  const double upper = 1.0 );

//------------------------------------------------------------------------------
//	Triangulation
//------------------------------------------------------------------------------
    void		_triangulate	( void );

    void		_updateEdges	( Network & net );

    void		_calcNeighbor	( void );
#ifdef SKIP
    void		_calcBSkeleton	( void );
#endif	// SKIP
    void		_wrapBSkeleton	( void );
    void		_calcWrapper	( void );
    
    void		_calcCnvProximity	( void );
    void		_calcProximity		( void ) {
	_calcCnvProximity();
    }

#ifdef USING_SIMILARITY_CONJOINING
    void		_calcCnvSimilarity	( void );
    void		_calcSimilarity		( void ) {
	_calcCnvSimilarity();
    }
#endif	// USING_SIMILARITY_CONJOINING

    static void		_labelComponents( Network & net, vector< Set > & label );	

    static void		_convexForLabel	( Network & net, const Set & label,
					  Polygon2 & CH, Set & PN );
    static void		_convexForLabel	( Network & net, const Set & label,
					  Polygon2 & CH ) {
	Set dummyID;
	_convexForLabel( net, label, CH, dummyID );
    }
#ifdef USE_CONCAVE_HULLS
    void		_concaveForLabel( Network & net, const Set & label,
					  Polygon2 & CH, Set & PN );
    void		_concaveForLabel( Network & net, const Set & label,
					  Polygon2 & CH ) {
	Set dummyID;
	_concaveForLabel( net, label, CH, dummyID );
    }
#endif	// USE_CONCAVE_HULLS
    
    double		_proximityCost	( Network & net, const Set & label,
					  const Polygon2 & hull );
#ifdef USING_SIMILARITY_CONJOINING
    double		_similarityCost	( Network & net, const Set & label );
#endif	// USING_SIMILARITY_CONJOINING
    
    void		_assignLabels	( void );
    void		_calcStdDataCost( void );
    void		_calcDataCost	( void );
    void		_calcSmoothCost	( void );
    void		_calcLabelCost	( void );

    void		_graphCut	( void );
#ifdef USE_CONVEX_HULLS
    void		_aggregateLabels( const vector< Set > & gestalt );
#else	// USE_CONVEX_HULLS
    void		_aggregateLabels( const vector< Set > & gestalt,
					  const vector< Polygon2 > & layout,
					  const vector< Set > & index );
#endif	// USE_CONVEX_HULLS

    void		_simplifyPolys	( void );
    void		_squarePolys	( void );

    void		_clear		( void );
    
    
public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    Drawing();			// default constructor
    Drawing( const Drawing & obj );
				// copy constructor

//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
    ~Drawing();			// destructor

//------------------------------------------------------------------------------
//	Static variables
//------------------------------------------------------------------------------
    static unsigned int		num_trials;

    static double		interval_threshold;

    static double		proximity_upper;
#ifdef USING_SIMILARITY_CONJOINING
    static double		similarity_lower;
#endif	// USING_SIMILARITY_CONJOINING
    static double		proximity_ratio;
#ifdef USING_SIMILARITY_CONJOINING
    static double		similarity_ratio;
#endif	// USING_SIMILARITY_CONJOINING
    
    static double		data_cost_lower;
    static double		data_cost_upper;
    static double		data_cost_inside;

    static double		smooth_cost_lower;
    static double		smooth_cost_upper;

    static double		label_cost_lower;
    static double		label_cost_upper;
    static double		label_cost_single;

    static void			(*redraw)( void );

//------------------------------------------------------------------------------
//	Referrring to members
//------------------------------------------------------------------------------
    const vector< Tuple > & assoc( void ) const
						{ return _assoc; }
    const vector< Link > & web( void ) const	{ return _web; }

    const Directed & wrapper( void ) const 	{ return _wrapper; }
    Directed & wrapper( void )			{ return _wrapper; }


    const vector< Polygon2 > & poly( void ) const { return _poly; }
    vector< Polygon2 > & poly( void )		{ return _poly; }
    const vector< Set > & glID( void ) const	{ return _glID; }
    vector< Set > & glID( void )		{ return _glID; }

    const vector< Polygon2 > & bound( void ) const
						{ return _bound; }
    vector< Polygon2 > & bound( void )		{ return _bound; }

    const vector< vector< Triangle2 > > tri( void ) const
						{ return _tri; }
    vector< vector< Triangle2 > > & tri( void ) { return _tri; }

    const Network & netNbr( void ) const	{ return _netNbr; }
    Network & netNbr( void )			{ return _netNbr; }
    const Network & netPrx( void ) const	{ return _netPrx; }
    Network & netPrx( void )			{ return _netPrx; }
#ifdef USING_SIMILARITY_CONJOINING
    const Network & netSim( void ) const	{ return _netSim; }
    Network & netSim( void )			{ return _netSim; }
#endif	// USING_SIMILARITY_CONJOINING

    void		initWrapper	( void ) {
	_wrapBSkeleton();
	_calcWrapper();
    }
				    // for debugging purpose
    
    static void		convexForLabel	( Network & net, const Set & label,
					  Polygon2 & CH, Set & PN ) {
	_convexForLabel( net, label, CH, PN );
    }
    static void		convexForLabel	( Network & net, const Set & label,
					  Polygon2 & CH ) {
	_convexForLabel( net, label, CH );
    }
#ifdef USE_CONCAVE_HULLS
    void		concaveForLabel	( Network & net, const Set & label,
					  Polygon2 & CH, Set & PN ) {
	_concaveForLabel( net, label, CH, PN );
    }
    void		concaveForLabel	( Network & net, const Set & label,
					  Polygon2 & CH ) {
	_concaveForLabel( net, label, CH );
    }
#endif	// USE_CONCAVE_HULLS

    const vector< Polygon2 > & hullPrx( void ) const {
	return _hullPrx;
    }
    vector< Polygon2 > & hullPrx( void )	{ return _hullPrx; }

#ifdef USING_SIMILARITY_CONJOINING
    const vector< Polygon2 > & hullSim( void ) const {
	return _hullSim;
    }
    vector< Polygon2 > & hullSim( void )	{ return _hullSim; }
#endif	// USING_SIMILARITY_CONJOINING
    
    const vector< Expansion > & expand( void )
					  const { return _expand; }
    vector< Expansion > & expand( void )	{ return _expand; }
    
//------------------------------------------------------------------------------
//	Geometric computations
//------------------------------------------------------------------------------
    double distPolys( const unsigned int & i, const unsigned int & j ) {
	return _distPolys( i, j );
    }
#ifdef USING_SIMILARITY_CONJOINING
    double simPolys( const unsigned int & i, const unsigned int & j ) {
	return _simPolys( i, j );
    }
#endif	// USING_SIMILARITY_CONJOINING

//------------------------------------------------------------------------------
//	Initialization
//------------------------------------------------------------------------------
    void clear( void ) { _clear(); }

//------------------------------------------------------------------------------
//	Triangulation
//------------------------------------------------------------------------------
    void triangulate( void ) { _triangulate(); }
//------------------------------------------------------------------------------
//	Computations for finiding Gestalts
//------------------------------------------------------------------------------
    void conjoin( void ) {
	_assignLabels();
	_calcLabelCost();
	_calcSmoothCost();
	_calcDataCost();
    }
    void optimize( void ) {
	_graphCut();
    }
#ifdef USE_CONVEX_HULLS
    void aggregate( int id ) {
	_aggregateLabels( _expand[ id ].cluster() );
	// _simplifyPolys();
#ifdef AUTOMATIC_CONJOINING
	conjoin();
	optimize();
#endif	// AUTOMATIC_CONJOINING
	_bound = _poly;
    }
#else	// USE_CONVEX_HULLS
    void aggregate( int id,
		    const vector< Polygon2 > & layout,
		    const vector< Set > & index ) {
	_aggregateLabels( _expand[ id ].cluster(), layout, index );
	// _simplifyPolys();
#ifdef AUTOMATIC_CONJOINING
	conjoin();
	optimize();
#endif	// AUTOMATIC_CONJOINING
	_bound = _poly;
    }
#endif	// USE_CONVEX_HULLS
    void simplify( void ) {
	_simplifyPolys();
    }
    void square( void ) {
	_squarePolys();
    }

    
//------------------------------------------------------------------------------
//	Functions for sorting
//------------------------------------------------------------------------------
#ifdef SKIP
    friend int compare ( const Drawing * a, const Drawing * b );
    friend int compare ( const Drawing ** a, const Drawing ** b );
				// comparison
#endif	// SKIP

//------------------------------------------------------------------------------
//	equality
//------------------------------------------------------------------------------
#ifdef SKIP
    friend int operator == ( const Drawing & a, const Drawing & b );
    friend int operator != ( const Drawing & a, const Drawing & b ) {
	return ( ! ( a == b ) );
    }				// equality
#endif	// SKIP

//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------
    Drawing & operator = ( const Drawing & obj );
				// assignment

//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
    friend ostream & operator << ( ostream & stream, const Drawing & obj );
				// output
    friend istream & operator >> ( istream & stream, Drawing & obj );
				// input

//------------------------------------------------------------------------------
//	Class name
//------------------------------------------------------------------------------
    virtual const char * className( void ) const { return "Drawing"; }
				// class name
};



#endif // _Drawing_H

// end of header file
// Do not add any stuff under this line.
