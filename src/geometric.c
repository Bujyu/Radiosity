#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


#include "geometric.h"

POINT_3D addPoint3D( double x, double y, double z ){

    POINT_3D pt;

    pt.x = x;
    pt.y = y;
    pt.z = z;

    return pt;

}

SURFACE_3D addSurface3D( int amount, ... ){

    int i;
    SURFACE_3D face;

    va_list val;
    va_start( val, amount );

    face.plist = ( POINT_3D* ) malloc( sizeof( POINT_3D ) * amount );
    face.n_point = amount;

    for( i = 0 ; i < amount ; i++ )
        face.plist[i] = va_arg( val, POINT_3D );

    va_end( val );

    return face;

}

PATCH createPatch(){

    PATCH p;

    p.flist = NULL;
    p.fend = NULL;


}

void addPatch( PATCH patch, SURFACE_3D face ){



}

int main(){

    POINT_3D a, b, c, d;
    SURFACE_3D f;

    int i;

    a = addPoint3D( 0, 0, 0 );
    b = addPoint3D( 0, 10, 0 );
    c = addPoint3D( 10, 0, 0 );
    d = addPoint3D( 10, 10, 0 );

    f = addSurface3D( 4, a, b, c, d );

    for( i = 0 ; i < f.n_point; i++ )
        printf( "%d-%lf %lf %lf\n", i, f.plist[i].x, f.plist[i].y, f.plist[i].z );

    return 0;

}

