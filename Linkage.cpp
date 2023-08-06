//==============================================================================
// Linkage.cpp
//	: program file for data likeage for hierarhical clustring
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-07-25 14:15:42 shigeo"
//
//==============================================================================

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include "Common.h"
#include "Linkage.h"


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Private Functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------

//
//  Linkage::_centroid --	dissimilarity based on the centroid linkage
//
//  Inputs
//	clusterS, clusterL : two clusters
//
//  Outputs
//	dissimilarity
//
double Linkage::_centroid( const Set & clusterS, const Set & clusterL )
{
    double aveS = 0.0, aveL = 0.0;

    for ( unsigned int i = 0; i < clusterS.size(); ++i ) 
	aveS += _expand[ clusterS[ i ] ].totalCost();
    aveS /= ( double )clusterS.size();
    
    for ( unsigned int j = 0; j < clusterL.size(); ++j )
	aveL += _expand[ clusterL[ j ] ].totalCost();
    aveL /= ( double )clusterL.size();

    return abs( aveS - aveL );
}

//
//  Linkage::_maximum --	dissimilarity based on the maximum linkage
//
//  Inputs
//	clusterS, clusterL : two clusters
//
//  Outputs
//	dissimilarity
//
double Linkage::_maximum( const Set & clusterS, const Set & clusterL )
{
    double maxDist = 0.0;

    for ( unsigned int i = 0; i < clusterS.size(); ++i ) 
	for ( unsigned int j = 0; j < clusterL.size(); ++j ) {
	    double curDist =
		abs( _expand[ clusterS[ i ] ].totalCost() -
		     _expand[ clusterL[ j ] ].totalCost() );
	    if ( curDist > maxDist )
		maxDist = curDist;
	}

    return maxDist;
}

//
//  Linkage::_minimum --	dissimilarity based on the minimum linkage
//
//  Inputs
//	clusterS, clusterL : two clusters
//
//  Outputs
//	dissimilarity
//
double Linkage::_minimum( const Set & clusterS, const Set & clusterL )
{
    double minDist = INFINITY;

    for ( unsigned int i = 0; i < clusterS.size(); ++i ) 
	for ( unsigned int j = 0; j < clusterL.size(); ++j ) {
	    double curDist =
		abs( _expand[ clusterS[ i ] ].totalCost() -
		     _expand[ clusterL[ j ] ].totalCost() );
	    if ( curDist < minDist )
		minDist = curDist;
	}

    return minDist;
}


//
//  Linkage::_dissim --	compute the dissimilarity between the data clusters
//
//  Inputs
//	type	 :	linkage type
//	idS, idL :	cluster IDs
//
//  Outputs
//	dissimilarity between the clusters
//
double Linkage::_dissim( LinkageType type, unsigned int idS, unsigned int idL )
{
    double dissim;
	
    switch ( type ) {
      case CENTROID:
	  dissim = _centroid	( _cluster[ idS ], _cluster[ idL ] );
	  break;
      case MAXIMUM:
	  dissim = _maximum	( _cluster[ idS ], _cluster[ idL ] );
	  break;
      case MINIMUM:
	  dissim = _minimum	( _cluster[ idS ], _cluster[ idL ] );
	  break;
      default:
	  dissim = 0.0;
	  cerr << HERE << " Illegal case" << endl;
	  break;
    }

    return dissim;
}

//
//  Linkage::_initVars --	initialize the data matrix
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Linkage::_initVars( void )
{
    _matID.clear();
    _cluster.clear();
    _lower.clear();
    _upper.clear();
    for ( unsigned int i = 0; i < _expand.size(); ++i ) {
	_matID.push_back( i );
	Set member;
	member.push_back( i );
	_cluster.push_back( member );
	_lower.push_back( 0.0 );
	_upper.push_back( INFINITY );
    }
    _merge.clear();

}

//
//  Linkage::_initMat --	initialize the dissimilarity matrix
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Linkage::_initMat( void )
{
    // cerr << HERE << " matID : ";
    // for ( unsigned int i = 0; i < _matID.size(); ++i )
    // cerr << _matID[ i ] << " ";
    // cerr << endl;

    _distMat.clear();
    _distMat.resize( _matID.size() );
    for ( unsigned int i = 0; i < _matID.size(); ++i ) {
	_distMat[ i ].resize( _matID.size() );
	for ( unsigned int j = i+1; j < _matID.size(); ++j ) {
	    _distMat[ i ][ j ] = _dissim( _type, _matID[ i ], _matID[ j ] );
#ifdef SKIP
	    cerr << HERE << " mat[ " << _matID[ i ] << " ][ " << _matID[ j ] << " ] = "
		 << _distMat[ i ][ j ] << endl;
#endif	// SKIP
	}
    }
}


