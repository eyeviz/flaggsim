//==============================================================================
// Drawing.cpp
//	: program file for line drawings
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-10-21 14:58:10 shigeo"
//
//==============================================================================

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include "Drawing.h"
#include "Spatial.h"
#include "graphcut.h"
#include "Dijkstra.h"
#include "Triangulate.h"
#include "Contour.h"
#include "Votes.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------
// for dijkstra search in beta-skeleton
// #define CONSTANT_EDGE_WEIGHTS



//------------------------------------------------------------------------------
//	Variables
//------------------------------------------------------------------------------
unsigned int	Drawing::num_trials			= NUM_TRIALS;
double		Drawing::interval_threshold		= LIMIT_BUILDING_INTERVAL;
double		Drawing::proximity_upper		= PROXIMITY_UPPER;
#ifdef USING_SIMILARITY_CONJOINING
double		Drawing::similarity_lower		= SIMILARITY_LOWER;
#endif	// USING_SIMILARITY_CONJOINING
double		Drawing::proximity_ratio		= PROXIMITY_RATIO;
#ifdef USING_SIMILARITY_CONJOINING
double		Drawing::similarity_ratio		= SIMILARITY_RATIO;
#endif	// USING_SIMILARITY_CONJOINING

double		Drawing::data_cost_lower		= DATA_COST_LOWER;
double		Drawing::data_cost_upper		= DATA_COST_UPPER;
double		Drawing::data_cost_inside		= DATA_COST_INSIDE;

double		Drawing::smooth_cost_lower		= SMOOTH_COST_LOWER;
double		Drawing::smooth_cost_upper		= SMOOTH_COST_UPPER;

double		Drawing::label_cost_lower		= LABEL_COST_LOWER;
double		Drawing::label_cost_upper		= LABEL_COST_UPPER;
double		Drawing::label_cost_single		= LABEL_COST_SINGLE;

void		(*Drawing::redraw)( void )		= NULL;


//------------------------------------------------------------------------------
//	Private Functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------

//
//  Drawing::_maxMinVSets --	return the distance between two polygons p and q
//
//  Inputs
//	p, q	: polygons
//
//  Outputs
//	distance
//
double Drawing::_maxMinVSets( const Polygon2 & p, const Polygon2 & q )
{
    double maxDoubleI = 0.0;
    for ( unsigned int i = 0; i < p.size(); ++i ) {
	double minDoubleJ = 1.0e+8;
	for ( unsigned int j = 0; j < q.size(); ++j ) {
	    double curDouble = ( p[ i ] - q[ j ] ).squared_length();
	    // cerr << " i = " << i << " j = " << j << " D = " << curDouble << endl;
	    if ( curDouble < minDoubleJ ) {
		minDoubleJ = curDouble;
		// cerr << " i = " << i << " j = " << j << " minDJ = " << minDoubleJ << endl;
	    }
	}
	if ( minDoubleJ > maxDoubleI ) {
	    maxDoubleI = minDoubleJ;
	    // cerr << " i = " << i << " maxDI = " << maxDoubleI << endl;
	}
    }
    // cerr << " FINAL maxDI = " << maxDoubleI << endl;
    return sqrt( maxDoubleI );
}


//
//  Drawing::_minMinVSets --	return the distance between two polygons p and q
//
//  Inputs
//	p, q	: polygons
//
//  Outputs
//	distance
//
double Drawing::_minMinVSets( const Polygon2 & p, const Polygon2 & q )
{
    double minDoubleI = 1.0e+8;
    for ( unsigned int i = 0; i < p.size(); ++i ) {
	double minDoubleJ = 1.0e+8;
	for ( unsigned int j = 0; j < q.size(); ++j ) {
	    double curDouble = ( p[ i ] - q[ j ] ).squared_length();
	    // cerr << " i = " << i << " j = " << j << " D = " << curDouble << endl;
	    if ( curDouble < minDoubleJ ) {
		minDoubleJ = curDouble;
		// cerr << " i = " << i << " j = " << j << " minDJ = " << minDoubleJ << endl;
	    }
	}
	if ( minDoubleJ < minDoubleI ) {
	    minDoubleI = minDoubleJ;
	    // cerr << " i = " << i << " maxDI = " << maxDoubleI << endl;
	}
    }
    // cerr << " FINAL maxDI = " << maxDoubleI << endl;
    return sqrt( minDoubleI );
}


//
//  Drawing::_aspectRatio --	return the aspect ratio of the rectangle
//				(i.e., ratio of its longer side to its shorter side)
//  Inputs
//	a, b	: two sides of the rectangular bouding box
//
//  Outputs
//	distance
//
double Drawing::_aspectRatio( const double & a, const double & b )
{
    if ( a < b ) return (a/b);
    else return (b/a);
}


#ifdef USING_SIMILARITY_CONJOINING
//
//  Drawing::_simBbox --	return the simimlarity between two bounding boxes
//
//  Inputs
//	p, q	: two boundaing boxes of polygons
//
//  Outputs
//	similarity
//
double Drawing::_simBbox( const Bbox2 & p, const Bbox2 & q )
{
    double Wp = p.xmax() - p.xmin();
    double Hp = p.ymax() - p.ymin();
    double Wq = q.xmax() - q.xmin();
    double Hq = q.ymax() - q.ymin();
    double sim = ( _aspectRatio( Wp, Wq ) + _aspectRatio( Hp, Hq ) )/2.0;
    return sim;
}
#endif	// USING_SIMILARITY_CONJOINING


//
//  Drawing::_normalizeVector --	normalize the vector of costs by referring to
//					the maximum and minimum component values
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Drawing::_normalizeVector( vector< double > & cost,
				const double lowerLimit, const double upperLimit )
{
    double cMax = 0.0, cMin = INFINITY;
    for ( unsigned int i = 0; i < cost.size(); ++i ) {
	if ( cost[ i ] > cMax ) cMax = cost[ i ];
	if ( cost[ i ] < cMin ) cMin = cost[ i ];
    }
    if ( cMax == cMin ) {
	for ( unsigned int i = 0; i < cost.size(); ++i ) {
	    cost[ i ] = 0.50;
	}
    }
    else {
	for ( unsigned int i = 0; i < cost.size(); ++i ) {
	    double r = ( cost[ i ] - cMin ) / ( cMax - cMin );
	    cost[ i ] = ( 1.0 - r ) * lowerLimit + r * upperLimit;
	    // cerr << HERE << " i = " << i << " normalized cost = " << nm << endl;
	}
    }
}


//
//  Drawing::_normalizeMatrix --	normalize the matrix of costs by referring to
//					the maximum and minimum component values
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Drawing::_normalizeMatrix( vector< vector< double > > & cost,
				const double lowerLimit, const double upperLimit )
{
    double cMax = 0.0, cMin = INFINITY;
    for ( unsigned int i = 0; i < cost.size(); ++i ) {
	for ( unsigned int j = 0; j < cost[ i ].size(); ++j ) {
	    if ( cost[ i ][ j ] > cMax ) cMax = cost[ i ][ j ];
	    if ( cost[ i ][ j ] < cMin ) cMin = cost[ i ][ j ];

	}
    }
    if ( cMax == cMin ) {
	for ( unsigned int i = 0; i < cost.size(); ++i ) {
	    for ( unsigned int j = 0; j < cost[ i ].size(); ++j ) {
		cost[ i ][ j ] = 0.50;
	    }
	}
    }
    else {
	for ( unsigned int i = 0; i < cost.size(); ++i ) {
	    for ( unsigned int j = 0; j < cost[ i ].size(); ++j ) {
		double r = ( cost[ i ][ j ] - cMin ) / ( cMax - cMin );
		cost[ i ][ j ] = ( 1.0 - r ) * lowerLimit + r * upperLimit;
	    }
	}
    }
}


//
//  Drawing::_triangulate --	triangulate each polygon
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Drawing::_triangulate( void )
{
//------------------------------------------------------------------------------
//	Use _bound instead of _poly
//------------------------------------------------------------------------------
    _tri.clear();
    for ( unsigned int i = 0; i < _bound.size(); ++i ) {
	Polygon2 & each = _bound[ i ];
	Polygon2 seq;
	Set index;
	Triangulate::process( each, seq, index );
	
	vector< Triangle2 > cluster;
	for ( unsigned int k = 0; k < seq.size(); k += 3 ) {
	    Triangle2 one( seq[ k ], seq[ k+1 ], seq[ k+2 ] );
	    cluster.push_back( one );
	}
	_tri.push_back( cluster );
    }
}

//
//  Drawing::_updateEdges --	update the edge attributes according to the
//				network topology
//
//  Inputs
//	net : graph
//
//  Outputs
//	none
//
void Drawing::_updateEdges( Network & net )
{
    NetEdgeWeightMap		edgeWeight	= get( edge_weight, net );
    NetEdgeNDstMap		edgeNDst	= get( edge_myndst, net );
    NetEdgeNInvMap		edgeNInv	= get( edge_myninv, net );
#ifdef USING_SIMILARITY_CONJOINING
    NetEdgeRatioMap		edgeRatio	= get( edge_myratio, net );
    NetEdgeNAspMap		edgeNAsp	= get( edge_mynasp, net );
#endif	// USING_SIMILARITY_CONJOINING

//------------------------------------------------------------------------------
//	Supplementary computations for edges
//------------------------------------------------------------------------------
    double maxD = -INFINITY, minD = INFINITY;
#ifdef USING_SIMILARITY_CONJOINING
    double maxR = -INFINITY, minR = INFINITY; 
#endif	// USING_SIMILARITY_CONJOINING
    BGL_FORALL_EDGES( ed, net, Network ) {
	double dist = edgeWeight[ ed ];
	if ( dist < minD ) minD = dist;
	if ( dist > maxD ) maxD = dist;
#ifdef USING_SIMILARITY_CONJOINING
	double ratio = edgeRatio[ ed ];
	if ( ratio < minR ) minR = ratio;
	if ( ratio > maxR ) maxR = ratio;
#endif	// USING_SIMILARITY_CONJOINING
    }
#ifdef DEBUG
    cerr << HERE << " minD = " << minD << " maxD = " << maxD << endl;
    cerr << HERE << " minR = " << minR << " maxR = " << maxR << endl;
#endif	// DEBUG

    // avoid division by zero
    if ( maxD - minD < 1.0e-8 ) maxD = minD + 1.0e-4;
#ifdef USING_SIMILARITY_CONJOINING
    if ( maxR - minR < 1.0e-8 ) maxR = minR + 1.0e-4;
#endif	// USING_SIMILARITY_CONJOINING
    // normalizing the attribute values
    double maxI = 1.0/minD, minI = 1.0/maxD;
    BGL_FORALL_EDGES( ed, net, Network ) {
	double dist = edgeWeight[ ed ];
	edgeNDst[ ed ] = ( dist - minD ) / ( maxD - minD );
	double invDist = 1.0 / dist;
	edgeNInv[ ed ] = ( invDist - minI ) / ( maxI - minI );
#ifdef USING_SIMILARITY_CONJOINING
	double ratio = edgeRatio[ ed ];
	edgeNAsp[ ed ] = ( ratio - minR ) / ( maxR - minR );
#endif	// USING_SIMILARITY_CONJOINING
    }

    cerr << HERE << " calculated the k-neighbor graph" << endl;

    //------------------------------------------------------------------------------
    //	Print out the list of neighbor vertices
    //------------------------------------------------------------------------------
#ifdef PRINT_COSTS
    cerr << HERE << " Printing out the adjacency between polygons: " << endl;
    BGL_FORALL_EDGES( ed, net, Network ) {
	VertexDescriptor vdS = source( ed, net );
	VertexDescriptor vdT = target( ed, net );
	unsigned int idS = vertexID[ vdS ];
	unsigned int idT = vertexID[ vdT ];
	double weight = edgeNInv[ ed ];
	cerr << idS << "\t" << idT << "\t" << weight << endl;
    }
#endif	// PRINT_COSTS
}

