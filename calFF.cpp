///To calculate all pairs of mesh's Form Factor

#include <windows.h>
#include <gl/glut.h>
#include <vector>
#include <cstdio>
#include "geometric.h"

#define SIDE 2
#define PI 3.1415926535

enum plane{ top = 0, front, right, back, left };
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

    return ( ( n == top ? 1 : pt.y ) * dA ) / ( PI * r * r );

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
            face.FF = calHemiCubeFF( surfaceCenter( face ), top, dA );

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
            face.FF = calHemiCubeFF( surfaceCenter( face ), right, dA );

            addPatch( &patch[right], face );

            //Left
            a = addPoint3D( -1.0, (double)         i * dy, (double)         j * dz );
            b = addPoint3D( -1.0, (double)         i * dy, (double) ( j + 1 ) * dz );
            c = addPoint3D( -1.0, (double) ( i + 1 ) * dy, (double) ( j + 1 ) * dz );
            d = addPoint3D( -1.0, (double) ( i + 1 ) * dy, (double)         j * dz );

            face = addSurface3D( 4, a, b, c, d );
            face.FF = calHemiCubeFF( surfaceCenter( face ), left, dA );

            addPatch( &patch[left], face );

        }
    }

}

int sign( double n ){
    return n == 0 ? 0 : n < 0 ? -1 : 1;
}

int checkInOut( int plane, POINT_3D st, POINT_3D ed, POINT_3D pt ){

    VEC v1 = vectorPP( st, ed );
    VEC v2 = vectorPP( st, pt );
    VEC cross = vCross( v1, v2 );

    //(Bx-Ax)*(Y-Ay) - (By-Ay)*(X-Ax)
    return sign( cross.vector[plane == top ? 1 : ( plane == front || plane == back ) ? 2 : 0] );

}

double clipPlane( int plane, int n, VEC ray[] ){

    bool flag = true;
    int i, j, k;
    double FF = 0.0;
    POINT_3D ipt[n];

    // Calculate ipt

    // If not hve any ipt
    if( !flag )
        return 0.0;

    // Else
    else{

        // Check cell in or out
        for( i = 0 ; i < n ; i++ ){
            for( j = 0 ; j < patch[plane].n_face ; j++ ){

                if( patch[plane].flist[j].visited == -1 )
                    continue;
                else{

                    for( k = 0 ; k < patch[plane].flist[j].n_point ; k++ )
                       if( checkInOut(  i,
                                        ipt[k%(patch[plane].flist[j].n_point)],
                                        ipt[(k+1)%(patch[plane].flist[j].n_point)],
                                        patch[plane].flist[j].plist[k] ) > 0 )
                            patch[plane].flist[j].visited++;

                    patch[plane].flist[j].visited = patch[plane].flist[j].visited > 3 ? 0 : -1;

                }
            }
        }

        // Sum all cell in the clip zone
        for( i = 0 ; i < patch[plane].n_face ; i++ )
            if( patch[plane].flist[i].visited > 0 ){
                FF += patch[plane].flist[i].FF;
                patch[plane].flist[i].visited = 0;   // Reset visited
            }

        return FF;
    }

}

double clipHemiCube( int n, VEC ray[] ){

    return  clipPlane( top, n, ray )   +
            clipPlane( front, n, ray ) +
            clipPlane( back, n, ray )  +
            clipPlane( right, n, ray ) +
            clipPlane( left, n, ray );

}

void drawHemiCube(){

    int i, j, k;
    double FF;
    for( i = 0 ; i < 5 ; i++ ){
        //printf("%d\n", patch[i].n_face );
        FF = 0.0;
        for( j = 0 ; j < patch[i].n_face ; j++ ){
            //printf("%d-%d FF : %lf\n", i, j, patch[i].flist[j].FF );
            FF += patch[i].flist[j].FF;
            glColor3f( patch[i].flist[j].FF * 100000, 0.0, 0.0 );
            glBegin( GL_POLYGON );
            for( k = 0 ; k < patch[i].flist[j].n_point ; k++ ){
                glVertex3f( patch[i].flist[j].plist[k].x,
                            patch[i].flist[j].plist[k].y,
                            patch[i].flist[j].plist[k].z  );
            }
            glEnd();
        }
        printf( "%d-%lf\n", i, FF );
    }

}
