#include <iostream>
#include <vector>
#include <ctime>

#include <windows.h>
#include <gl/gluit.h>
#include <gl.hh>

#include "matrix.hpp"
#include "vector.hpp"

#include "mesh.hpp"
#include "geometric.h"

#define CLIP 0

extern void hemiCubeGenrator();
extern void drawHemiCube();
extern void test();

extern double meshToHC( SURFACE_3D i, SURFACE_3D j );
extern double calMeshFF( SURFACE_3D i, VEC inormal, SURFACE_3D j, VEC jnormal );

extern VEC matrix_solution( VEC emission, VEC reflection, MAT FF );

//Mesh mesh;
//Mesh mesh2;
int gw, gh;

//Main Func
int mainwin;
int viewer;

//Model
PATCH sphere;
PATCH square;
PATCH wall[5];
PATCH lightSource;

SCENE scene;

// Matrix
MAT FF;
VEC e[3];
VEC p[3];
VEC b[3];

void sphereCreate( double x, double y, double z ){

    SURFACE_3D face;
    POINT_3D a, b, c, d;

    POINT_3D **sp;

    int deg, fi;
    int r = 4;

    VEC va, vb, n;

    sphere = createPatch();

    sp = (POINT_3D**) malloc( 19 * sizeof( POINT_3D ) );
    for( deg = 0; deg < 19 ; deg++ )
        sp[deg] = (POINT_3D*) malloc( 37 * sizeof( POINT_3D ) );

    for( deg = 0 ; deg <= 18 ; deg++ ){
        for( fi = 0 ; fi <= 36 ; fi++ ){
            sp[deg][fi].x = r * cos( ( fi - 18 ) * PI / 18 ) * cos( ( deg - 9 ) * PI / 18 );
            sp[deg][fi].y = r * sin( ( deg - 9 ) * PI / 18 );
            sp[deg][fi].z = r * sin( ( fi - 18 ) * PI / 18 ) * cos( ( deg - 9 ) * PI / 18 );
       }
    }

    for( deg = 0 ; deg < 18 ; deg++ ){
        for( fi = 0 ; fi <= 36 ; fi++ ){

            a = addPoint3D(          sp[deg][fi].x + x,          sp[deg][fi].y + y,          sp[deg][fi].z + z );
            b = addPoint3D(        sp[deg+1][fi].x + x,        sp[deg+1][fi].y + y,        sp[deg+1][fi].z + z );
            c = addPoint3D( sp[deg+1][(fi+1)%36].x + x, sp[deg+1][(fi+1)%36].y + y, sp[deg+1][(fi+1)%36].z + z );
            d = addPoint3D(   sp[deg][(fi+1)%36].x + x,   sp[deg][(fi+1)%36].y + y,   sp[deg][(fi+1)%36].z + z );

            va = vectorPP( sp[deg+1][fi], sp[deg+1][(fi+1)%36] );
            vb = vectorPP( sp[deg+1][fi], sp[deg][fi] );
            n = vCross( va, vb );

            face = ( deg != 0 || deg != 17 ) ? addSurface3D( 4, a, b, c, d ) : addSurface3D( 3, a, b, c );
            setSurface3DNormal( &face, n.vector[0], n.vector[1], n.vector[2] );
            addPatch( &sphere, face );

            vDestroy( va );
            vDestroy( vb );
            vDestroy( n );

        }
    }

}

