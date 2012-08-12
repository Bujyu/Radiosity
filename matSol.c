#include "vector.h"
#include "matrix.h"
#include <stdio.h>

///To check result is converge or not.
///return 0 if true
///return 1 if false
int converge( VEC b, VEC e, VEC reflection, MAT FF, double boundary ){

    VEC minusE;
    MAT t = mCreate( FF.col, FF.row, EMPTY );
    MAT r;
    int i, j;

    minusE = vScalar( e, -1 );

    for( i = 0 ; i < FF.row ; i++ )
        for( j = 0 ; j < FF.col ; j++ )
            t.matrix[i][j] = i == j ? 1 - ( reflection.vector[i] * FF.matrix[i][j] ) :
                                      reflection.vector[i] * FF.matrix[i][j];

    r = mMultiplication( t, vecToMat( b ) );

    if( vLength( vAddition( matToVec( r, 0 ), minusE ) ) > boundary )
            return 1;

    return 0;

}

VEC matrix_solution( VEC emission, VEC reflection, MAT FF ){

    VEC x = vClone( emission );
    VEC nx = vCreate( emission.elements );
    int i, j, k;
    double r;

    k = 0;
    do{

        for( i = 0 ; i < emission.elements ; i++ ){
            r = 0;
            for( j = 0 ; j < emission.elements ; j++ ){
                if( i != j )
                    r += reflection.vector[i] * FF.matrix[i][j] * x.vector[j];
            }
            nx.vector[i] = emission.vector[i] - r;
        }
        x = vClone( nx );

        k++;

    }while( converge( x, emission, reflection, FF, 0.00001 ) );

    //printf("k = %d\n", k);

    return x;

}
