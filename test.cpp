#include <iostream>
#include <cstdio>
#include <vector>
#include <ctime>

#include <windows.h>
#include <gl/gluit.h>

#include "matrix.hpp"
#include "vector.hpp"

#include "geometric.h"

#define CLIP 2
#define FF_TYPE 0
#define OCCULSION_CHK 1

extern void hemiCubeGenrator();
extern void drawHemiCube();
extern void test();

extern float occlusion( SURFACE_3D i, SURFACE_3D j, int isn, int jsn );

extern double meshToHC( SURFACE_3D i, SURFACE_3D j );
extern double calMeshFF( SURFACE_3D i, VEC inormal, SURFACE_3D j, VEC jnormal );

extern VEC matrix_solution( VEC emission, VEC reflection, MAT FF );

int gw, gh;

//Main Func
int mainwin;
int viewer;

int colorMode;
int gouraudShader;
int grid;

//Model
MODEL sphere;
MODEL square[2];
MODEL wall[6];
MODEL lightSource;

SCENE scene;


std::vector<POINT_3D> **pcollect;

// Matrix
MAT FF;
MAT visible;
VEC e[3];
VEC p[3];
VEC b[3];

void progressBar( int total, int now, int barLength ){

    double percentComplete = (double) ( now + 1 ) / total;

    printf("\r[");
    for( int i = 0 ; i < barLength ; i++ )
        printf("%c", floor( percentComplete * barLength - 0.5 ) >= i   ? '=' : ' ' );
    printf("] %3.0lf%%",  (double) ( now + 1 ) / total * 100 );
    printf("\t%5d/%-5d", now + 1, total );

}

void sphereCreate( double x, double y, double z ){

    PATCH patch;
    SURFACE_3D face;
    POINT_3D a, b, c, d, n;

    POINT_3D **sp;

    int deg, fi;
    int r = 4;

    sphere = createModel();

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

            patch = createPatch();
            face = ( deg != 0 || deg != 17 ) ? addSurface3D( 4, a, b, c, d ) : addSurface3D( 3, a, b, c );
            n = surfaceCenter( face );
            setSurface3DNormal( &face, n.x, n.y, n.z );
            addPatch( &patch, face );
            addModel( &sphere, patch );

        }
    }

}

void squareCreate( MODEL *target, double size[3], double pos[3] ){

    PATCH patch;
    SURFACE_3D face;
    POINT_3D pt[8];
    double x, y, z;

    *target = createModel();

    // Length, Width, Height
    x = size[0] / 2;
    y = size[1] / 2;
    z = size[2] / 2;

    pt[0] = addPoint3D( -x + pos[0], -y + pos[1], -z + pos[2] );
    pt[1] = addPoint3D(  x + pos[0], -y + pos[1], -z + pos[2] );
    pt[2] = addPoint3D(  x + pos[0],  y + pos[1], -z + pos[2] );
    pt[3] = addPoint3D( -x + pos[0],  y + pos[1], -z + pos[2] );
    pt[4] = addPoint3D( -x + pos[0], -y + pos[1],  z + pos[2] );
    pt[5] = addPoint3D(  x + pos[0], -y + pos[1],  z + pos[2] );
    pt[6] = addPoint3D(  x + pos[0],  y + pos[1],  z + pos[2] );
    pt[7] = addPoint3D( -x + pos[0],  y + pos[1],  z + pos[2] );

    // 3 2 1 0
    patch = createPatch();
    face = addSurface3D( 4, pt[3], pt[2], pt[1], pt[0] );
    setSurface3DNormal( &face, 0, 0, -1 );
    addPatch( &patch, face );
    addModel( target, patch );

    // 2 6 5 1
    patch = createPatch();
    face = addSurface3D( 4, pt[2], pt[6], pt[5], pt[1] );
    setSurface3DNormal( &face, 1, 0, 0 );
    addPatch( &patch, face );
    addModel( target, patch );

    // 6 7 4 5
    patch = createPatch();
    face = addSurface3D( 4, pt[6], pt[7], pt[4], pt[5] );
    setSurface3DNormal( &face, 0, 0, 1 );
    addPatch( &patch, face );
    addModel( target, patch );

    // 7 3 0 4
    patch = createPatch();
    face = addSurface3D( 4, pt[7], pt[3], pt[0], pt[4] );
    setSurface3DNormal( &face, -1, 0, 0 );
    addPatch( &patch, face );
    addModel( target, patch );

    //Buttom 0 1 5 4
    patch = createPatch();
    face = addSurface3D( 4, pt[0], pt[1], pt[5], pt[4] );
    setSurface3DNormal( &face, 0, -1, 0 );
    addPatch( &patch, face );
    addModel( target, patch );

    //Top 7 6 2 3
    patch = createPatch();
    face = addSurface3D( 4, pt[7], pt[6], pt[2], pt[3] );
    setSurface3DNormal( &face, 0, 1, 0 );
    addPatch( &patch, face );
    addModel( target, patch );

}

