#ifndef _GROMETRIC_H
#define _GROMETRIC_H

#include <vector>

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

} SURFACE_3D;

typedef struct{

    #ifdef __cplusplus
    std::vector<SURFACE_3D> flist;
    #endif

    //SURFACE_3D *flist;
    //SURFACE_3D *fend;

    int n_faces;

} PATCH;

//Function implement
POINT_3D addPoint3D( double x, double y, double z );
VEC vectorPP( POINT_3D st, POINT_3D ed );

SURFACE_3D addSurface3D( int amount, ... );

PATCH createPatch();
inline void addPatch( PATCH patch, SURFACE_3D face );

#endif