#ifdef OBSOLETE
//
//  Drawing::_calcNeighbor --	calculate the k-neighbor graph over the set of polygons
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Drawing::_calcNeighbor( void )
{
    // Drawing &		fig		= *this;

    Network &		net		= _netNbr;

    NetVertexIDMap	vertexID        = get( vertex_myid, net );
    NetVertexPolyMap	vertexPoly	= get( vertex_mypoly, net );
    NetVertexGlobMap	vertexGlob	= get( vertex_myglob, net );
    NetVertexCntrMap	vertexCntr	= get( vertex_mycntr, net );

    NetEdgeIDMap	edgeID		= get( edge_myid, net );
    NetEdgeWeightMap	edgeWeight	= get( edge_weight, net );
#ifdef USING_SIMILARITY_CONJOINING
    NetEdgeRatioMap	edgeRatio	= get( edge_myratio, net );
#endif	// USING_SIMILARITY_CONJOINING
    
    net.clear();
    for ( unsigned int i = 0; i < _poly.size(); ++i ) {
	// cerr << HERE << " Vertex " << i << endl;
	NetVertexDescriptor vd = add_vertex( net );
	vertexID[ vd ] = i;
	Polygon2 & curPoly = _poly[ i ];
	Set & curGlob = _glID[ i ];
	vertexPoly[ vd ] = curPoly;
	vertexGlob[ vd ] = curGlob;
	Vector2 sum( 0.0, 0.0 );
	for ( unsigned int k = 0; k < curPoly.size(); ++k )
	    sum += curPoly[ k ] - CGAL::ORIGIN;
	vertexCntr[ vd ] = CGAL::ORIGIN + sum / (double)curPoly.size();
    }

    unsigned int nV = num_vertices( net );
    cerr << HERE << " Number of vertices = " << nV << endl;
    
    unsigned idE = 0;
    for ( unsigned int i = 0; i < nV; ++i ) {

	// sorted list
	map< double, unsigned int > nbr;

	for ( unsigned int j = 0; j < nV; ++j ) {
	    if ( i != j )  {
		// cerr << HERE << " Edge distance evaluation " << i << " == " << j << endl;
		double curDist = _distPolys( i, j );
		// cerr << HERE << " curDist = " << curDist << endl;
		nbr.insert( std::make_pair( curDist, j ) );
	    }
	}
#ifdef DEBUG
	for ( map< double, unsigned int >::iterator iter = nbr.begin();
	      iter != nbr.end(); iter++ ) {
	    cerr << " iter->second = " << iter->second
		 << " dist = " << iter->first << endl;
	}
#endif	// DEBUG
	unsigned int kN = std::min( (unsigned int)K_NEIGHBOR, nV - 1 );
	for ( map< double, unsigned int >::iterator iter = nbr.begin();
	      std::distance( nbr.begin(), iter ) < kN; iter++ ) {
	    // cerr << HERE << " iter->second = " << iter->second
	    // << " dist = " << iter->first << endl;
	    unsigned int idS = std::min( i, iter->second );
	    unsigned int idL = std::max( i, iter->second );
	    // cerr << HERE << " idS = " << idS << " idL = " << idL << endl;
	    NetVertexDescriptor vdI = vertex( idS, net );
	    NetVertexDescriptor vdJ = vertex( idL, net );
	    if ( ! isConnected( vdI, vdJ, net ) ) {
		NetEdgeDescriptor ed;
		bool ok;
		tie( ed, ok ) = add_edge( vdI, vdJ, net );
		assert( ok );
		// cerr << " New edge ID = " << idE << endl;
		edgeID[ ed ] = idE++;
		// cerr << HERE << " iter->first = " << iter->first << endl;
		edgeWeight[ ed ] = iter->first;
#ifdef USING_SIMILARITY_CONJOINING
		// aspect ratio
		edgeRatio[ ed ] = _simPolys( idS, idL );
#endif	// USING_SIMILARITY_CONJOINING
	    }
	    else {
		// cerr << " The identical edge already exists" << endl;
	    }
	}
	// cerr << HERE << "==============================" << endl;
    }

    cerr << HERE << " Supplementary computations for edges " << endl;
    
//------------------------------------------------------------------------------
//	Supplementary computations for edges
//------------------------------------------------------------------------------
    _updateEdges( net );
}
#endif	// OBSOLETE


//
//  Drawing::_wrapBSkeleton --	calculate the neighbor graph over the set of
//				polygons by wraping the polygons with
//				Beta-skeleton
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Drawing::_wrapBSkeleton( void )
{
    const string title = "Drawing::_wrapBSkeleton";
    // Drawing &		fig		= *this;

//------------------------------------------------------------------------------
//	Wrapping polygons with a Beta-skeleton
//------------------------------------------------------------------------------
    vector< vector< double > > corner;
    _assoc.clear();
    corner.clear();
    
    // Store the tuple (i,j) for each point into the variable _assoc,
    // if the point is the j-th sample on the i-th polygon
    // The variable corner retains the coordinates of each sample in order
    for ( unsigned int i = 0; i < _poly.size(); ++i ) {
	for ( unsigned int j = 0; j < _poly[ i ].size(); ++j ) {
	    Tuple tuple( i, j );
	    vector< double > coord;
	    coord.push_back( _poly[ i ][ j ].x() );
	    coord.push_back( _poly[ i ][ j ].y() );
	    // add the tuple ( polygonID, cornerID )
	    _assoc.push_back( tuple );
	    corner.push_back( coord );
	}
    }

    // The variable _web represents the beta-skeleton
    _web.clear();
    // Calculate the Beta-skeleton with the parameter 1.2
    geometricNeighbors( 2, corner, BSkeleton, 1.2, _web );
    // geometricNeighbors( 2, corner, BSkeleton, 0.95, _web );

//------------------------------------------------------------------------------
//	Composing the proximity graph by referring to the beta-skeleton that
//	wraps the set of building polygons
//------------------------------------------------------------------------------
    // proximity graph for polygon aggregation search
    Network &		net		= _netNbr;

    NetVertexIDMap	vertexID        = get( vertex_myid, net );
    NetVertexPolyMap	vertexPoly	= get( vertex_mypoly, net );
    NetVertexGlobMap	vertexGlob	= get( vertex_myglob, net );
    NetVertexCntrMap	vertexCntr	= get( vertex_mycntr, net );

    NetEdgeIDMap	edgeID		= get( edge_myid, net );
    NetEdgeWeightMap	edgeWeight	= get( edge_weight, net );
#ifdef USING_SIMILARITY_CONJOINING
    NetEdgeRatioMap	edgeRatio	= get( edge_myratio, net );
#endif	// USING_SIMILARITY_CONJOINING
    
    // initialize the proximity graph
    net.clear();
    // each polygon corresponds to a single node in the proximity graph
    for ( unsigned int i = 0; i < _poly.size(); ++i ) {
	// cerr << HERE << " Vertex " << i << endl;
	NetVertexDescriptor vd = add_vertex( net );
	// Set the the polygon ID as the vertexID
	vertexID[ vd ] = i;
	Polygon2 & curPoly = _poly[ i ];
	Set & curGlob = _glID[ i ];
	// Set the polygon shape as the node attribure
	vertexPoly[ vd ] = curPoly;
	// Set the global sample ID as the node attribure
	vertexGlob[ vd ] = curGlob;
	// Set the barycenter of the polygon as the center coordinate
	Vector2 sum( 0.0, 0.0 );
	for ( unsigned int k = 0; k < curPoly.size(); ++k )
	    sum += curPoly[ k ] - CGAL::ORIGIN;
	vertexCntr[ vd ] = CGAL::ORIGIN + sum / (double)curPoly.size();
    }

    unsigned int nV = num_vertices( net );
    cerr << HERE << title << " Number of vertices = " << nV << endl;
    
//------------------------------------------------------------------------------
//	Add the edges of beta-skelton to the proximity network
//	if they connect different polygons  
//------------------------------------------------------------------------------
    unsigned idE = 0;
    for ( unsigned int i = 0; i < _web.size(); ++i ) {
	// for each edges in the beta-skeleton,
	// retrieve the indices of endpoints.
	unsigned int indexS = _web[ i ].first;
	unsigned int indexT = _web[ i ].second;
	// Obtain the tuple of each endpoint
	Tuple & ptrS = _assoc[ indexS ];
	Tuple & ptrT = _assoc[ indexT ];
	// Refer to the positions of the endpoint samples in the beta skeleton
	Point2 & coordS = _poly[ ptrS.first ][ ptrS.second ];
	Point2 & coordT = _poly[ ptrT.first ][ ptrT.second ];
	// If the two endpoints belong to the same polygon, go out.
	if ( ptrS.first == ptrT.first ) continue;

//------------------------------------------------------------------------------
//	New criteria for composing the proximity graph among building
//	polygons by ST on 2023/05/16
//	Check whether the current beta skeleton edge connects two buidling
//	polygons within the specific distance
//------------------------------------------------------------------------------
	double linkLength = sqrt( ( coordT - coordS ).squared_length() );
	// If the link length between two buildings is larger than theshold,
	// just skip
	// cerr << HERE << title << " interval between buildings : " << linkLength << endl;
	// breakpoint();
	if ( linkLength > Drawing::interval_threshold ) continue;
	
	unsigned int idPolyE, idPolyL;
	Polygon2 polyE, polyL;
	// if the source polygon has an early ID then the target polygon
	if ( ptrS.first < ptrT.first ) {
	    idPolyE = ptrS.first;
	    polyE = _poly[ ptrS.first ];
	    idPolyL = ptrT.first;
	    polyL = _poly[ ptrT.first ];
	}
	// otherwise
	else {
	    idPolyE = ptrT.first;
	    polyE = _poly[ ptrT.first ];
	    idPolyL = ptrS.first;
	    polyL = _poly[ ptrS.first ];
	}
	    
	// identify the polygon vertex of the earlier ID
	NetVertexDescriptor vdPolyE = vertex( idPolyE, net );
	// identify the polygon vertex of the later ID
	NetVertexDescriptor vdPolyL = vertex( idPolyL, net );
	// cerr << HERE << " idS = " << idS << " idL = " << idL << endl;

	// if the two polygons corresponding to the endpoints of a skelton edge
	// are not yet connected,
	if ( ! isConnected( vdPolyE, vdPolyL, net ) ) {
	    NetEdgeDescriptor ed;
	    bool ok;
	    tie( ed, ok ) = add_edge( vdPolyE, vdPolyL, net );
	    assert( ok );
	    cerr << HERE << " New edge ID = " << idE << " : "
		 << " adding Edge " << idPolyE << " ==> " << idPolyL << endl;
	    edgeID[ ed ] = idE++;
	    edgeWeight[ ed ] = _distPolys( idPolyE, idPolyL );
#ifdef USING_SIMILARITY_CONJOINING
	    // aspect ratio
	    edgeRatio[ ed ] = _simPolys( idPolyE, idPolyL );
#endif	// USING_SIMILARITY_CONJOINING
	}
	else {
	    // cerr << HERE << " Illegal case: The identical edge already exists" << endl;
	    // cerr << HERE << " Edge : " << idS << " == " << idL << endl;
	    // exit( 0 );
	}
    }

    cerr << HERE << " Supplementary computations for edges " << endl;
    
//------------------------------------------------------------------------------
//	Supplementary computations for edges
//------------------------------------------------------------------------------
    _updateEdges( net );
}

