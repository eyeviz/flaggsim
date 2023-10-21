//==============================================================================
// Contour.h
//	: header file for contour shapes
//
//------------------------------------------------------------------------------
//
//				Date: Tue Sep 19 02:34:27 2017
//
//==============================================================================

#ifndef _Contour_H		// begining of header file
#define _Contour_H		// notifying that this file is included

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include "CGAL.h"


//------------------------------------------------------------------------------
//	Defining data types
//------------------------------------------------------------------------------
typedef enum { POSITIVE, NEGATIVE, NONE }	Scheme; // contraction schemes
typedef enum { EXPAND, SHRINK, BOTH }		Move;	// edge move types
typedef enum { CONVEX, REFLEX }			Turn;	// angle type

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------
// avoid contracting positive/negative configurations that share an outer edge
// #define CONSERVATIVE_CONFIGURATION_MATCHING

#define CHECK_CONFLICTS_WITH_OTHERS

//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class Contour {

  private:

    // workspace memory
    multimap< double, unsigned int >			_mmapPos;
    multimap< double, unsigned int >			_mmapNeg;
    // Kernel density estimation
    vector< double >					_density;
    // vector< double >					_proj;
    // vector< Vector2 >					_orient;
    
    Point2	_innerDiv	( const Point2 & P, const Point2 & Q, const double & t );
    void	_polyVertex	( const unsigned int & id, const Point2 & coord );
    double	_solveQuadratic	( const double & a, const double & b, const double & c );
    double	_Gaussian	( double x );
    

protected:

    bool			_isContractible;
    
    Polygon2			_polygon;
    vector< double >		_eangle;	// external angle
    vector< Turn >		_turn;		// 1 for convex; -1 for replex
    vector< Move >		_move;
    vector< vector< uint > >	_config;	// list of conflicting edges

    vector< double >		_distPos;
    vector< double >		_distNeg;
    vector< Segment2 >		_segPos;
    vector< Segment2 >		_segNeg;
    vector< double >		_areaPos;
    vector< double >		_areaNeg;
    
    uint			_idPri, _idSec;
    Scheme			_scheme;

#ifdef CHECK_CONFLICTS_WITH_OTHERS
    vector< Segment2 >		_obstacle;	// avoid edge-moves to conflict
						// with the obstacles
#endif	// CHECK_CONFLICTS_WITH_OTHERS
    
    void		_clear( void ) {
	_polygon.clear();
	_eangle.clear();
	_turn.clear();
	_move.clear();
	_config.clear();
	
	_distPos.clear();
	_distNeg.clear();
	_segPos.clear();
	_segNeg.clear();
	_areaPos.clear();
	_areaNeg.clear();
    }

    void	_initConfig( void );
    void	_incrementConfig( unsigned int configID );
    void	_decrementConfig( unsigned int configID );
    
    void	_load		( const char * filename );
    void	_save		( const char * filename );

    void	_reset		( void );
    void	_normalize	( void );

    void	_calc		( void );

    void	_arrange	( void );
    bool	_match		( void );

    bool	_prepare	( void ) {
	while ( _delIdentical() );
	while ( _delCollinear() );
	_calc();
	_arrange();
	return _match();
    }
    
    static bool _mycollinear	( const Point2 & p, const Point2 & q, const Point2 & r );
    bool	_delIdentical	( void );
    bool	_delCollinear	( void );

    bool	_contract	( double areaUpperLimit, int numLowerLimit );

    bool	_simplifyOnce	( void );
    bool	_simplifyByArea	( double areaUpperLimit );
    bool	_simplifyByNum	( int numLowerLimit );
    
    void	_registerConflicts	( const unsigned int & id,
					  const vector< Polygon2 > & conflict );
    void	_fullySimplify	( void );

    vector< double >	_voteAngles	( const double bandwidth );
    void		_alignEdges	( const vector< double > & proxy );
    
    
  public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    Contour();			// default constructor
    Contour( const Contour & obj );
				// copy constructor

