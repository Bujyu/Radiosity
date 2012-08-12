#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "vector.h"

VEC vCreate( int elements ){

    VEC v;
    int i;

    v.elements = elements;
    v.vector = (double*) malloc( sizeof( double ) * elements );

    for( i = 0 ; i < elements ; i++ )
        v.vector[i] = 0.0;

    return v;

}

void vPrint( VEC v ){

    int i;

    printf( "Vector =\n" );
    printf("[ ");
    for( i = 0 ; i < v.elements ; i++ )
        printf( "%lf%s", v.vector[i], i != v.elements - 1 ? ", " : " " );
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
    double dot = 0;

    if( v1.elements != v2.elements ){
        printf( "Elements is not matching.\n" );
        exit( 0 );
    }

    for( i = 0 ; i < v1.elements ; i++ )
        dot += v1.vector[i] * v2.vector[i];

    return dot;

}

inline double vCos( VEC v1, VEC v2 ){
    return vDot( v1, v2 ) / ( vLength( v1 ) * vLength( v2 ) );
}

double vLength( VEC v ){

    int i;
    double length = 0;

    for( i = 0 ; i < v.elements ; i++ )
        length += pow( v.vector[i], 2 );

    return sqrt( length );

}

inline VEC vNormalize( VEC v ){
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

inline void vDestroy( VEC v ){

    free( v.vector );

}
/*
int main(){

    int i;
    VEC v1, v2;

    srand( time( NULL ) );

    v1 = vCreate( 3 );
    v2 = vCreate( 3 );

    //Assign value
    v1.vector[0] = 1;
    v1.vector[1] = 0;
    v1.vector[2] = 0;

    v2.vector[0] = 1;
    v2.vector[1] = 1;
    v2.vector[2] = 0;

    vPrint( v1 );
    vPrint( v2 );

    printf("vector dot : %lf\n", vDot( v1, v2 ) );
    printf("lenght : %lf\n", vLength( v1 ) );
    printf("Cos : %lf\n", acos( vCos( v1, v2 ) ) * 180.0 / M_PI );
    vPrint( vNormalize( v1 ) );

    return 0;

}
*/