//
//  Drawing::_calcWrapper --	calculate the network that wraps the corner vertices
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Drawing::_calcWrapper( void )
{
    Directed &		dig		= _wrapper;

    DirVertexAddrMap	vertexAddr      = get( vertex_myaddr, dig );
    DirVertexPointMap	vertexPoint	= get( vertex_mypoint, dig ); // vertex coordinates
    DirVertexGroupMap	vertexGroup	= get( vertex_mygroup, dig ); // polygon group ID
    DirVertexCodeMap	vertexCode	= get( vertex_mycode, dig ); // global ID

    DirEdgeAddrMap	edgeAddr	= get( edge_myaddr, dig );
    DirEdgeWeightMap	edgeWeight	= get( edge_weight, dig );

#ifdef CONSTANT_EDGE_WEIGHTS
    const double	wEdge		= 1.0;
    const double	wBeta		= 100.0;
#endif	// CONSTANT_EDGE_WEIGHTS

    cerr << HERE << "Start to calculate the wrapper network" << endl; 
    
    dig.clear();
    _onelayer.clear();
    _twolayer.clear();

    // Insert corner vertices
    unsigned int vertexCounter = 0;
    for ( unsigned int i = 0; i < _poly.size(); ++i ) {
	vector< NetVertexDescriptor > eachLayer;
#ifdef SKIP
	cerr << HERE << " Global IDs : i = " << i << _glID[ i ];
	getchar();
#endif	// SKIP
	for ( unsigned int j = 0; j < _poly[ i ].size(); ++j ) {
	    NetVertexDescriptor vd = add_vertex( dig );
	    _onelayer.push_back( vd );
	    eachLayer.push_back( vd );
	    vertexAddr[ vd ]	= vertexCounter++;
	    vertexPoint[ vd ]	= _poly[ i ][ j ];
	    vertexCode[ vd ]	= _glID[ i ][ j ];
	    vertexGroup[ vd ]	= i;
	}
	_twolayer.push_back( eachLayer );
    }
    
    // Insert original edges that constitute building boundaries
    unsigned int edgeCounter = 0;
    for ( unsigned int i = 0; i < _poly.size(); ++i ) {
	unsigned int sz = _poly[ i ].size();
	for ( unsigned int j = 0; j < _poly[ i ].size(); ++j ) {
	    DirVertexDescriptor vdI = _twolayer[ i ][ j ];
	    DirVertexDescriptor vdJ = _twolayer[ i ][ (j+1)%sz ];
	    DirEdgeDescriptor ed;
	    bool ok;
	    tie( ed, ok ) = add_edge( vdI, vdJ, dig );
	    assert( ok );
	    edgeAddr[ ed ] = edgeCounter++;
#ifdef CONSTANT_EDGE_WEIGHTS
	    edgeWeight[ ed ] = wEdge;
#else	// CONSTANT_EDGE_WEIGHTS
	    edgeWeight[ ed ] = sqrt( ( vertexPoint[ vdJ ] - vertexPoint[ vdI ] ).squared_length() );
#endif	// CONSTANT_EDGE_WEIGHTS
	}
    }

#ifdef DEBUG
    BGL_FORALL_EDGES( ed, dig, Directed ) {
	DirVertexDescriptor vdS = source( ed, dig );
	DirVertexDescriptor vdT = target( ed, dig );
	cerr << HERE << " edge addr = " << edgeAddr[ ed ]
	     << " : " << vertexAddr[ vdS ] << " -- "
	     << vertexAddr[ vdT ] << endl;
    }
#endif	// DEBUG
    
    // Insert edges of the beta-skeleton that bridges different building polygons
    // unsigned idE = 0;
    for ( unsigned int i = 0; i < _web.size(); ++i ) {
	unsigned int indexS = _web[ i ].first;
	unsigned int indexT = _web[ i ].second;
	Tuple & ptrS = _assoc[ indexS ];
	Tuple & ptrT = _assoc[ indexT ];
	if ( ptrS.first == ptrT.first ) continue;

	DirVertexDescriptor vdS = _twolayer[ ptrS.first ][ ptrS.second ];
	DirVertexDescriptor vdT = _twolayer[ ptrT.first ][ ptrT.second ];

	// forward directed edge
	if ( ! isConnected( vdS, vdT, dig ) ) {
	    DirEdgeDescriptor ed;
	    bool ok;
	    tie( ed, ok ) = add_edge( vdS, vdT, dig );
	    assert( ok );
	    edgeAddr[ ed ] = edgeCounter++;
#ifdef CONSTANT_EDGE_WEIGHTS
	    edgeWeight[ ed ] = wBeta;
#else	// CONSTANT_EDGE_WEIGHTS
	    edgeWeight[ ed ] = sqrt( ( vertexPoint[ vdT ] - vertexPoint[ vdS ] ).squared_length() );
#endif	// CONSTANT_EDGE_WEIGHTS
#ifdef DEBUG
	    cerr << HERE << "[F] adding Edge : " << vertexAddr[ vdS ]
		 << " == " << vertexAddr[ vdT ] << endl;
#endif	// DEBUG
	}
	// backward directed edge
	if ( ! isConnected( vdT, vdS, dig ) ) {
	    DirEdgeDescriptor ed;
	    bool ok;
	    tie( ed, ok ) = add_edge( vdT, vdS, dig );
	    assert( ok );
	    edgeAddr[ ed ] = edgeCounter++;
#ifdef CONSTANT_EDGE_WEIGHTS
	    edgeWeight[ ed ] = wBeta;
#else	// CONSTANT_EDGE_WEIGHTS
	    edgeWeight[ ed ] = sqrt( ( vertexPoint[ vdS ] - vertexPoint[ vdT ] ).squared_length() );
#endif	// CONSTANT_EDGE_WEIGHTS
#ifdef DEBUG
	    cerr << HERE << "[S] adding Edge : " << vertexAddr[ vdT ]
		 << " == " << vertexAddr[ vdS ] << endl;
#endif	// DEBUG
	}
    }

#ifdef DEBUG
    BGL_FORALL_EDGES( ed, dig, Directed ) {
	DirVertexDescriptor vdS = source( ed, dig );
	DirVertexDescriptor vdT = target( ed, dig );
	cerr << HERE << " edge addr = " << edgeAddr[ ed ]
	     << " : " << vertexAddr[ vdS ] << " -- "
	     << vertexAddr[ vdT ] << endl;
    }
    getchar();
#endif	// DEBUG
}


//
//  Drawing::_calcNewProximity --	employ the new approach for
//					calculating the graph for proximity
//					labels
//
//  Inputs
//	ratio :	ratio (in [0.0, 1.0]) control the degree of proximity over the
//		beta-skeleton 
//
//  Outputs
//	none
//
void Drawing::_calcNewProximity( const double & ratio )
{
    Network &		nbr		= _netNbr;
    Network &		prx		= _netPrx;

    NetVertexIDMap	vertexIDN	= get( vertex_myid, nbr );
    NetVertexCntrMap	vertexCntrN	= get( vertex_mycntr, nbr );
    NetVertexPolyMap	vertexPolyN	= get( vertex_mypoly, nbr );
    NetEdgeIDMap	edgeIDN		= get( edge_myid, nbr );
    NetEdgeWeightMap	edgeWeightN	= get( edge_weight, nbr );
    NetEdgeNDstMap	edgeNDstN	= get( edge_myndst, nbr );
    NetEdgeNInvMap	edgeNInvN	= get( edge_myninv, nbr );
#ifdef USING_SIMILARITY_CONJOINING
    NetEdgeRatioMap	edgeRatioN	= get( edge_myratio, nbr );
    NetEdgeNAspMap	edgeNAspN	= get( edge_mynasp, nbr );
#endif	// USING_SIMILARITY_CONJOINING

    NetVertexIDMap	vertexIDP	= get( vertex_myid, prx );
    NetVertexCntrMap	vertexCntrP	= get( vertex_mycntr, prx );
    NetVertexPolyMap	vertexPolyP	= get( vertex_mypoly, prx );
    NetEdgeIDMap	edgeIDP		= get( edge_myid, prx );
    NetEdgeWeightMap	edgeWeightP	= get( edge_weight, prx );
    NetEdgeNDstMap	edgeNDstP	= get( edge_myndst, prx );
    NetEdgeNInvMap	edgeNInvP	= get( edge_myninv, prx );
#ifdef USING_SIMILARITY_CONJOINING
    NetEdgeRatioMap	edgeRatioP	= get( edge_myratio, prx );
    NetEdgeNAspMap	edgeNAspP	= get( edge_mynasp, prx );
#endif	// USING_SIMILARITY_CONJOINING

//------------------------------------------------------------------------------
//	Inserting vertices of the beta-skeleton into the proximity graph
//------------------------------------------------------------------------------
    prx.clear();
    BGL_FORALL_VERTICES( vdN, nbr, Network ) {
	NetVertexDescriptor vdP	= add_vertex( prx );
	vertexIDP[ vdP ]	= vertexIDN[ vdN ];
	vertexCntrP[ vdP ]	= vertexCntrN[ vdN ];
	vertexPolyP[ vdP ]	= vertexPolyN[ vdN ];
    }

//------------------------------------------------------------------------------
//	Sort edges according to the normalized distance for proximity search
//------------------------------------------------------------------------------
    map< double, NetEdgeDescriptor > edgeMap;
    BGL_FORALL_EDGES( edN, nbr, Network ) {
        double nDst = edgeNDstN[ edN ];
	if ( nDst <= ratio )
	    edgeMap.insert( std::make_pair( nDst, edN ) );
    }
    
    // Insert the set of candidate edges into the proximity graph without
    // exceptions
    for ( map< double, NetEdgeDescriptor >::iterator iter = edgeMap.begin();
	  iter != edgeMap.end(); iter++ ) {
	cerr << HERE << " Edge : " << iter->first << " , "
	     << iter->second << endl;
	NetEdgeDescriptor	edN	= iter->second;
        // double nDst = edgeNDstN[ edN ];
	NetVertexDescriptor	vdSN	= source( edN, nbr );
	NetVertexDescriptor	vdTN	= target( edN, nbr );
	unsigned int		idS	= vertexIDN[ vdSN ];
	unsigned int		idT	= vertexIDN[ vdTN ];
	NetVertexDescriptor	vdSP	= vertex( idS, prx );
	NetVertexDescriptor	vdTP	 = vertex( idT, prx );
	NetEdgeDescriptor	edP;
	bool			okP;
	tie( edP, okP )		= add_edge( vdSP, vdTP, prx );
	assert( okP );
	edgeIDP[ edP ]		= edgeIDN[ edN ];
	edgeWeightP[ edP ]	= edgeWeightN[ edN ];
	edgeNDstP[ edP ]	= edgeNDstN[ edN ];
	edgeNInvP[ edP ]	= edgeNInvN[ edN ];
#ifdef USING_SIMILARITY_CONJOINING
	edgeRatioP[ edP ]	= edgeRatioN[ edN ];
	edgeNAspP[ edP ]	= edgeNAspN[ edN ];
#endif	// USING_SIMILARITY_CONJOINING
    }
}


//
//  Drawing::_calcCnvProximity --	employ the conventional approach for
//					calculating the graph for proximity
//					labels
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Drawing::_calcCnvProximity( void )
{
    Network &		nbr		= _netNbr;
    Network &		prx		= _netPrx;

    NetVertexIDMap	vertexIDN	= get( vertex_myid, nbr );
    NetVertexCntrMap	vertexCntrN	= get( vertex_mycntr, nbr );
    NetVertexPolyMap	vertexPolyN	= get( vertex_mypoly, nbr );
    NetEdgeIDMap	edgeIDN		= get( edge_myid, nbr );
    NetEdgeWeightMap	edgeWeightN	= get( edge_weight, nbr );
    NetEdgeNDstMap	edgeNDstN	= get( edge_myndst, nbr );
    NetEdgeNInvMap	edgeNInvN	= get( edge_myninv, nbr );
#ifdef USING_SIMILARITY_CONJOINING
    NetEdgeRatioMap	edgeRatioN	= get( edge_myratio, nbr );
    NetEdgeNAspMap	edgeNAspN	= get( edge_mynasp, nbr );
#endif	// USING_SIMILARITY_CONJOINING

    NetVertexIDMap	vertexIDP	= get( vertex_myid, prx );
    NetVertexCntrMap	vertexCntrP	= get( vertex_mycntr, prx );
    NetVertexPolyMap	vertexPolyP	= get( vertex_mypoly, prx );
    NetEdgeIDMap	edgeIDP		= get( edge_myid, prx );
    NetEdgeWeightMap	edgeWeightP	= get( edge_weight, prx );
    NetEdgeNDstMap	edgeNDstP	= get( edge_myndst, prx );
    NetEdgeNInvMap	edgeNInvP	= get( edge_myninv, prx );
#ifdef USING_SIMILARITY_CONJOINING
    NetEdgeRatioMap	edgeRatioP	= get( edge_myratio, prx );
    NetEdgeNAspMap	edgeNAspP	= get( edge_mynasp, prx );
#endif	// USING_SIMILARITY_CONJOINING

//------------------------------------------------------------------------------
//	Inserting vertices of the beta-skeleton into the proximity graph
//------------------------------------------------------------------------------
    prx.clear();
    BGL_FORALL_VERTICES( vdN, nbr, Network ) {
	NetVertexDescriptor vdP	= add_vertex( prx );
	vertexIDP[ vdP ]	= vertexIDN[ vdN ];
	vertexCntrP[ vdP ]	= vertexCntrN[ vdN ];
	vertexPolyP[ vdP ]	= vertexPolyN[ vdN ];
    }

//------------------------------------------------------------------------------
//	Sort edges according to the normalized distance for proximity search
//------------------------------------------------------------------------------
    unsigned int nE = num_edges( nbr );
    // [Relative limit for the candidate edges]
    // Limit the number of candidate edges by the ratio wrt the total number or
    // edges in the beta-skeleton
    // This should be replaced with the absolute distance between the polygons
    // (Noted by ST on 2023/02/26)
#ifdef REDEFINING_CONNECTIVITY_BETWEEN_POLYGONS
    // We abondon the original version here.
    // (Noted by ST on 2023/05/16)
    unsigned int limitE = std::min( ( int )nE,
				    std::max( CLOSENESS_LIMIT,
					      (int)( Drawing::proximity_ratio * nE) ) );
    cerr << HERE << " nE = " << nE << " limitE = " << limitE << endl;
#else	// REDEFINING_CONNECTIVITY_BETWEEN_POLYGONS
    // Finally, we constrained the connectivity of the beta skeleton according
    // to the interval distance between building polygons, thus, we simply
    // employ the connectivity induced from the beta skeleton here.
    // (Noted by ST on 2023/05/16)
    unsigned int limitE = nE;
#endif	// REDEFINING_CONNECTIVITY_BETWEEN_POLYGONS

    map< double, NetEdgeDescriptor > edgeMap;
    BGL_FORALL_EDGES( edN, nbr, Network ) {
        double nDst = edgeNDstN[ edN ];
	edgeMap.insert( std::make_pair( nDst, edN ) );
    }
    
    // Insert the set of candidate edges into the proximity graph without
    // exceptions
    for ( map< double, NetEdgeDescriptor >::iterator iter = edgeMap.begin();
	  std::distance( edgeMap.begin(), iter ) < limitE; iter++ ) {
	cerr << HERE << " Edge : " << iter->first << " , "
	     << iter->second << endl;
	NetEdgeDescriptor	edN	= iter->second;
        // double nDst = edgeNDstN[ edN ];
	NetVertexDescriptor	vdSN	= source( edN, nbr );
	NetVertexDescriptor	vdTN	= target( edN, nbr );
	unsigned int		idS	= vertexIDN[ vdSN ];
	unsigned int		idT	= vertexIDN[ vdTN ];
	NetVertexDescriptor	vdSP	= vertex( idS, prx );
	NetVertexDescriptor	vdTP	 = vertex( idT, prx );
	NetEdgeDescriptor	edP;
	bool			okP;
	tie( edP, okP )		= add_edge( vdSP, vdTP, prx );
	assert( okP );
	edgeIDP[ edP ]		= edgeIDN[ edN ];
	edgeWeightP[ edP ]	= edgeWeightN[ edN ];
	edgeNDstP[ edP ]	= edgeNDstN[ edN ];
	edgeNInvP[ edP ]	= edgeNInvN[ edN ];
#ifdef USING_SIMILARITY_CONJOINING
	edgeRatioP[ edP ]	= edgeRatioN[ edN ];
	edgeNAspP[ edP ]	= edgeNAspN[ edN ];
#endif	// USING_SIMILARITY_CONJOINING
    }
}


