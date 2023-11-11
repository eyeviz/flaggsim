//#include <cstdio>
//#include <cstdlib>
#include <cmath>
//#include <cstring>
//#include <ctime>
#include <cassert> 
#include <iostream>

using namespace std;

#include "Common.h"

#include "GCoptimization.h"

#include "graphcut.h"

//------------------------------------------------------------------------------
#define BEYOND_LABEL_PENALTY	(2)
#define WITHIN_LABEL_PENALTY	(1)
//------------------------------------------------------------------------------
//#define BEYOND_LABEL_PENALTY	(1)
//#define WITHIN_LABEL_PENALTY	(1)
//------------------------------------------------------------------------------
//#define BEYOND_LABEL_PENALTY	(20)
//#define WITHIN_LABEL_PENALTY	(19)
//------------------------------------------------------------------------------
//#define BEYOND_LABEL_PENALTY	(20)
//#define WITHIN_LABEL_PENALTY	(20)
//------------------------------------------------------------------------------
//#define BEYOND_LABEL_PENALTY	(100)
//#define WITHIN_LABEL_PENALTY	(99)
//------------------------------------------------------------------------------

// #define NO_EDGE_WEIGHT		(10000)

////////////////////////////////////////////////////////////////////////////////
// in this version, set data and smoothness terms using arrays
// grid neighborhood is set up "manually". Uses spatially varying terms. Namely
// V(p1,p2,l1,l2) = w_{p1,p2}*[min((l1-l2)*(l1-l2),4)], with 
// w_{p1,p2} = p1+p2 if |p1-p2| == 1 and w_{p1,p2} = p1*p2 if |p1-p2| is not 1

int graph_cut( const unsigned int & num_pixels,
	       const unsigned int & num_labels,
	       const vector< Link > & edge,
	       const vector< int > & weight,
	       const vector< vector< int > > & d_cost,
	       const vector< int > & l_cost,
	       vector< unsigned int > & group )
{
    int energy = INFINITY_COST; 
    int *result = new int[num_pixels];   // stores result of optimization

    // first set up the array for data costs
    //  Data costs: D(pixel,label) = 0 if pixel < 25 and label = 0
    //            : D(pixel,label) = 10 if pixel < 25 and label is not  0
    //            : D(pixel,label) = 0 if pixel >= 25 and label = 5
    //            : D(pixel,label) = 10 if pixel >= 25 and label is not  5
    
    int *data = new int[num_pixels*num_labels];
    for ( unsigned int i = 0; i < num_pixels; i++ )
	for ( unsigned int l = 0; l < num_labels; l++ )
	    data[ i*num_labels + l ] = d_cost[ i ][ l ];

    // next set up the array for smooth costs
    int *smooth = new int[ num_labels*num_labels ];
    // initialize to zeros
    for ( unsigned int l1 = 0; l1 < num_labels; l1++ )
	for ( unsigned int l2 = 0; l2 < num_labels; l2++ ) {
	    if ( l1 == l2 ) {
		smooth[ l1 + l2*num_labels ] = WITHIN_LABEL_PENALTY;
	    }
	    else {
		smooth[ l1 + l2*num_labels ] = BEYOND_LABEL_PENALTY;
	    }
	}

    // next set up the array of label costs
    int *label = new int[ num_labels ];
    for ( unsigned int i = 0; i < num_labels; ++i )
	label[ i ] = l_cost[ i ];
    
    try{
	GCoptimizationGeneralGraph *gc =
	    new GCoptimizationGeneralGraph(num_pixels,num_labels);

#ifdef SKIP
	// first we set the dummy edge weight to every possible pair of vertices
	for ( unsigned int i = 0; i < num_pixels; ++i ) 
	    for ( unsigned int j = i+1; j < num_pixels; ++j )
		gc->setNeighbors( i, j, NO_EDGE_WEIGHT );
#endif	// SKIP
	
	// now set up a given neighborhood system
	for ( unsigned int i = 0; i < edge.size(); ++i ) {
	    unsigned int idS = edge[ i ].first;
	    unsigned int idT = edge[ i ].second;
	    gc->setNeighbors( idS, idT, weight[ i ] );
	}

	gc->setDataCost( data );
	gc->setSmoothCost( smooth );
	gc->setLabelCost( label );
	
	//#ifdef SKIP
	for ( unsigned int i = 0; i < num_pixels; ++i )
	    // gc->setLabel( i, i % 3 );
	    gc->setLabel( i, i % num_labels );
	    // #endif	// SKIP

	cerr << HERE << "Before optimization energy is " << gc->compute_energy() << endl;
	cerr << HERE;
	gc->printStatus( ">>>>>" );
	cerr << HERE << "Labels : " << ends;
	for ( unsigned int i = 0; i < num_pixels; ++i ) {
	    cerr << " " << gc->whatLabel( i ) << ends;
	}
	cerr << endl;
	
	gc->setVerbosity( 3 );
	// gc->expansion();	// no arguments for optimization until convergence
	gc->expansion( -1 );	// no arguments for optimization until convergence

	cerr << HERE << "After optimization energy is " << gc->compute_energy() << endl;
	energy = gc->compute_energy();
	cerr << HERE << "Labels : " << ends;
	for ( unsigned int i = 0; i < num_pixels; ++i ) {
	    cerr << " " << gc->whatLabel( i ) << ends;
	}
	cerr << endl;
	
	group.clear();
	for ( unsigned int i = 0; i < num_pixels; ++i ) {
	    result[ i ] = gc->whatLabel( i );
	    group.push_back( result[ i ] );
	}

	delete gc;
    }
    catch (GCException e){
	e.Report();
    }
    
    delete [] result;
    delete [] smooth;
    delete [] data;
    delete [] label;

    return energy;
}


