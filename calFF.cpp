///To calculate all pairs of mesh's Form Factor

#include <windows.h>
#include <gl/glut.h>
#include <vector>
#include <cstdio>
#include <cmath>

#include "geometric.h"
#include "vector.hpp"
#include "matrix.hpp"

#define SIDE 32
#ifndef PI
    #define PI 3.1415926535
#endif

enum plane{ top = 0, front, right, back, left };
PATCH patch[5];

double calFF( POINT_3D a, VEC nor_a, POINT_3D b, VEC nor_b ){

    VEC aTob = vectorPP( a, b );
    VEC bToa = vectorPP( b, a );
    double r = lengthPP( a, b );
    double FF;
    double cosA, cosB;

    cosA = vCos( nor_a, aTob );
    cosB = vCos( nor_b, bToa );

    FF = ( r - 0.0 < 0.000001 ) ?  0.0 : ( clap( cosA, 0.0, 1.0 ) * clap( cosB, 0.0, 1.0 ) ) / ( PI * r * r );

    free( aTob.vector );
    free( bToa.vector );

    return FF;

}

#define STEP 0.1
#define HSTEP 0.05
double calMeshFF( SURFACE_3D i, VEC inormal, SURFACE_3D j, VEC jnormal ){

    //int i, j;
    double diA, diu, div;
    double djA, dju, djv;

    double iarea, jarea;

    double FFij = 0.0;
    double FF;

    POINT_3D iipt, jipt;

    // Calculate Area
    iarea = surfaceArea( i );
    diA = iarea * ( STEP * STEP );

    jarea = surfaceArea( j );
    djA = jarea * ( STEP * STEP );

    for( diu = HSTEP ; diu <= 1.0 ; diu += STEP ){
        for( div = HSTEP ; div <= 1.0 ; div += STEP ){

            interpolation( &iipt, i, diu, div );

            FF = 0.0;

            for( dju = HSTEP ; dju <= 1.0 ; dju += STEP ){
                for( djv = HSTEP ; djv <= 1.0 ; djv += STEP ){

                    interpolation( &jipt, j, dju, djv);
                    FF += calFF( iipt, inormal, jipt, jnormal ) * djA;

                }
            }

            FFij += FF * diA;

        }

    }

    return FFij / iarea;

}

double calHemiCubeFF( POINT_3D pt, int n ){

    double r = pt.x * pt.x + ( n == top ? pt.z * pt.z : pt.y * pt.y ) + 1;

    return ( n == top ? 1 : pt.y ) / ( PI * r * r );

}