void squareCreate( double x, double y, double z ){

    SURFACE_3D face;
    POINT_3D pt[8];

    square = createPatch();

    pt[0] = addPoint3D( -2 + x, -4 + y, -2 + z );
    pt[1] = addPoint3D(  2 + x, -4 + y, -2 + z );
    pt[2] = addPoint3D(  2 + x,  4 + y, -2 + z );
    pt[3] = addPoint3D( -2 + x,  4 + y, -2 + z );
    pt[4] = addPoint3D( -2 + x, -4 + y,  2 + z );
    pt[5] = addPoint3D(  2 + x, -4 + y,  2 + z );
    pt[6] = addPoint3D(  2 + x,  4 + y,  2 + z );
    pt[7] = addPoint3D( -2 + x,  4 + y,  2 + z );

    // 3 2 1 0
    face = addSurface3D( 4, pt[3], pt[2], pt[1], pt[0] );
    setSurface3DNormal( &face, 0, 0, -1 );
    addPatch( &square, face );

    // 2 6 5 1
    face = addSurface3D( 4, pt[2], pt[6], pt[5], pt[1] );
    setSurface3DNormal( &face, -1, 0, 0 );
    addPatch( &square, face );

    // 6 7 4 5
    face = addSurface3D( 4, pt[6], pt[7], pt[4], pt[5] );
    setSurface3DNormal( &face, 0, 0, 1 );
    addPatch( &square, face );

    // 7 3 0 4
    face = addSurface3D( 4, pt[7], pt[3], pt[0], pt[4] );
    setSurface3DNormal( &face, 1, 0, 0 );
    addPatch( &square, face );

    //Buttom 0 1 5 4
    face = addSurface3D( 4, pt[1], pt[0], pt[5], pt[4] );
    setSurface3DNormal( &face, 0, 1, 0 );
    addPatch( &square, face );

    //Top 7 6 2 3
    face = addSurface3D( 4, pt[7], pt[6], pt[2], pt[3] );
    setSurface3DNormal( &face, 0, -1, 0 );
    addPatch( &square, face );

}

