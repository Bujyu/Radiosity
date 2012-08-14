#include <iostream>
#include <vector>
#include <ctime>

#include <windows.h>
#include <gl/gluit.h>
#include <gl.hh>

#include "mesh.hpp"

extern void hemiCubeGenrater();
extern void drawHemiCube();

Mesh mesh;
Mesh mesh2;
int gw, gh;

//Main Func
int mainwin;
int viewer;

/*
void drawMesh(){

    Mesh::ConstFaceIter         f_it( _mesh.faces_begin() ),
                                f_end( _mesh.faces_end() );
    Mesh::ConstFaceVertexIter   fv_it;

    glColor3f( 1.0, 1.0, 1.0 );
    //glBegin( GL_TRIANGLES );
    for( ; f_it != f_end ; ++f_it ){
        glBegin( GL_LINE_LOOP );
        fv_it = _mesh.cfv_iter( f_it.handle() );
        GL::glVertex( _mesh.point( fv_it ) );
        ++fv_it;
        GL::glVertex( _mesh.point( fv_it ) );
        ++fv_it;
        GL::glVertex( _mesh.point( fv_it ) );
        glEnd();
    }
    //glEnd();
    //glFlush();

}

void display( void ){

    glClear( GL_COLOR_BUFFER_BIT );

    drawMesh();
    drawMeshAnime();

}

void reshape( int w, int h ){

    gw = w;
    gh = h;

}
*/
void keyboard( unsigned char key, int x, int y ){

    switch( key ){
        case 's':
        case 'S':
            meshSplit( &mesh );
            meshSplit( &mesh2 );
            break;
        case 'e':
        case 'E':
            edgeMeshSplit( &mesh );
            edgeMeshSplit( &mesh2 );
            break;
        default:
            break;
    }

    //glClear( GL_COLOR_BUFFER_BIT );
    glutPostRedisplay();

}
/*
void init( void ){

    float center[3], radius = 0.0;

    readMesh( (char*) "Models/mushroom.off", center, radius );

    glClearColor( 0.0, 0.0, 0.0, 1.0 );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( -1.0, 1.0, -1.0, 1.0, -1.0, 1.0 );

}

int main( int argc, char **argv ){

    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_SINGLE | GLUT_RGBA );
    glutInitWindowSize( 600, 600 );
    glutCreateWindow( "TEST" );

    init();

    glutDisplayFunc( display );
    glutReshapeFunc( reshape );
    glutKeyboardFunc( keyboard );
    glutMainLoop();

    return 0;

}
*/

void axis(){

    glPushAttrib( GL_ALL_ATTRIB_BITS );

    glLineWidth( 3.0 );

    glColor3f( 1.0, 0.0, 0.0 );
    glBegin( GL_LINES );
        glVertex3f( 0.0, 0.0, 0.0 );
        glVertex3f( 0.1, 0.0, 0.0 );
    glEnd();

    glColor3f( 0.0, 1.0, 0.0 );
    glBegin( GL_LINES );
        glVertex3f( 0.0, 0.0, 0.0 );
        glVertex3f( 0.0, 0.1, 0.0 );
    glEnd();

    glColor3f( 0.0, 0.0, 1.0 );
    glBegin( GL_LINES );
        glVertex3f( 0.0, 0.0, 0.0 );
        glVertex3f( 0.0, 0.0, 0.1 );
    glEnd();

    glPopAttrib();

}

void init(){

    float center[3], radius = 0.0;

    hemiCubeGenrater();

    //Depth
    glEnable( GL_DEPTH_TEST );

    readMesh( &mesh, (char*) "Models/five-face.off", center, radius );
    readMesh( &mesh2, (char*) "Models/mushroom.off", center, radius );

    //Initial color
    glClearColor( 0.0, 0.0, 0.0, 1.0 );

}

void reshape( int w, int h ){

	glViewport( 0, 0, w, h );
	ViewerReshape( viewer );

}

void display(){}

