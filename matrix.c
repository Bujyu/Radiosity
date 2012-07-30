#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "vector.h"
#include "matrix.h"

extern VEC matrix_solution( VEC e, VEC refection, MAT FF );

MAT mCreate( int row, int col, enum matCreateType type ){

    MAT m;
    int i, j;

    m.type = ( row == col ) ? SQUARE : UNSQUARE;
    m.row = row;
    m.col = col;

    m.matrix = (double**) malloc( sizeof( double* ) * row );
    for( i = 0 ; i < row ; i++ )
         m.matrix[i] = (double*) malloc( sizeof( double ) * col );

    for( i = 0 ; i < row ; i++ )
        for( j = 0 ; j < col ; j++ )
            m.matrix[i][j] = ( type == EMPTY ) ? 0 : ( type == IDENTITY && i == j ) ? 1 : 0;

    return m;

}

void mPrint( MAT m ){

    int i, j;

    printf( "Matrix =\n" );
    for( i = 0 ; i < m.row ; i++ ){
        printf( "%c ", i == 0 ? '[' : ' ' );
        for( j = 0 ; j < m.col ; j++ )
            printf( "%lf%s", m.matrix[i][j], j != m.col - 1 ? ", " : " " );
        printf( " %c\n", i == m.row - 1 ? ']' : ' ' );
    }
    printf("\n");

}

/// Basic Function
/// 1. matrix addition
/// 2. matrix scalar
/// 3. matrix multiplication
/// 4. matrix transpose
MAT mAddition( MAT m1, MAT m2 ){

    MAT m;
    int i, j;

    if( m1.row != m2.row || m1.col != m2.col ){
        printf( "Row or column is not matching.\n" );
        exit( 0 );
    }

    m = mCreate( m1.row, m1.col, EMPTY );

    for( i = 0 ; i < m1.row ; i++ )
        for( j = 0 ; j < m1.col ; j++ )
            m.matrix[i][j] = m1.matrix[i][j] + m2.matrix[i][j];

    return m;

}

MAT mScalar( MAT m1, double scalar ){

    MAT m;
    int i, j;

    m = mCreate( m1.row, m1.col, EMPTY );
    for( i = 0 ; i < m1.row ; i++ )
        for( j = 0 ; j < m1.col ; j++ )
            m.matrix[i][j] = m1.matrix[i][j] * scalar;

    return m;

}

//O( n ^ 3 )
MAT mMultiplication( MAT m1, MAT m2 ){

    MAT m;
    int i, j, k;

    if( m1.col != m2.row ){
        printf( "The column of Matrix 1 is not equal to The row of Matrix 2.\n" );
        exit( 0 );
    }

    m = mCreate( m1.row, m2.col, EMPTY );

    for( i = 0 ; i < m1.row ; i++ )
        for( j = 0 ; j < m1.col ; j++ )
            for( k = 0 ; k < m2.col ; k++ )
                m.matrix[i][k] += m1.matrix[i][j] * m2.matrix[j][k];

    return m;

}

MAT mTranspose( MAT m1 ){

    MAT m;
    int i, j;

    m = mCreate( m1.col, m1.row, EMPTY );

    for( i = 0 ; i < m1.row ; i++ )
        for( j = 0 ; j < m1.col ; j++ )
            m.matrix[j][i] = m1.matrix[i][j];

    return m;

}

/// Advanced Function
/// 1. matrix determinant
/// 2. inverse matrix
double mDeterminant( MAT m ){

    int i, j, elements;
    double det = 0;
    double content;

    if( m.type == UNSQUARE ){
        printf( " The matrix is not square matrix.\n" );
        exit( 0 );
    }

    elements = m.row;

    if( elements == 2 )
        det = m.matrix[0][0] * m.matrix[1][1] - m.matrix[0][1] * m.matrix[1][0];

    else{

        //Upper part
        for( i = 0 ; i < elements ; i++ ){
            for( j = 0, content = 1 ; j < elements ; j++ )
                content *= m.matrix[j][(j+i)%elements];
            det += content;
        }

        //Lowwer part
        for( i = 0 ; i < elements ; i++ ){
            for( j = 0, content = 1 ; j < elements ; j++ )
                content *= m.matrix[j][elements - 1 - (j+i)%elements];
            det -= content;
        }

    }

    return det;

}

MAT vecToMat( VEC v ){

    int i;
    MAT m = mCreate( v.elements, 1, EMPTY );

    for( i = 0 ; i < v.elements ; i++ )
        m.matrix[i][0] = v.vector[i];

    return m;

}

VEC matToVec( MAT m, int col ){

    int i;
    VEC v = vCreate( m.row );

    for( i = 0 ; i < m.row ; i++ )
        v.vector[i] = m.matrix[i][col];

    return v;

}

int main(){

    VEC b;

    MAT FF = mCreate( 2, 2, EMPTY );
    VEC e = vCreate( 2 );
    VEC r = vCreate( 2 );

    FF.matrix[0][0] = 0;
    FF.matrix[0][1] = 0.5;
    FF.matrix[1][0] = 0.8;
    FF.matrix[1][1] = 0;

    e.vector[0] = 0;
    e.vector[1] = 1;

    r.vector[0] = r.vector[1] = 0.9;

    b = matrix_solution( e, r, FF );

    vPrint( b );

    return 0;

}
