#include <vector>

using namespace std;

#include "Expansion.h"
#include "CGAL.h"

#define INFINITY_COST		(10000000)

extern int graph_cut		( const unsigned int & num_pixels,
				  const unsigned int & num_labels,
				  const vector< Link > & edge,
				  const vector< int > & weight,
				  const vector< vector< int > > & d_cost,
				  const vector< int > & l_cost,
				  vector< unsigned int > & group );
extern int iterative_graph_cut	( const unsigned int & num_pixels,
				  const unsigned int & num_labels,
				  const vector< Link > & edge,
				  const vector< int > & weight,
				  const vector< vector< int > > & d_cost,
				  const vector< int > & l_cost,
				  vector< Expansion > & history );
