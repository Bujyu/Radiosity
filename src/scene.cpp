#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <vector>

#include "scene.hpp"

MODEL createModel( char* path, float pos[], float color[] ){

    MODEL m;
    float center[3], radius = 0.0;

    readMesh( &(m.mesh), path, center, radius );

    //Update Face Normal
    m.mesh.request_face_normals();
	m.mesh.update_normals();

    //Attribute
    SET( m.position, pos );
    SET( m.color, color );

    SET( m.center, center );
    m.radius = radius;

    m.faces = m.mesh.n_faces();
    m.edges = m.mesh.n_edges();
    m.vertices = m.mesh.n_vertices();

    return m;

}

SCENE createScene(){

    SCENE scene;

    scene.faces = 0;
    scene.edges = 0;
    scene.vertices = 0;

    return scene;

}

SCENE readScene( char* path ){

    SCENE scene = createScene();
    MODEL model;
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

        model = createModel( mpath, pos, color );
        scene.vmodel.push_back( model );

        //Update scene attributes
        scene.faces += model.faces;
        scene.edges += model.edges;
        scene.vertices += model.vertices;

    }

    fclose( file );

    return scene;

}

int getMeshHandle( int n, SCENE scene ){

    int i;

    if( n > scene.faces )
        exit( -1 );

    for( i = 0 ; i < (int) scene.vmodel.size() ; i++ ){
        if( n < scene.vmodel[i].faces )
            return i;
        else
            n -= scene.vmodel[i].faces;
    }

    return -1;

}

Mesh::FaceHandle getFaceHandle( int n, SCENE scene ){

    int i;

    if( n > scene.faces )
        exit( -1 );

    for( i = 0 ; i < (int) scene.vmodel.size() ; i++ ){
        if( n < scene.vmodel[i].faces )
            return scene.vmodel[i].mesh.face_handle( n );
        else
            n -= scene.vmodel[i].faces;
    }

    return scene.vmodel[0].mesh.face_handle( 0 );;

}

int main(){

    int i;
    int num;
    SCENE scene;
    Mesh::FaceHandle f;
    Mesh::ConstFaceVertexIter   fv_it;

    scene = readScene( (char*) "scene.sce" );

    std::cout << scene.faces << std::endl;

    for( i = 0 ; i < scene.faces ; i++ ){

        num = getMeshHandle( i, scene );
        f = getFaceHandle( i, scene );

        fv_it = scene.vmodel[num].mesh.cfv_iter( f );;
        std::cout << scene.vmodel[num].mesh.point( fv_it ) << std::endl;
        ++fv_it;
        std::cout << scene.vmodel[num].mesh.point( fv_it ) << std::endl;
        ++fv_it;
        std::cout << scene.vmodel[num].mesh.point( fv_it ) << std::endl << std::endl;

    }

    return 0;

}
