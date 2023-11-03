#define _USE_MATH_DEFINES
#include <cmath>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <cassert>
#include <unordered_map>
#include <cstdint>
#include <cfloat>

using namespace std;

#include <CGAL/Boolean_set_operations_2.h>

// #define ENABLE_DEBUG_MESSAGES
// #define ENABLE_ALGORITHM_ANIMATION

#pragma warning(push, 0)
#include <flann/flann.hpp>
#pragma warning(pop)

#define USE_OPENMP

#if defined USE_OPENMP
#if !defined _OPENMP
#pragma message("You've chosen to want OpenMP usage but have not made it a compilation option. Compile with /openmp")
#endif
#endif

#ifdef ENABLE_ALGORITHM_ANIMATION
//#define WAITING_TIME	(5)
#define WAITING_TIME	(200)
#endif	// ENABLE_ALGORITHM_ANIMATION

#define ERROR_TOLERANCE	(1.0e-8)

#include "CGAL.h"
#include "Common.h"
#include "Concave.h"

using std::uint64_t;

struct Track
{
    Point2 v = Point2( 0.0, 0.0 );
    uint64_t id = 0;

    Track() = default;

    Track(double x, double y)
    {
	v = Point2( x, y );
    }
};

struct TrackValue
{
    Track track;
    double distance = 0.0;
    double angle = 0.0;
};

// static const size_t stride = 24; // size in bytes of x, y, id

using TrackVector = std::vector<Track>;
using TrackValueVector = std::vector<TrackValue>;
#ifdef SKIP
using LineSegment = std::pair<Track, Track>;
#endif	// SKIP

#ifdef ENABLE_ALGORITHM_ANIMATION
static TrackVector global_hull;
static TrackVector global_cPoints;
static TrackVector global_ends;
#endif	// ENABLE_ALGORITHM_ANIMATION

// Floating point comparisons
auto Equal(double a, double b) -> bool;
auto ApproxEqual(double a, double b) -> bool;
auto Zero(double a) -> bool;
auto LessThan(double a, double b) -> bool;
auto LessThanOrEqual(double a, double b) -> bool;
auto GreaterThan(double a, double b) -> bool;
auto MoreObtuseThan(TrackValue a, TrackValue b) -> bool;


// I/O
#ifdef SKIP
auto Usage() -> void;
auto FindArgument(int argc, char **argv, const std::string &name) -> int;
auto ParseArgument(int argc, char **argv, const std::string &name, std::string &val) -> int;
auto ParseArgument(int argc, char **argv, const std::string &name, int &val) -> int;
auto HasExtension(const std::string &filename, const std::string &ext) -> bool;
auto ReadFile(const std::string &filename, int fieldX = 1, int fieldY = 2) -> TrackVector;
auto Print(const std::string &filename, const TrackVector &points) -> void;
auto Print(FILE *out, const TrackVector &points, const char *format = "%.3f  %.3f\n") -> void;
auto Split(const std::string &value, const char *delims) -> std::vector<std::string>;
#endif	// SKIP

// Algorithm-specific
auto NearestNeighboursFlann(flann::Index<flann::L2<double>> &index, const Track &p, size_t k) -> TrackValueVector;
auto ConcaveHull(TrackVector &dataset, size_t k, bool iterate) -> TrackVector;
auto ConcaveHull(TrackVector &dataset, size_t k, TrackVector &hull) -> bool;
auto SortByAngle(TrackValueVector &values, const Track &p, double prevAngle) -> TrackVector;
auto AddPoint(TrackVector &points, const Track &p) -> void;

// General maths
auto PointsEqual(const Track &a, const Track &b) -> bool;
auto Angle(const Track &a, const Track &b) -> double;
auto NormaliseAngle(double radians) -> double;
auto PointInPolygon(const Track &p, const TrackVector &list) -> bool;
#ifdef SKIP
auto Intersects(const LineSegment &a, const LineSegment &b) -> bool;
#endif	// SKIP

// Point list utilities
auto FindMinYPoint(const TrackVector &points) -> Track;
auto RemoveDuplicates(TrackVector &points) -> void;
auto IdentifyPoints(TrackVector &points) -> void;
auto RemoveHull(TrackVector &points, const TrackVector &hull) -> TrackVector::iterator;
auto MultiplePointInPolygon(TrackVector::iterator begin, TrackVector::iterator end, const TrackVector &hull) -> bool;

// Unit tests
#ifdef SKIP
auto TestAngle() -> void;
auto TestIntersects() -> void;
auto TestSplit() -> void;
#endif	// SKIP


#ifdef SKIP
// Get command line index of name
auto FindArgument(int argc, char **argv, const std::string &name) -> int
{
	for (int i = 1; i < argc; ++i)
		{
		if (std::string(argv[i]) == name)
			return i;
		}
	return -1;
}
#endif	// SKIP

#ifdef SKIP
// Get the command line value (string) for name
auto ParseArgument(int argc, char **argv, const std::string &name, std::string &val) -> int
{
	int index = FindArgument(argc, argv, name) + 1;
	if (index > 0 && index < argc)
		val = argv[index];

	return index - 1;
}
#endif	// SKIP