#ifdef USING_SIMILARITY_CONJOINING
//
//  Drawing::_calcCnvSimilarity --	employ the conventional approach for
//					calculating the graph for similarity labels 
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Drawing::_calcCnvSimilarity( void )
{
    Network &		nbr		= _netNbr;
    Network &		sim		= _netSim;
    
    NetVertexIDMap	vertexIDN	= get( vertex_myid, nbr );
    NetVertexCntrMap	vertexCntrN	= get( vertex_mycntr, nbr );
    NetVertexPolyMap	vertexPolyN	= get( vertex_mypoly, nbr );
    NetEdgeIDMap	edgeIDN		= get( edge_myid, nbr );
    NetEdgeWeightMap	edgeWeightN	= get( edge_weight, nbr );
    NetEdgeNDstMap	edgeNDstN	= get( edge_myndst, nbr );
    NetEdgeNInvMap	edgeNInvN	= get( edge_myninv, nbr );
    NetEdgeRatioMap	edgeRatioN	= get( edge_myratio, nbr );
    NetEdgeNAspMap	edgeNAspN	= get( edge_mynasp, nbr );

    NetVertexIDMap	vertexIDS	= get( vertex_myid, sim );
    NetVertexCntrMap	vertexCntrS	= get( vertex_mycntr, sim );
    NetVertexPolyMap	vertexPolyS	= get( vertex_mypoly, sim );
    NetEdgeIDMap	edgeIDS		= get( edge_myid, sim );
    NetEdgeWeightMap	edgeWeightS	= get( edge_weight, sim );
    NetEdgeNDstMap	edgeNDstS	= get( edge_myndst, sim );
    NetEdgeNInvMap	edgeNInvS	= get( edge_myninv, sim );
    NetEdgeRatioMap	edgeRatioS	= get( edge_myratio, sim );
    NetEdgeNAspMap	edgeNAspS	= get( edge_mynasp, sim );

    sim.clear();
    BGL_FORALL_VERTICES( vdN, nbr, Network ) {
	NetVertexDescriptor vdS = add_vertex( sim );
	vertexIDS[ vdS ] = vertexIDN[ vdN ];
	vertexCntrS[ vdS ] = vertexCntrN[ vdN ];
	vertexPolyS[ vdS ] = vertexPolyN[ vdN ];
    }

//------------------------------------------------------------------------------
//	Sort edges according to the normalized aspect ratio for similarity search
//------------------------------------------------------------------------------
    unsigned int nE = num_edges( nbr );
    //------------------------------------------------------------------------------
    // We do not touch the definition of this limit number because similarity
    // will explore more possibilities irrespective of the polygon proximity
    // suggested by the beta-skeleton.
    // (Noted by ST on 2023/05/16)
    //------------------------------------------------------------------------------
    unsigned int limitE = std::min( ( int )nE,
				    std::max( CLOSENESS_LIMIT,
					      (int)(Drawing::similarity_ratio * nE) ) ); 
    cerr << HERE << " nE = " << nE << " limitE = " << limitE << endl;

    map< double, NetEdgeDescriptor, greater< double > > edgeMap;
    BGL_FORALL_EDGES( edN, nbr, Network ) {
        double nAsp = edgeNAspN[ edN ];
	edgeMap.insert( std::make_pair( nAsp, edN ) );
    }

    for ( map< double, NetEdgeDescriptor >::iterator iter = edgeMap.begin();
	  std::distance( edgeMap.begin(), iter ) < limitE; iter++ ) {
	cerr << HERE << " Edge : " << iter->first << " , "
	     << iter->second << endl;
	NetEdgeDescriptor edN = iter->second;
	NetVertexDescriptor vdSN = source( edN, nbr );
	NetVertexDescriptor vdTN = target( edN, nbr );
	unsigned int idS = vertexIDN[ vdSN ];
	unsigned int idT = vertexIDN[ vdTN ];
	// double nAsp = edgeNAspN[ edN ];
	NetVertexDescriptor vdSS = vertex( idS, sim );
	NetVertexDescriptor vdTS = vertex( idT, sim );
	NetEdgeDescriptor edS;
	bool okS;
	tie( edS, okS ) = add_edge( vdSS, vdTS, sim );
	assert( okS );
	edgeIDS[ edS ] = edgeIDN[ edN ];
	edgeWeightS[ edS ] = edgeWeightN[ edN ];
	edgeNDstS[ edS ] = edgeNDstN[ edN ];
	edgeNInvS[ edS ] = edgeNInvN[ edN ];
	edgeRatioS[ edS ] = edgeRatioN[ edN ];
	edgeNAspS[ edS ] = edgeNAspN[ edN ];
    }
}
#endif	// USING_SIMILARITY_CONJOINING


//
//  Drawing::_labelComponents --	retrieve labels from the graphs for Gestalts
//
//  Inputs
//	net	: network for the gestalts of the specific type
//	label	: a set of labels to be returned	
//
//  Outputs
//	none
//
void Drawing::_labelComponents( Network & net, vector< Set > & label )
{
    label.clear();
    
    vector< unsigned int > component( num_vertices( net ) );
    unsigned int num = connected_components( net, &component[ 0 ] );
    
    label.resize( num );
    for ( unsigned int i = 0; i < component.size(); ++i ) {
	// cerr << " Vertex No. " << i << " is in Component No. " << component[ i ] << endl;
	unsigned int id = component[ i ];
	assert( id < num );
	label[ id ].push_back( i );
    }

    unsigned int count = 0;
    while ( count < label.size() ) {
	// cerr << HERE << " count = " << count << " : ";
	unsigned int index = label.size() - 1 - count;
	if ( label[ index ].size() == 1 ) {
	    // cerr << " Delete element at " << index << endl;
	    label.erase( label.begin() + index );
	}
	else {
	    // cerr << " Skip element at " << index << " and then increment counter " << endl;
	    count++;
	}
    }
    
    // Sort the IDs in each group
    for ( unsigned int i = 0; i < label.size(); ++i )
	sort( label[ i ].begin(), label[ i ].end() );
    
    // Debug messages
    cerr << HERE << " retrieving groups of polygons " << endl;
    for ( unsigned int k = 0; k < label.size(); ++k ) {
	cerr << " Group No. " << setw( 3 ) << k << " : ";
	for ( unsigned int m = 0; m < label[ k ].size(); ++m ) {
	    cerr << setw( 3 ) << label[ k ][ m ] << " ";
	}
	cerr << endl;
    }
}
    

//
//  Drawing::_convexForLabel	--	compute the convex hull of the polygons
//					tagged with the specific label ID.
//
//  Inputs
//	net	: k-neighbor graph
//	label	: Label IDs in the label
//
//  Outputs
//	convex hull as an enclosure
//
void Drawing::_convexForLabel( Network & net, const Set & label,
			       Polygon2 & CH, Set & PN )
{
    vector< Point2 >		point;
    vector< unsigned int >	glob;
    NetVertexPolyMap		vertexPoly	= get( vertex_mypoly, net );
    NetVertexGlobMap		vertexGlob	= get( vertex_myglob, net );
    
    CH.clear();
    PN.clear();
    
    for ( unsigned int i = 0; i < label.size(); ++i ) {
	NetVertexDescriptor vd = vertex( label[ i ], net );
	Polygon2 & poly = vertexPoly[ vd ];
	for ( unsigned int j = 0; j < poly.size(); ++j ) {
	    point.push_back( poly[ j ] );
	}
	Set & curID = vertexGlob[ vd ];
	for ( unsigned int j = 0; j < curID.size(); ++j ) {
	    glob.push_back( curID[ j ] );
	}
    }
    // cerr << HERE << " Number of corners in the polygon = " << points.size();

    vector< unsigned int >	indices( point.size() ), out;
    iota( indices.begin(), indices.end(), 0 );
    // CGAL::convex_hull_2( points.begin(), points.end(), std::back_inserter( result ) );
    CGAL::convex_hull_2( indices.begin(), indices.end(), std::back_inserter( out ),
			 Convex_hull_traits_2( CGAL::make_property_map( point ) ) );

    // cerr << result.size() << " points on the convex hull" << std::endl;

    CH.clear();
#ifdef SKIP
    for ( unsigned int k = 0; k < result.size(); ++k ) {
	// cerr << "[ " << k << " ] : ( " << result[ k ] << " ) " << endl;
	CH.push_back( result[ k ] );
    }
#endif	// SKIP

    for ( unsigned int k = 0; k < out.size(); ++k ) {
	// Convex hull
	CH.push_back( point[ out[ k ] ] );
	// Associated point numbers
	PN.push_back( glob[ out[ k ] ] );
    }
    
    return;
}


//
//  Drawing::_concaveForLabel	--	compute the concave hull (alpha shape)
//					of the polygons tagged with the specific
//					label ID
//
//  Inputs
//	net	: proximity graph
//	label	: Label IDs in the label
//
//  Outputs
//	concave hull as an enclosure
//
void Drawing::_concaveForLabel( Network & net, const Set & label,
				Polygon2 & CH, Set & PN )
{
    Polygon2 coord;
    Set index;

    _convexForLabel( net, label, coord, index );
    
#ifdef DEBUG
    cerr << "========================================" << endl;
    cerr << HERE << " convex hull indices : " << ends;
    for ( unsigned int i = 0; i < index.size(); ++i ) {
	cerr << " " << index[ i ] << ends;
    }
    cerr << endl;
#endif	// DEBUG

    DirVertexPointMap		vertexPoint	= get( vertex_mypoint, _wrapper );
    DirVertexGroupMap		vertexGroup	= get( vertex_mygroup, _wrapper );
    DirEdgeWeightMap		edgeWeight	= get( edge_weight, _wrapper );
    // DirEdgeAddrMap		edgeAddr	= get( edge_myaddr, _wrapper );


    //------------------------------------------------------------------------------
    //	edge assignment replacemnt	
    //------------------------------------------------------------------------------
    // unsigned int nEdges = num_edges( _wrapper );
    vector< double > dist;
    const double maxWeight = 1.0e+8;
    unsigned int counter = 0;
    BGL_FORALL_EDGES( ed, _wrapper, Directed ) {
	dist.push_back( edgeWeight[ ed ] );
	NetVertexDescriptor vdS = source( ed, _wrapper );
	NetVertexDescriptor vdT = target( ed, _wrapper );
	unsigned int polyS = vertexGroup[ vdS ]; 
	unsigned int polyT = vertexGroup[ vdT ]; 
#ifdef SKIP
	if ( label.size() >= 2 ) {
	    cerr << HERE << " polyS = " << polyS << " polyT = " << polyT << endl;
	    cerr << HERE << " label = " << label << endl;
	}
#endif	// SKIP
	if ( ! ( label.isContained( polyS ) && label.isContained( polyT ) ) ) {
#ifdef SKIP
	    if ( label.size() >= 2 ) {
		cerr << " Not contained " << endl;
	    }
#endif	// SKIP
	    edgeWeight[ ed ] = maxWeight;
	}
    }

    CH.clear();
    PN.clear();
    
#ifdef DEBUG
    cerr << HERE << "********** Edge search ********** " << endl;
    cerr << HERE << " index.size = " << index.size() << endl;
#endif	// DEBUG
    for ( unsigned int i = 0; i < index.size(); ++i ) {
	// bool doCheck = false;
	unsigned int idS = index[ i ];
	unsigned int idT = index[ (i+1)%index.size() ];
	Set path = shortestPath( idS, idT, _wrapper );
	// #ifdef DEBUG
	bool isFlag = false;
	for ( int k = 0; k < ( int )path.size() - 1; ++k ) {
	    if ( ( 520 <= path[ k ] ) && ( path[ k ] < 530 ) ||
		 ( path[ k ] == 335 ) ) isFlag = true;
	}

	if ( isFlag ) cerr << HERE << "( " << idS << " -> " << idT << " ) => " << ends;
	// #endif	// DEBUG

	// Check whether the shortest path already contains the same vertex
	// as intermediate points
	bool isCircle = true;
	for ( int k = 0; k < ( int )path.size() - 1; ++k ) {
	    unsigned int idM = path[ k ];
	    Set::iterator it = find( PN.begin(), PN.end(), idM );
	    if ( it != PN.end() ) isCircle = false;
	    if ( !isCircle ) break;
	}

	// Insert the first point in any case
	DirVertexDescriptor vdS = _onelayer[ idS ];
	CH.push_back( vertexPoint[ vdS ] );
	PN.push_back( idS );

	// If the path does not contain the existing intermediate point,
	// replace the convex hull edge with concave hull edges except for the
	// last point
	if ( isCircle ) {
	    if ( isFlag ) cerr << HERE << " PATH: ";
	    for ( unsigned int k = 0; k < path.size() - 1; ++k ) {
		//#ifdef DEBUG
		if ( isFlag ) cerr << " " << setw( 3 ) << path[ k ];
		// #endif	// DEBUG
		unsigned int idM = path[ k ];
		NetVertexDescriptor vdM = _onelayer[ idM ];
		CH.push_back( vertexPoint[ vdM ] );
		PN.push_back( idM );
	    }
	    //#ifdef DEBUG
	    if ( isFlag ) cerr << endl;
	    // #endif	// DEBUG
	}
	// getchar();
    }
    
#ifdef DEBUG
    cerr << HERE << " concave hull indices : " << ends;
    for ( unsigned int i = 0; i < PN.size(); ++i ) {
	cerr << " " << PN[ i ] << ends;
    }
    cerr << endl;
#endif	// DEBUG
    
    //------------------------------------------------------------------------------
    //	edge assignment recovery	
    //------------------------------------------------------------------------------
    counter = 0;
    BGL_FORALL_EDGES( ed, _wrapper, Directed ) {
	edgeWeight[ ed ] = dist[ counter++ ];
    }

#ifdef SKIP
    cerr << HERE << " Concave : " << ends;
    for ( unsigned int i = 0; i < PN.size(); ++i ) 
	cerr << " " << setw( 3 ) << PN[ i ] << ends;
    cerr << endl;
#endif	// SKIP

    return;
}


