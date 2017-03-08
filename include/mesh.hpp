#ifndef MESH_H
#define MESH_H

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

typedef OpenMesh::TriMesh_ArrayKernelT<>  Mesh;

/// Read
void readMesh( Mesh *_mesh, char* path, float center[3], float radius );

/// Draw
void drawMesh( Mesh _mesh );
void drawMeshAnime( Mesh _mesh );

/// Split
void edgeSplit( Mesh *_mesh, int eidx );
void edgeMeshSplit( Mesh *_mesh );
void checkEdge( Mesh *_mesh );
//2 to 4
void triMeshSplit( Mesh *_mesh );
void meshSplit( Mesh *_mesh );

#endif