#ifdef SKIP
// Get the command line value (int) for name
auto ParseArgument(int argc, char **argv, const std::string &name, int &val) -> int
{
	int index = FindArgument(argc, argv, name) + 1;

	if (index > 0 && index < argc)
		val = atoi(argv[index]);

	return (index - 1);
}
#endif	// SKIP

#ifdef SKIP
// Check whether a string ends with a specified suffix.
auto HasExtension(const std::string &filename, const std::string &ext) -> bool
{
	if (filename.length() >= ext.length())
		return (0 == filename.compare(filename.length() - ext.length(), ext.length(), ext));
	return false;
}
#endif	// SKIP

#ifdef SKIP
// Read a file of coordinates into a vector. First two fields of comma/tab/space delimited input are used.
auto ReadFile(const std::string &filename, int fieldX, int fieldY) -> TrackVector
{
	fieldX--; // from 1-based index to 0-based
	fieldY--;

	TrackVector list;
	Point p;
	std::string line;
	std::vector<std::string> tokens;

	std::ifstream fin(filename.c_str());
	if (fin.is_open())
		{
		while (fin.good())
			{
			getline(fin, line);
			if (!line.empty())
				{
				tokens = Split(line, " ,\t");
				if (tokens.size() >= 2)
					{
					p.x = std::atof(tokens[fieldX].c_str());
					p.y = std::atof(tokens[fieldY].c_str());
					list.push_back(p);
					}
				}
			}
		}

	return list;
}
#endif	// SKIP

#ifdef SKIP
// Output a point list to a file
auto Print(const std::string &filename, const TrackVector &points) -> void
{
	std::string format;
	if (HasExtension(filename, ".csv"))
		format = "%.3f,%.3f\n";
	else
		format = "%.3f  %.3f\n";

	FILE *out = fopen(filename.c_str(), "w+");
	if (out)
		{
		Print(out, points, format.c_str());

		fclose(out);
		}
}
#endif	// SKIP

#ifdef SKIP
// Output a point list to a stream with a given format string
auto Print(FILE *out, const TrackVector &points, const char *format) -> void
{
	for (const auto &p : points)
		{
		fprintf(out, format, p.x, p.y);
		}
}
#endif	// SKIP

// Iteratively call the main algorithm with an increasing k until success
auto ConcaveHull(TrackVector &dataset, size_t k, bool iterate) -> TrackVector
{
    while (k < dataset.size()) {
#ifdef ENABLE_ALGORITHM_ANIMATION
	// TrackVector hull;
	if (ConcaveHull(dataset, k, global_hull) || !iterate) {
	    return global_hull;
	}
#else	// ENABLE_ALGORITHM_ANIMATION
	TrackVector hull;
	if (ConcaveHull(dataset, k, hull) || !iterate) {
	    return hull;
	}
#endif	// ENABLE_ALGORITHM_ANIMATION
	k++;
    }

    cerr << HERE << " Returning a null hull" << endl;

    return{};
}