void lightCreate(){

    PATCH patch;
    SURFACE_3D face;
    POINT_3D pt[12];

    lightSource = createModel();


    pt[0] = addPoint3D( 5, 9, -5 );
    pt[1] = addPoint3D( -5, 9, -5 );
    pt[2] = addPoint3D( 5, 9, 5 );
    pt[3] = addPoint3D( -5, 9, 5 );

    // Light Source
    patch = createPatch();
    face = addSurface3D( 4, pt[2], pt[3], pt[1], pt[0] );
    setSurface3DNormal( &face, 0, -1, 0 );
    addPatch( &patch, face );
    addModel( &lightSource, patch );

}

void wallCreate(){

    PATCH patch;
    SURFACE_3D face;
    POINT_3D pt[12];

    wall[0] = createModel();
    wall[1] = createModel();
    wall[2] = createModel();
    wall[3] = createModel();
    wall[4] = createModel();
    //wall[5] = createModel();

    pt[0] = addPoint3D( -10, -10, -10 );
    pt[1] = addPoint3D( 10, -10, -10 );
    pt[2] = addPoint3D( 10, 10, -10 );
    pt[3] = addPoint3D( -10, 10, -10 );
    pt[4] = addPoint3D( -10, -10, 10 );
    pt[5] = addPoint3D( 10, -10, 10 );
    pt[6] = addPoint3D( 10, 10, 10 );
    pt[7] = addPoint3D( -10, 10, 10 );

    pt[8] = addPoint3D( 5, -5, 10 );
    pt[9] = addPoint3D( -5, -5, 10 );
    pt[10] = addPoint3D( -5, 5, 10 );
    pt[11] = addPoint3D( 5, 5, 10 );

    // 0 1 2 3
    patch = createPatch();
    face = addSurface3D( 4, pt[0], pt[1], pt[2], pt[3] );
    setSurface3DNormal( &face, 0, 0, 1 );
    addPatch( &patch, face );
    addModel( &wall[0], patch );

    // 1 5 6 2
    patch = createPatch();
    face = addSurface3D( 4, pt[1], pt[5], pt[6], pt[2] );
    setSurface3DNormal( &face, -1, 0, 0 );
    addPatch( &patch, face );
    addModel( &wall[1], patch );

    // 4 0 3 7
    patch = createPatch();
    face = addSurface3D( 4, pt[4], pt[0], pt[3], pt[7] );
    setSurface3DNormal( &face, 1, 0, 0 );
    addPatch( &patch, face );
    addModel( &wall[2], patch );

    // Buttom 4 5 1 0
    patch = createPatch();
    face = addSurface3D( 4, pt[4], pt[5], pt[1], pt[0] );
    setSurface3DNormal( &face, 0, 1, 0 );
    addPatch( &patch, face );
    addModel( &wall[3], patch );

    // Top 3 2 6 7
    patch = createPatch();
    face = addSurface3D( 4, pt[3], pt[2], pt[6], pt[7] );
    setSurface3DNormal( &face, 0, -1, 0 );
    addPatch( &patch, face );
    addModel( &wall[4], patch );

    // Light Source
    patch = createPatch();
    face = addSurface3D( 4, pt[5], pt[4], pt[7], pt[6] );
    //face = addSurface3D( 4, pt[8], pt[9], pt[10], pt[11] );
    setSurface3DNormal( &face, 0, 0, -1 );
    addPatch( &patch, face );
    addModel( &wall[5], patch );

}

