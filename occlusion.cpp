#include <cstdio>

#include "geometric.h"
#include "vector.hpp"


float occlusion( SURFACE_3D i, SURFACE_3D j ){

    double visibility = 0.0;

    // Pass 1
    VEC aTob = vectorPP( i.center, j.center );
    VEC bToa = vectorPP( j.center, i.center );
    double cosA, cosB;

    cosA = vCos( i.normal, aTob );
    cosB = vCos( j.normal, bToa );

    if( cosA < 0.0 || cosB < 0.0 )
        return visibility;

    // Pass 2

    vDestroy( aTob );
    vDestroy( bToa );

    return 1.0;

}
