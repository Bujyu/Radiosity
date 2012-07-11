#include <stdio.h>
#include <stdlib.h>

#inlcude <baseDef.h>

SCENE createScene(){

    SCENE scene = (SCENE) malloc( sizeof( SCENE ) );

    scene->faces = 0;
    scene->edges = 0;
    scene->vertices = 0;

    scene->model = NULL;

    return scene;

}

SCENE readScene( char* path ){

    SCENE scene = createScene();




}
