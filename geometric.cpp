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

POINT_3D surfaceCenter( SURFACE_3D face ){

    int n;
    POINT_3D center = addPoint3D( 0, 0, 0 );

    for( n = 0 ; n < face.n_point ; n++ ){
        center.x += face.plist[n].x;
        center.y += face.plist[n].y;
        center.z += face.plist[n].z;
    }

    center.x /= face.n_point;
    center.y /= face.n_point;
    center.z /= face.n_point;

    return center;

}

VEC vectorPP( POINT_3D st, POINT_3D ed ){

    VEC v = vCreate( 3 );

    v.vector[0] = ed.x - st.x;
    v.vector[1] = ed.y - st.y;
    v.vector[2] = ed.z - st.z;

    return v;

}

double lengthPP( POINT_3D a, POINT_3D b ){
    return sqrt( pow( b.x - a.x, 2 ) + pow( b.y - a.y, 2 ) + pow( b.z - a.z, 2 ) );
}

void ptPrint( POINT_3D p ){
    printf("[%.4lf %.4lf %.4lf]\n", p.x, p.y, p.z );
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
    face.visited = 0;

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
    p.n_face = 0;

    return p;

}

void addPatch( PATCH *patch, SURFACE_3D face ){

    (*patch).flist.push_back( face );
    (*patch).n_face++;

}

/// Point of intersection
double clap( double num, double mboundary, double pboundary ){
    return ( num > pboundary ) ? pboundary : ( num < mboundary ) ? mboundary : num;
}

int vIntersection( VEC v, int plane, POINT_3D *ipt ){

    float t;

    // Top
    if( plane == 0 ){
        t = v.vector[1] > 0.707 ? 1.0 / v.vector[1] : 0.0;
        (*ipt).x = v.vector[1] > 0.707 ? v.vector[0] * t : clap( v.vector[0], -1.0, 1.0 );
        (*ipt).y = 1.0;
        (*ipt).z = v.vector[1] > 0.707 ? v.vector[2] * t : clap( v.vector[2], -1.0, 1.0 );
        return 1;
    }
    else if( plane == 1 || plane == 3 ){
        t = fabs( v.vector[2] ) > 0.707 ? 1.0 / v.vector[2] : 0.0;

        if( t < 0.0 )
            return 0;

        (*ipt).x = fabs( v.vector[2] ) > 0.707 ? v.vector[0] * t : clap( v.vector[0], -1.0, 1.0 );
        (*ipt).y = fabs( v.vector[2] ) > 0.707 ? v.vector[1] * t : clap( v.vector[1], 0.0, 1.0 );
        (*ipt).z = plane == 1 ? -1.0 : 1.0;

        return 1;
    }
    else if( plane == 2 || plane == 4 ){
        t = fabs( v.vector[0] ) > 0.707 ? 1.0 / v.vector[0] : 0.0;

        if( t < 0.0 )
            return 0;

        (*ipt).x = plane == 2 ? -1.0 : 1.0;
        (*ipt).y = fabs( v.vector[0] ) > 0.707 ? v.vector[1] * t : clap( v.vector[1], 0.0, 1.0 );
        (*ipt).z = fabs( v.vector[0] ) > 0.707 ? v.vector[2] * t : clap( v.vector[2], -1.0, 1.0 );

        return 1;
    }

    return 0;

}


/*
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
*/

