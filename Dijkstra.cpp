//==============================================================================
// Dijkstra.cpp
//	: program file for dijkstra shortest path search
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-05-16 22:05:11 shigeo"
//
//==============================================================================

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include "Dijkstra.h"

//#include <boost/config.hpp>
//#include <boost/graph/graph_traits.hpp>
//#include <boost/graph/adjacency_list.hpp>
//#include <boost/graph/iteration_macros.hpp>

#include <boost/graph/dijkstra_shortest_paths.hpp>
using namespace boost;

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Functions
//------------------------------------------------------------------------------

Set shortestPath( unsigned int idS, unsigned int idT, Directed & dig )
{
    DirVertexAddrMap	vertexAddr	= get( vertex_myaddr, dig );
    DirVertexCodeMap	vertexCode	= get( vertex_mycode, dig );
    // DirEdgeWeightMap		edgeWeight	= get( edge_weight, net );

    // Check the maximum vertex ID
    unsigned int maxCode = 0;
    BGL_FORALL_VERTICES( vd, dig, Directed ) {
	if ( maxCode < vertexCode[ vd ] )
	    maxCode = vertexCode[ vd ];
    }
    
    unsigned int nCodes = maxCode + 1;
    // cerr << HERE << " nCodes = " << nCodes << endl;
    // cerr << HERE << " idS = " << idS << " idT = " << idT << endl;
    // vector for parents
    vector< DirVertexDescriptor > parent( nCodes );
    // vector for storing distance property
    vector< double > distance( nCodes );

    // starting point
    DirVertexDescriptor vdS = vertex( idS, dig );

    dijkstra_shortest_paths( dig, vdS,
			     predecessor_map( &parent[0] ).distance_map( &distance[0] ) );
    
#ifdef SKIP
    cerr << "distances from : " << idS << endl;
    BGL_FORALL_VERTICES( vd, net, Network ) {
	int idV = vertexAddr[ vd ];
	cerr << " to " << idV << " = " << distance[ idV ] << ", "
	     << " Previous city = " << vertexAddr[ parent[ vd ] ] << endl;
    }
#endif	// SKIP

    DirVertexDescriptor vdT = vertex( idT, dig );
#ifdef SKIP
    // cerr << HERE << " size of parents = " << parent.size() << endl;
    cerr << HERE << " Shortast path from " << vertexAddr[ vdS ] << " to "
	 << vertexAddr[ vdT ] << " : " << endl;
#endif	// SKIP
    DirVertexDescriptor vd = vdT;
    stack< DirVertexDescriptor > revPath;
    while ( vd != vdS ) {
#ifdef SKIP
	cerr << " " << vertexAddr[ vd ] << ends;
#endif	// SKIP
	revPath.push( vd );
	vd = parent[ vd ];
    }
#ifdef SKIP
    cerr << " " << vertexAddr[ vd ] << endl;
#endif	// SKIP
    revPath.push( vd );
    
    DirVertexDescriptor prev = revPath.top();
    // DirVertexDescriptor prev;
    revPath.pop();
    Set path;
    while ( ! revPath.empty() ) {
	DirVertexDescriptor next = revPath.top();
	path.push_back( vertexAddr[ next ] );
	// DirEdgeDescriptor ed = edge( prev, next, net ).first;
#ifdef SKIP
	cerr  << " --( " << edgeWeight[ ed ] << " )-- " << vertexAddr[ next ] << ends;
#endif	// SKIP
	prev = next;
	revPath.pop();
    }
#ifdef SKIP
    cerr << endl;
#endif	// SKIP

    return path;
}



// end of header file
// Do not add any stuff under this line.