//
//  Drawing::_proximityCost	--	return the proximity cost for the
//					specific label
//
//  Inputs
//	net	: k-neighbor graph
//	label	: Label IDs in the label
//	hull	: associated convex hull
//
//  Outputs
//	cost for the proximity label
//
double Drawing::_proximityCost( Network & net, const Set & label, const Polygon2 & hull )
{
    NetVertexPolyMap	vertexPoly	= get( vertex_mypoly, net );

    double		area_int	= 0.0;
    double		area_ext	= 0.0;
    double		diff;
    
    for ( unsigned int i = 0; i < label.size(); ++i ) {
	NetVertexDescriptor vd = vertex( label[ i ], net );
	Polygon2 & poly = vertexPoly[ vd ];
	if ( poly.orientation() == CGAL::CLOCKWISE )
	    area_int += ( -1.0 ) * poly.area();
	else
	    area_int += (  1.0 ) * poly.area();
    }
    area_ext = hull.area();
#ifdef PROXIMITY_RATIO_COST
    diff = 1.0 - area_int/area_ext;
#else	// PROXIMITY_RATIO_COST
    diff = area_ext - area_int;
#endif	// PROXIMITY_RATIO_COST
    
#ifdef DEBUG
    cerr << HERE << " area_int = " << area_int << " area_ext = " << area_ext << endl;
    cerr << HERE << " area diff = " << diff << endl;
    cerr << HERE << " area ratio = " << area_int/area_ext << endl;
#endif	// DEBUG
    
    return diff;
}

#ifdef USING_SIMILARITY_CONJOINING
//
//  Drawing::_similarityCost	--	return the similarity cost for the
//					specific label
//
//  Inputs
//	net	: k-neighbor graph
//	label	: Label IDs in the label
//
//  Outputs
//	cost for the similarity label
//
double Drawing::_similarityCost( Network & g, const Set & label )
{
    // NetVertexPolyMap	vertexPoly	= get( vertex_mypoly, net );
    vector< double >	aspect;
    
    aspect.clear();
    for ( unsigned int i = 0; i < label.size(); ++i ) {
	for ( unsigned int j = i + 1; j < label.size(); ++j ) {
	    double dblSim = 2.0 * _simPolys( i, j );
	    aspect.push_back( dblSim );
	    // cerr << dblSim << " ";
	}
    }
    // cerr << endl;
    
    double ave = 0.0;
    double var = 0.0;
    for ( unsigned int k = 0; k < aspect.size(); ++k ) {
	ave += aspect[ k ];
    }
    ave /= ( double )aspect.size();
    for ( unsigned int k = 0; k < aspect.size(); ++k ) {
	var += SQUARE( aspect[ k ] - ave );
    }
    var /= ( double )aspect.size();
    
#ifdef DEBUG
    cerr << HERE << " ave = " << ave << " var = " << var << endl;
#endif	// DEBUG

    return var;
}
#endif	// USING_SIMILARITY_CONJOINING



//------------------------------------------------------------------------------
//	Main functions for preparing the label cost optimization 
//------------------------------------------------------------------------------
//
//  Drawing::_assignLabels --	assign Gestalt labels to the line drawing
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Drawing::_assignLabels( void )
{
    const string title = "Drawing::_assignLabels";
    _wrapBSkeleton();
    _calcWrapper();
    
    // Assign a label to each polygon component as an individual set
    unsigned int nVertices = num_vertices( _netNbr );
    _labelSgl.clear();
    for ( unsigned int i = 0; i < nVertices; ++i ) {
	Set eachSet;
	eachSet.push_back( i );
	_labelSgl.push_back( eachSet );
    }
    _labelAll = _labelSgl;
    
//------------------------------------------------------------------------------
//	multi-step expansion of proximity over the beta-skeleton
//	implemented by ST on 2023/08/21
//------------------------------------------------------------------------------
    // Assign a label to each proximity set of polygon components
    cerr << HERE << title << " Constructed the proximity graph " << endl;
    const unsigned int numLevels = 3;
    _labelPrx.clear();
    _calcNewProximity( 1.0 );
    _labelComponents( _netPrx, _labelPrx );
    cerr << HERE << "[ 1.0 ] Number of proximity labels = " << _labelPrx.size() << endl;
    for ( unsigned int k = 1; k < numLevels; ++k ) {
	double curRatio = 1.0/pow( 2.0, ( double )k );
	vector< Set >	curLabelPrx;
	_calcNewProximity( curRatio );
	_labelComponents( _netPrx, curLabelPrx );
	for ( unsigned int i = 0; i < curLabelPrx.size(); ++i ) {
	    bool doExist = false;
	    for ( unsigned int j = 0; j < _labelPrx.size(); ++j ) {
		if ( _labelPrx[ j ] == curLabelPrx[ i ] ) doExist = true;
		if ( doExist ) break;
	    }
	    if ( ! doExist ) _labelPrx.push_back( curLabelPrx[ i ] );
	}
	cerr << HERE << "[ " << fixed << setw( 3 ) << setprecision( 1 ) << curRatio
	     << " ] Number of proximity labels = " << _labelPrx.size() << endl;
    }
    _labelAll.insert( _labelAll.end(), _labelPrx.begin(), _labelPrx.end() );
    
#ifdef USING_SIMILARITY_CONJOINING
    // Assign a label to each similarity set of polygon components
    vector< Set >	curLabelSim;
    _labelSim.clear();
    _calcSimilarity();
    _labelComponents( _netSim, curLabelSim );
    _labelSim.clear();
    cerr << HERE << title << " Constructed the similarity graph " << endl;
//------------------------------------------------------------------------------
//	Special function for removing idential grouping labels across
//	similiarity and other properties (written by ST on 2023/06/28)
//------------------------------------------------------------------------------
    for ( unsigned int i = 0; i < curLabelSim.size(); ++i ) {
	bool doExist = false;
	for ( unsigned int j = 0; j < _labelAll.size(); ++j ) {
	    if ( _labelAll[ j ] == curLabelSim[ i ] ) doExist = true;
	    if ( doExist ) break;
	}
	if ( ! doExist ) _labelSim.push_back( curLabelSim[ i ] );
    }
    _labelAll.insert( _labelAll.end(), _labelSim.begin(), _labelSim.end() );
#endif	// USING_SIMILARITY_CONJOINING
	
//------------------------------------------------------------------------------
//	Special function for removing idential grouping labels across design
//	and other properties (written by ST on 2023/08/21)
//------------------------------------------------------------------------------
    vector< Set >	tmpLabelDes;
    for ( unsigned int i = 0; i < _labelDes.size(); ++i ) {
	bool doExist = false;
	for ( unsigned int j = 0; j < _labelAll.size(); ++j ) {
	    if ( _labelAll[ j ] == _labelDes[ i ] ) doExist = true;
	    if ( doExist ) break;
	}
	if ( ! doExist ) tmpLabelDes.push_back( _labelDes[ i ] );
    }
    _labelAll.insert( _labelAll.end(), tmpLabelDes.begin(), tmpLabelDes.end() );

    ofstream ofs_group( GROUP_CHOICE_FILE );
    streambuf* buf_group;
    if ( ofs_group ) buf_group = cerr.rdbuf( ofs_group.rdbuf() );
    cerr << HERE << "********** Total number of labels = " << _labelAll.size() << endl;
    for ( unsigned int k = 0; k < _labelAll.size(); ++k ) {
	cerr << HERE;
	if ( k < _labelSgl.size() ) cerr << " [Sgl] ";
	else if ( k < _labelSgl.size() + _labelPrx.size() ) cerr << " [Prx] ";
#ifdef USING_SIMILARITY_CONJOINING
	else if ( k < _labelSgl.size() + _labelPrx.size() + _labelSim.size() )
	    cerr << " [Sim] ";
#endif	// USING_SIMILARITY_CONJOINING
	else cerr << " [Des] ";
	cerr << " label ID = " << k << " : ";
	for ( unsigned int m = 0; m < _labelAll[ k ].size(); ++m ) {
	    cerr << _labelAll[ k ][ m ] << " ";
	}
	cerr << endl;
    }
    if ( ofs_group ) {
	cerr.rdbuf( buf_group );
	ofs_group.close();
    }
}

//------------------------------------------------------------------------------
//	Functions for calculating the data/smooth/label costs
//------------------------------------------------------------------------------

//
//  Drawing::_calcStdDataCost --calculate the data cost for each polygon
//				in term of the respective labels
//				(for standard algorithm)
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Drawing::_calcStdDataCost( void )
{
    Network &		net		= _netNbr;
    unsigned int	nVertices	= num_vertices( net );

    // initialize the data cost
//------------------------------------------------------------------------------
//	data costs for the individual primitives
//------------------------------------------------------------------------------
    vector< vector< double > > dataCostSgl;
    
    for ( unsigned int i = 0; i < nVertices; ++i ) {
	vector< double > itemCost;
	for ( unsigned int j = 0; j < nVertices; ++j ) {
	    double curCost;
	    if ( i == j ) curCost = Drawing::data_cost_inside;
	    else curCost = Drawing::data_cost_upper;
	    itemCost.push_back( curCost );
	}
	// SKIP
	// _normalizeVector( itemCost, DATA_COST_SINGLE, DATA_COST_UPPER );
	dataCostSgl.push_back( itemCost );
    }

//------------------------------------------------------------------------------
//	data costs for the proximity gestalts
//------------------------------------------------------------------------------
    vector< vector< double > > dataCostPrx;
    for ( unsigned int i = 0; i < nVertices; ++i ) {
	vector< double > itemCost;
	unsigned int idS = i;

	for ( unsigned int j = 0; j < _labelPrx.size(); ++j ) {
	    Set & curItem = _labelPrx[ j ];
	    double minDist = INFINITY;
	    for ( unsigned int k = 0; k < curItem.size(); ++k ) {
		unsigned int idT = curItem[ k ];
		double curDist = _distPolys( idS, idT );
#ifdef DATA_COST_PANALTY_WITH_TOPOLOGY
		if ( ! isConnected( idS, idT, net ) )
		     curDist *= DATA_COST_PENALTY_SCALE;
#endif	// DATA_COST_PANALTY_WITH_TOPOLOGY
		if ( curDist < minDist ) minDist = curDist;
	    }
	    itemCost.push_back( minDist );
	}
	_normalizeVector( itemCost, Drawing::data_cost_lower, Drawing::data_cost_upper );
	dataCostPrx.push_back( itemCost );
    }

#ifdef USING_SIMILARITY_CONJOINING
//------------------------------------------------------------------------------
//	data costs for the similarity gestalts
//------------------------------------------------------------------------------
    vector< vector< double > > dataCostSim;
    for ( unsigned int i = 0; i < nVertices; ++i ) {
	vector< double > itemCost;
	unsigned int idS = i;
	
	for ( unsigned int j = 0; j < _labelSim.size(); ++j ) {
	    Set & curItem = _labelSim[ j ];
	    double sumRatio = 0.0;
	    for ( unsigned int k = 0; k < curItem.size(); ++k ) {
		unsigned int idT = curItem[ k ];
		double curRatio = 2.0 * _simPolys( idS, idT );
#ifdef DATA_COST_PANALTY_WITH_TOPOLOGY
		if ( ! isConnected( idS, idT, net ) )
		    curRatio /= DATA_COST_PENALTY_SCALE;
#endif	// DATA_COST_PANALTY_WITH_TOPOLOGY
		sumRatio += curRatio;
	    }
	    double aveRatio = sumRatio / ( double )curItem.size();
	    itemCost.push_back( aveRatio );
	}
	_normalizeVector( itemCost );
	// Change the cost to 1 - data_cost_sim
	for ( unsigned int j = 0; j < itemCost.size(); ++j )
	    itemCost[ j ] = 1.0 - itemCost[ j ];
	_normalizeVector( itemCost, Drawing::data_cost_lower, Drawing::data_cost_upper );
	dataCostSim.push_back( itemCost );
    }
#endif	// USING_SIMILARITY_CONJOINING
	
//------------------------------------------------------------------------------
//	Combine data costs for proximity and similarity 
//------------------------------------------------------------------------------
    _dataCost.clear();
    for ( unsigned int i = 0; i < nVertices; ++i ) {
	vector< double > costForLabel = dataCostSgl[ i ];
	costForLabel.insert( costForLabel.end(),
			     dataCostPrx[ i ].begin(), dataCostPrx[ i ].end() );
#ifdef USING_SIMILARITY_CONJOINING
	costForLabel.insert( costForLabel.end(),
			     dataCostSim[ i ].begin(), dataCostSim[ i ].end() );
#endif	// USING_SIMILARITY_CONJOINING
	_dataCost.push_back( costForLabel );
    }

    //#ifdef PRINT_COSTS
    cerr << HERE <<  " Data costs " << endl;
    cerr << "      " << ends;
    for ( unsigned int j = 0; j < _dataCost[ 0 ].size(); ++j ) {
	cerr << "[" << setw( 3 ) << j << "] " << ends;
    }
    cerr << endl;
    for ( unsigned int i = 0; i < _dataCost.size(); ++i ) {
	cerr << "[" << setw( 3 ) << i << "] " << ends;
	for ( unsigned int j = 0; j < _dataCost[ i ].size(); ++j ) {
	    double cost = _dataCost[ i ][ j ];
	    cerr << std::fixed << setprecision( 1 ) << setw( 5 ) << cost << " " << ends;
	}
	cerr << endl;
    }
    //#endif	// PRINT_COSTS

    cerr << HERE << " Obtained data costs for all labels" << endl;
}

