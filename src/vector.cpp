#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <cmath>
#include <ctime>

#include "vector.hpp"

VEC vCreate( int elements ){

    VEC v;

    v.elements = elements;
    v.vector = (double*) malloc( sizeof( double ) * elements );
    for( int i = 0 ; i < elements ; i++ )
        v.vector[i] = 0.0;

    return v;

}

VEC vCreateArg( int elements, ... ){

    VEC v;

    va_list val;
    va_start( val, elements );

    v.elements = elements;
    v.vector = (double*) malloc( sizeof( double ) * elements );
    for( int i = 0 ; i < elements ; i++ )
        v.vector[i] = va_arg( val, double );

    va_end( val );

    return v;

}

void vPrint( VEC v ){

    int i;

    printf( "Vector =\n" );
    printf("[ ");
    for( i = 0 ; i < v.elements ; i++ )
        printf( "%f%s", v.vector[i], i != v.elements - 1 ? ", " : " " );
    printf("]\n\n");

}

/// Basic Function
/// 1. vector addition
/// 2. vector scalar
/// 3. vector dot
/// 4. vector length
/// 5. vector normalize
/// 6. vector transpose( to matrix )
/// 7. vector clone
VEC vAddition( VEC v1, VEC v2 ){

    int i;
    VEC v;

    if( v1.elements != v2.elements ){
        printf( "Elements is not matching.\n" );
        exit( 0 );
    }

    v = vCreate( v1.elements );

    for( i = 0 ; i < v1.elements ; i++ )
        v.vector[i] = v1.vector[i] + v2.vector[i];

    return v;

}

VEC vScalar( VEC v1, double scalar ){

    int i;
    VEC v;

    v = vCreate( v1.elements );
    for( i = 0 ; i < v1.elements ; i++ )
        v.vector[i] = v1.vector[i] * scalar;

    return v;

}

double vDot( VEC v1, VEC v2 ){

    int i;
    double dot = 0.0;

    if( v1.elements != v2.elements ){
        printf( "Elements is not matching.\n" );
        exit( 0 );
    }

    for( i = 0 ; i < v1.elements ; i++ )
        dot += ( v1.vector[i] * v2.vector[i] );

    return dot;

}

double vCos( VEC v1, VEC v2 ){
    return vDot( v1, v2 ) / ( vLength( v1 ) * vLength( v2 ) );
}

VEC vCross( VEC v1, VEC v2 ){

    VEC cross = vCreate( 3 );

    if( v1.elements != 3 && v2.elements != 3 ){
        printf( "Dimension is not matching.\n" );
        exit( 0 );
    }

    cross.vector[0] = v1.vector[1] * v2.vector[2] - v1.vector[2] * v2.vector[1];
    cross.vector[1] = v1.vector[2] * v2.vector[0] - v1.vector[0] * v2.vector[2];
    cross.vector[2] = v1.vector[0] * v2.vector[1] - v1.vector[1] * v2.vector[0];

    return cross;

}

double vLength( VEC v ){

    int i;
    double length = 0;

    for( i = 0 ; i < v.elements ; i++ )
        length += v.vector[i] * v.vector[i];

    return sqrt( length );

}

VEC vNormalize( VEC v ){
    return vScalar( v, 1 / vLength( v ) );
}

/*
MAT vTranspose( VEC v ){

    MAT m;
    int i;

    m = mCreate( v.elements, 1, EMPTY );

    for( i = 0 ; i < v.elements ; i++ )
        m.matrix[i][0] = v.vector[i];

    return m;

}
*/

VEC vClone( VEC v ){

    int i;
    VEC clone = vCreate( v.elements );

    for( i = 0; i < v.elements ; i++ )
        clone.vector[i] = v.vector[i];

    return clone;

}

void vDestroy( VEC v ){

    free( v.vector );

}

/// Trasform
float axisRot( VEC v1, VEC v2, int axis ){

    float degree;
    double sign;
    VEC temp = vClone( v2 );

    temp.vector[axis] = 0;

    sign = ( v1.vector[(axis+1)%3] * v2.vector[(axis+2)%3] ) - ( v1.vector[(axis+2)%3] * v2.vector[(axis+1)%3] );
    degree = ( sign > 0 ? 1.0 : -1.0 ) * acos( vCos( v1, temp ) ) * 180 / PI;

    vDestroy( temp );

    return degree;

}

/*
int main(){

    VEC v1, v2;
    VEC aTob, bToa;

    POINT_3D a, b;

    a = addPoint3D( 0, 0, 0 );
    b = addPoint3D( 0, 5, 0 );

    aTob = vectorPP( a, b );
    bToa = vectorPP( b, a );

    v1 = vCreate( 3 );
    v2 = vCreate( 3 );

    //Assign value
    v1.vector[0] = 0;
    v1.vector[1] = 1;
    v1.vector[2] = 0;

    v2.vector[0] = 0;
    v2.vector[1] = 1;
    v2.vector[2] = 0;

    vPrint( v1 );
    vPrint( v2 );
    vPrint( aTob );
    vPrint( bToa );

    printf( "%lf\n", vCos( v1, aTob ) );
    printf( "%lf\n", vCos( v2, bToa ) );

    return 0;

}
*/
