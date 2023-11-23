#ifndef _CGAL_H		// begining of header file
#define _CGAL_H		// notifying that this file is included

#include <vector>
using namespace std;

// for fundamentals 
#include <CGAL/Cartesian.h>
#include <CGAL/Bbox_2.h>
#include <CGAL/Polygon_2.h>

// for convex hull computation
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/Convex_hull_traits_adapter_2.h>
#include <CGAL/property_map.h>

//------------------------------------------------------------------------------
//	CGAL type definitions
//------------------------------------------------------------------------------
// typedef CGAL::Cartesian< double >			Kernel;
typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;

typedef Kernel::Vector_2				Vector2;
typedef Kernel::Point_2					Point2;
typedef Kernel::Direction_2				Direction2;
typedef Kernel::Line_2					Line2;
//typedef CGAL::Segment_2< Kernel >			Segment2;
typedef Kernel::Segment_2				Segment2;
typedef vector< Point2 >::iterator			VertexIterator;
typedef CGAL::Polygon_2< Kernel >			Polygon2;
typedef CGAL::Triangle_2< Kernel >			Triangle2;
typedef CGAL::Object					Object;
// typedef Vertex_iterator					VIterator;

typedef CGAL::Aff_transformation_2< Kernel >		Transformation; 

typedef CGAL::Bbox_2					Bbox2;

typedef vector< Point2 >				Polyline2;

typedef pair< unsigned int, Point2 >			Geo;
typedef pair< Geo, Geo >				GeoPair;
typedef pair< GeoPair, Geo >				GeoTuple;

typedef Kernel::Vector_3				Vector3;

typedef CGAL::Convex_hull_traits_adapter_2<Kernel,
        CGAL::Pointer_property_map<Point2>::type > Convex_hull_traits_2;

#endif // _CGAL_H

// end of header file
// Do not add any stuff under this line.