int iterative_graph_cut( const unsigned int & num_pixels,
			 const unsigned int & num_labels,
			 const vector< Link > & edge,
			 const vector< int > & weight,
			 const vector< vector< int > > & d_cost,
			 const vector< int > & l_cost,
			 vector< Expansion > & history )
{
    int energy = INFINITY_COST; 
    int *result = new int[num_pixels];   // stores result of optimization

    // initialize the history
    // history.clear();
    
    // first set up the array for data costs
    //  Data costs: D(pixel,label) = 0 if pixel < 25 and label = 0
    //            : D(pixel,label) = 10 if pixel < 25 and label is not  0
    //            : D(pixel,label) = 0 if pixel >= 25 and label = 5
    //            : D(pixel,label) = 10 if pixel >= 25 and label is not  5
    
    int *data = new int[num_pixels*num_labels];
    for ( unsigned int i = 0; i < num_pixels; i++ )
	for ( unsigned int l = 0; l < num_labels; l++ )
	    data[ i*num_labels + l ] = d_cost[ i ][ l ];

    // next set up the array for smooth costs
    int *smooth = new int[ num_labels*num_labels ];
    // initialize to zeros
    for ( unsigned int l1 = 0; l1 < num_labels; l1++ )
	for ( unsigned int l2 = 0; l2 < num_labels; l2++ ) {
	    if ( l1 == l2 ) {
		smooth[ l1 + l2*num_labels ] = WITHIN_LABEL_PENALTY;
	    }
	    else {
		smooth[ l1 + l2*num_labels ] = BEYOND_LABEL_PENALTY;
	    }
	}

    // next set up the array of label costs
    int *label = new int[ num_labels ];
    for ( unsigned int i = 0; i < num_labels; ++i )
	label[ i ] = l_cost[ i ];
    
    try{
	GCoptimizationGeneralGraph *gc =
	    new GCoptimizationGeneralGraph(num_pixels,num_labels);

#ifdef SKIP
	// first we set the dummy edge weight to every possible pair of vertices
	for ( unsigned int i = 0; i < num_pixels; ++i ) 
	    for ( unsigned int j = i+1; j < num_pixels; ++j )
		gc->setNeighbors( i, j, NO_EDGE_WEIGHT );
#endif	// SKIP
	
	// now set up a given neighborhood system
	for ( unsigned int i = 0; i < edge.size(); ++i ) {
	    unsigned int idS = edge[ i ].first;
	    unsigned int idT = edge[ i ].second;
	    gc->setNeighbors( idS, idT, weight[ i ] );
	}

	gc->setDataCost( data );
	gc->setSmoothCost( smooth );
	gc->setLabelCost( label );
	
	// This initialization is quite important!!
	// noted by ST on 2023/11/11
	for ( unsigned int i = 0; i < num_pixels; ++i )
	    gc->setLabel( i, i % num_labels );

	cerr << HERE << "Before optimization energy is " << gc->compute_energy() << endl;

	gc->printStatus( ">>>>>" );
	cerr << HERE << "Labels : " << ends;
	for ( unsigned int i = 0; i < num_pixels; ++i ) {
	    cerr << " " << gc->whatLabel( i ) << ends;
	}
	cerr << endl;
	
//------------------------------------------------------------------------------
//	Special treatment
//------------------------------------------------------------------------------
	for ( unsigned int i = 0; i < MAX_ITERATIONS; ++i ) {
	    for ( unsigned int j = 0; j < MAX_PIXELS; ++j ) {
		gc->trackLabel[ i ][ j ] = -1;
	    }
	    for ( unsigned int k = 0; k < NUM_COSTS; ++k ) {
		gc->trackCost[ i ][ k ] = -1;
	    }
	}
	gc->nTracks = 0;
	
	gc->setVerbosity( 2 );
	// gc->setVerbosity( 3 );
	gc->expansion( -1 );

	int nSteps = gc->nTracks;
	for ( int m = 0; m < nSteps; ++m ) {
	    cerr << HERE << " Energy = " << setw( 4 )
		 << gc->trackCost[ m ][ 0 ]
		 << " ( " << ends;
	    for ( unsigned int j = 1; j < NUM_COSTS; ++j ) {
		cerr << setw( 4 ) << gc->trackCost[ m ][ j ] << ends;
		if ( j != NUM_COSTS - 1 ) cerr << "+" << ends;
	    }
	    cerr << ") : " << ends;
	    for ( unsigned int j = 0; j < num_pixels; ++j ) {
		cerr << setw( 3 ) << gc->trackLabel[ m ][ j ] << " " << ends;
	    }
	    cerr << endl;
	}
    
	energy = gc->compute_energy();
	cerr << HERE << "After optimization energy is " << energy << endl;
	cerr << HERE << "Labels : " << ends;
	for ( unsigned int i = 0; i < num_pixels; ++i ) {
	    cerr << " " << gc->whatLabel( i ) << ends;
	}
	cerr << endl;
	
	// history.clear();
	for ( int m = 0; m < nSteps; ++m ) {
	    Expansion step;
	    for ( unsigned int i = 0; i < num_pixels; ++i ) {
		result[ i ] = gc->trackLabel[ m ][ i ];
		step.group().push_back( result[ i ] );
		step.setTotalCost( gc->trackCost[ m ][ 0 ] );
		step.setDataCost( gc->trackCost[ m ][ 1 ] );
		step.setSmoothCost( gc->trackCost[ m ][ 2 ] );
		step.setLabelCost( gc->trackCost[ m ][ 3 ] );
	    }
	    history.push_back( step );
	}

	delete gc;
    }
    catch (GCException e){
	e.Report();
    }
    
    delete [] result;
    delete [] smooth;
    delete [] data;
    delete [] label;

    cerr << HERE << " Number of histories = " << history.size() << endl;
    
    return energy;
}
