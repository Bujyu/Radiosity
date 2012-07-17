#ifndef BASE_DEF_H
#define BASE_DEF_H

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
typedef OpenMesh::TriMesh_ArrayKernelT<>  Mesh;

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

    //Next ptr
    struct model* next;

} MODEL;

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


#endif