//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
    ~Contour();			// destructor

//------------------------------------------------------------------------------
//	Referrring to members
//------------------------------------------------------------------------------
    const bool &	isContractible( void ) const	{ return _isContractible; };
    void setContractible( void )			{ _isContractible = true; };
    void clearContractible( void )			{ _isContractible = false; };

    const Polygon2 &	polygon( void ) const		{ return _polygon; }
    Polygon2 &		polygon( void )			{ return _polygon; }

    const vector< double > &	eangle( void ) const	{ return _eangle; }
    vector< double > &		eangle( void )		{ return _eangle; }

    const vector< Segment2 > &	segPos( void ) const	{ return _segPos; }
    vector< Segment2 > &	segPos( void )		{ return _segPos; }
    const vector< Segment2 > &	segNeg( void ) const	{ return _segNeg; }
    vector< Segment2 > &	segNeg( void )		{ return _segNeg; }

    const vector< double > &	areaPos( void ) const	{ return _areaPos; }
    vector< double > &		areaPos( void )		{ return _areaPos; }
    const vector< double > &	areaNeg( void ) const	{ return _areaNeg; }
    vector< double > &		areaNeg( void )		{ return _areaNeg; }

    const uint &		idPri( void ) const	{ return _idPri; }
    const uint &		idSec( void ) const	{ return _idSec; }
    const Scheme &		scheme( void ) const	{ return _scheme; }

#ifdef CHECK_CONFLICTS_WITH_OTHERS
    const vector< Segment2 > &	obstacle( void ) const	{ return _obstacle; }
    vector< Segment2 > &	obstacle( void ) 	{ return _obstacle; }
#endif	// CHECK_CONFLICTS_WITH_OTHERS
    

//------------------------------------------------------------------------------
//	Fundamental functions
//------------------------------------------------------------------------------
    void clear( void )					{ _clear(); }

    void load( const char * filename )			{ _load( filename ); }
    void save( const char * filename )			{ _save( filename ); }

    void reset( void )					{ _reset(); }
    
//------------------------------------------------------------------------------
//	Schematization
//------------------------------------------------------------------------------
    void calc		( void )			{ _calc(); }

    void arrange	( void )			{ _arrange(); }
    bool match		( void )			{ return _match(); }

    bool prepare	( void )			{ return _prepare(); }

    bool contract	( double areaUpperLimit = INFINITY, int numLowerLimit = 8 )	{
	return _contract( areaUpperLimit, numLowerLimit );
    }


    bool simplifyOnce	( void ) {
	return _simplifyOnce();
    }
    // iteratively schematize the polygon until it converges
    bool simplifyByArea	( double areaUpperLimit = 0.05 )	{
	return _simplifyByArea( areaUpperLimit );
    }
    bool simplifyByNum	( int numLowerLimit = 8 )	{
	return _simplifyByNum( numLowerLimit );
    }
    void registerConflicts	( const unsigned int & id,
				  const vector< Polygon2 > & conflict ) {
	_registerConflicts( id, conflict );
    }
    void fullySimplify	( void ) {
	_fullySimplify();
    }

    vector< double > squaring( const double bandwidth = 180.0/16.0 ) {
	vector< double > reps = _voteAngles( bandwidth );
	_alignEdges( reps );
	_isContractible = _prepare();
	return reps;
    }
    
//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------
    Contour & operator = ( const Contour & obj );
				// assignment

//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
    friend ostream & operator << ( ostream & stream, const Contour & obj );
				// output
    friend istream & operator >> ( istream & stream, Contour & obj );
				// input

//------------------------------------------------------------------------------
//	Class name
//------------------------------------------------------------------------------
    virtual const char * className( void ) const { return "Contour"; }
				// class name
};


#endif // _Contour_H

// end of header file
// Do not add any stuff under this line.
