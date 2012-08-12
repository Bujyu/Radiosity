///To calculate all pairs of mesh's Form Factor

#include "geometric.h"

#define SIDE 4096
#define PI 3.1415926535
enum { top = 0, front, right, back, left };

PATCH patch[5];

POINT_3F interpolation( POINT_3D a, POINT_3D b, POINT_3D c, POINT_3D d, double u, double v ){

    POINT_3F ipt;
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

double calFF( POINT a, VEC nor_a, POINT b, VEC nor_b, double dA ){

    VEC aTob = vectorPP( a, b );
    VEC bToa = vectorPP( b, a );
    double len = vLength( aTob );

    return ( vCos( nor_a, aTob ) * vCos( nor_b, bToa ) * dA ) / ( PI * len * len );

}

double calCellFF( SURFACE_3D face, VEC normal ){

    int i, j;
    double dA, du, dv;
    double Area;
    double FF = 0.0;

    // Center Point & Normal Vector
    POINT_3D center = addPoint3D( 0.0, 0.0, 0.0 );;
    VEC nc = vCreate( 3 );
    nc.vector[1] = 1.0;

    // Calculate Area
    Area = ;

    for( du = 0.0 ; du < 1.0 ; du += 0.001 ){
        for( dv = 0.0 ; dv < 1.0 ; dv += 0.001 ){
            FF += calFF( center, nc,
                         interpolation( face.plist[0], face.plist[1], face.plist[2], face.plist[3], du, dv  ), normal,
                         dA );

    return FF / Area;

}

//n_cell is 64 cells or 1024 cells per side
void hemiCubeGenrater( int n_cells ){

    // Points in cell is 64 points or 4 points per cell
    double cellSize = SIDE / n_cells;
    double halfSize = n_cells / 2;
    int i, j;

    POINT_3D a, b, c, d;
    SURFACE_3D face;

    // Normal Vector
    VEC n[5];

    n[top] = vCreate( 3 );
    n.vector[1] = -1.0;

    n[front] = vCreate( 3 );
    n.vector[2] = 1.0;

    n[back] = vCreate( 3 );
    n.vector[2] = -1.0;

    n[right] = vCreate( 3 );
    n.vector[0] = -1.0;

    n[left] = vCreate( 3 );
    n.vector[0] = 1.0;

    //Top
    for( i = -halfSize ; i < halfSize ; i++ ){
        for( j = -halfSize ; j < halfSize ; j++ ){

            a = addPoint3D(         i * cellSize, halfSize * cellSize, j * cellSize );
            b = addPoint3D(         i * cellSize, halfSize * cellSize, ( j + 1 ) * cellSize );
            c = addPoint3D( ( i + 1 ) * cellSize, halfSize * cellSize, ( j + 1 ) * cellSize);
            d = addPoint3D( ( i + 1 ) * cellSize, halfSize * cellSize, j * cellSize );

            face = addSurface3D( 4, a, b, c, d );
            face.FF = callCellFF( face, n[top] );

            addPatch( patch[top], face );

        }
    }

    //Front and Back
    for( i = -halfSize ; i < halfSize ; i++ ){
        for( j = 0; j < halfSize ; j++ ){

            //Back
            a = addPoint3D( ( i + 1 ) * cellSize, j * cellSize        , halfSize * cellSize );
            b = addPoint3D( ( i + 1 ) * cellSize, ( j + 1 ) * cellSize, halfSize * cellSize );
            c = addPoint3D(         i * cellSize, ( j + 1 ) * cellSize, halfSize * cellSize );
            d = addPoint3D(         i * cellSize, j * cellSize        , halfSize * cellSize );
            f = addSurface3D( 4, a, b, c, d );

            face = addSurface3D( 4, a, b, c, d );
            face.FF = callCellFF( face, n[back] );

            addPatch( patch[back], face );

            //Front
            a = addPoint3D(         i * cellSize, j * cellSize        , -halfSize * cellSize );
            b = addPoint3D(         i * cellSize, ( j + 1 ) * cellSize, -halfSize * cellSize );
            c = addPoint3D( ( i + 1 ) * cellSize, ( j + 1 ) * cellSize, -halfSize * cellSize );
            d = addPoint3D( ( i + 1 ) * cellSize, j * cellSize        , -halfSize * cellSize );

            face = addSurface3D( 4, a, b, c, d );
            face.FF = callCellFF( face, n[front] );

            addPatch( patch[front], face );

        }
    }

    // Right and Left
    for( i = 0; i < halfSize ; i++ ){
        for( j = -halfSize; j < halfSize ; j++ ){

            //Right
            a = addPoint3D( halfSize * cellSize, ( i + 1 ) * cellSize, j * cellSize         );
            b = addPoint3D( halfSize * cellSize, ( i + 1 ) * cellSize, ( j + 1 ) * cellSize );
            c = addPoint3D( halfSize * cellSize,         i * cellSize, ( j + 1 ) * cellSize );
            d = addPoint3D( halfSize * cellSize,         i * cellSize, j * cellSize         );

            face = addSurface3D( 4, a, b, c, d );
            face.FF = callCellFF( face, n[right] );

            addPatch( patch[right], face );

            //Left
            a = addPoint3D( -halfSize * cellSize,         i * cellSize, j * cellSize         );
            b = addPoint3D( -halfSize * cellSize,         i * cellSize, ( j + 1 ) * cellSize );
            c = addPoint3D( -halfSize * cellSize, ( i + 1 ) * cellSize, ( j + 1 ) * cellSize );
            d = addPoint3D( -halfSize * cellSize, ( i + 1 ) * cellSize, j * cellSize         );

            face = addSurface3D( 4, a, b, c, d );
            face.FF = callCellFF( face, n[left] );

            addPatch( patch[left], face );

        }
    }

}
