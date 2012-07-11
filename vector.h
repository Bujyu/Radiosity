#ifndef VECTOR_H
#define VECTOR_H

typedef struct vec{

    double *vector;
    int elements;

} VEC;

VEC     vCreate     ( int elements );
void    vPrint      ( VEC v );

/// Basic Function
/// 1. vector addition
/// 2. vector scalar
/// 3. vector dot
/// 4. vector length
/// 5. vector normalize
/// 6. vector transpose( to matrix )
VEC     vAddition   ( VEC v1, VEC v2 );
VEC     vScalar     ( VEC v1, double scalar );
double  vDot        ( VEC v1, VEC v2 );
double  vLength     ( VEC v );
VEC     vNormalize  ( VEC v );
//MAT     vTranspose  ( VEC v );

#endif