// The main algorithm from the Moreira-Santos paper.
auto ConcaveHull(TrackVector &pointList, size_t k, TrackVector &hull) -> bool
{
    hull.clear();

    if ( pointList.size() < 3 ) {
	return true;
    }
    if ( pointList.size() == 3 ) {
	hull = pointList;
	return true;
    }

    flann::Matrix<double> matrix( new double [ pointList.size() * 2 ],
				  pointList.size(), 2 );

    // construct a randomized kd-tree index using 4 kd-trees
    // 2 columns, but stride = 24 bytes in width (x, y, ignoring id)
    for ( auto i = 0; i < pointList.size(); ++i ) {
	matrix[ i ][ 0 ] = pointList[ i ].v.x();
	matrix[ i ][ 1 ] = pointList[ i ].v.y();
    }

    flann::Index<flann::L2<double>> flannIndex( matrix, flann::KDTreeIndexParams(4) );
    flannIndex.buildIndex();

    // std::cout << "\rFinal 'k'        : " << k;

    // Initialise hull with the min-y point
    Track firstPoint = FindMinYPoint(pointList);
#ifdef ENABLE_DEBUG_MESSAGES
    cerr << HERE << " firstPoint = " << firstPoint.v.x() << " , "
	 << firstPoint.v.y() << " [ " << firstPoint.id << " ] " << endl;
#endif	// ENABLE_DEBUG_MESSAGES
    AddPoint(hull, firstPoint);

    // Until the hull is of size > 3 we want to ignore the first point from nearest neighbour searches
    Track currentPoint = firstPoint;
    flannIndex.removePoint(firstPoint.id);

    double prevAngle = 0.0;
    int step = 1;

    // Iterate until we reach the start, or until there's no points left to process
    while ( ( ( ! PointsEqual( currentPoint, firstPoint ) ) || ( step == 1 ) )
	    &&
	    ( hull.size() != pointList.size() ) ) {
	if ( step == 4 ) {
	    // Put back the first point into the dataset and into the flann index
	    firstPoint.id = pointList.size();
	    flann::Matrix<double> firstPointMatrix( new double[ 1*2 ], 1, 2 );
	    firstPointMatrix[ 0 ][ 0 ] = firstPoint.v.x();
	    firstPointMatrix[ 0 ][ 1 ] = firstPoint.v.y();
	    flannIndex.addPoints(firstPointMatrix);
	}

	TrackValueVector kNearestNeighbours =
	    NearestNeighboursFlann( flannIndex, currentPoint, k );

#ifdef ENABLE_DEBUG_MESSAGES
	for ( auto k = 0; k < hull.size(); ++k ) {
	    cerr << HERE << " hull[ " << setw(2)
		 << k << " ] = " << hull[ k ].v.x()
		 << " , " << hull[ k ].v.y()
		 << " [ " << setw( 2 ) << hull[ k ].id << " ] " << endl;
	}
#endif	// ENABLE_DEBUG_MESSAGES
	
#ifdef ENABLE_DEBUG_MESSAGES
	cerr << HERE << " k-nearest neighbors : " << endl;
	for ( auto & pv : kNearestNeighbours ) {
	    cerr << HERE << pv.track.v.x() << " , " << pv.track.v.y()
		 << " id = " << pv.track.id
		 << " dist = " << pv.distance
		 << " angle = " << pv.angle << endl;
	}

	cerr << HERE << " prevAngle = " << prevAngle << endl;
	cerr << HERE << " currentPoint = "
	     << currentPoint.v.x() << " , " << currentPoint.v.y()
	     << " [ " << currentPoint.id << " ] " << endl;
#endif	// ENABLE_DEBUG_MESSAGES

#ifdef ENABLE_ALGORITHM_ANIMATION
	global_cPoints = SortByAngle( kNearestNeighbours, currentPoint, prevAngle );
#else	// ENABLE_ALGORITHM_ANIMATION
	TrackVector cPoints = SortByAngle(kNearestNeighbours, currentPoint, prevAngle);
#endif	// ENABLE_ALGORITHM_ANIMATION

#ifdef ENABLE_DEBUG_MESSAGES
	cerr << HERE << " closest points : " << endl;
#ifdef ENABLE_ALGORITHM_ANIMATION
	for ( auto & p : global_cPoints )
#else	// ENABLE_ALGORITHM_ANIMATION
	for ( auto & p : cPoints )
#endif	// ENABLE_ALGORITHM_ANIMATION
        {
	    cerr << HERE << " cPoints : " << p.v.x() << " , " << p.v.y()
		 << " [ " << p.id << " ] "
		 << " angle = " << NormaliseAngle( Angle( currentPoint, p ) - prevAngle )
		 << endl;
	}
#endif	// ENABLE_DEBUG_MESSAGES


	bool its = true;
	size_t i = 0;

	while ( its &&
#ifdef ENABLE_ALGORITHM_ANIMATION
		( i < global_cPoints.size() )
#else	// ENABLE_ALGORITHM_ANIMATION
		( i < cPoints.size() )
#endif	// ENABLE_ALGORITHM_ANIMATION
		) {
	    size_t lastPoint = 0;
#ifdef ENABLE_ALGORITHM_ANIMATION
	    if ( PointsEqual( global_cPoints[i], firstPoint ) )
		lastPoint = 1;
#else	// ENABLE_ALGORITHM_ANIMATION
	    if ( PointsEqual( cPoints[i], firstPoint ) )
		lastPoint = 1;
#endif	// ENABLE_ALGORITHM_ANIMATION

	    size_t j = 2;
	    its = false;

#ifdef ENABLE_DEBUG_MESSAGES
	    cerr << HERE << " i = " << i << " j = " << j << " step = " << step << endl;
	    cerr << HERE << " lastPoint = " << lastPoint << endl;
	    if ( its ) cerr << HERE << " its TRUE " << endl;
	    else cerr << HERE << " its FALSE " << endl;
#ifdef ENABLE_ALGORITHM_ANIMATION
	    display();
#endif	// ENABLE_ALGORITHM_ANIMATION
	    // std::this_thread::sleep_for( std::chrono::milliseconds( WAITING_TIME ) );
	    getchar();
#endif	// ENABLE_DEBUG_MESSAGES

	    while ( ( !its ) && ( j < hull.size() - lastPoint ) ) {
#ifdef ENABLE_ALGORITHM_ANIMATION
		// auto line1 = std::make_pair(hull[step - 1], global_cPoints[i]);
		// auto line2 = std::make_pair(hull[step - j - 1], hull[step - j]);
		Segment2 seg1( hull[ step - 1 ].v, global_cPoints[ i ].v );
		Segment2 seg2( hull[ step - j - 1 ].v, hull[ step - j ].v );
		global_ends.clear();
		global_ends.push_back( line1.first );
		global_ends.push_back( line1.second );
		global_ends.push_back( line2.first );
		global_ends.push_back( line2.second );
#else	// ENABLE_ALGORITHM_ANIMATION
		// auto line1 = std::make_pair(hull[step - 1], cPoints[i]);
		// auto line2 = std::make_pair(hull[step - j - 1], hull[step - j]);
		Segment2 seg1( hull[ step - 1 ].v, cPoints[ i ].v );
		Segment2 seg2( hull[ step - j - 1 ].v, hull[ step - j ].v );
#endif	// ENABLE_ALGORITHM_ANIMATION
		// its = Intersects( line1, line2 );

#ifdef SKIP
		Point2 source1( line1.first.x, line1.first.y );
		Point2 target1( line1.second.x, line1.second.y );
		Point2 source2( line2.first.x, line2.first.y );
		Point2 target2( line2.second.x, line2.second.y );
		Segment2 segment1( source1, target1 );
		Segment2 segment2( source2, target2 );
#endif	// SKIP
		its = ( do_overlap( seg1.bbox(), seg2.bbox() ) ||
			do_intersect( seg1, seg2 ) );
		
#ifdef ENABLE_DEBUG_MESSAGES
		cerr << HERE << " seg1 : " << seg1;
		cerr << HERE << " seg2 : " << seg2;

		if ( its ) cerr << HERE << " its TRUE " << endl;
		else cerr << HERE << " its FALSE " << endl;
#endif	// ENABLE_DEBUG_MESSAGES
#ifdef ENABLE_ALGORITHM_ANIMATION
		display();
		std::this_thread::sleep_for( std::chrono::milliseconds( WAITING_TIME ) );
		// getchar(); 
#endif	// ENABLE_ALGORITHM_ANIMATION

		j++;

#ifdef ENABLE_ALGORITHM_ANIMATION
		global_ends.clear();
#endif	// ENABLE_ALGORITHM_ANIMATION
	    }

#ifdef ENABLE_ALGORITHM_ANIMATION
	    cerr << " Outside the loop : ";
	    if ( its ) cerr << HERE << " its TRUE " << endl;
	    else cerr << HERE << " its FALSE " << endl;
	    display();
	    // std::this_thread::sleep_for( std::chrono::milliseconds( WAITING_TIME ) );
	    getchar();
#endif	// ENABLE_ALGORITHM_ANIMATION

	    if (its)
		i++;
	}

	if (its)
	    return false;

#ifdef ENABLE_ALGORITHM_ANIMATION
	currentPoint = global_cPoints[i];
#else	// ENABLE_ALGORITHM_ANIMATION
	currentPoint = cPoints[i];
#endif	// ENABLE_ALGORITHM_ANIMATION

	AddPoint(hull, currentPoint);

	prevAngle = Angle(hull[step], hull[step - 1]);
	
	flannIndex.removePoint(currentPoint.id);
	
#ifdef ENABLE_ALGORITHM_ANIMATION
	global_cPoints.clear();
	cerr << HERE << " step = " << step << endl;
	if ( its ) cerr << HERE << " its TRUE " << endl;
	else cerr << HERE << " its FALSE " << endl;
#endif	// ENABLE_ALGORITHM_ANIMATION
	
	step++;
    }

    // The original points less the points belonging to the hull need to be fully enclosed by the hull in order to return true.
    TrackVector dataset = pointList;

    auto newEnd = RemoveHull(dataset, hull);
    bool allEnclosed = MultiplePointInPolygon( begin(dataset), newEnd, hull );

#ifdef ENABLE_ALGORITHM_ANIMATION
    if ( allEnclosed ) cerr << HERE << " All enclosed TRUE" << endl;
    else cerr << HERE << " All enclosed FALSE" << endl;
    getchar();
#endif	// ENABLE_ALGORITHM_ANIMATION
    
    return allEnclosed;
}