void content( void ){

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    drawMesh( mesh );
    drawMesh( mesh2 );

/*
    int n_cells = 8;

    double cellSize = (double) 2 / n_cells;
    double halfSize = n_cells / 2;
    int i, j, k;

    //Top
    for( i = -halfSize ; i < halfSize ; i++ ){
        for( j = -halfSize ; j < halfSize ; j++ ){

            glPolygonMode( GL_BACK, GL_LINE );
            glBegin( GL_POLYGON );
            glVertex3f( (double)i * cellSize, halfSize * cellSize, (double)j * cellSize );
            glVertex3f( (double)i * cellSize, halfSize * cellSize, (double)( j + 1 ) * cellSize );
            glVertex3f( (double)( i + 1 ) * cellSize, halfSize * cellSize, (double)( j + 1 ) * cellSize);
            glVertex3f( (double)( i + 1 ) * cellSize, halfSize * cellSize, (double)j * cellSize );
            glEnd();

        }
    }

    //Front and Back
    for( i = -halfSize ; i < halfSize ; i++ ){
        for( j = 0; j < halfSize ; j++ ){

            glBegin( GL_POLYGON );
            glVertex3f( (double)( i + 1 ) * cellSize, (double)j * cellSize        , (double)halfSize * cellSize );
            glVertex3f( (double)( i + 1 ) * cellSize, (double)( j + 1 ) * cellSize, (double)halfSize * cellSize );
            glVertex3f( (double)        i * cellSize, (double)( j + 1 ) * cellSize, (double)halfSize * cellSize );
            glVertex3f( (double)        i * cellSize, (double)j * cellSize        , (double)halfSize * cellSize );
            glEnd();

            glBegin( GL_POLYGON );
            glVertex3f( (double)        i * cellSize, (double)j * cellSize        , (double)-halfSize * cellSize );
            glVertex3f( (double)        i * cellSize, (double)( j + 1 ) * cellSize, (double)-halfSize * cellSize );
            glVertex3f( (double)( i + 1 ) * cellSize, (double)( j + 1 ) * cellSize, (double)-halfSize * cellSize );
            glVertex3f( (double)( i + 1 ) * cellSize, (double)j * cellSize        , (double)-halfSize * cellSize );
            glEnd();

        }
    }

    // Right and Left
    for( i = 0; i < halfSize ; i++ ){
        for( j = -halfSize; j < halfSize ; j++ ){

            glBegin( GL_POLYGON );
            glVertex3f( (double)halfSize * cellSize, (double)( i + 1 ) * cellSize, (double)j * cellSize         );
            glVertex3f( (double)halfSize * cellSize, (double)( i + 1 ) * cellSize, (double)( j + 1 ) * cellSize );
            glVertex3f( (double)halfSize * cellSize, (double)        i * cellSize, (double)( j + 1 ) * cellSize );
            glVertex3f( (double)halfSize * cellSize, (double)        i * cellSize, (double)j * cellSize         );
            glEnd();

            glBegin( GL_POLYGON );
            glVertex3f( (double)-halfSize * cellSize, (double)        i * cellSize, (double)j * cellSize         );
            glVertex3f( (double)-halfSize * cellSize, (double)        i * cellSize, (double)( j + 1 ) * cellSize );
            glVertex3f( (double)-halfSize * cellSize, (double)( i + 1 ) * cellSize, (double)( j + 1 ) * cellSize );
            glVertex3f( (double)-halfSize * cellSize, (double)( i + 1 ) * cellSize, (double)j * cellSize         );
            glEnd();
        }
    }
*/

    drawHemiCube();

	glutSwapBuffers();

}

int main( int argc, char** argv ){

    //Initial
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( 600, 600 );

    //Create Window
    mainwin = glutCreateWindow( "test" );

    //Call Back Function
    glutDisplayFunc( display );
    glutReshapeFunc( reshape );

    //Veiwer
    viewer = ViewerNew( mainwin, 0, 0, 600, 600, (void*(*)()) content);
    init();

    ViewerExtent (viewer, 16);
	glutKeyboardFunc (keyboard);

	glutMainLoop();

    return 0;

}
