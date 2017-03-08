#ifndef _GROMETRIC_H
#define _GROMETRIC_H

#include <windows.h>
#include <GL/glut.h>
#include <vector>
#include "vector.hpp"

#define PTEQU( a, b ) ( a.x == b.x && a.y == b.y && a.z == b.z )

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

    //Color
    double rad[3];
    int count;

} POINT_3D;

// Line
typedef struct{

    POINT_3D start;
    POINT_3D end;

} LINE_3D;

// Surface
typedef struct surface{

    POINT_3D *plist;
    POINT_3D center;
    int n_point;

    //Addition information
    VEC normal;
    double FF;
    int visited;

    struct surface* ne;
    struct surface* nw;
    struct surface* se;
    struct surface* sw;

} SURFACE_3D;

// Patch
typedef struct{

    std::vector<SURFACE_3D> flist;

    int n_face;

} PATCH;

// Model
typedef struct{

    std::vector<PATCH> plist;

    int n_face;
    int n_patch;

    double emission[3];
    double reflection[3];

} MODEL;

// Scene
typedef struct{

    std::vector<MODEL> list;

    int n_face;
    int n_patch;
    int n_model;

} SCENE;

//Function implement
// Point
POINT_3D addPoint3D( double x, double y, double z );
VEC vectorPP( const POINT_3D st, const POINT_3D ed );
int checkInOutPP( POINT_3D st, POINT_3D ed, POINT_3D pt );
POINT_3D centerPP( POINT_3D a, POINT_3D b );
double areaTri( POINT_3D pa, POINT_3D pb, POINT_3D pc );
double lengthPP( POINT_3D a, POINT_3D b );
void ptPrint( POINT_3D p );

// Surface
SURFACE_3D addSurface3D( int amount, ... );
void addSurface3DNode( SURFACE_3D *face, int amount, ... );
POINT_3D surfaceCenter( const SURFACE_3D &face );
void setSurface3DNormal( SURFACE_3D *face, double x, double y, double z );

void interpolationSqr( POINT_3D *ipt, const SURFACE_3D &face, double u, double v );
void interpolationTri( POINT_3D *ipt, const SURFACE_3D &face, double u, double v );
void interpolation( POINT_3D *ipt, const SURFACE_3D &face, double u, double v );

double squareArea( SURFACE_3D face );
double triangleArea( SURFACE_3D face );
double triangleAreaPP( double a, double b, double c );
double surfaceArea( SURFACE_3D face );

// Patch
PATCH createPatch();
void addPatch( PATCH *patch, SURFACE_3D face );
void drawPatch( const PATCH &patch, int c );

void clipQuadSurface( PATCH *patch );

// Model
MODEL createModel();
void addModel( MODEL *model, PATCH patch );
void setEmission( MODEL *model, double r, double g, double b );
void setReflection( MODEL *model, double r, double g, double b );
void clipPatch( MODEL *model );

// Refine Clip
int subdiv( SURFACE_3D *f, int Aeps );
double FormFactorEstimate( SURFACE_3D *i,  SURFACE_3D *j );
void refineClip( SURFACE_3D *i, SURFACE_3D *j, double Aeps, double Feps );
void travelNode( SURFACE_3D *face );
void delNode( SURFACE_3D *face );

// Scene
SCENE createScene();
void addScene( SCENE *scene, MODEL model );
int searchSceneSurface( const SCENE &scene, int count, int *m, int *p, int *f );
int searchScenePatch( const SCENE &scene, int count, int *m, int *p );
int searchSceneModel( const SCENE &scene, int count, int *m );

// Extension
double clap( double num, double mboundary, double pboundary );
int vIntersection( VEC v, int plane, POINT_3D *ipt );

// OpenGL
inline void glVertex3fp( POINT_3D pt ){glVertex3f( pt.x, pt.y, pt.z );}

#endif