//
//  Drawing::_calcDataCost --	calculate the data cost for each polygon in term
//				of the respective labels
//				( for aggregating building polygons )
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Drawing::_calcDataCost( void )
{
    Network &		net		= _netNbr;
    unsigned int	nVertices	= num_vertices( net );

    // initialize the data cost
//------------------------------------------------------------------------------
//	data costs for the individual primitives
//------------------------------------------------------------------------------
    vector< vector< double > > dataCostSgl;
    
    for ( unsigned int i = 0; i < nVertices; ++i ) {
	vector< double > itemCost;
	for ( unsigned int j = 0; j < nVertices; ++j ) {
	    double curCost;
	    if ( i == j ) curCost = Drawing::data_cost_inside;
	    else curCost = Drawing::data_cost_upper;
	    itemCost.push_back( curCost );
	}
	// SKIP
	// _normalizeVector( itemCost, DATA_COST_SINGLE, DATA_COST_UPPER );
	dataCostSgl.push_back( itemCost );
    }

//------------------------------------------------------------------------------
//	data costs for the proximity gestalts
//------------------------------------------------------------------------------
    vector< vector< double > > dataCostPrx;
    for ( unsigned int i = 0; i < nVertices; ++i ) {
	vector< double > itemCost;
	unsigned int idS = i;

	// cerr << HERE << " Polygon ID = " << idS << endl;
	
	for ( unsigned int j = 0; j < _labelPrx.size(); ++j ) {
	    Set curItem = _labelPrx[ j ];
	    Polygon2 hullOrg;

	    // Just use the convex hull for computing data costs
	    _convexForLabel( net, curItem, hullOrg );

	    double areaOrg = hullOrg.area();
	    if ( ! hullOrg.is_simple() )
		areaOrg *= 1.0e-8;
	    else if ( hullOrg.orientation() == CGAL::CLOCKWISE )
		areaOrg *= -1.0;

	    if ( ! curItem.isContained( idS ) ) {
		curItem.push_back( idS );
	    }
	    Polygon2 hullExt;

	    // Just use the convex hull for computing data costs
	    _convexForLabel( net, curItem, hullExt );

	    double areaExt = hullExt.area();
	    if ( ! hullExt.is_simple() )
		areaExt *= 1.0e-8;
	    else if ( hullExt.orientation() == CGAL::CLOCKWISE )
		areaExt *= -1.0;
	    
	    assert( areaOrg >= 0.0 );
	    assert( areaExt >= 0.0 );

	    double costR =  min( 1.0, 1.0 - ( areaOrg / areaExt ) );
#ifdef SKIP
	    cerr << HERE << " Prx Label # " << setw( 2 ) << j
		 << " areaOrg = " << areaOrg << " areaExt = " << areaExt
		 << " cost = " << costR << endl;
#endif	// SKIP
	    itemCost.push_back( costR );
	}
	dataCostPrx.push_back( itemCost );
    }
    _normalizeMatrix( dataCostPrx, Drawing::data_cost_lower, Drawing::data_cost_upper );

#ifdef USING_SIMILARITY_CONJOINING
//------------------------------------------------------------------------------
//	data costs for the similarity gestalts
//------------------------------------------------------------------------------
    vector< vector< double > > dataCostSim;
    for ( unsigned int i = 0; i < nVertices; ++i ) {
	vector< double > itemCost;
	unsigned int idS = i;
	
	// cerr << HERE << " Polygon ID = " << idS << endl;

	for ( unsigned int j = 0; j < _labelSim.size(); ++j ) {
	    Set curItem = _labelSim[ j ];
	    Polygon2 hullOrg;

	    // Just use the convex hull for computing data costs
	    _convexForLabel( net, curItem, hullOrg );

	    double areaOrg = hullOrg.area();
	    if ( ! hullOrg.is_simple() )
		areaOrg *= 1.0e-8;
	    else if ( hullOrg.orientation() == CGAL::CLOCKWISE )
		areaOrg *= -1.0;

	    if ( ! curItem.isContained( idS ) ) {
		curItem.push_back( idS );
	    }
	    Polygon2 hullExt;

	    // Just use the convex hull for computing data costs
	    _convexForLabel( net, curItem, hullExt );

	    double areaExt = hullExt.area();
	    if ( ! hullExt.is_simple() )
		areaExt *= 1.0e-8;
	    else if ( hullExt.orientation() == CGAL::CLOCKWISE )
		areaExt *= -1.0;

	    assert( areaOrg >= 0.0 );
	    assert( areaExt >= 0.0 );

	    double costR =  min( 1.0, 1.0 - ( areaOrg / areaExt ) );

#ifdef SKIP
	    cerr << HERE << " Sim Label # " << setw( 2 ) << j
		 << " areaOrg = " << areaOrg << " areaExt = " << areaExt
		 << " cost = " << costR << endl;
#endif	// SKIP
	    itemCost.push_back( costR );
	}
	dataCostSim.push_back( itemCost );
    }
    _normalizeMatrix( dataCostSim, Drawing::data_cost_lower, Drawing::data_cost_upper );
#endif	// USING_SIMILARITY_CONJOINING
	
//------------------------------------------------------------------------------
//	data costs for the design gestalts
//------------------------------------------------------------------------------
    vector< vector< double > > dataCostDes;
    for ( unsigned int i = 0; i < nVertices; ++i ) {
	vector< double > itemCost;
	unsigned int idS = i;

	// cerr << HERE << " Polygon ID = " << idS << endl;
	
	for ( unsigned int j = 0; j < _labelDes.size(); ++j ) {
	    Set curItem = _labelDes[ j ];
	    Polygon2 hullOrg;

	    // Just use the convex hull for computing data costs
	    _convexForLabel( net, curItem, hullOrg );

	    double areaOrg = hullOrg.area();
	    if ( ! hullOrg.is_simple() )
		areaOrg *= 1.0e-8;
	    else if ( hullOrg.orientation() == CGAL::CLOCKWISE )
		areaOrg *= -1.0;

	    if ( ! curItem.isContained( idS ) ) {
		curItem.push_back( idS );
	    }
	    Polygon2 hullExt;

	    // Just use the convex hull for computing data costs
	    _convexForLabel( net, curItem, hullExt );

	    double areaExt = hullExt.area();
	    if ( ! hullExt.is_simple() )
		areaExt *= 1.0e-8;
	    else if ( hullExt.orientation() == CGAL::CLOCKWISE )
		areaExt *= -1.0;
	    
	    assert( areaOrg >= 0.0 );
	    assert( areaExt >= 0.0 );

	    double costR =  min( 1.0, 1.0 - ( areaOrg / areaExt ) );
#ifdef SKIP
	    cerr << HERE << " curItem : ";
	    for ( unsigned int k = 0; k < curItem.size(); ++k )
		cerr << "[ " << curItem[ k ] << " ] ";
	    cerr << endl;
#endif	// SKIP
		
#ifdef SKIP
	    cerr << HERE << " Prx Label # " << setw( 2 ) << j
		 << " areaOrg = " << areaOrg << " areaExt = " << areaExt
		 << " cost = " << costR << endl;
#endif	// SKIP
	    itemCost.push_back( costR );
	}
	dataCostDes.push_back( itemCost );
    }
    // Skip normalizing the data cost for design labels
    _normalizeMatrix( dataCostDes, Drawing::data_cost_lower, Drawing::data_cost_upper );

    // Intentionally assign minimal data cost to the polygons specifically
    // selected for aggregation
    // noted by ST on 2023/10/21
    for ( unsigned int j = 0; j < _labelDes.size(); ++j ) {
	Set curMember = _labelDes[ j ];
	for ( unsigned int k = 0; k < curMember.size(); ++k )
	    dataCostDes[ curMember[ k ] ][ j ] = DESIGN_DATA_COST;
    }

    // std::this_thread::sleep_for( std::chrono::milliseconds( 5000 ) );

//------------------------------------------------------------------------------
//	Combine data costs for proximity and similarity 
//------------------------------------------------------------------------------
    _dataCost.clear();
    for ( unsigned int i = 0; i < nVertices; ++i ) {
	vector< double > costForLabel = dataCostSgl[ i ];
	costForLabel.insert( costForLabel.end(),
			     dataCostPrx[ i ].begin(), dataCostPrx[ i ].end() );
#ifdef USING_SIMILARITY_CONJOINING
	costForLabel.insert( costForLabel.end(),
			     dataCostSim[ i ].begin(), dataCostSim[ i ].end() );
#endif	// USING_SIMILARITY_CONJOINING
	costForLabel.insert( costForLabel.end(),
			     dataCostDes[ i ].begin(), dataCostDes[ i ].end() );
	_dataCost.push_back( costForLabel );
    }

    // #ifdef PRINT_COSTS
    //------------------------------------------------------------------------------
    //	Print out the data costs
    //------------------------------------------------------------------------------
    ofstream ofs_data( DATA_COST_FILE );
    streambuf* buf_data;
    if ( ofs_data ) buf_data = cerr.rdbuf( ofs_data.rdbuf() );
    cerr << HERE <<  " Data costs " << endl;
    cerr << "      ";
    for ( unsigned int j = 0; j < _dataCost[ 0 ].size(); ++j ) {
	cerr << "[" << setw( 3 ) << j << "] ";
    }
    cerr << endl;
    for ( unsigned int i = 0; i < _dataCost.size(); ++i ) {
	cerr << "[" << setw( 3 ) << i << "] ";
	for ( unsigned int j = 0; j < _dataCost[ i ].size(); ++j ) {
	    double cost = _dataCost[ i ][ j ];
	    cerr << std::fixed << setprecision( 1 ) << setw( 5 ) << cost << " ";
	}
	cerr << endl;
    }
    if ( ofs_data ) {
	cerr.rdbuf( buf_data );
	ofs_data.close();
    }
    //#endif	// PRINT_COSTS

    cerr << HERE << " Obtained data costs for all labels" << endl;
}


//
//  Drawing::_calcSmoothCost --	calculate the smooth cost for each pair of
//				polygons in term of the labels assigned to the
//				end polygons
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Drawing::_calcSmoothCost( void )
{
    Network &		net		= _netNbr;
    unsigned int	nVertices	= num_vertices( net );

    // initialize the smooth cost
    _smoothCost.clear();
    
    for ( unsigned int i = 0; i < nVertices; ++i ) {
	vector< double > pairCost;
	for ( unsigned int j = 0; j < nVertices; ++j ) {
	    if ( i == j ) pairCost.push_back( 0.0 );
	    else {
		double dist = _distPolys( i, j );
#ifdef SQUARED_SMOOTH_COST
		pairCost.push_back( 1.0/(dist*dist) );
#else	// SQUARED_SMOOTH_COST
		pairCost.push_back( 1.0/dist );
#endif	// SQUARED_SMOOTH_COST
	    }
	}
	_smoothCost.push_back( pairCost );
    }
    _normalizeMatrix( _smoothCost, Drawing::smooth_cost_lower, Drawing::smooth_cost_upper );

    if ( _smoothCost.size() == 0 ) {
	cerr << HERE << " No smoothness costs" << endl;
	cerr << HERE << " smooth_cost_upper = " << Drawing::smooth_cost_upper << endl;
	cerr << HERE << " smooth_cost_lower = " << Drawing::smooth_cost_lower << endl;
	return;
    }

    //#ifdef PRINT_COSTS
    //------------------------------------------------------------------------------
    //	Print out the smoothness costs
    //------------------------------------------------------------------------------
    ofstream ofs_smooth( SMOOTH_COST_FILE );
    streambuf* buf_smooth;
    if ( ofs_smooth ) buf_smooth = cerr.rdbuf( ofs_smooth.rdbuf() );
    cerr << HERE <<  " Smoothness costs " << endl;
    cerr << "      ";
    for ( unsigned int j = 0; j < _smoothCost[ 0 ].size(); ++j ) {
	cerr << "[" << setw( 3 ) << j << "] ";
    }
    cerr << endl;
    for ( unsigned int i = 0; i < _smoothCost.size(); ++i ) {
	cerr << "[" << setw( 3 ) << i << "] ";
	for ( unsigned int j = 0; j < _smoothCost[ i ].size(); ++j ) {
	    double cost = _smoothCost[ i ][ j ];
	    cerr << std::fixed << setprecision( 1 ) << setw( 5 ) << cost << " ";
	}
	cerr << endl;
    }
    if ( ofs_smooth ) {
	cerr.rdbuf( buf_smooth );
	ofs_smooth.close();
    }
    //#endif	// PRINT_COSTS

    cerr << HERE << " Obtained smooth costs for all labels" << endl;
}

