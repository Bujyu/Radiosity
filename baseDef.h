#ifndef BASE_DEF_H
#define BASE_DEF_H

#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
typedef OpenMesh::TriMesh_ArrayKernelT<>  Mesh;

//Scene
typedef struct {

    //Model
    MODEL *model;

    //Axis
    double x_axis;
    double y_axis;
    double z_axis;

    //Element#
    int faces;
    int edges;
    int vertices;

} SCENE;

typedef struct {

    //Main variable
    Mesh mesh;

    //Attributes
    int type;
    float color[3];
    float position[3];

    //Model information
    float center[3];
    float radius;

    //Next ptr
    MODEL* next;

} MODEL;

#endif