std::vector< unsigned int > concave_hull_2( const std::vector< Point2 > & points, int k )
{
    TrackVector pv;
    for ( unsigned int i = 0; i < points.size(); ++i ) {
	Track p;
	p.v = points[ i ];
	p.id = i;
	pv.push_back( p );
    }
    
#ifdef ENABLE_DEBUG_MESSAGES
    cerr << HERE << " point.size = " << points.size() << endl;
    for ( unsigned int i = 0; i < points.size(); ++i ) {
	cerr << HERE << " point[ " << i << " ] = "
	     << setprecision( 8 )
	     << points[ i ] << endl;
    }
#endif	// ENABLE_DEBUG_MESSAGES

    // remove duplicates and id the points
    // RemoveDuplicates( pv );
    // IdentifyPoints( pv );

#ifdef ENABLE_DEBUG_MESSAGES
    cerr << HERE << " pv.size = " << pv.size() << endl;
    for ( unsigned int i = 0; i < pv.size(); ++i ) {
	cerr << HERE << " pv[ " << i << " ] = "
	     << setprecision( 8 )
	     << pv[ i ].v.x() << " , " << pv[ i ].v.y()
	     << " [ " << pv[ i ].id << " ] " << endl;
    }
#endif	// ENABLE_DEBUG_MESSAGES

#ifdef ENABLE_ALGORITHM_ANIMATION
    cerr << HERE << "##############################" << endl;
    cerr << HERE << "##############################" << endl;
    cerr << HERE << "##############################" << endl;
    display();
    std::this_thread::sleep_for( std::chrono::milliseconds( WAITING_TIME ) );
#endif	// ENABLE_ALGORITHM_ANIMATION

    bool iterate = true;
    TrackVector hull = ConcaveHull( pv, (size_t)k, iterate );

#ifdef ENABLE_DEBUG_MESSAGES
    cerr << HERE << " hull size = " << hull.size() << endl;
    for ( unsigned int i = 0; i < hull.size(); ++i ) {
	cerr << HERE << " hull[ " << i << " ] "
	     << hull[ i ].v.x() << " , " << hull[ i ].v.y() << endl;
    }
#endif	// ENABLE_DEBUG_MESSAGES

    if ( hull.size() <= 1 ) {
	cerr << HERE << "Illegal hull size = " << hull.size() << endl;
	assert( hull.size() > 1 );
    }

    std::vector< unsigned int > idv;
    for ( unsigned int i = 0; i < hull.size() - 1; ++i ) {
	idv.push_back( hull[ i ].id ); 
    }

#ifdef ENABLE_DEBUG_MESSAGES
    for ( unsigned int i = 0; i < idv.size(); ++i )
	cerr << HERE << " idv[ " << i  << " ] = " << idv[ i ] << endl;
#endif	// ENABLE_DEBUG_MESSAGES

    return idv;
}

