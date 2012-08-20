#ifndef MARTIX_H
#define MATRIX_H

#include <cmath>

#include "geometric.h"
#include "vector.h"

#ifndef PI
    #define PI 3.1415926535
#endif

///Matrix based structure
enum matCreateType{ EMPTY = 0, IDENTITY };
enum matType{ SQUARE = 0, UNSQUARE };

typedef struct mat{

    double **matrix;
    enum matType type;
    int row;
    int col;

} MAT;

MAT mCreate( int, int, enum matCreateType );
void mPrint( MAT );

/// Basic Function
/// 1. matrix addition
/// 2. matrix scalar
/// 3. matrix multiplication
/// 4. matrix transpose
MAT mAddition( MAT, MAT );
MAT mScalar( MAT, double );

//O( n ^ 3 )
MAT mMultiplication( MAT, MAT );
MAT mTranspose( MAT );

/// Advanced Function
/// 1. matrix determinant
/// 2. inverse matrix
double mDeterminant( MAT );

/// Vector to Matrix
MAT vecToMat( VEC );
VEC matToVec( MAT, int );

/// Transform Matrix
VEC transform( VEC origin, MAT transM );
MAT rotate3D( MAT transM,float rDegree, float x_axis, float y_axis, float z_axis );
MAT translate3D( MAT transM, float x, float y, float z );
MAT scale3D( MAT transM, float x, float y, float z );

#endif