//
//  Drawing::_calcLabelCost --	calculate the label cost for each label
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Drawing::_calcLabelCost( void )
{
//------------------------------------------------------------------------------
//	label costs for the individual primitives
//------------------------------------------------------------------------------
    _hullSgl.clear();
    vector< double > costSgl;
    costSgl.clear();
    Network & net = _netNbr;
    unsigned int nVertices = num_vertices( net );
    for ( unsigned int i = 0; i < nVertices; ++i ) {
	_hullSgl.push_back( _poly[ i ] );
	double cost = Drawing::label_cost_single;
	costSgl.push_back( cost );
   }

//------------------------------------------------------------------------------
//	label costs for the proximity gestalts
//------------------------------------------------------------------------------
    _hullPrx.clear();
    _boundPrx.clear();
    vector< double > costPrx;
    costPrx.clear();
    for ( unsigned int i = 0; i < _labelPrx.size(); ++i ) {
	Polygon2 hull;
	_concaveForLabel( _netNbr, _labelPrx[ i ], hull );
	_boundPrx.push_back( hull );
	_hullPrx.push_back( hull );
	double cost = _proximityCost( _netNbr, _labelPrx[ i ], hull );
	costPrx.push_back( cost );
    }
    _normalizeVector( costPrx, Drawing::label_cost_lower, Drawing::label_cost_upper );
    
//------------------------------------------------------------------------------
//	label costs for the similarity gestalts
//------------------------------------------------------------------------------
#ifdef USING_SIMILARITY_CONJOINING
    _hullSim.clear();
    _boundSim.clear();
    vector< double > costSim;
    costSim.clear();
    for ( unsigned int i = 0; i < _labelSim.size(); ++i ) {
	Polygon2 hull;
	_concaveForLabel( _netNbr, _labelSim[ i ], hull );
	_boundSim.push_back( hull );
	_hullSim.push_back( hull );
	double cost = _similarityCost( _netNbr, _labelSim[ i ] );
	costSim.push_back( cost );
    }
    _normalizeVector( costSim, Drawing::label_cost_lower, Drawing::label_cost_upper );
#endif	// USING_SIMILARITY_CONJOINING

//------------------------------------------------------------------------------
//	label costs for the design gestalts
//------------------------------------------------------------------------------
    _hullDes.clear();
    _boundDes.clear();
    vector< double > costDes;
    costDes.clear();
    for ( unsigned int i = 0; i < _labelDes.size(); ++i ) {
	Polygon2 hull;
	_concaveForLabel( _netNbr, _labelDes[ i ], hull );
	_boundDes.push_back( hull );
	_hullDes.push_back( hull );
	// Intentionally assign minimal label cost to the polygons specifically
	// selected for aggregation
	// noted by ST on 2023/10/21
	double cost = DESIGN_LABEL_COST;
	costDes.push_back( cost );
    }
    // Skip normalizing the label cost for design labels
    // _normalizeVector( costDes, Drawing::label_cost_lower, Drawing::label_cost_upper );

//------------------------------------------------------------------------------
//	Combine sets of convex hulls
//------------------------------------------------------------------------------
    _hullAll = _hullSgl;
    _hullAll.insert( _hullAll.end(), _hullPrx.begin(), _hullPrx.end() );
#ifdef USING_SIMILARITY_CONJOINING
    _hullAll.insert( _hullAll.end(), _hullSim.begin(), _hullSim.end() );
#endif	// USING_SIMILARITY_CONJOINING
    _hullAll.insert( _hullAll.end(), _hullDes.begin(), _hullDes.end() );
    
    _boundAll = _boundSgl;
    _boundAll.insert( _boundAll.end(), _boundPrx.begin(), _boundPrx.end() );
#ifdef USING_SIMILARITY_CONJOINING
    _boundAll.insert( _boundAll.end(), _boundSim.begin(), _boundSim.end() );
#endif	// USING_SIMILARITY_CONJOINING
    _boundAll.insert( _boundAll.end(), _boundDes.begin(), _boundDes.end() );

    _labelCost = costSgl;
    _labelCost.insert( _labelCost.end(), costPrx.begin(), costPrx.end() );
#ifdef USING_SIMILARITY_CONJOINING
    _labelCost.insert( _labelCost.end(), costSim.begin(), costSim.end() );
#endif	// USING_SIMILARITY_CONJOINING
    _labelCost.insert( _labelCost.end(), costDes.begin(), costDes.end() );
    
    cerr << HERE << " Obtained convex hulls for all labels" << endl;

#ifdef SKIP
    cerr << HERE << " Concatenated all the lists of penalty values" << endl;
    for ( unsigned int i = 0; i < costSgl.size(); ++i )
	cerr << costSgl[ i ] << " ";
    cerr << endl;
    for ( unsigned int i = 0; i < costPrx.size(); ++i )
	cerr << costPrx[ i ] << " ";
    cerr << endl;
    for ( unsigned int i = 0; i < costSim.size(); ++i )
	cerr << costSim[ i ] << " ";
    cerr << endl;
    for ( unsigned int i = 0; i < costDes.size(); ++i )
	cerr << costDes[ i ] << " ";
    cerr << endl;
    for ( unsigned int i = 0; i < _costAll.size(); ++i )
	cerr << _costAll[ i ] << " ";
    cerr << endl;
#endif	// SKIP

    //------------------------------------------------------------------------------
    //	Print out the label costs
    //------------------------------------------------------------------------------
    //#ifdef PRINT_COSTS
    ofstream ofs_label( LABEL_COST_FILE );
    streambuf* buf_label;
    if ( ofs_label ) buf_label = cerr.rdbuf( ofs_label.rdbuf() );
    cerr << HERE <<  " Label costs " << endl;
    cerr << "      ";
    for ( unsigned int i = 0; i < _labelCost.size(); ++i ) {
	cerr << "[" << setw( 3 ) << i << "] ";
    }
    cerr << endl;
    cerr << "      ";
    for ( unsigned int i = 0; i < _labelCost.size(); ++i ) {
	double cost = _labelCost[ i ];
	cerr << std::fixed << setprecision( 1 ) << setw( 5 ) << cost << " ";
    }
    cerr << endl;
    if ( ofs_label ) {
	cerr.rdbuf( buf_label );
	ofs_label.close();
    }
    //#endif	// PRINT_COSTS
}

//
//  Drawing::_graphCut --	label cost optimization through graph cut
//
//  Inputs
//	none:
//
//  Outputs
//	set of labels (polygon ID sets) as a group of Gestalts
//
void Drawing::_graphCut( void )
{
    // First of all, clear up the set of expansions
    _expand.clear();

    if ( _labelAll.size() < 2 ) {
	cerr << HERE << ">>>>>>>>>> Skip optimization with less label assignments" << endl;
	cerr << HERE << " Number of labels = " << _labelAll.size() << endl;
	cerr << HERE << " Number of labels must be 2 or more" << endl;
#ifdef SKIP
	vector< Set > dummy;
	return dummy;
#endif	// SKIP
	return;
    }

    unsigned int	numLabels	= _labelAll.size();
    
    Network &		net		= _netNbr;
    unsigned int	numPixels	= num_vertices( net );
    // NetEdgeNInvMap		edgeNInv	= get( edge_myninv, net );

    vector< Link >	edge;
    vector< int >	weight;

//------------------------------------------------------------------------------
//	Smooth cost data pool
//------------------------------------------------------------------------------
    NetVertexIDMap	vertexID	= get( vertex_myid, net );
    cerr << HERE << " Graph topology in GraphCut computation " << endl;
    BGL_FORALL_EDGES( ed, net, Network ) {
	NetVertexDescriptor vdS = source( ed, net );
	NetVertexDescriptor vdT = target( ed, net );
	unsigned int idS = vertexID[ vdS ];
	unsigned int idT = vertexID[ vdT ];
	// int w = edgeNInv[ ed ] * MAX_QUANTIZATION_LEVEL;
	int w = round( COST_SCALE * _smoothCost[ idS ][ idT ] );
	cerr << "( " << setw( 3 ) << idS << " -- " << setw( 3 ) << idT
	     << " : " << setw( 3 ) << w << " ) " << ends;
	edge.push_back( Link( idS, idT ) );
	weight.push_back( w );
    }
    cerr << endl;

//------------------------------------------------------------------------------
//	Data cost table
//------------------------------------------------------------------------------
    vector< vector< int > > dc;
    for ( unsigned int i = 0; i < _dataCost.size(); ++i ) {
	vector< int > onerow;
	for ( unsigned int j = 0; j < _dataCost[ i ].size(); ++j ) {
	    // int cost = _dataCost[ i ][ j ] * MAX_QUANTIZATION_LEVEL;
	    int cost = round( COST_SCALE * _dataCost[ i ][ j ] );
	    onerow.push_back( cost );
	}
	dc.push_back( onerow );
    }

//------------------------------------------------------------------------------
//	Label cost array
//------------------------------------------------------------------------------
    vector< int > lc;		// label cost array
    for ( unsigned int i = 0; i < _labelCost.size(); ++i ) {
	// int cost = _labelCost[ i ] * MAX_QUANTIZATION_LEVEL;
	int cost = round( COST_SCALE * _labelCost[ i ] );
	lc.push_back( cost );
    }
    
//------------------------------------------------------------------------------
//	Iterative optimization
//------------------------------------------------------------------------------
    Expansion::nLabels = numLabels;
    vector< unsigned int > optGroup;
    int optEnergy = INFINITY_COST;
    // _expand.clear();
    for ( unsigned int m = 0; m < NUM_TRIALS; ++m ) {
	int curEnergy =
	    // graph_cut( numPixels, numLabels, edge, weight, dc, lc, curGroup );
	    iterative_graph_cut( numPixels, numLabels, edge, weight, dc, lc, _expand );
	if ( curEnergy < optEnergy ) {
	    optEnergy = curEnergy;
	    optGroup = _expand.back().group();
	}
    }
	
    // sort expansion samples
    //map< int, Expansion, greater< int >  > expandMap;
    map< int, Expansion > expandMap;
    for ( unsigned int k = 0; k < _expand.size(); ++k ) {
	expandMap.insert( pair< int, Expansion >( _expand[ k ].totalCost(),
						  _expand[ k ] ) );
    }
    _expand.clear();
    for ( map< int, Expansion >::iterator iter = expandMap.begin();
	  iter != expandMap.end(); iter++ ) {
	_expand.push_back( iter->second );
    }

//------------------------------------------------------------------------------
//	Very important: Delete the last expansion HERE!!
//------------------------------------------------------------------------------
    _expand.erase( _expand.end() - 1 );


#ifdef DEBUG
    for ( unsigned int k = 0; k < _expand.size(); ++k ) {
	cerr << HERE << _expand[ k ] << " " << ends;
	vector< Set > cluster = _expand[ k ].cluster();
	cerr << HERE << " expand.cluster size = " << cluster.size() << " : " << ends;
	for ( unsigned int m = 0; m < cluster.size(); ++m ) {
	    cerr << "++++++++++++++++++++" << endl;
	    cerr << cluster[ m ] << endl;
	}
	cerr << "++++++++++++++++++++" << endl;
    }
#endif	// DEBUG
    
#ifdef SKIP
    // collect polygon IDs for each label
    vector< Set > team( numLabels );
    for ( unsigned int k = 0; k < numLabels; ++k ) team[ k ].clear();
#endif	// SKIP

    cerr << "##############################" << endl;
    cerr << "##############################" << endl;
    cerr << HERE << " Optimized cost = " << optEnergy << endl;
    cerr << HERE << " Optimized labels : " << endl;
    cerr << "      " << ends;
    for ( unsigned int i = 0; i < optGroup.size(); ++i ) {
	cerr << "[" << setw( 3 ) << i << "] " << ends;
    }
    cerr << endl;
    cerr << "      " << ends;
    for ( unsigned int i = 0; i < optGroup.size(); ++i )
	cerr << setw( 5 ) << optGroup[ i ] << " " << ends;
    cerr << endl;
    cerr << "##############################" << endl;

#ifdef SKIP
    cerr << HERE << " Grouping by labels" << endl;
    for ( unsigned int i = 0; i < optGroup.size(); ++i ) {
	unsigned int labelID = optGroup[ i ];
	cerr << " " << labelID << ends;
	team[ labelID ].push_back( i );
    }
    cerr << endl;

    // delete the empty labels
    unsigned int count = 0;
    while ( count < numLabels ) {
	unsigned int index = numLabels - 1 - count;
	if ( team[ index ].size() == 0 ) {
#ifdef DEBUG
	    cerr << HERE << " Deleting " << index << "-th team label" << endl;
#endif	// DEBUG
	    team.erase( team.begin() + index );
	}
	count++;
#ifdef DEBUG
	cerr << HERE << " Incrementing the count to " << count << endl;
#endif	// DEBUG
    }
    cerr << HERE << " Number of valid labels = " << team.size() << endl;

    return team;
#endif	// SKIP

    return;
}
 

