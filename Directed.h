//==============================================================================
// Directed.h
//	: header file for directed graphs/networks
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2022-07-13 21:39:58 shigeo"
//
//==============================================================================

#ifndef	_Directed_H
#define _Directed_H

#include "Common.h"
#include "CGAL.h"
#include "Set.h"

#include <boost/config.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <boost/graph/connected_components.hpp>

using namespace boost;

// Customizing vertex properties
enum vertex_mygroup_t	{ vertex_mygroup };	// vertex label -> group for polygon ID in dijkstra
enum vertex_mycode_t	{ vertex_mycode };	// vertex global IDs
enum vertex_mypoint_t	{ vertex_mypoint };	// cntr -> point
enum vertex_myaddr_t	{ vertex_myaddr };	// id -> addr

// Customizing edge properties
enum edge_myaddr_t	{ edge_myaddr }; // id -> addr

// Installing the associted vertex and edge properties
namespace boost {
    BOOST_INSTALL_PROPERTY( vertex, mygroup );	// unsigned int
    BOOST_INSTALL_PROPERTY( vertex, mycode );	// unsigned int
    BOOST_INSTALL_PROPERTY( vertex, mypoint );	// coordinates of corner vertices
    BOOST_INSTALL_PROPERTY( vertex, myaddr );	// vertex ID

    BOOST_INSTALL_PROPERTY( edge,   myaddr );	// edge ID
}


// Encapsulating vertex properties
typedef property< vertex_index_t, int >					DirVertexIndex;
typedef property< vertex_mygroup_t, unsigned int, DirVertexIndex >	DirVertexGroup;
typedef property< vertex_mycode_t, unsigned int, DirVertexGroup >	DirVertexCode;
typedef property< vertex_mypoint_t, Point2, DirVertexCode >		DirVertexPoint;
typedef property< vertex_myaddr_t, unsigned int, DirVertexPoint >	DirVertexProperty;

// Encampsulating edge properties
typedef property< edge_weight_t, double >				DirEdgeWeight;
typedef property< edge_myaddr_t, unsigned int, DirEdgeWeight >		DirEdgeProperty;

// Graph definition with specific vertex and edge properties
#define ENABLE_CONNECTED_COMPONENTS
#ifdef ENABLE_CONNECTED_COMPONENTS
// [Notice]
// The triplet "vecS, vecS, undirectedS" is mandatory for enabling the
// computation for connected components.
// The triplet "vecS, listS, undirectedS" does not work.
// ( The triplet "listS, vecS, undirectedS" may work, but do not check it. )
// by ST on 2022/01/01
typedef adjacency_list< vecS, vecS, directedS,
			DirVertexProperty,
			DirEdgeProperty >			Directed;
#else	// ENABLE_CONNECTED_COMPONENTS
typedef adjacency_list< vecS, listS, directedS,
			DirVertexProperty,
			DirEdgeProperty >			Directed;
#endif	// ENABLE_CONNECTED_COMPONENTS

// Definitions for network components
typedef graph_traits< Directed >				DirectedTraits;
typedef graph_traits< Directed >::vertex_descriptor		DirVertexDescriptor;
typedef graph_traits< Directed >::edge_descriptor		DirEdgeDescriptor;

typedef graph_traits< Directed >::vertex_iterator		DirVertexIterator;
typedef graph_traits< Directed >::edge_iterator			DirEdgeIterator;
typedef graph_traits< Directed >::out_edge_iterator		DiOutEdgeIterator;

// Customizing vertex property maps
typedef property_map< Directed, vertex_index_t >::type		DirVertexIndexMap;
typedef property_map< Directed, vertex_mygroup_t >::type	DirVertexGroupMap;
typedef property_map< Directed, vertex_mycode_t >::type		DirVertexCodeMap;
typedef property_map< Directed, vertex_mypoint_t >::type	DirVertexPointMap;
typedef property_map< Directed, vertex_myaddr_t >::type		DirVertexAddrMap;

// Customizing edge property maps
typedef property_map< Directed, edge_weight_t >::type		DirEdgeWeightMap;
typedef property_map< Directed, edge_myaddr_t >::type		DirEdgeAddrMap;

#ifdef SKIP

//------------------------------------------------------------------------------
//	Function declaration
//------------------------------------------------------------------------------
inline bool isConnected( unsigned int idS, unsigned int idT, Directed & dnet )
{
    return edge( vertex( idS, dnet ), vertex( idT, dnet ), dnet ).second;
}
#endif	// SKIP

inline bool isConnected( DirVertexDescriptor & vdS, DirVertexDescriptor & vdT,
			 Directed & dnet )
{
    return edge( vdS, vdT, dnet ).second;
}


#endif	// _Directed_H
