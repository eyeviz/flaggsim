//==============================================================================
// Network.h
//	: header file for networks
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-06-02 17:36:59 shigeo"
//
//==============================================================================

#ifndef	_Network_H
#define _Network_H

#include "Common.h"
#include "CGAL.h"
#include "Set.h"

#include <boost/config.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <boost/graph/connected_components.hpp>

using namespace boost;

//------------------------------------------------------------------------------
//	Macro switches
//------------------------------------------------------------------------------
#define USING_SIMILARITY_CONJOINING



// Customizing vertex properties
enum vertex_mylabel_t	{ vertex_mylabel }; // vertex label -> polygon ID in dijkstra
enum vertex_mypoly_t	{ vertex_mypoly }; // individual polygon
enum vertex_myglob_t	{ vertex_myglob }; // point global IDs
enum vertex_mycntr_t	{ vertex_mycntr }; // polygon center -> coordinates in dijkstra
enum vertex_myid_t	{ vertex_myid };

// Customizing edge properties
enum edge_myndst_t	{ edge_myndst };
enum edge_myninv_t	{ edge_myninv };
//------------------------------------------------------------------------------
// USING_SIMILARITY_CONJOINING is expected to be defined in Drawing.h
//------------------------------------------------------------------------------
#ifdef USING_SIMILARITY_CONJOINING
enum edge_myratio_t	{ edge_myratio };
enum edge_mynasp_t	{ edge_mynasp };
#endif	// USING_SIMILARITY_CONJOINING
enum edge_myid_t	{ edge_myid };

// Installing the associted vertex and edge properties
namespace boost {
    BOOST_INSTALL_PROPERTY( vertex, mylabel );	// unsigned int
    BOOST_INSTALL_PROPERTY( vertex, mypoly );	// 2D polygon
    BOOST_INSTALL_PROPERTY( vertex, mycntr );	// center of gravity
    BOOST_INSTALL_PROPERTY( vertex, myglob );	// global IDs of points
    BOOST_INSTALL_PROPERTY( vertex, myid );	// vertex ID

    BOOST_INSTALL_PROPERTY( edge,   myndst );	// normalized distance
    BOOST_INSTALL_PROPERTY( edge,   myninv );	// normalized inverse distance
#ifdef USING_SIMILARITY_CONJOINING
    BOOST_INSTALL_PROPERTY( edge,   myratio );	// aspect ratio
    BOOST_INSTALL_PROPERTY( edge,   mynasp );	// normalized aspect ratio
#endif	// USING_SIMILARITY_CONJOINING
    BOOST_INSTALL_PROPERTY( edge,   myid );	// edge ID
}


// Encapsulating vertex properties
typedef property< vertex_index_t, int >					NetVertexIndex;
typedef property< vertex_mylabel_t, unsigned int, NetVertexIndex >	NetVertexLabel;
typedef property< vertex_mypoly_t, Polygon2, NetVertexLabel >		NetVertexPoly;
typedef property< vertex_myglob_t, Set, NetVertexPoly >			NetVertexGlob;
typedef property< vertex_mycntr_t, Point2, NetVertexGlob >		NetVertexCntr;
typedef property< vertex_myid_t, unsigned int, NetVertexCntr >		NetVertexProperty;

// Encampsulating edge properties
typedef property< edge_weight_t, double >				NetEdgeWeight;
typedef property< edge_myndst_t, double, NetEdgeWeight >		NetEdgeNDst;
typedef property< edge_myninv_t, double, NetEdgeNDst >			NetEdgeNInv;
#ifdef USING_SIMILARITY_CONJOINING
typedef property< edge_myratio_t, double, NetEdgeNInv >			NetEdgeRatio;
typedef property< edge_mynasp_t, double, NetEdgeRatio >			NetEdgeNAsp;
typedef property< edge_myid_t, unsigned int, NetEdgeNAsp >		NetEdgeProperty;
#else	// USING_SIMILARITY_CONJOINING
typedef property< edge_myid_t, unsigned int, NetEdgeNInv >		NetEdgeProperty;
#endif	// USING_SIMILARITY_CONJOINING

// Graph definition with specific vertex and edge properties
#define ENABLE_CONNECTED_COMPONENTS
#ifdef ENABLE_CONNECTED_COMPONENTS
// [Notice]
// The triplet "vecS, vecS, undirectedS" is mandatory for enabling the
// computation for connected components.
// The triplet "vecS, listS, undirectedS" does not work.
// ( The triplet "listS, vecS, undirectedS" may work, but do not check it. )
// by ST on 2022/01/01
typedef adjacency_list< vecS, vecS, undirectedS,
			NetVertexProperty,
			NetEdgeProperty >				Network;
#else	// ENABLE_CONNECTED_COMPONENTS
typedef adjacency_list< vecS, listS, undirectedS,
			NetVertexProperty,
			NetEdgeProperty >				Network;
#endif	// ENABLE_CONNECTED_COMPONENTS

// Definitions for network components
typedef graph_traits< Network >						NetworkTraits;
typedef graph_traits< Network >::vertex_descriptor			NetVertexDescriptor;
typedef graph_traits< Network >::edge_descriptor			NetEdgeDescriptor;

typedef graph_traits< Network >::vertex_iterator			NetVertexIterator;
typedef graph_traits< Network >::edge_iterator				NetEdgeIterator;
typedef graph_traits< Network >::out_edge_iterator			NetOutEdgeIterator;

// Customizing vertex property maps
typedef property_map< Network, vertex_index_t >::type		NetVertexIndexMap;
typedef property_map< Network, vertex_mylabel_t >::type		NetVertexLabelMap;
typedef property_map< Network, vertex_mypoly_t >::type		NetVertexPolyMap;
typedef property_map< Network, vertex_myglob_t >::type		NetVertexGlobMap;
typedef property_map< Network, vertex_mycntr_t >::type		NetVertexCntrMap;
typedef property_map< Network, vertex_myid_t >::type		NetVertexIDMap;

// Customizing edge property maps
typedef property_map< Network, edge_weight_t >::type		NetEdgeWeightMap;
typedef property_map< Network, edge_myndst_t >::type		NetEdgeNDstMap;
typedef property_map< Network, edge_myninv_t >::type		NetEdgeNInvMap;
#ifdef USING_SIMILARITY_CONJOINING
typedef property_map< Network, edge_myratio_t >::type		NetEdgeRatioMap;
typedef property_map< Network, edge_mynasp_t >::type		NetEdgeNAspMap;
#endif	// USING_SIMILARITY_CONJOINING
typedef property_map< Network, edge_myid_t >::type		NetEdgeIDMap;

//------------------------------------------------------------------------------
//	Function declaration
//------------------------------------------------------------------------------

inline bool isConnected( unsigned int idS, unsigned int idT, Network & net )
{
    return edge( vertex( idS, net ), vertex( idT, net ), net ).second;
}

inline bool isConnected( NetVertexDescriptor & vdS, NetVertexDescriptor & vdT,
			 Network & net )
{
    return edge( vdS, vdT, net ).second;
}

#endif	// _Network_H
