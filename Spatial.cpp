//==============================================================================
// Spatial.cpp
//	: program file for geometric (spatial) neighbor graphs
//
//------------------------------------------------------------------------------
//
//				Time-stamp: "2023-07-24 23:08:17 shigeo"
//
//==============================================================================

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include "Spatial.h"


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
//	Functions
//------------------------------------------------------------------------------

void geometricNeighbors( const int & dims,
			 const vector< vector< double > > & sample,
			 const NeighborType method,
			 const double beta,
			 vector< Link > & link )
{
    int num_points = sample.size();
    vector< double* > points;
    for ( int i = 0; i < num_points; ++i ) {
	double* p = new double[ dims ];
	for ( int k = 0; k < dims; ++k ) {
	    p[ k ] = sample[ i ][ k ];
	}
	points.push_back( p );
    }
    // Create double precision vector space.
    ngl::DoubleVectorSpace space( dims );
    
    // Instantiate neighbor graph builder.
    ngl::NeighborhoodBuilderD* builder;
    switch ( method ) {
      case RNG:
	  builder = new ngl::RelativeNeighborGraphBuilderD( space );
	  break;
      case Gabriel:
	  builder = new ngl::GabrielGraphBuilderD( space );
	  break;
      case BSkeleton:
	  builder = new ngl::BSkeletonBuilderD( space, beta );
	  break;
      default:
	  cerr << HERE << " Illegal method for neighbor graphs " << endl;
	  exit( 0 );
    }

    // Add points to the builder.
    builder->addPoints( points );
    
    // Compute neighbor graph.
    ngl::NeighborGraphImpl neighborGraph;
    builder->computeNeighborGraph( &neighborGraph );
    
    // Output the graph to stdout.
    link.clear();
    for (int i = 0; i < neighborGraph.size(); ++i) {
	int src;
	int dst;
	neighborGraph.getEdge(i, &src, &dst);
	// printf("%d %d\n", src, dst);
	link.push_back( Link( src, dst ) );
    }
    delete builder;

    for ( int i = 0; i < num_points; ++i ) {
	delete [] points[ i ];
    }
    points.clear();
}

// end of header file
// Do not add any stuff under this line.
