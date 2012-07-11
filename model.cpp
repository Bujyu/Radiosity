#include <stdio.h>
#include <stdlib.h>

#inlcude <baseDef.h>

MODEL readModel( char* path ){

    Mesh mesh;

    OpenMesh::IO::read_mesh( mesh, path );

    Mesh::ConstVertexIter	v_it( mesh.vertices_begin() ),
                            v_end( mesh.vertices_end() );

	Mesh::Point             bbMin, bbMax;


	bbMin = bbMax = mesh.point(v_it);  // accessing the Point of a vertex
	for (; v_it!=v_end; ++v_it){
		bbMin.minimize( mesh.point( v_it ) );
		bbMax.maximize( mesh.point( v_it ) );
	}

	for( int i = 0; i < 3; i++)
		center[i] = ( bbMin[i] + bbMax[i] ) / 2;
	radius = ( bbMax - bbMin ).norm() / 2;  // set radius as half of bbox diagonal

	mesh.request_face_normals();
	mesh.update_normals();

}