/*
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
        case 'M':
        case 'm':
            colorMode ^= 1;
            break;
        case 'G':
        case 'g':
            grid ^= 1;
            break;
        case 'S':
        case 's':
            gouraudShader ^= 1;
            break;
        default:
            break;
    }

    glClear( GL_COLOR_BUFFER_BIT );
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

    // Initial Var
    colorMode = 0;  // Diffuse
    gouraudShader = 0;  //No interpolation
    grid = 0;

    int im, ip, iface;
    int jm, jp, jface;

    LARGE_INTEGER t1, t2, ts;
    QueryPerformanceFrequency(&ts);

    //float center[3], radius = 0.0;

    //Model Create
    sphereCreate( 4, -6, 0 );
    setReflection( &sphere, 0.54, 0.54, 0.54 );

    double squ0_size[3] = { 7, 7, 7 };
    double squ0_pos[3] = { -5, ( squ0_size[1] / 2 ) - 10, 5 };
    squareCreate( &square[0], squ0_size, squ0_pos );
    setReflection( &square[0], 0.54, 0.54, 0.54 );

    double squ1_size[3] = { 7, 14, 7 };
    double squ1_pos[3] = { 5, ( squ1_size[1] / 2 ) - 10, 0 };
    squareCreate( &square[1], squ1_size, squ1_pos );
    setReflection( &square[1], 0.54, 0.54, 0.54 );

    wallCreate();
    setReflection( &wall[0], 0.84, 0.84, 0.84 );
    setReflection( &wall[1], 1.0, 0.0, 0.0 );
    setReflection( &wall[2], 0.0, 0.0, 1.0 );
    setReflection( &wall[3], 0.84, 0.84, 0.84 );
    setReflection( &wall[4], 0.54, 0.54, 0.54 );
    setReflection( &wall[5], 0.84, 0.84, 0.84 );

    lightCreate();
    setReflection( &lightSource, 0.8, 0.8, 0.8 );
    setEmission( &lightSource, 1.27, 1.27, 1.27 );

    for( int i = 0 ; i < CLIP ; i++ ){
        clipPatch( &wall[0] );
        clipPatch( &wall[1] );
        clipPatch( &wall[2] );
        clipPatch( &wall[3] );
        clipPatch( &wall[4] );
        clipPatch( &wall[5] );
    }
    for( int i = 0 ; i < CLIP-1 ; i++ ){
        clipPatch( &square[0] );
        clipPatch( &square[1] );
        clipPatch( &lightSource );
    }

    scene = createScene();
    // Light
    addScene( &scene, lightSource );

    // Model
    //addScene( &scene, sphere );
    addScene( &scene, square[0] );
    addScene( &scene, square[1] );

    addScene( &scene, wall[0] );
    addScene( &scene, wall[1] );
    addScene( &scene, wall[2] );
    addScene( &scene, wall[3] );
    addScene( &scene, wall[4] );
    addScene( &scene, wall[5] );
/*
    // refine clip
    for( int i = 0 ; i < scene.n_face ; i++ ){
        for( int j = i + 1 ; j < scene.n_face ; j++ ){
            searchSceneSurface( scene, i, &im, &ip, &iface );
            searchSceneSurface( scene, j, &jm, &jp, &jface );
            refineClip( &scene.list[im].plist[ip].flist[iface], &scene.list[jm].plist[jp].flist[jface], 1, 0.3 );
        }
    }

    extern std::vector<SURFACE_3D> fcolect;
    SURFACE_3D ftemp;
    for( int i = 0 ; i < scene.n_patch ; i++ ){
        searchScenePatch( scene, i, &im, &ip );

        // Clear vector
        fcolect.clear();

        ftemp = scene.list[im].plist[ip].flist[0];
        scene.list[im].plist[ip].flist.pop_back();

        travelNode( &ftemp );

        //delNode( ftemp.ne );
        //delNode( ftemp.nw );
        //delNode( ftemp.se );
        //delNode( ftemp.sw );

        // Store element to patch
        scene.list[im].plist[ip].n_face = fcolect.size();
        for( int j = 0 ; j < (int) fcolect.size() ; j++ )
            scene.list[im].plist[ip].flist.push_back( fcolect[j] );

    }

    // Update information
    scene.n_face = 0;
    for( int i = 0 ; i < scene.n_model ; i++ ){
        scene.list[i].n_face = 0;
        for( int j = 0 ; j < scene.list[i].n_patch ; j++ ){
            scene.list[i].n_face += scene.list[i].plist[j].n_face;
        }
        scene.n_face += scene.list[i].n_face;
    }
*/
    printf( "M:%d P:%d S:%d\n", scene.n_model, scene.n_patch, scene.n_face );

    // Hemi-Cube Generate
    if( FF_TYPE ){
        printf("Start Hemi-Cube Generate\n");
        QueryPerformanceCounter(&t1);
        hemiCubeGenrator();
        QueryPerformanceCounter(&t2);
        printf("Complete Hemi-Cube Generate\t%lf s\n", (t2.QuadPart-t1.QuadPart)/(double)(ts.QuadPart) );
        //test();
    }

    // Visible matrix generate & check
    visible = mCreate( scene.n_face, scene.n_face, EMPTY );

    printf("Start occlusion checking\n");
    QueryPerformanceCounter(&t1);
    for( int i = 0 ; i < scene.n_face ; i++ ){
        progressBar( scene.n_face, i, 25 );
        for( int j = i ; j < scene.n_face ; j++ ){
            if( i == j )
                visible.matrix[i][j] = 0.0;
            else{
                searchSceneSurface( scene, i, &im, &ip, &iface );
                searchSceneSurface( scene, j, &jm, &jp, &jface );
                visible.matrix[i][j] = OCCULSION_CHK ? occlusion( scene.list[im].plist[ip].flist[iface], scene.list[jm].plist[jp].flist[jface], i, j ) : 1.0;
                visible.matrix[j][i] = visible.matrix[i][j];
            }
        }
    }
    QueryPerformanceCounter(&t2);
    printf("\nComplete occlusion checking\t%lf s\n", (t2.QuadPart-t1.QuadPart)/(double)(ts.QuadPart) );

    //mPrint( visible );

    // Emissio and Reflection setting
    for( int i = 0 ; i < 3 ; i++ ){
        e[i] = vCreate( scene.n_face );
        p[i] = vCreate( scene.n_face );
    }

    for( int i = 0 ; i < scene.n_face ; i++ ){

        searchSceneSurface( scene, i, &im, &ip, &iface );

        e[0].vector[i] = scene.list[im].emission[0];
        e[1].vector[i] = scene.list[im].emission[1];
        e[2].vector[i] = scene.list[im].emission[2];

        p[0].vector[i] = scene.list[im].reflection[0];
        p[1].vector[i] = scene.list[im].reflection[1];
        p[2].vector[i] = scene.list[im].reflection[2];

    }

    // FF matrix generate
    FF = mCreate( scene.n_face, scene.n_face, EMPTY );

    // FF calculate
    printf("Start FF calculation\n");
    QueryPerformanceCounter(&t1);
    for( int i = 0 ; i < scene.n_face ; i++ ){
        progressBar( scene.n_face, i, 25 );
        searchSceneSurface( scene, i, &im, &ip, &iface );
        for( int j = i ; j < scene.n_face ; j++ ){
            if( i == j )
                FF.matrix[i][j] = 0.0;
            else{
                searchSceneSurface( scene, j, &jm, &jp, &jface );
                FF.matrix[i][j] = visible.matrix[i][j] *
                                  ( FF_TYPE ? meshToHC( scene.list[im].plist[ip].flist[iface], scene.list[jm].plist[jp].flist[jface] ) :
                                              calMeshFF( scene.list[im].plist[ip].flist[iface], scene.list[im].plist[ip].flist[iface].normal,
                                                         scene.list[jm].plist[jp].flist[jface], scene.list[jm].plist[jp].flist[jface].normal ) );
                FF.matrix[j][i] = ( surfaceArea( scene.list[im].plist[ip].flist[iface] ) / surfaceArea( scene.list[jm].plist[jp].flist[jface] ) ) * FF.matrix[i][j];
            }
        }
    }
    QueryPerformanceCounter(&t2);
    printf("\nComplete FF calculation\t\t%lf s\n", (t2.QuadPart-t1.QuadPart)/(double)(ts.QuadPart) );

    printf("Start matrix solution\n");
    QueryPerformanceCounter(&t1);
    for( int i = 0 ; i < 3 ; i++ )
        b[i] = matrix_solution( e[i], p[i], FF );
    QueryPerformanceCounter(&t2);
    printf("Complete matrix solution\t%lf s\n", (t2.QuadPart-t1.QuadPart)/(double)(ts.QuadPart) );

    // Gouraud Shader prepared
    pcollect = new std::vector<POINT_3D>*[scene.n_model];
    for( int i = 0 ; i < scene.n_model ; i++ )
        pcollect[i] = new std::vector<POINT_3D>[scene.list[i].n_patch];

    // Collect Point
    int flag = 0;

    for( int i = 0 ; i < scene.n_face ; i++ ){
        searchSceneSurface( scene, i, &im, &ip, &iface );
        for( int j = 0 ; j < scene.list[im].plist[ip].flist[iface].n_point ; j++ ){
            for( int k = 0 ; k < (int) pcollect[im][ip].size() ; k++ ){
                if( PTEQU( pcollect[im][ip][k], scene.list[im].plist[ip].flist[iface].plist[j] ) )
                    flag = 1;
            }
            if( !flag )
                pcollect[im][ip].push_back( scene.list[im].plist[ip].flist[iface].plist[j] );
            flag = 0;
        }
    }

    // Update point rad

    for( int i = 0 ; i < scene.n_face ; i++ ){
        searchSceneSurface( scene, i, &im, &ip, &iface );
        for( int j = 0 ; j < scene.list[im].plist[ip].flist[iface].n_point ; j++ ){
            for( int k = 0 ; k < (int) pcollect[im][ip].size() ; k++ ){
                if( PTEQU( pcollect[im][ip][k], scene.list[im].plist[ip].flist[iface].plist[j] ) ){
                    pcollect[im][ip][k].rad[0] += b[0].vector[i];
                    pcollect[im][ip][k].rad[1] += b[1].vector[i];
                    pcollect[im][ip][k].rad[2] += b[2].vector[i];
                    pcollect[im][ip][k].count++;
                    break;
                }
            }
        }
    }

    // average point rad
    for( int i = 0 ; i < scene.n_patch ; i++ ){
        searchScenePatch( scene, i, &im, &ip );
        for( int j = 0 ; j < (int) pcollect[im][ip].size() ; j++ ){
            pcollect[im][ip][j].rad[0] /= pcollect[im][ip][j].count;
            pcollect[im][ip][j].rad[1] /= pcollect[im][ip][j].count;
            pcollect[im][ip][j].rad[2] /= pcollect[im][ip][j].count;
        }
    }

    /*for( int i = 0 ; i < scene.n_model ; i++ ){
        printf( "Models %d - %d points\n", i, pcollect[i].size() );
        for( int j = 0 ; j < (int) pcollect[i].size() ; j++ )
            printf( "\t%lf %lf %lf [ %lf %lf %lf ] %d\n", pcollect[i][j].x, pcollect[i][j].y, pcollect[i][j].z,
                                                  pcollect[i][j].rad[0], pcollect[i][j].rad[1], pcollect[i][j].rad[2],
                                                  pcollect[i][j].count );
    }*/

    glEnable( GL_DEPTH_TEST );

    //Initial color
    glClearColor( 0.0, 0.0, 0.0, 1.0 );

}

