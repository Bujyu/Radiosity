///To calculate all pairs of mesh's Form Factor

#include <windows.h>
#include <gl/glut.h>
#include <vector>
#include <cstdio>
#include "geometric.h"

#define SIDE 1024
#ifndef PI
    #define PI 3.1415926535
#endif

enum plane{ top = 0, front, right, back, left };
PATCH patch[5];

void interpolation( POINT_3D *ipt, POINT_3D a, POINT_3D b, POINT_3D c, POINT_3D d, double u, double v ){

/*  Triangle
    ipt.x = ( 1.0 - u - v ) * p0.x + u * p1.x + v * p2.x;
    ipt.y = ( 1.0 - u - v ) * p0.y + u * p1.y + v * p2.y;
    ipt.z = ( 1.0 - u - v ) * p0.z + u * p1.z + v * p2.z;
*/

    (*ipt).x = ( 1.0 - u ) * ( 1.0 - v ) * a.x +
            ( 1.0 - u ) * v * b.x +
            u * v * c.x +
            u * ( 1.0 - v ) * d.x;

    (*ipt).y = ( 1.0 - u ) * ( 1.0 - v ) * a.y +
            ( 1.0 - u ) * v * b.y +
            u * v * c.y +
            u * ( 1.0 - v ) * d.y;

    (*ipt).z = ( 1.0 - u ) * ( 1.0 - v ) * a.z +
            ( 1.0 - u ) * v * b.z +
            u * v * c.z +
            u * ( 1.0 - v ) * d.z;

}

double calFF( POINT_3D a, VEC nor_a, POINT_3D b, VEC nor_b ){

    VEC aTob = vectorPP( a, b );
    VEC bToa = vectorPP( b, a );
    double r = lengthPP( a, b );
    double FF;

    FF = ( r - 0.0 < 0.000001 ) ?  0.0 : ( vCos( nor_a, aTob ) * vCos( nor_b, bToa ) ) / ( PI * r * r );

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

            interpolation( &iipt, i.plist[0], i.plist[1], i.plist[2], i.plist[3], diu, div );

            FF = 0.0;

            for( dju = HSTEP ; dju <= 1.0 ; dju += STEP ){
                for( djv = HSTEP ; djv <= 1.0 ; djv += STEP ){

                    interpolation( &jipt, j.plist[0], j.plist[1], j.plist[2], j.plist[3], dju, djv);
                    FF += calFF( iipt, inormal, jipt, jnormal ) * djA;

                }
            }

            FFij += FF * diA;

        }

    }

    return FFij / iarea;

}

double calHemiCubeFF( POINT_3D pt, int n, double dA ){

    double r = lengthPP(addPoint3D(0,0,0), pt);

    return ( ( n == top ? 1 : pt.y ) * dA ) / ( PI * r * r );

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
    return n > 0 ? 1 : n < 0 ? -1 : 0;
}

int checkInOut( int plane, POINT_3D st, POINT_3D ed, POINT_3D pt ){
/*
    int s;

    VEC v1 = vectorPP( st, ed );
    VEC v2 = vectorPP( st, pt );
    VEC cross = vCross( v1, v2 );

    vDestroy( v1 );
    vDestroy( v2 );


    switch( plane ){
        case right: case left:
            s = sign( cross.vector[0] );
        case front: case back:
            s = sign( cross.vector[2] );
        default:
            s = sign( cross.vector[1] );
    }

    vDestroy( v1 );
    vDestroy( v2 );
    vDestroy( cross );

    return s;
*/
    //(Bx-Ax)*(Y-Ay) - (By-Ay)*(X-Ax)
    if( plane == front || plane == back )
        return sign( ( ed.x - st.x ) * ( pt.y - st.y ) - ( ed.y - st.y ) * ( pt.x - st.x  ) );
    else if( plane == right || plane == left )
        return sign( ( ed.y - st.y ) * ( pt.z - st.z ) - ( ed.z - st.z ) * ( pt.y - st.y  ) );
    else
        return sign( ( ed.x - st.x ) * ( pt.z - st.z ) - ( ed.z - st.z ) * ( pt.x - st.x  ) );

}

double clipPlane( int plane, int n, VEC ray[] ){

    int count = 0;

    int i, j, k;
    double FF = 0.0;
    POINT_3D *ipt;

    ipt = (POINT_3D*) malloc( sizeof( POINT_3D ) * n );

    // Calculate ipt
    for( i = 0 ; i < n ; i++ )
        count += vIntersection( ray[i], plane, &ipt[i] );

    if( count ){

        // Check cell in or out
        for( i = 0 ; i < n ; i++ ){
            for( j = 0 ; j < patch[plane].n_face ; j++ ){

                if( patch[plane].flist[j].visited == -1 )
                    continue;
                else{
                    count = 0;
                    for( k = 0 ; k < patch[plane].flist[j].n_point ; k++ ){
                        if( checkInOut(  plane,
                                         ipt[i%(patch[plane].flist[j].n_point)],
                                         ipt[(i+1)%(patch[plane].flist[j].n_point)],
                                         patch[plane].flist[j].plist[k] ) >= 0 ){
                            count++;
                        }
                    }
                    patch[plane].flist[j].visited = count > 0 ? 1 : -1;
                }
            }
        }

        // Sum all cell in the clip zone
        for( i = 0 ; i < patch[plane].n_face ; i++ ){
            if( patch[plane].flist[i].visited == 1 ){
                FF += patch[plane].flist[i].FF;
                //patch[plane].flist[i].visited = 0;   // Reset visited
            }
        }

    }

    free( ipt );

    return FF;

}

double clipHemiCube( int n, VEC ray[] ){

    double FF = 0.0;

    FF += clipPlane( top, n, ray );
    FF += clipPlane( front, n, ray );
    FF += clipPlane( back, n, ray );
    FF += clipPlane( right, n, ray );
    FF += clipPlane( left, n, ray );

    return FF;

}

double meshToHC( SURFACE_3D i, SURFACE_3D j ){

    VEC *ray;
    POINT_3D ipt;
    int u, v, n;

    double FF = 0.0;
    double area = surfaceArea( i );

    double dA;
    int step = 100;

    dA = area / ( step * step );

    ray = (VEC*) malloc( sizeof( VEC ) * j.n_point );

    for( u = 0 ; u <= step ; u++ ){
        for( v = 0 ; v <= step ; v++ ){
            interpolation( &ipt, i.plist[0], i.plist[1], i.plist[2], i.plist[3], (double) u / step, (double) v / step );
            for( n = 0 ; n < j.n_point ; n++ )
                ray[n] = vNormalize( vectorPP( ipt, j.plist[n] ) );
            FF += clipHemiCube( j.n_point, ray ) * dA;
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
    FF = clipHemiCube( i.n_point, ray );

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

