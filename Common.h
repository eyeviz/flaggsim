#ifndef _Common_H		// begining of header file
#define _Common_H		// notifying that this file is included

#include <iostream>
#include <vector>
using namespace std;
// #define VALIDATE_WEIGHT

// #define ACTIVATE_SNAPS

// #define USING_SCALE_512


#define SHRINK_BUILDING_POLYGONS

#ifdef SHRINK_BUILDING_POLYGONS
#define DEFAULT_SHRUNK_RATIO		(0.98)
#endif	// SHRINK_BUILDING_POLYGONS


#define TEXTURE_DIR		"texture"
#define TEXTURE_NAME		"label"

//------------------------------------------------------------------------------
//	Macro switches
//------------------------------------------------------------------------------
#define ACTIVATE_RECORDING_MODE

#ifdef ACTIVATE_RECORDING_MODE
#define INPUT_FILE_NAME		"in.png"
#define AGGREGATE_FILE_NAME	"out-a.png"
#define SIMPLIFY_FILE_NAME	"out-s.png"
#endif	// ACTIVATE_RECORDING_MODE

//------------------------------------------------------------------------------
//	Macro definitions
//------------------------------------------------------------------------------
#ifndef TRUE
#define TRUE	(1)
#endif	// TRUE

#ifndef FALSE
#define FALSE	(0)
#endif	// FALSE

#ifndef MAX2
#define MAX2( x, y )		(( (x)<(y) )? (y) : (x) )
#endif	// MAX2

#ifndef MIN2
#define MIN2( x, y )		(( (x)>(y) )? (y) : (x) )
#endif	// MIN2

#ifndef SQUARE
#define SQUARE( x )		((x)*(x))
#endif	// SQUARE

#ifndef NO_INDEX
#define NO_INDEX	        (10001)
#endif	// NO_INDEX

#ifndef NO_NAME
#define NO_NAME			(10001)
#endif	// NO_NAME
    
// EPS should be avoided due to conflicts with CGAL
#ifndef EPSILON
#define EPSILON		(1.0e-8)
#endif	// EPSILON

#ifndef INTERMEDIATE_EPSILON
//#define INTERMEDIATE_EPSILON	(5.0e-6)
#define INTERMEDIATE_EPSILON	(2.0e-6)
#endif	// INTERMEDIATE_EPSILON

#ifndef FINAL_EPSILON
//#define FINAL_EPSILON		(5.0e-7)
#define FINAL_EPSILON		(2.0e-7)
#endif	// FINAL_EPSILON

// 1.0e-3 - 1.0e-6
#ifndef AUGMENTED_EPSILON
// #define AUGMENTED_EPSILON	(1.0e-3) // NG
// #define AUGMENTED_EPSILON	(1.0e-4) // NG
// #define AUGMENTED_EPSILON	(1.0e-5) // NG
#define AUGMENTED_EPSILON	(1.0e-6) // OK
#endif	// AUGMENTED_EPSILON

#ifndef TRANSFORMATION_STEP
//#define TRANSFORMATION_STEP		(0.25) // too strong
//#define TRANSFORMATION_STEP		(0.20)
//#define TRANSFORMATION_STEP		(0.15)
// #define TRANSFORMATION_STEP		(0.10) // <==
// #define TRANSFORMATION_STEP		(0.05) // large graphs
#define TRANSFORMATION_STEP		(0.02)
// #define TRANSFORMATION_STEP		(0.01)
#endif	// TRANSFORMATION_STEP

#ifndef DISPLACEMENT_LIMIT
#define DISPLACEMENT_LIMIT		(1.00)
#endif // DISPLACEMENT_LIMIT

#ifndef CELL_RATIO
//#define CELL_RATIO			(4.00)
#define CELL_RATIO			(10.00)
#endif	// CELL_RATIO


#ifndef BUFFER_SIZE
#define	BUFFER_SIZE		(256)
#endif	// BUFFER_SIZE

#ifndef SELECTION_SIZE
#define	SELECTION_SIZE		(4096)
#endif	// SELECTION_SIZE

#ifndef MAX_INT
#define MAX_INT		(1.0e+8)
#endif	// MAX_INT

#ifndef NO_DIST
#define NO_DIST		(-1)
#endif	// NO_DIST

#ifndef INFINITY
#define INFINITY	(1.0e+8)
#endif	// INFINITY

#ifndef HERE
#define HERE		" " << __FILE__ << ":" << __LINE__ << " "
#endif	// HERE

#ifndef INFINITY
#define	INFINITY		(1.0e+8)
#endif	// INFINITY
#ifndef MAX_VALUE
#define MAX_VALUE		(1.0e+8)
#endif	// MAX_VALUE


#ifndef FONT_SIZE
//#define FONT_SIZE		(12)
//#define FONT_SIZE		(16)
#define FONT_SIZE		(24)
#endif

typedef enum {
    ORIGINAL, SINGLE, MULTIPLE
} MapMode;

typedef unsigned int					ID;
typedef vector< unsigned int >				Arc;
typedef pair< unsigned int, unsigned int >		Link;
typedef pair< unsigned int, unsigned int >		Tuple;
typedef vector< Link >					Diag;
//typedef vector< unsigned int >				Set;

//------------------------------------------------------------------------------
//	Inline functions
//------------------------------------------------------------------------------
inline void breakpoint( void )
{
    cerr << " Press enter to proceed : ";
    getchar();
}


#endif // _Common_H

// end of header file
// Do not add any stuff under this line.
