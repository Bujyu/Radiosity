#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <vector>

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

void addModel( MODEL **mlist, MODEL *model ){

    model->next = *mlist;
    *mlist = model;

}

SCENE createScene(){

    SCENE scene;

    scene.faces = 0;
    scene.edges = 0;
    scene.vertices = 0;

    scene.model = NULL;

    return scene;

}

SCENE readScene( char* path ){

    SCENE scene = createScene();
    MODEL *model;
    FILE *file = fopen( (char*) path, "r+" );

    char mpath[1000];
    float pos[3];
    float color[3];

    if( file == NULL ){
        printf("File is not exist.\n");
        exit( 0 );
    }

    while( 1 ){

        fscanf( file, "%s", mpath );

        if( !strcmp( mpath, "#" ) )
            break;

        fscanf( file, "%f %f %f", &pos[0], &pos[1], &pos[2] );
        fscanf( file, "%f %f %f", &color[0], &color[1], &color[2] );

        model = (MODEL*) malloc( sizeof( MODEL ) );
        *model = createModel( mpath, pos, color );

        addModel( &scene.model, model );
        scene.vmodel.push_back( *model );

    }

    fclose( file );

    for( ; scene.model ; scene.model = scene.model->next )
        printf("%d\n", scene.model->mesh.n_faces() );

    for( int n = 0 ; n < scene.vmodel.size() ; n++ )
        printf("%d\n", scene.vmodel[n].mesh.n_faces() );

    return scene;

}

int main(){

    SCENE scene;

    scene = readScene( (char*) "scene.sce" );

    return 0;

}