// Compare a and b for equality
auto Equal(double a, double b) -> bool
{
    return fabs(a - b) <= DBL_EPSILON;
}

// Compare a and b for equality apprximately
auto ApproxEqual(double a, double b) -> bool
{
    return fabs(a - b) <= ERROR_TOLERANCE;
}

// Compare value to zero
auto Zero(double a) -> bool
{
	return fabs(a) <= DBL_EPSILON;
}

// Compare for a < b
auto LessThan(double a, double b) -> bool
{
	return a < (b - DBL_EPSILON);
}

// Compare for a <= b
auto LessThanOrEqual(double a, double b) -> bool
{
	return a <= (b + DBL_EPSILON);
}

// Compare for a > b
auto GreaterThan(double a, double b) -> bool
{
	return a > (b + DBL_EPSILON);
}

// Compare for a > b
auto MoreObtuseThan(TrackValue a, TrackValue b) -> bool
{
    // Equal -> ApproxEqual
    if ( ApproxEqual( a.angle, b.angle ) ) {
	if ( LessThan( a.distance, b.distance ) ) return true;
	else return false;
    }
    else if ( GreaterThan( a.angle, b.angle ) ) {
	return true;
    }
    else {
	return false;
    }
}

// Compare whether two points have the same x and y
auto PointsEqual(const Track &a, const Track &b) -> bool
{
    return Equal( a.v.x(), b.v.x() ) && Equal( a.v.y(), b.v.y() );
}

// Remove duplicates in a list of points
auto RemoveDuplicates(TrackVector &points) -> void
{
    sort(begin(points), end(points), [](const Track & a, const Track & b) {
	if ( Equal( a.v.x(), b.v.x() ) )
	    return LessThan( a.v.y(), b.v.y() );
	else
	    return LessThan( a.v.x(), b.v.x() );
    });

    auto newEnd = unique(begin(points), end(points), [](const Track & a, const Track & b)
    {
	return PointsEqual(a, b);
    });

    points.erase(newEnd, end(points));
}

// Uniquely id the points for binary searching
auto IdentifyPoints(TrackVector &points) -> void
{
    uint64_t id = 0;

    for (auto itr = begin(points); itr != end(points); ++itr, ++id) {
	itr->id = id;
    }
}

// Find the point having the smallest y-value
auto FindMinYPoint(const TrackVector &points) -> Track
{
    assert(!points.empty());

    auto itr = min_element(begin(points), end(points), [](const Track & a, const Track & b)
    {
	if ( Equal( a.v.y(), b.v.y() ) )
	    return GreaterThan( a.v.x(), b.v.x() );
	else
	    return LessThan( a.v.y(), b.v.y() );
    });

    return *itr;
}

// Lookup by ID and remove a point from a list of points
auto RemovePoint( TrackVector &list, const Track &p ) -> void
{
    auto itr = std::lower_bound(begin(list), end(list), p, [](const Track & a, const Track & b)
    {
	return a.id < b.id;
    });

    assert(itr != end(list) && itr->id == p.id);

    if (itr != end(list))
	list.erase(itr);
}

// Add a point to a list of points
auto AddPoint(TrackVector &points, const Track &p) -> void
{
    points.push_back(p);
}

// Return the k-nearest points in a list of points from the given point p (uses Flann library).
auto NearestNeighboursFlann(flann::Index<flann::L2<double>> &index, const Track &p, size_t k) -> TrackValueVector
{
    std::vector<int> vIndices(k);
    std::vector<double> vDists(k);
    double test[] = { p.v.x(), p.v.y() };

    flann::Matrix<double> query(test, 1, 2);
    flann::Matrix<int> mIndices(vIndices.data(), 1, static_cast<int>(vIndices.size()));
    flann::Matrix<double> mDists(vDists.data(), 1, static_cast<int>(vDists.size()));

    int count_ = index.knnSearch(query, mIndices, mDists, k, flann::SearchParams(128));
    size_t count = static_cast<size_t>(count_);

    TrackValueVector result(count);

    for ( size_t i = 0; i < count; ++i ) {
	int id = vIndices[i];
	const double *point = index.getPoint(id);
	result[i].track.v = Point2( point[ 0 ], point[ 1 ] );
	result[i].track.id = id;
	result[i].distance = vDists[i];
    }

    return result;
}