//n_cell is 64 cells or 1024 cells per side
void hemiCubeGenrator(){

    int i, j;
    double dx, dy, dz;
    double dA;

    POINT_3D a, b, c, d;
    SURFACE_3D face;

    patch[0] = createPatch();
    patch[1] = createPatch();
    patch[2] = createPatch();
    patch[3] = createPatch();
    patch[4] = createPatch();

    dx = dy = dz = (double) 2.0 / SIDE;
    dA = (double) 4.0 / ( SIDE * SIDE );

    //Top
    for( i = -SIDE / 2 ; i < SIDE / 2 ; i++ ){
        for( j = -SIDE / 2 ; j < SIDE / 2 ; j++ ){

            a = addPoint3D(  (double)         i * dx, 1.0, (double)         j * dz );
            b = addPoint3D(  (double)         i * dx, 1.0, (double) ( j + 1 ) * dz );
            c = addPoint3D(  (double) ( i + 1 ) * dx, 1.0, (double) ( j + 1 ) * dz );
            d = addPoint3D(  (double) ( i + 1 ) * dx, 1.0, (double)         j * dz );

            face = addSurface3D( 4, a, b, c, d );
            face.FF = calHemiCubeFF( surfaceCenter( face ), top ) * dA;

            addPatch( &patch[top], face );

        }
    }

    //Front and Back
    for( i = -SIDE / 2 ; i < SIDE / 2 ; i++ ){
        for( j = 0 ; j < SIDE / 2 ; j++ ){

            //Back
            a = addPoint3D( (double) ( i + 1 ) * dx, (double)         j * dy, 1.0 );
            b = addPoint3D( (double) ( i + 1 ) * dx, (double) ( j + 1 ) * dy, 1.0 );
            c = addPoint3D( (double)         i * dx, (double) ( j + 1 ) * dy, 1.0 );
            d = addPoint3D( (double)         i * dx, (double)         j * dy, 1.0 );

            face = addSurface3D( 4, a, b, c, d );
            face.FF = calHemiCubeFF( surfaceCenter( face ), top ) * dA;

            addPatch( &patch[back], face );

            //Front
            a = addPoint3D( (double)         i * dx, (double)         j * dy, -1.0 );
            b = addPoint3D( (double)         i * dx, (double) ( j + 1 ) * dy, -1.0 );
            c = addPoint3D( (double) ( i + 1 ) * dx, (double) ( j + 1 ) * dy, -1.0 );
            d = addPoint3D( (double) ( i + 1 ) * dx, (double)         j * dy, -1.0 );

            face = addSurface3D( 4, a, b, c, d );
            face.FF = calHemiCubeFF( surfaceCenter( face ), top ) * dA;

            addPatch( &patch[front], face );

        }
    }

    // Right and Left
    for( i = 0 ; i < SIDE / 2 ; i++ ){
        for( j = -SIDE / 2 ; j < SIDE / 2 ; j++ ){
            //Right
            a = addPoint3D( 1.0, (double) ( i + 1 ) * dy, (double)         j * dz );
            b = addPoint3D( 1.0, (double) ( i + 1 ) * dy, (double) ( j + 1 ) * dz );
            c = addPoint3D( 1.0, (double)         i * dy, (double) ( j + 1 ) * dz );
            d = addPoint3D( 1.0, (double)         i * dy, (double)         j * dz );

            face = addSurface3D( 4, a, b, c, d );
            face.FF = calHemiCubeFF( surfaceCenter( face ), top ) * dA;

            addPatch( &patch[right], face );

            //Left
            a = addPoint3D( -1.0, (double)         i * dy, (double)         j * dz );
            b = addPoint3D( -1.0, (double)         i * dy, (double) ( j + 1 ) * dz );
            c = addPoint3D( -1.0, (double) ( i + 1 ) * dy, (double) ( j + 1 ) * dz );
            d = addPoint3D( -1.0, (double) ( i + 1 ) * dy, (double)         j * dz );

            face = addSurface3D( 4, a, b, c, d );
            face.FF = calHemiCubeFF( surfaceCenter( face ), top ) * dA;

            addPatch( &patch[left], face );

        }
    }

}

void checkInOut( VEC v1, VEC v2 ){

    VEC fnormal = vCross( v2, v1 );
    POINT_3D center;
    double d;

    for( int i = 0 ; i < 5 ; i++ ){
        for( int j = 0 ; j < patch[i].n_face ; j++ ){

            if( patch[i].flist[j].visited ){
                center = surfaceCenter( patch[i].flist[j] );
                d = ( center.x * fnormal.vector[0] ) + ( center.y * fnormal.vector[1] ) + ( center.z * fnormal.vector[2] );
                if( d < 0.0 )
                    patch[i].flist[j].visited = 0;
            }

        }
    }

    vDestroy( fnormal );

}

double clipPlane( int n, VEC ray[] ){

    double FF;

    for( int i = 0 ; i < n ; i++ )
        checkInOut( ray[i], ray[(i+1)%n] );

    for( int i = 0 ; i < 5 ; i++ )
        for( int j = 0 ; j < patch[i].n_face ; j++ )
            if( patch[i].flist[j].visited == 1 )
                FF += patch[i].flist[j].FF;

    return FF;

}

void normalRot( VEC fnormal, int n, VEC ray[] ){

    VEC normal = vCreate( 3 );
    VEC fn = vClone( fnormal );
    MAT m = mCreate( 4, 4, IDENTITY );

    normal.vector[1] = 1.0;
    m = rotate3D( m, axisRot( fn, normal, 0 ), 1, 0, 0 );
    fn = transform( fn, m );
    m = rotate3D( m, axisRot( fn, normal, 2 ), 0, 0, 1 );

    for( int i = 0 ; i < n ; i++ )
        ray[i] = transform( ray[i], m );

    vDestroy( fn );
    vDestroy( normal );
    mDestroy( m );

}

