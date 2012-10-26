#include "vector.hpp"
#include "matrix.hpp"
#include <stdio.h>

///To check result is converge or not.
///return 0 if true
///return 1 if false
int converge( VEC b, VEC e, VEC reflection, MAT FF, double boundary ){

    VEC rv = vCreate( b.elements );
    int i, j;
    int flag = 0;
    double r;

    for( i = 0 ; i < FF.row ; i++ ){
        r = 0;
        for( j = 0 ; j < FF.col ; j++ )
            r += ( i == j ? 1 : -( reflection.vector[i] * FF.matrix[i][j] ) ) * b.vector[j];
        rv.vector[i] = r;
    }

    for( i = 0 ; i < rv.elements && !flag ; i++ ){
        if( fabs( rv.vector[i] - e.vector[i] ) > boundary )
            flag = 1;
    }

    vDestroy( rv );

    return flag;

}

VEC matrix_solution( VEC emission, VEC reflection, MAT FF ){

    VEC x = vClone( emission );
    VEC nx = vCreate( emission.elements );
    int i, j;
    double r;

    do{

        for( i = 0 ; i < emission.elements ; i++ ){
            r = 0.0;
            for( j = 0 ; j < emission.elements ; j++ ){
                if( i != j )
                    r += FF.matrix[i][j] * x.vector[j];
            }
            nx.vector[i] = emission.vector[i] + ( reflection.vector[i] * r );
        }

        for( i = 0; i < x.elements ; i++ )
            x.vector[i] = nx.vector[i];

    }while( converge( x, emission, reflection, FF, 0.0001 ) );

    vDestroy( nx );

    return x;

}