// Returns a list of points sorted in descending order of clockwise angle
auto SortByAngle(TrackValueVector &values, const Track &from, double prevAngle) -> TrackVector
{
    for_each(begin(values), end(values), [from, prevAngle](TrackValue & to)
    {
	to.angle = NormaliseAngle(Angle(from, to.track) - prevAngle);
#ifdef ENABLE_DEBUG_MESSAGES
	cerr << HERE << " to.angle = " << to.angle << endl;
#endif	// ENABLE_DEBUG_MESSAGES
    });

    sort(begin(values), end(values), [](const TrackValue & a, const TrackValue & b)
    {
	// return GreaterThan(a.angle, b.angle);
	return MoreObtuseThan( a, b );
    });

    TrackVector angled(values.size());

    transform(begin(values), end(values), begin(angled), [](const TrackValue & pv)
    {
	return pv.track;
    });

    return angled;
}

// Get the angle in radians measured clockwise from +'ve x-axis
auto Angle(const Track &a, const Track &b) -> double
{
    double angle = -atan2( b.v.y() - a.v.y(), b.v.x() - a.v.x() );

    return NormaliseAngle( angle );
}

// Return angle in range: 0 <= angle < 2PI
auto NormaliseAngle(double radians) -> double
{
	if (radians < 0.0)
		return radians + M_PI + M_PI;
	else
		return radians;
}

// Return the new logical end after removing points from dataset having ids belonging to hull
auto RemoveHull(TrackVector &points, const TrackVector &hull) -> TrackVector::iterator
{
	std::vector<uint64_t> ids(hull.size());

	transform(begin(hull), end(hull), begin(ids), [](const Track & p)
	{
	    return p.id;
	});

	sort(begin(ids), end(ids));

	return remove_if(begin(points), end(points), [&ids](const Track & p)
	{
	    return binary_search(begin(ids), end(ids), p.id);
	});
}

// Uses OpenMP to determine whether a condition exists in the specified range of elements. https://msdn.microsoft.com/en-us/library/ff521445.aspx
template <class InIt, class Predicate>
bool omp_parallel_any_of(InIt first, InIt last, const Predicate &pr)
{
	typedef typename std::iterator_traits<InIt>::value_type item_type;

	// A flag that indicates that the condition exists.
	bool found = false;

	#pragma omp parallel for
	for (int i = 0; i < static_cast<int>(last - first); ++i)
		{
		if (!found)
			{
			item_type &cur = *(first + i);

			// If the element satisfies the condition, set the flag to cancel the operation.
			if (pr(cur))
				{
				found = true;
				}
			}
		}

	return found;
}

// Check whether all points in a begin/end range are inside hull.
auto MultiplePointInPolygon( TrackVector::iterator begin, TrackVector::iterator end,
			     const TrackVector & hull ) -> bool
{
    auto test = [&hull](const Track & p)
    {
	return !PointInPolygon(p, hull);
    };

    bool anyOutside = true;
#if defined USE_OPENMP
    anyOutside = omp_parallel_any_of(begin, end, test); // multi-threaded
#else
    anyOutside = std::any_of(begin, end, test); // single-threaded
#endif

    return !anyOutside;
}

// Point-in-polygon test
auto PointInPolygon(const Track &p, const TrackVector &list) -> bool
{
    if ( list.size() <= 2 )
	return false;

    // cerr << HERE << " list.size = " << list.size() << endl;

    Polygon2 enclosure;
    for ( auto i = 0; i < list.size() - 1; ++i )
	enclosure.push_back( list[ i ].v );
    
    if ( enclosure.orientation() != CGAL::COUNTERCLOCKWISE ) {
	cerr << HERE << " CW Polygon " << endl;
	enclosure.reverse_orientation();
    }

    auto os = CGAL::oriented_side( p.v, enclosure );
    if ( ( os == CGAL::POSITIVE ) || ( os == CGAL::ON_ORIENTED_BOUNDARY ) )
	return true;
    else
	return false;
    
#ifdef SKIP
    const double &x = p.v.x();
    const double &y = p.v.y();

    int inout = 0;
    int onedge = 0;
    auto v0 = list.begin();
    auto v1 = v0 + 1;

    while ( v1 != list.end() ) {
	if ( ( LessThanOrEqual(v0->v.y(), y) && LessThan(y, v1->v.y()) ) ||
	     ( LessThanOrEqual(v1->v.y(), y) && LessThan(y, v0->v.y()) ) ) {
	    if (!Zero(v1->v.y() - v0->v.y())) {
		double tdbl1 = (y - v0->v.y()) / (v1->v.y() - v0->v.y());
		double tdbl2 = v1->v.x() - v0->v.x();
		double cfx = v0->v.x() + (tdbl2 * tdbl1 );

#ifdef ENABLE_ALGORITHM_ANIMATION
		cerr << HERE << " Comparison: x = " << x
		     << " cfx = " << cfx << endl;

		if ( LessThan( x, cfx ) ) cerr << "Less than!!" << endl;
		else cerr << "NOT less than" << endl;

		if ( LessThanOrEqual( x, cfx ) ) cerr << "Less than or equal!!" << endl;
		else cerr << "NOT less than or equal" << endl;

		if ( Equal( x, cfx ) ) cerr << "Equal!!" << endl;
		else cerr << "NOT equal" << endl;

		if ( GreaterThan( x, cfx ) ) cerr << "Greater than!!" << endl;
		else cerr << "NOT greater than" << endl;
#endif	// ENABLE_ALGORITHM_ANIMATION

		if ( LessThan( x, cfx ) ) inout++;
		if ( Equal( x, cfx ) ) onedge++;
	    }
	}

	v0 = v1;
	v1++;
    }

#ifdef ENABLE_ALGORITHM_ANIMATION
    cerr << HERE << " inout = " << inout << " onedge = " << onedge
	 << " ( " << p.v.x() << " , " << p.v.y() << " ) [ id = " << p.id << " ] " << endl;
#endif	// ENABLE_ALGORITHM_ANIMATION

    if ( onedge > 0 )
	return true;
    else if ( inout == 0 )
	return false;
    else if ( inout % 2 == 0 )
	return false;
    else
	return true;
#endif	// SKIP
}