void reshape( int w, int h ){

	glViewport( 0, 0, w, h );
	ViewerReshape( viewer );

}

void display(){}

void content( void ){

    int fm, fp, f;
    GLfloat color[3];

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    axis();

    //drawHemiCube();
    glPushAttrib( GL_ALL_ATTRIB_BITS );
    for( int i = 0 ; i < scene.n_face ; i++ ){

        searchSceneSurface( scene, i, &fm, &fp, &f );

        if( fm == 8 )
            continue;

        color[0] = colorMode ? scene.list[fm].reflection[0] : b[0].vector[i] * 3;
        color[1] = colorMode ? scene.list[fm].reflection[1] : b[1].vector[i] * 3;
        color[2] = colorMode ? scene.list[fm].reflection[2] : b[2].vector[i] * 3;

        if( !gouraudShader )
            glColor3f( clap( color[0], 0.0, 1.0 ), clap( color[1], 0.0, 1.0 ), clap( color[2], 0.0, 1.0 ) );

        grid ? glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ) : glPolygonMode( GL_BACK, GL_LINE );
        glBegin( GL_POLYGON );
        for( int k = 0 ; k < scene.list[fm].plist[fp].flist[f].n_point ; k++ ){
            for( int n = 0 ; n < (int) pcollect[fm][fp].size() && gouraudShader ; n++ ){
                if( PTEQU( scene.list[fm].plist[fp].flist[f].plist[k], pcollect[fm][fp][n] ) ){
                    glColor3f( clap( pcollect[fm][fp][n].rad[0] * 3, 0.0, 1.0 ), clap( pcollect[fm][fp][n].rad[1] * 3, 0.0, 1.0 ), clap( pcollect[fm][fp][n].rad[2] * 3, 0.0, 1.0 ) );
                    break;
                }
            }
            glVertex3f( scene.list[fm].plist[fp].flist[f].plist[k].x,
                        scene.list[fm].plist[fp].flist[f].plist[k].y,
                        scene.list[fm].plist[fp].flist[f].plist[k].z  );
        }
        glEnd();

    }
    glPopAttrib();

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
