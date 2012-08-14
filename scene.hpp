#ifndef BASE_DEF_H
#define BASE_DEF_H

#include <vector>

#include "mesh.hpp"

#define SET( m, data ) \
        { \
        m[0] = data[0];\
        m[1] = data[1];\
        m[2] = data[2];\
        }

typedef struct model {

    //Main variable
    Mesh mesh;

    //Attributes
    int type;
    float color[3];
    float position[3];

    //Model information
    float center[3];
    float radius;

    int faces;
    int edges;
    int vertices;

    //Next ptr
    struct model* next;

} MODEL;

//Scene
typedef struct scene{

    //Model
    std::vector<MODEL> vmodel;  // By using std library

    //Axis
    double x_axis;
    double y_axis;
    double z_axis;

    //Element#
    int faces;
    int edges;
    int vertices;

} SCENE;


MODEL createModel( char* path, float pos[], float color[] );

SCENE createScene();
SCENE readScene( char* path );

#endif
