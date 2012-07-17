#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "baseDef.h"

#define SET( m, data ) \
        { \
        m[0] = data[0];\
        m[1] = data[1];\
        m[2] = data[2];\
        }

MODEL createModel( char* path, float pos[], float color[] ){

    MODEL m;
    float center[3], radius;

    OpenMesh::IO::read_mesh( m.mesh, path );

    Mesh::ConstVertexIter	v_it( m.mesh.vertices_begin() ),
                            v_end( m.mesh.vertices_end() );
	Mesh::Point             bbMin, bbMax;

	bbMin = bbMax = m.mesh.point(v_it);  // accessing the Point of a vertex
	for (; v_it!=v_end; ++v_it){
		bbMin.minimize( m.mesh.point(v_it));
		bbMax.maximize( m.mesh.point(v_it));
	}

	for (int i = 0; i < 3; i++)
		center[i] = (bbMin[i] +bbMax[i])/2;
	radius = (bbMax-bbMin).norm()/2;  // set radius as half of bbox diagonal

	m.mesh.request_face_normals();
	m.mesh.update_normals();

    SET( m.position, pos );
    SET( m.color, color );
    SET( m.center, center );
    m.radius = radius;

    return m;

}

void addModel( SCENE *scene, MODEL m ){

    (*scene).model = &m;
    m.next = NULL;


}

SCENE createScene(){

    SCENE scene;;

    scene.faces = 0;
    scene.edges = 0;
    scene.vertices = 0;

    scene.model = NULL;

    return scene;

}

SCENE readScene( char* path ){

    SCENE scene = createScene();
    FILE* file = fopen( path, "r" );

    char mpath[1000];
    float pos[3];
    float color[3];

    if( file == NULL ){
        printf("File is not exist.\n");
        exit( 0 );
    }

    while( true ){

        fscanf( file, "%s", mpath );

        if( strcmpi( mpath, "#" ) )
            break;

        fscanf( file, "%f %f %f", &pos[0], &pos[1], &pos[2] );
        fscanf( file, "%f %f %f", &color[0], &color[1], &color[2] );

        addModel( &scene, createModel( mpath, pos, color ) );

    }

    return scene;

}