#ifdef SKIP
// Test whether two line segments intersect each other
auto Intersects(const LineSegment &a, const LineSegment &b) -> bool
{
    // https://www.topcoder.com/community/data-science/data-science-tutorials/geometry-concepts-line-intersection-and-its-applications/

    const double &ax1 = a.first.v.x();
    const double &ay1 = a.first.v.y();
    const double &ax2 = a.second.v.x();
    const double &ay2 = a.second.v.y();
    const double &bx1 = b.first.v.x();
    const double &by1 = b.first.v.y();
    const double &bx2 = b.second.v.x();
    const double &by2 = b.second.v.y();

    double a1 = ay2 - ay1;
    double b1 = ax1 - ax2;
    double c1 = a1 * ax1 + b1 * ay1;
    double a2 = by2 - by1;
    double b2 = bx1 - bx2;
    double c2 = a2 * bx1 + b2 * by1;
    double det = a1 * b2 - a2 * b1;

    if (Zero(det)) {
	cerr << HERE << " det = " << det << endl;
	return false;
    }
    else {
	double x = (b2 * c1 - b1 * c2) / det;
	double y = (a1 * c2 - a2 * c1) / det;

	bool on_both = true;
	on_both = on_both && LessThanOrEqual(std::min(ax1, ax2), x) && LessThanOrEqual(x, std::max(ax1, ax2));
	on_both = on_both && LessThanOrEqual(std::min(ay1, ay2), y) && LessThanOrEqual(y, std::max(ay1, ay2));
	on_both = on_both && LessThanOrEqual(std::min(bx1, bx2), x) && LessThanOrEqual(x, std::max(bx1, bx2));
	on_both = on_both && LessThanOrEqual(std::min(by1, by2), y) && LessThanOrEqual(y, std::max(by1, by2));

	cerr << HERE << " x = " << x << " y = " << y;
	cerr << " ax1 = " << ax1 << " ax2 = " << ax2;
	cerr << " ay1 = " << ay1 << " ay2 = " << ay2;
	cerr << " bx1 = " << bx1 << " bx2 = " << bx2;
	cerr << " by1 = " << by1 << " by2 = " << by2 << endl;

	return on_both;
    }
}
#endif	// SKIP

#ifdef SKIP
// Unit test of Angle() function
auto TestAngle() -> void
{
	auto ToDegrees = [](double radians)
		{
		return radians * 180.0 / M_PI;
		};

	auto Test = [&](const Track &p, double expected)
		{
		double actual = ToDegrees(Angle({ 0.0, 0.0 }, p));
		assert(Equal(actual, expected));
		};

	double value = ToDegrees(atan(3.0 / 4.0));

	Test({  5.0,  0.0 }, 0.0);
	Test({  4.0,  3.0 }, 360.0 - value);
	Test({  3.0,  4.0 }, 270.0 + value);
	Test({  0.0,  5.0 }, 270.0);
	Test({ -3.0,  4.0 }, 270.0 - value);
	Test({ -4.0,  3.0 }, 180.0 + value);
	Test({ -5.0,  0.0 }, 180.0);
	Test({ -4.0, -3.0 }, 180.0 - value);
	Test({ -3.0, -4.0 }, 90.0 + value);
	Test({  0.0, -5.0 }, 90.0);
	Test({  3.0, -4.0 }, 90.0 - value);
	Test({  4.0, -3.0 }, 0.0 + value);
}
#endif	// SKIP