//
//  Linkage::_onestep --	conduct one hierarhical clustering
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Linkage::_onestep( void )
{
    int idS, idL;
    double minSim = INFINITY;
    
    for ( unsigned int i = 0; i < _matID.size(); ++i ) {
	for ( unsigned int j = i+1; j < _matID.size(); ++j ) {
	    if ( _distMat[ i ][ j ] < minSim ) {
		minSim = _distMat[ i ][ j ];
		idS = _matID[ i ];
		idL = _matID[ j ];
	    }
	}
    }

    // merge two clusters
    Join newjoin;
    newjoin.idS = idS;
    newjoin.idL = idL;
    newjoin.idM = _cluster.size();
    newjoin.threshold = minSim;
    _merge.push_back( newjoin );
    _upper[ idS ] = minSim;
    _upper[ idL ] = minSim;
    
    
    Set newset = _cluster[ idS ];
    newset.insert( newset.end(), _cluster[ idL ].begin(), _cluster[ idL ].end() );
    sort( newset.begin(), newset.end() );
    _cluster.push_back( newset );
    _lower.push_back( minSim );
    _upper.push_back( INFINITY );
    
    // イテレート中に要素削除をするような場合には、
    // 範囲for文は使用できない
    for (auto it = _matID.begin(); it != _matID.end(); ) {
	// 条件一致した要素を削除する
	if ( *it == idS ) {
	    // 削除された要素の次を指すイテレータが返される。
	    it = _matID.erase( it );
	}
	else if ( *it == idL ) {
	    // 削除された要素の次を指すイテレータが返される。
	    it = _matID.erase( it );
	}
	// 要素削除をしない場合に、イテレータを進める
	else {
	    ++it;
	}
    }

    _matID.push_back( newjoin.idM );
    
    // cerr << HERE << " matID : ";
    // for ( unsigned int i = 0; i < _matID.size(); ++i )
    // cerr << _matID[ i ] << " ";
    // cerr << endl;

#ifdef SKIP
    cerr << HERE << " cluster : " << endl;
    for ( unsigned int i = 0; i < _cluster.size(); ++i ) {
	cerr << " cluster[ " << i << " ] : [ " << _lower[ i ]
	     << ", " << _upper[ i ] << " ] => ";
	for ( unsigned int j = 0; j < _cluster[ i ].size(); ++j )
	    cerr << _cluster[ i ][ j ] << " ";
	cerr << endl;
    }
#endif	// SKIP

    _initMat();
}

//
//  Linkage::_retrieve --	retrieve the clusters of elements specified by
//				the given threshold
//
//  Inputs
//	threshold :	threshold to cut across the dendrogram of hierarchical
//			clustering 
//
//  Outputs
//	sets of elements
//
vector< Set > Linkage::_retrieve( double threshold )
{
    vector< Set > result;

    assert( _cluster.size() == _lower.size() );
    assert( _cluster.size() == _upper.size() );
    for ( unsigned int i = 0; i < _cluster.size(); ++i ) {
	if ( ( _lower[ i ] <= threshold ) && ( threshold < _upper[ i ] ) ) {
	    result.push_back( _cluster[ i ] );
	}
    }
    
    sort( result.begin(), result.end() );
    return result;
}



//------------------------------------------------------------------------------
//	Public Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------

//
//  Linkage::Linkage --	default constructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Linkage::Linkage() 
{
    _matID.clear();
    _distMat.clear();
    _expand.clear();
    _type = CENTROID;
    _merge.clear();
    _cluster.clear();
}


//
//  Linkage::Linkage --	copy constructor
//
//  Inputs
//	obj	: object of this class
//
//  Outputs
//	none
//
Linkage::Linkage( const Linkage & obj )
{
    _matID	= obj._matID;
    _distMat	= obj._distMat;
    _expand	= obj._expand;
    _type	= obj._type;
    _merge	= obj._merge;
    _cluster	= obj._cluster;
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------

//
//  Linkage::~Linkage --	destructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Linkage::~Linkage()
{

}


//------------------------------------------------------------------------------
//	Referrring to members
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------

//
//  Linkage::operator = --	assignement
//
//  Inputs
//	obj	: objects of this class
//
//  Outputs
//	this object
//
Linkage & Linkage::operator = ( const Linkage & obj )
{
    if ( this != &obj ) {
	_matID		= obj._matID;
	_distMat	= obj._distMat;
	_expand		= obj._expand;
	_type		= obj._type;
	_merge		= obj._merge;
	_cluster	= obj._cluster;
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
ostream & operator << ( ostream & stream, const Linkage & obj )
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
istream & operator >> ( istream & stream, Linkage & obj )
{
    return stream;
}


// end of header file
// Do not add any stuff under this line.