void wallCreate(){

    SURFACE_3D face;
    POINT_3D pt[8];

    wall[0] = createPatch();
    wall[1] = createPatch();
    wall[2] = createPatch();
    wall[3] = createPatch();
    wall[4] = createPatch();

    lightSource = createPatch();

    pt[0] = addPoint3D( -10, -10, -10 );
    pt[1] = addPoint3D( 10, -10, -10 );
    pt[2] = addPoint3D( 10, 10, -10 );
    pt[3] = addPoint3D( -10, 10, -10 );
    pt[4] = addPoint3D( -10, -10, 10 );
    pt[5] = addPoint3D( 10, -10, 10 );
    pt[6] = addPoint3D( 10, 10, 10 );
    pt[7] = addPoint3D( -10, 10, 10 );

    // 0 1 2 3
    face = addSurface3D( 4, pt[0], pt[1], pt[2], pt[3] );
    setSurface3DNormal( &face, 0, 0, -1 );
    addPatch( &wall[0], face );

    // 1 5 6 2
    face = addSurface3D( 4, pt[1], pt[5], pt[6], pt[2] );
        setSurface3DNormal( &face, 1, 0, 0 );
    addPatch( &wall[1], face );

    // 4 0 3 7
    face = addSurface3D( 4, pt[4], pt[0], pt[3], pt[7] );
    setSurface3DNormal( &face, -1, 0, 0 );
    addPatch( &wall[2], face );

    // Buttom 4 5 1 0
    face = addSurface3D( 4, pt[4], pt[5], pt[1], pt[0] );
    setSurface3DNormal( &face, 0, 1, 0 );
    addPatch( &wall[3], face );

    // Top 3 2 6 7
    face = addSurface3D( 4, pt[3], pt[2], pt[6], pt[7] );
    setSurface3DNormal( &face, 0, -1, 0 );
    addPatch( &wall[4], face );

    // Light Source
    face = addSurface3D( 4, pt[5], pt[4], pt[7], pt[6] );
    setSurface3DNormal( &face, 0, 0, 1 );
    addPatch( &lightSource, face );

}

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
            //meshSplit( &mesh );
            //meshSplit( &mesh2 );
            break;
        case 'e':
        case 'E':
            //edgeMeshSplit( &mesh );
            //edgeMeshSplit( &mesh2 );
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

    LARGE_INTEGER t1, t2, ts;
    QueryPerformanceFrequency(&ts);

    //float center[3], radius = 0.0;

    //Model Create
    //sphereCreate( 4, -6, 0 );

    //squareCreate( -4, -6, 0 );
    //for( int i = 0 ; i < CLIP ; i++ )clipSurface( &square );

    wallCreate();
    setReflection( &wall[0], 0.84, 0.84, 0.84 );
    setReflection( &wall[1], 1.0, 0.0, 0.0 );
    setReflection( &wall[2], 0.0, 0.0, 1.0 );
    setReflection( &wall[3], 0.54, 0.54, 0.54 );
    setReflection( &wall[4], 0.84, 0.84, 0.84 );

    setReflection( &lightSource, 0.8, 0.8, 0.8 );
    setEmission( &lightSource, 1.27, 1.27, 1.27 );

    for( int i = 0 ; i < CLIP ; i++ ){
        clipSurface( &wall[0] );
        clipSurface( &wall[1] );
        clipSurface( &wall[2] );
        clipSurface( &wall[3] );
        clipSurface( &wall[4] );
        clipSurface( &lightSource );
    }

    scene = createScene();
    addScene( &scene, lightSource );
    //addScene( &scene, sphere );
    //addScene( &scene, square );
    addScene( &scene, wall[0] );
    addScene( &scene, wall[1] );
    addScene( &scene, wall[2] );
    addScene( &scene, wall[3] );
    addScene( &scene, wall[4] );

    // Hemi-Cube Generate
    printf("Start Hemi-Cube Generate\n");
    QueryPerformanceCounter(&t1);
    hemiCubeGenrator();
    QueryPerformanceCounter(&t2);
    printf("Complete Hemi-Cube Generate\t%lf s\n", (t2.QuadPart-t1.QuadPart)/(double)(ts.QuadPart) );
    //test();

    // FF matrix generate
    FF = mCreate( scene.n_face, scene.n_face, EMPTY );
    for( int i = 0 ; i < 3 ; i++ ){
        e[i] = vCreate( scene.n_face );
        p[i] = vCreate( scene.n_face );
    }

    // FF calculate
    int ip, iface;
    int jp, jface;

    printf("Start FF calculation\n");
    QueryPerformanceCounter(&t1);
    for( int i = 0 ; i < scene.n_face ; i++ ){

        searchScene( scene, i, &ip, &iface );

        e[0].vector[i] = scene.list[ip].emission[0];
        e[1].vector[i] = scene.list[ip].emission[1];
        e[2].vector[i] = scene.list[ip].emission[2];

        p[0].vector[i] = scene.list[ip].reflection[0];
        p[1].vector[i] = scene.list[ip].reflection[1];
        p[2].vector[i] = scene.list[ip].reflection[2];

        for( int j = 0 ; j < scene.n_face ; j++ ){
            if( i == j )
                FF.matrix[i][j] = 0.0;
            else{
                searchScene( scene, j, &jp, &jface );
                //FF.matrix[i][j] = meshToHC( scene.list[ip].flist[iface], scene.list[jp].flist[jface] );
                FF.matrix[i][j] = calMeshFF( scene.list[ip].flist[iface], scene.list[ip].flist[iface].normal,
                                             scene.list[jp].flist[jface], scene.list[jp].flist[jface].normal );
            }
        }
    }
    QueryPerformanceCounter(&t2);
    printf("Complete FF calculation\t\t%lf s\n", (t2.QuadPart-t1.QuadPart)/(double)(ts.QuadPart) );

    printf("Start matrix solution\n");
    QueryPerformanceCounter(&t1);
    for( int i = 0 ; i < 3 ; i++ )
        b[i] = matrix_solution( e[i], p[i], FF );
    QueryPerformanceCounter(&t2);
    printf("Complete matrix solution\t%lf s\n", (t2.QuadPart-t1.QuadPart)/(double)(ts.QuadPart) );

    glEnable( GL_DEPTH_TEST );

    //readMesh( &mesh, (char*) "Models/five-face.off", center, radius );
    //readMesh( &mesh2, (char*) "Models/mushroom.off", center, radius );

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

    axis();

    //drawMesh( mesh );
    //drawMesh( mesh2 );

    //drawHemiCube();
/*
    for( int i = 0 ; i < scene.n_patch ; i++ )
        drawPatch( scene.list[i], i < 1 ? 1 : 3 );
*/

    int pc, fc;

    for( int i = 1 * scene.list[0].n_face ; i < scene.n_face ; i++ ){

        glColor3f( fabs( b[0].vector[i] ), fabs( b[1].vector[i] ), fabs( b[2].vector[i] ) );
        searchScene( scene, i, &pc, &fc );
        glPolygonMode( GL_BACK, GL_LINE );
        glBegin( GL_POLYGON );
        for( int k = 0 ; k < scene.list[pc].flist[fc].n_point ; k++ ){
            glVertex3f( scene.list[pc].flist[fc].plist[k].x,
                        scene.list[pc].flist[fc].plist[k].y,
                        scene.list[pc].flist[fc].plist[k].z  );
        }
        glEnd();

    }

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

    ViewerExtent (viewer, 35);
	glutKeyboardFunc (keyboard);

	glutMainLoop();

    return 0;

}
