#ifndef VECTOR_H
#define VECTOR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct vec{

    double *vector;
    int elements;

} VEC;

VEC vCreate( int elements );
void vPrint( VEC v );

/// Basic Function
/// 1. vector addition
/// 2. vector scalar
/// 3. vector dot
/// 4. vector length
/// 5. vector normalize
/// 6. vector transpose( to matrix )
/// 7. vector clone
VEC vAddition( VEC v1, VEC v2 );
VEC vScalar( VEC v1, double scalar );
double vDot( VEC v1, VEC v2 );
double vCos( VEC v1, VEC v2 );
VEC vCross( VEC v1, VEC v2 );    //Only in 3-Dimension vector
double vLength( VEC v );
inline VEC vNormalize( VEC v );
//MAT vTranspose  ( VEC v );
VEC vClone( VEC v );
inline void vDestroy( VEC v );

#ifdef __cplusplus
}
#endif

#endif