void initialHemiCube(){

    for( int i = 0 ; i < 5 ; i++ )
        for( int j = 0 ; j < patch[i].n_face ; j++ )
            patch[i].flist[j].visited = 1;

}

double clipHemiCube( VEC fnormal, int n, VEC ray[] ){

    double FF = 0.0;

    initialHemiCube();
    normalRot( fnormal, n, ray );
    FF = clipPlane( n, ray );

    return FF;

}

double meshToHC( SURFACE_3D i, SURFACE_3D j ){

    VEC *ray;
    POINT_3D ipt;
    double du, dv;
    double r;
    int n;

    double FF = 0.0;
    double area = surfaceArea( i );
    double dA = area * ( STEP * STEP );

    ray = (VEC*) malloc( sizeof( VEC ) * j.n_point );
    for( n = 0 ; n < j.n_point ; n++ )
        ray[n] = vCreate( 3 );

    for( du = HSTEP ; du <= 1.0 ; du += STEP ){
        for( dv = HSTEP ; dv <= 1.0 ; dv += STEP ){

            interpolation( &ipt, i, du, dv );
            for( n = 0 ; n < j.n_point ; n++ ){
                r = lengthPP( j.plist[n], ipt );
                ray[n].vector[0] = ( j.plist[n].x - ipt.x ) / r;
                ray[n].vector[1] = ( j.plist[n].y - ipt.y ) / r;
                ray[n].vector[2] = ( j.plist[n].z - ipt.z ) / r;
            }
            FF += clipHemiCube( i.normal, j.n_point, ray ) * dA;


        }

    }

    for( n = 0 ; n < j.n_point ; n++ )
        vDestroy( ray[n] );
    free( ray );

    return FF / area;

}

double pmeshToHC( SURFACE_3D i ){

    VEC *ray;
    POINT_3D ipt = addPoint3D( 0, 0, 0 );
    int n;

    double FF = 0.0;

    ray = (VEC*) malloc( sizeof( VEC ) * i.n_point );

    for( n = 0 ; n < i.n_point ; n++ ){
        ray[n] = vNormalize( vectorPP( ipt, i.plist[n] ) );
        glColor3f( 0.0, 0.0, 1.0 );
        glBegin( GL_LINES );
            glVertex3f( ipt.x, ipt.y, ipt.z );
            glVertex3f( ipt.x + ( 2 * ray[n].vector[0] ), ipt.y + ( 2 * ray[n].vector[1] ), ipt.z + ( 2 * ray[n].vector[2] ) );
        glEnd();
    }
    FF = clipHemiCube( i.normal, i.n_point, ray );

    for( n = 0 ; n < i.n_point ; n++ )
        vDestroy( ray[n] );
    free( ray );

    return FF;

}

void drawHemiCube(){

    int i, j, k;
    //double FF;
    int count = 0;

    for( i = 0 ; i < 5 ; i++ ){
        //FF = 0.0;
        for( j = 0 ; j < patch[i].n_face ; j++ ){
            //FF += patch[i].flist[j].FF;
            if( patch[i].flist[j].visited == 0 ){
            count++;
            glColor3f( patch[i].flist[j].FF * 1000, 0.0, 0.0 );
            glBegin( GL_POLYGON );
            for( k = 0 ; k < patch[i].flist[j].n_point ; k++ ){
                glVertex3f( patch[i].flist[j].plist[k].x,
                            patch[i].flist[j].plist[k].y,
                            patch[i].flist[j].plist[k].z  );
            }
            glEnd();
            }
        }
        //printf( "%d-%lf\n", i, FF );
        //printf( "%d\n", count );
    }

}

void test(){

    SURFACE_3D i, j;
    double FF = 0.0;

    i = addSurface3D( 4, addPoint3D( 1, 0, 1 ), addPoint3D( -1, 0, 1 ), addPoint3D( -1, 0, -1 ), addPoint3D( 1, 0, -1 ) );
    j = addSurface3D( 4, addPoint3D( 1, 0, 1 ), addPoint3D( -1, 0, 1 ), addPoint3D( -1, 0, -1 ), addPoint3D( 1, 0, -1 ) );

    FF = meshToHC( i, j );
    printf("%lf\n", FF );

    free( i.plist );

}

