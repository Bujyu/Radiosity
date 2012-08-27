#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "matrix.hpp"

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

void mDestroy( MAT m ){

    int i;

    for( i = 0 ; i < m.row ; i++ )
        free( m.matrix[i] );

    free( m.matrix );

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

// Transform Matrix
VEC transform( VEC origin, MAT transM ){

    VEC rv = vCreate( 3 );

    rv.vector[0] = origin.vector[0] * transM.matrix[0][0] + origin.vector[1] * transM.matrix[0][1] + origin.vector[2] * transM.matrix[0][2] + transM.matrix[0][3];
    rv.vector[1] = origin.vector[0] * transM.matrix[1][0] + origin.vector[1] * transM.matrix[1][1] + origin.vector[2] * transM.matrix[1][2] + transM.matrix[1][3];
    rv.vector[2] = origin.vector[0] * transM.matrix[2][0] + origin.vector[1] * transM.matrix[2][1] + origin.vector[2] * transM.matrix[2][2] + transM.matrix[2][3];

    return rv;

}

MAT rotate3D( MAT transM, float rDegree, float x_axis, float y_axis, float z_axis ){

    VEC u = vCreate( 3 );
    MAT m = mCreate( 4, 4, IDENTITY );
    float len = sqrt( ( x_axis * x_axis ) + ( y_axis * y_axis ) + ( z_axis * z_axis ) );

    u.vector[0] = x_axis / len;
    u.vector[1] = y_axis / len;
    u.vector[2] = z_axis / len;

    m.matrix[0][0] = u.vector[0] * u.vector[0] + cos( rDegree / 180 * PI ) * ( 1 - ( u.vector[0] * u.vector[0] ) ) + sin( rDegree / 180 * PI ) * 0;
    m.matrix[0][1] = u.vector[0] * u.vector[1] + cos( rDegree / 180 * PI ) * ( 0 - ( u.vector[0] * u.vector[1] ) ) + sin( rDegree / 180 * PI ) * -u.vector[2];
    m.matrix[0][2] = u.vector[0] * u.vector[2] + cos( rDegree / 180 * PI ) * ( 0 - ( u.vector[0] * u.vector[2] ) ) + sin( rDegree / 180 * PI ) * u.vector[1];
    m.matrix[1][0] = u.vector[1] * u.vector[0] + cos( rDegree / 180 * PI ) * ( 0 - ( u.vector[1] * u.vector[0] ) ) + sin( rDegree / 180 * PI ) * u.vector[2];
    m.matrix[1][1] = u.vector[1] * u.vector[1] + cos( rDegree / 180 * PI ) * ( 1 - ( u.vector[1] * u.vector[1] ) ) + sin( rDegree / 180 * PI ) * 0;
    m.matrix[1][2] = u.vector[1] * u.vector[2] + cos( rDegree / 180 * PI ) * ( 0 - ( u.vector[1] * u.vector[2] ) ) + sin( rDegree / 180 * PI ) * -u.vector[0];
    m.matrix[2][0] = u.vector[2] * u.vector[0] + cos( rDegree / 180 * PI ) * ( 0 - ( u.vector[2] * u.vector[0] ) ) + sin( rDegree / 180 * PI ) * -u.vector[1];
    m.matrix[2][1] = u.vector[2] * u.vector[1] + cos( rDegree / 180 * PI ) * ( 0 - ( u.vector[2] * u.vector[1] ) ) + sin( rDegree / 180 * PI ) * u.vector[0];
    m.matrix[2][2] = u.vector[2] * u.vector[2] + cos( rDegree / 180 * PI ) * ( 1 - ( u.vector[2] * u.vector[2] ) ) + sin( rDegree / 180 * PI ) * 0;

    return mMultiplication( m, transM );

}

MAT translate3D( MAT transM, float x, float y, float z ){

    MAT m = mCreate( 4, 4, IDENTITY );

    m.matrix[0][3] += x;
    m.matrix[1][3] += y;
    m.matrix[2][3] += z;

    return mMultiplication( m, transM );

}

MAT scale3D( MAT transM, float x, float y, float z ){

    MAT m = mCreate( 4, 4, IDENTITY );

    m.matrix[0][0] *= x;
    m.matrix[1][1] *= y;
    m.matrix[2][2] *= z;

    return mMultiplication( m, transM );

}
/*
int main(){

    MAT m = mCreate( 4, 4, IDENTITY );
    VEC v = vCreate( 3 );
    VEC rv;

    v.vector[0] = 1;
    v.vector[1] = 1;
    v.vector[2] = 1;

    mPrint( m );

    m = scale3D( m, 1, 2, 5 );
    m = rotate3D( m, 45, 1, 0, 0 );
    m = translate3D( m, 1, 0, 0 );

    mPrint( m );

    rv = transform( v, m );

    vPrint( v );
    vPrint( rv );

    return 0;

}
*/
