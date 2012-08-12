#include <cstdlib>
#include <cstdarg>

#include <vector>

#include "geometric.h"
#include "vector.h"

// Point
POINT_3D addPoint3D( double x, double y, double z ){

    POINT_3D pt;

    pt.x = x;
    pt.y = y;
    pt.z = z;

    return pt;

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

// Patch
PATCH createPatch(){

    PATCH p;

    p.n_faces = 0;
    return p;

}

inline void addPatch( PATCH patch, SURFACE_3D face ){

    patch.flist.push_back( face );

}
/*
int main(){

    POINT_3D a, b, c, d;
    SURFACE_3D f;
    PATCH p;

    int i;

    a = addPoint3D( 0, 0, 0 );
    b = addPoint3D( 0, 10, 0 );
    c = addPoint3D( 10, 0, 0 );
    d = addPoint3D( 10, 10, 0 );

    f = addSurface3D( 4, a, b, c, d );

    p = createPatch();
    addPatch( p, f );


    return 0;

}
*/

