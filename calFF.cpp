///To calculate all pairs of mesh's Form Factor

#include <windows.h>
#include <gl/glut.h>
#include <vector>
#include <cstdio>
#include "geometric.h"

#define SIDE 64
#define PI 3.1415926535
enum { top = 0, front, right, back, left };

PATCH patch[5];

POINT_3D interpolation( POINT_3D a, POINT_3D b, POINT_3D c, POINT_3D d, double u, double v ){

    POINT_3D ipt;
/*  Triangle
    ipt.x = ( 1.0 - u - v ) * p0.x + u * p1.x + v * p2.x;
    ipt.y = ( 1.0 - u - v ) * p0.y + u * p1.y + v * p2.y;
    ipt.z = ( 1.0 - u - v ) * p0.z + u * p1.z + v * p2.z;
*/

    ipt.x = ( 1.0 - u ) * ( 1.0 - v ) * a.x +
            ( 1.0 - u ) * v * b.x +
            u * v * c.x +
            u * ( 1.0 - v ) * d.x;

    ipt.y = ( 1.0 - u ) * ( 1.0 - v ) * a.y +
            ( 1.0 - u ) * v * b.y +
            u * v * c.y +
            u * ( 1.0 - v ) * d.y;

    ipt.z = ( 1.0 - u ) * ( 1.0 - v ) * a.z +
            ( 1.0 - u ) * v * b.z +
            u * v * c.z +
            u * ( 1.0 - v ) * d.z;

    return ipt;

}

double calFF( POINT_3D a, VEC nor_a, POINT_3D b, VEC nor_b ){

    VEC aTob = vectorPP( a, b );
    VEC bToa = vectorPP( b, a );
    double r = lengthPP( a, b );

    return ( vCos( nor_a, aTob ) * vCos( nor_b, bToa ) ) / ( PI * r * r );

}

double calCellFF( SURFACE_3D face, VEC normal ){

    //int i, j;
    double dA, du, dv;
    double area;
    double FF = 0.0;

    // Center Point & Normal Vector
    POINT_3D center = addPoint3D( 0.0, 0.0, 0.0 );;
    VEC nc = vCreate( 3 );
    nc.vector[1] = 1.0;

    // Calculate Area
    area = surfaceArea( face );
    dA = area * ( 0.1 * 0.1 );

    for( du = 0.0 ; du <= 1.0 ; du += 0.1 )
        for( dv = 0.0 ; dv <= 1.0 ; dv += 0.1 )
            FF += dA * calFF( center, nc,
                              interpolation( face.plist[0], face.plist[1], face.plist[2], face.plist[3], du, dv  ), normal );

    return FF;

}

double calHemiCubeFF( POINT_3D pt, int n, double dA ){

    double r = lengthPP(addPoint3D(0,0,0), pt);

    return ( n == top ? 1 : pt.y ) * dA / PI * r * r;

}

//n_cell is 64 cells or 1024 cells per side
void hemiCubeGenrater(){

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
            //face.FF = calHemiCubeFF( surfaceCenter( face ), top, dA );
            face.FF = calHemiCubeFF( a, top, dA );

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
            face.FF = calHemiCubeFF( surfaceCenter( face ), back, dA );

            addPatch( &patch[back], face );

            //Front
            a = addPoint3D( (double)         i * dx, (double)         j * dy, -1.0 );
            b = addPoint3D( (double)         i * dx, (double) ( j + 1 ) * dy, -1.0 );
            c = addPoint3D( (double) ( i + 1 ) * dx, (double) ( j + 1 ) * dy, -1.0 );
            d = addPoint3D( (double) ( i + 1 ) * dx, (double)         j * dy, -1.0 );

            face = addSurface3D( 4, a, b, c, d );
            face.FF = calHemiCubeFF( surfaceCenter( face ), front, dA );

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
            //face.FF = calHemiCubeFF( surfaceCenter( face ), right, dA );
            face.FF = calHemiCubeFF( a, right, dA );

            addPatch( &patch[right], face );

            //Left
            a = addPoint3D( -1.0, (double)         i * dy, (double)         j * dz );
            b = addPoint3D( -1.0, (double)         i * dy, (double) ( j + 1 ) * dz );
            c = addPoint3D( -1.0, (double) ( i + 1 ) * dy, (double) ( j + 1 ) * dz );
            d = addPoint3D( -1.0, (double) ( i + 1 ) * dy, (double)         j * dz );

            face = addSurface3D( 4, a, b, c, d );
            //face.FF = calHemiCubeFF( surfaceCenter( face ), left, dA );
            face.FF = calHemiCubeFF( a, left, dA );

            addPatch( &patch[left], face );

        }
    }

}

void drawHemiCube(){

    int i, j, k;

    for( i = 0 ; i < 1 ; i++ ){
        //printf("%d\n", patch[i].n_face );
        for( j = 0 ; j < patch[i].n_face ; j++ ){
            printf("%d-%d FF : %lf\n", i, j, patch[i].flist[j].FF );
            glColor3f( patch[i].flist[j].FF * 500, patch[i].flist[j].FF * 500, patch[i].flist[j].FF * 500 );
            glBegin( GL_POLYGON );
            for( k = 0 ; k < patch[i].flist[j].n_point ; k++ ){
                glVertex3f( patch[i].flist[j].plist[k].x,
                            patch[i].flist[j].plist[k].y,
                            patch[i].flist[j].plist[k].z  );
            }
            glEnd();
        }
    }

}