#ifdef SKIP
// Unit test the Intersects() function
auto TestIntersects() -> void
{
	using std::make_pair;

	std::unordered_map<char, Track> values;
	values['A'] = {  0.0,  0.0 };
	values['B'] = { -1.5,  3.0 };
	values['C'] = {  2.0,  2.0 };
	values['D'] = { -2.0,  1.0 };
	values['E'] = { -2.5,  5.0 };
	values['F'] = { -1.5,  7.0 };
	values['G'] = {  1.0,  9.0 };
	values['H'] = { -4.0,  7.0 };
	values['I'] = {  3.0, 10.0 };
	values['J'] = {  2.0, 11.0 };
	values['K'] = { -1.0, 11.0 };
	values['L'] = { -3.0, 11.0 };
	values['M'] = { -5.0,  9.5 };
	values['N'] = { -6.0,  7.5 };
	values['O'] = { -6.0,  4.0 };
	values['P'] = { -5.0,  2.0 };

	auto Test = [&values](const char a1, const char a2, const char b1, const char b2, bool expected)
		{
		assert(Intersects(make_pair(values[a1], values[a2]), make_pair(values[b1], values[b2])) == expected);
		assert(Intersects(make_pair(values[a2], values[a1]), make_pair(values[b1], values[b2])) == expected);
		assert(Intersects(make_pair(values[a1], values[a2]), make_pair(values[b2], values[b1])) == expected);
		assert(Intersects(make_pair(values[a2], values[a1]), make_pair(values[b2], values[b1])) == expected);
		};

	Test('B', 'D', 'A', 'C', false);
	Test('A', 'B', 'C', 'D', true);
	Test('L', 'K', 'H', 'F', false);
	Test('E', 'C', 'F', 'B', true);
	Test('P', 'C', 'E', 'B', false);
	Test('P', 'C', 'A', 'B', true);
	Test('O', 'E', 'C', 'F', false);
	Test('L', 'C', 'M', 'N', false);
	Test('L', 'C', 'N', 'B', false);
	Test('L', 'C', 'M', 'K', true);
	Test('L', 'C', 'G', 'I', false);
	Test('L', 'C', 'I', 'E', true);
	Test('M', 'O', 'N', 'F', true);
}
#endif	// SKIP

#ifdef SKIP
auto TestSplit() -> void
{
	std::vector<double> expected = { -123.456, -987.654 };

	auto Test = [&expected](const std::string &input)
		{
		auto actual = Split(input, " ,\t");
		assert(actual.size() >= 2);
		assert(Equal(atof(actual[0].c_str()), expected[0]));
		assert(Equal(atof(actual[1].c_str()), expected[1]));
		};

	Test("-123.456 -987.654");
	Test("-123.4560 -987.6540");
	Test("-123.45600 -987.65400");
	Test("-123.456 -987.654 ");
	Test("-123.456 -987.654 100.5");
	Test("-123.456 -987.654 hello");
	Test("-123.456 -987.654 hello world");

	Test("-123.456,-987.654");
	Test("-123.4560,-987.6540");
	Test("-123.45600,-987.65400");
	Test("-123.456,-987.654,");
	Test("-123.456,-987.654,100.5");
	Test("-123.456,-987.654,hello");
	Test("-123.456,-987.654,hello,world");

	Test("-123.456\t-987.654");
	Test("-123.4560\t-987.6540");
	Test("-123.45600\t-987.65400");
	Test("-123.456\t-987.654\t");
	Test("-123.456\t-987.654\t100.5");
	Test("-123.456\t-987.654\thello");
	Test("-123.456\t-987.654\thello\tworld");

	Test(" -123.456   -987.654   ");
	Test(" -123.4560  -987.6540  ");
	Test(" -123.45600 -987.65400 ");
	Test(" -123.456   -987.654  ");
	Test(" -123.456   -987.654  100.5");
	Test(" -123.456   -987.654  hello");
	Test(" -123.456   -987.654  hello   world");
}
#endif	// SKIP

#ifdef SKIP
// String tokenise using any one of delimiters, adjacent spaces are treated as one
auto Split(const std::string &value, const char *delims) -> std::vector<std::string>
{
	std::vector<std::string> ret;

	size_t start = value.find_first_not_of(' ', 0);
	while (start != std::string::npos)
		{
		size_t pos = value.find_first_of(delims, start);
		if (pos == std::string::npos)
			{
			ret.push_back(value.substr(start));
			break;
			}
		else
			{
			ret.push_back(value.substr(start, pos - start));

			if (value[pos] == ' ' && strchr(delims, ' '))
				start = value.find_first_not_of(' ', pos);
			else
				start = pos + 1;
			}
		}

	return ret;
}
#endif	// SKIP


#ifdef SKIP
// Print program usage info.
auto Usage() -> void
{
	std::cout << "Usage: concave.exe filename [-out arg] [-k arg] [-field_for_x arg] [-field_for_y arg] [-no_out] [-no_iterate]\n";
	std::cout << "\n";
	std::cout << " filename      (required) : file of input coordinates, one row per point.\n";
	std::cout << " -out          (optional) : output file for the hull polygon coordinates. Default=stdout.\n";
	std::cout << " -k            (optional) : start iteration K value. Default=3.\n";
	std::cout << " -field_for_x  (optional) : 1-based column number of input for x-coordinate. Default=1.\n";
	std::cout << " -field_for_y  (optional) : 1-based column number of input for y-coordinate. Default=2.\n";
	std::cout << " -no_out       (optional) : disable output of the hull polygon coordinates.\n";
	std::cout << " -no_iterate   (optional) : stop after only one iteration of K, irrespective of result.\n";
}
#endif	// SKIP
