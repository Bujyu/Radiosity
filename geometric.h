#ifndef _GROMETRIC_H
#define _GROMETRIC_H

#include <windows.h>
#include <GL/glut.h>
#include <vector>
#include "vector.hpp"

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
    VEC normal;
    double FF;
    int visited;

} SURFACE_3D;

typedef struct{

    std::vector<SURFACE_3D> flist;

    //SURFACE_3D *flist;
    //SURFACE_3D *fend;

    int n_face;
    double emission[3];
    double reflection[3];

} PATCH;

typedef struct{

    #ifdef __cplusplus
    std::vector<PATCH> list;
    #endif

    int n_patch;
    int n_face;

} SCENE;

//Function implement
POINT_3D addPoint3D( double x, double y, double z );
VEC vectorPP( const POINT_3D st, const POINT_3D ed );
int checkInOutPP( POINT_3D st, POINT_3D ed, POINT_3D pt );
POINT_3D centerPP( POINT_3D a, POINT_3D b );
double lengthPP( POINT_3D a, POINT_3D b );
void ptPrint( POINT_3D p );

SURFACE_3D addSurface3D( int amount, ... );
POINT_3D surfaceCenter( const SURFACE_3D &face );
void setSurface3DNormal( SURFACE_3D *face, double x, double y, double z );

double triangleArea( SURFACE_3D face );
double squareArea( SURFACE_3D face );
double surfaceArea( SURFACE_3D face );

PATCH createPatch();
void addPatch( PATCH *patch, SURFACE_3D face );
void setEmission( PATCH *patch, double r, double g, double b );
void setReflection( PATCH *patch, double r, double g, double b );
void drawPatch( const PATCH &patch, int c );
void clipSurface( PATCH *patch );

SCENE createScene();
void addScene( SCENE *scene, PATCH patch );
int searchScene( const SCENE &scene, int count, int *p, int *f );

//
double clap( double num, double mboundary, double pboundary );
int vIntersection( VEC v, int plane, POINT_3D *ipt );

//OpenGL
inline void glVertex3fg( POINT_3D pt ){
    glVertex3f( pt.x, pt.y, pt.z );
}

#endif