//
//  Drawing::_aggregateLabels --	aggreate Gestalt polygons of the given labels
//
//  Inputs
//	gestalt :	set of labels (polygon ID sets) as a group of Gestalts
//
//  Outputs
//	none
//
void Drawing::_aggregateLabels( const vector< Set > & gestalt,
				const vector< Polygon2 > & layout,
				const vector< Set > & index )
{
#ifdef SKIP
    for ( unsigned int i = 0; i < index.size(); ++i ) {
	cerr << HERE << " i = " << i << index[ i ];
	getchar();
    }
#endif	// SKIP

    // Network &		net	 = _netNbr;
    vector< Polygon2 >	hullSet;
    vector< Polygon2 >	boundSet;
    vector< Set >	extrmSet;

    if ( ( gestalt.size() != layout.size() ) ||
	 ( layout.size() != index.size() ) ||
	 ( gestalt.size() != index.size() ) ) {
	cerr << HERE << " gestalt.size = " << gestalt.size()
	     << " layout.size = " << layout.size()
	     << " index.size = " << index.size()
	     << endl;
	assert( ( gestalt.size() == layout.size() ) &&
		( layout.size() == index.size() ) );
    }
    
    // Prepare the convex hull to be newly introduced
    for ( unsigned int k = 0; k < gestalt.size(); k++ ) {
#ifdef SKIP
	Polygon2 hull;
	Set glob;
	_concaveForLabel( net, gestalt[ k ], hull, glob );
	boundSet.push_back( hull );
#endif	// SKIP
	cerr << HERE << " Gestalt k = " << k << " : " << gestalt[ k ] << endl;
	cerr << HERE << " Gestalt k = " << k << " hull size = " << layout[ k ].size() << endl;
	const Polygon2 & hull = layout[ k ]; 
	const Set & glob = index[ k ]; 
	assert( hull.size() > 0 );
	hullSet.push_back( hull );
	extrmSet.push_back( glob );
    }
    
    // Collect the IDs of polygons to be removed
    Set idSet;
    for ( unsigned int k = 0; k < gestalt.size(); ++k ) {
	for ( unsigned int i = 0; i < gestalt[ k ].size(); ++i ) {
	    idSet.push_back( gestalt[ k ][ i ] );
	}
    }
    sort( idSet.begin(), idSet.end() );

    cerr << HERE << " IDs of polygon to be removed" << endl; 
    for ( unsigned int i = 0; i < idSet.size(); ++i ) {
	cerr << " " << idSet[ i ] << ends;
    }
    cerr << endl;

    // Shrink the line drawing
    unsigned int count = 0;
    unsigned int init_num_polys = _poly.size();
    while ( count < init_num_polys ) {
	unsigned int index = init_num_polys - 1 - count;
	if ( idSet.isContained( index ) ) {
#ifdef DEBUG
	    cerr << HERE << "########## Deleting " << index << "-th polygon" << endl;
#endif	// DEBUG
	    // Delete the polygon
	    _poly.erase( _poly.begin() + index );
	    // Delete the set of point numbers also
	    _glID.erase( _glID.begin() + index );
	    cerr << HERE << "########## Number of polygons " << _poly.size() << endl;
	}
	count++;
#ifdef DEBUG
	cerr << HERE << " Incrementing the count to " << count << endl;
#endif	// DEBUG
    }

    for ( unsigned int j = 0; j < hullSet.size(); ++j ) { 
	cerr << HERE << "*********** New convex hull # " << j
	     << " added. Number of corners = "
	     << hullSet[ j ].size() << endl;
	// Add the convex hull
	_poly.push_back( hullSet[ j ] );
	// Add the set of extreme point numbers also
	_glID.push_back( extrmSet[ j ] );
    }
    cerr << HERE << " new convex hull is added to the tail of the polygon list" << endl;

    cerr << HERE << " Print out the polygons " << endl;
    for ( unsigned int k = 0; k < _poly.size(); ++k ) {
#ifdef SKIP
	cerr << HERE << setw( 2 ) << k << "-th polygon : size = " <<
	    _poly[ k ].size() << endl;
#endif	// SKIP
	assert( _poly[ k ].size() > 0 );
#ifdef SKIP
	for ( unsigned int m = 0; m < _poly[ k ].size(); ++m ) {
	    cerr << "(" << _poly[ k ][ m ] << ") " << ends;
	}
	cerr << endl;
#endif	// SKIP
    }
}


//
//  Drawing::_simplifyPolys --	simplify contour polygons
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Drawing::_simplifyPolys( void )
{
    // const double limitArea = 0.01;
    for ( unsigned int k = 0; k < _bound.size(); ++k ) {
	Contour contour;
	contour.polygon() = _bound[ k ];
#ifdef OBSOLETE
#ifdef CHECK_CONFLICTS_WITH_OTHERS
	contour.obstacle().clear();
	for ( unsigned int i = 0; i < _bound.size(); ++i ) {
	    if ( i != k ) {
		for ( unsigned int j = 0; j < _bound[ i ].size(); ++j ) {
		    contour.obstacle().push_back( _bound[ i ].edge( j ) );
		}
	    }
	}
#endif	// CHECK_CONFLICTS_WITH_OTHERS
	contour.reset();
	
	// contour.setContractible();
	if ( ! contour.prepare() ) contour.clearContractible();
	
	bool isPossible = true;
	do {
	    isPossible = contour.simplifyByArea( INFINITY );
	} while ( isPossible );
#endif	// OBSOLETE


#ifdef CHECK_CONFLICTS_WITH_OTHERS
	contour.registerConflicts( k, _bound );
#endif	// CHECK_CONFLICTS_WITH_OTHERS
	contour.fullySimplify();
	_bound[ k ] = contour.polygon();
    }
    cerr << HERE << " Exhausive simplificaiton done." << endl;
    // _poly = _form;
}


//
//  Drawing::_squarePolys --	square contour polygons
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Drawing::_squarePolys( void )
{
    // const double limitArea = 0.01;
    // _bound = _poly;
    for ( unsigned int k = 0; k < _bound.size(); ++k ) {
	Contour contour;
	contour.polygon() = _bound[ k ];
	contour.reset();
	contour.squaring();

	_bound[ k ] = contour.polygon();
    }
    cerr << HERE << " Squaring polygons done." << endl;
    // _poly = _form;
}


//
//  Drawing::_squareOutlines --	prepare contour outlines with multiple squaring
//				criteria 
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Drawing::_squareOutlines( void )
{
    const int nBands = 5;
    const double bandwidth[ nBands ] = {
	180.0/40.0, 180.0/25.0, 180.0/20.0, 180.0/16.0, 180.0/10.0
    };

    vector< vector< Votes > >	proxy;
	
    proxy.resize( _bound.size() );
    _outline.clear();
    _outlineID.clear();
    _outline.resize( _bound.size() );
    _outlineID.resize( _bound.size() );
    for ( unsigned int i = 0; i < _bound.size(); ++i ) {
	for ( unsigned int k = 0; k < nBands; ++k ) {
	    Contour contour;
	    contour.polygon() = _bound[ i ];
	    contour.reset();
	    Votes reps = contour.squaring( bandwidth[ k ] );
	    
	    bool doExist = false;
	    for ( unsigned int j = 0; j < proxy[ i ].size(); ++j ) {
		if ( proxy[ i ][ j ].approxequal( reps ) ) doExist = true;
		if ( doExist ) break;
	    }
	    if ( ! doExist ) {
		proxy[ i ].push_back( reps );
		contour.registerConflicts( i, _bound );
		contour.fullySimplify();
		_outline[ i ].push_back( contour.polygon() );
	    }
	}
    }

    for ( unsigned int i = 0; i < _outline.size(); ++i ) {
	_outlineID[ i ] = _outline[ i ].size() - 1;
    }

    for ( unsigned int i = 0; i < proxy.size(); ++i ) {
	for ( unsigned int k = 0; k < proxy[ i ].size(); ++k ) {
	    cerr << HERE
		 << " i = " << i << " vote[ " << k << " ] = " << proxy[ i ][ k ] << endl;
	}
	cerr << endl;
    }
}


//
//  Drawing::_clear --	clear the data
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Drawing::_clear( void )
{
    _web.clear();
    _assoc.clear();
    _onelayer.clear();
    _twolayer.clear();
    _wrapper.clear();
    
    _poly.clear();
    _glID.clear();

    _netNbr.clear();
    _netPrx.clear();
#ifdef USING_SIMILARITY_CONJOINING
    _netSim.clear();
#endif	// USING_SIMILARITY_CONJOINING

    _labelPrx.clear();
#ifdef USING_SIMILARITY_CONJOINING
    _labelSim.clear();
#endif	// USING_SIMILARITY_CONJOINING
    _labelDes.clear();
    _labelAll.clear();

    _hullPrx.clear();
#ifdef USING_SIMILARITY_CONJOINING
    _hullSim.clear();
#endif	// USING_SIMILARITY_CONJOINING
    _hullDes.clear();
    _hullAll.clear();

    _boundPrx.clear();
#ifdef USING_SIMILARITY_CONJOINING
    _boundSim.clear();
#endif	// USING_SIMILARITY_CONJOINING
    _boundDes.clear();
    _boundAll.clear();

    _dataCost.clear();
    _smoothCost.clear();
    _labelCost.clear();


    Drawing::interval_threshold		= LIMIT_BUILDING_INTERVAL;
    Drawing::data_cost_lower		= DATA_COST_LOWER;
    Drawing::data_cost_upper		= DATA_COST_UPPER;
    Drawing::data_cost_inside		= DATA_COST_INSIDE;
}


//------------------------------------------------------------------------------
//	Public Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------

//
//  Drawing::Drawing --	default constructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Drawing::Drawing() 
{
    _clear();
#ifdef SKIP
    _poly.clear();
    _glID.clear();

    _netNbr.clear();
    _netPrx.clear();
    _netSim.clear();

    _labelPrx.clear();
#ifdef USING_SIMILARITY_CONJOINING
    _labelSim.clear();
#endif	// USING_SIMILARITY_CONJOINING
    _labelDes.clear();
    _labelAll.clear();

    _hullPrx.clear();
#ifdef USING_SIMILARITY_CONJOINING
    _hullSim.clear();
#endif	// USING_SIMILARITY_CONJOINING
    _hullDes.clear();
    _hullAll.clear();

    _dataCost.clear();
    _smoothCost.clear();
    _labelCost.clear();
#endif	// SKIP
}


//
//  Drawing::Drawing --	copy constructor
//
//  Inputs
//	obj	: object of this class
//
//  Outputs
//	none
//
Drawing::Drawing( const Drawing & obj )
{
    _clear();

    _poly	= obj._poly;
    _glID	= obj._glID;
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------

//
//  Drawing::~Drawing --	destructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Drawing::~Drawing()
{
    _clear();
}


//------------------------------------------------------------------------------
//	Referrring to members
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------

//
//  Drawing::operator = --	assignement
//
//  Inputs
//	obj	: objects of this class
//
//  Outputs
//	this object
//
Drawing & Drawing::operator = ( const Drawing & obj )
{
    if ( this != &obj ) {
	_poly	= obj._poly;
	_glID	= obj._glID;
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
ostream & operator << ( ostream & stream, const Drawing & obj )
{
    stream << obj._poly.size() << endl;
    for ( unsigned int i = 0; i < obj._poly.size(); ++i ) {
	stream << obj._poly[ i ].size() << endl;
	for ( unsigned int j = 0; j < obj._poly[ i ].size(); ++j ) {
	    stream << obj._poly[ i ][ j ] << endl;
	}
    }
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
istream & operator >> ( istream & stream, Drawing & obj )
{
    unsigned int nPolys;
    obj._poly.clear();

    stream >> nPolys;
    for ( unsigned int i = 0; i < nPolys; ++i ) {
	unsigned int nCnrs;
	stream >> nCnrs;
	Polygon2 curPoly;
	for ( unsigned int j = 0; j < nCnrs; ++j ) {
	    Point2 cnr;
	    stream >> cnr;
	    curPoly.push_back( cnr );
	}
	obj._poly.push_back( curPoly );
    }

    return stream;
}


// end of header file
// Do not add any stuff under this line.
