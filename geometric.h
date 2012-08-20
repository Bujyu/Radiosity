#ifndef _GROMETRIC_H
#define _GROMETRIC_H

#include <vector>
#include "vector.h"

#ifdef __cplusplus
extern "C" {
#endif

// Data type definition
// 2-Dimension
// Point
typedef struct{

    double x;
    double y;

} POINT_2D;

// Line
typedef struct{

    POINT_2D start;
    POINT_2D end;

} LINE_2D;

// 3-Dimension
// Point
typedef struct{

    double x;
    double y;
    double z;

} POINT_3D;

// Line
typedef struct{

    POINT_3D start;
    POINT_3D end;

} LINE_3D;

// Surface
typedef struct{

    POINT_3D *plist;
    int n_point;

    //Addition information
    double FF;
    int visited;

} SURFACE_3D;

typedef struct{

    #ifdef __cplusplus
    std::vector<SURFACE_3D> flist;
    #endif

    //SURFACE_3D *flist;
    //SURFACE_3D *fend;

    int n_face;

} PATCH;

//Function implement
POINT_3D addPoint3D( double x, double y, double z );
VEC vectorPP( POINT_3D st, POINT_3D ed );
int checkInOutPP( POINT_3D st, POINT_3D ed, POINT_3D pt );
double lengthPP( POINT_3D a, POINT_3D b );
void ptPrint( POINT_3D p );

SURFACE_3D addSurface3D( int amount, ... );
POINT_3D surfaceCenter( SURFACE_3D face );
double triangleArea( SURFACE_3D face );
double squareArea( SURFACE_3D face );
double surfaceArea( SURFACE_3D face );

PATCH createPatch();
void addPatch( PATCH *patch, SURFACE_3D face );

//
double clap( double num, double mboundary, double pboundary );
int vIntersection( VEC v, int plane, POINT_3D *ipt );

#ifdef __cplusplus
}
#endif

#endif

