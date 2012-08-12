#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cmath>

#include <vector>

#include "geometric.h"

// Point
POINT_3D addPoint3D( double x, double y, double z ){

    POINT_3D pt;

    pt.x = x;
    pt.y = y;
    pt.z = z;

    return pt;

}

inline double lengthPP( POINT_3D a, POINT_3D b ){
    return sqrt( pow( b.x - a.x, 2 ) + pow( b.y - a.y, 2 ) + pow( b.z - a.z, 2 ) );
}

VEC vectorPP( POINT_3D st, POINT_3D ed ){

    VEC v = vCreate( 3 );

    v.vector[0] = ed.x - st.x;
    v.vector[1] = ed.y - st.y;
    v.vector[2] = ed.z - st.z;

    return v;

}

// Surface
SURFACE_3D addSurface3D( int amount, ... ){

    int i;
    SURFACE_3D face;

    va_list val;
    va_start( val, amount );

    face.plist = ( POINT_3D* ) malloc( sizeof( POINT_3D ) * amount );
    face.n_point = amount;
    face.FF = 0.0;

    for( i = 0 ; i < amount ; i++ )
        face.plist[i] = va_arg( val, POINT_3D );

    va_end( val );

    return face;

}

double triangleArea( SURFACE_3D face ){

    return -1.0;

}

double squareArea( SURFACE_3D face ){

    double s;
    double a, b, c, d;
    double angleB, angleD;
    double cos_halfBD;

    a = lengthPP( face.plist[0], face.plist[1] );
    b = lengthPP( face.plist[1], face.plist[2] );
    c = lengthPP( face.plist[2], face.plist[3] );
    d = lengthPP( face.plist[3], face.plist[0] );

    s = ( a + b + c + d ) / 2;

    angleB = acos( vCos( vectorPP( face.plist[1], face.plist[0] ), vectorPP( face.plist[1], face.plist[2] ) ) );
    angleD = acos( vCos( vectorPP( face.plist[3], face.plist[2] ), vectorPP( face.plist[3], face.plist[0] ) ) );
    cos_halfBD = cos( ( angleB + angleD ) / 2 );

    return sqrt( ( ( s - a ) * ( s - b ) * ( s - c ) * ( s - d ) ) - ( a * b * c * d * cos_halfBD * cos_halfBD ) );

}

double surfaceArea( SURFACE_3D face ){

    switch( face.n_point ){
        case 3:
            return triangleArea( face );
        case 4:
            return squareArea( face );
        default:
            return -1.0;
    }

}

// Patch
PATCH createPatch(){

    PATCH p;

    p.n_faces = 0;
    return p;

}

inline void addPatch( PATCH patch, SURFACE_3D face ){

    patch.flist.push_back( face );

}

int main(){

    POINT_3D a, b, c, d;
    SURFACE_3D f;
    PATCH p;

    a = addPoint3D( 0, 0, 0 );
    b = addPoint3D( 10, 0, 0 );
    c = addPoint3D( 10, 10, 0 );
    d = addPoint3D( 0, 10, 0 );

    f = addSurface3D( 4, a, b, c, d );
    printf( "%lf\n", surfaceArea( f ) );

    p = createPatch();
    addPatch( p, f );


    return 0;

}


