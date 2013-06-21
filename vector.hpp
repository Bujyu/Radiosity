#ifndef VECTOR_H
#define VECTOR_H

#ifndef PI
    #define PI 3.1415926535
#endif

typedef struct vec{

    double *vector;
    int elements;

} VEC;

VEC vCreate( int elements );
VEC vCreateArg( int elements, ... );
void vPrint( VEC v );

#define VASSIGN3( t, a ) {  \
            t.vector[0] = a.vector[0]; \
            t.vector[1] = a.vector[1]; \
            t.vector[2] = a.vector[2]; \
        }
#define VADD3( t, a, b ) {  \
            t.vector[0] = a.vector[0] + b.vector[0]; \
            t.vector[1] = a.vector[1] + b.vector[1]; \
            t.vector[2] = a.vector[2] + b.vector[2]; \
        }
#define VSUB3( t, a, b ) {  \
            t.vector[0] = a.vector[0] - b.vector[0]; \
            t.vector[1] = a.vector[1] - b.vector[1]; \
            t.vector[2] = a.vector[2] - b.vector[2]; \
        }
#define VMUL3( t, a, b ) {  \
            t.vector[0] = a.vector[0] * b.vector[0]; \
            t.vector[1] = a.vector[1] * b.vector[1]; \
            t.vector[2] = a.vector[2] * b.vector[2]; \
        }
#define VDIV3( t, a, b ) {  \
            t.vector[0] = a.vector[0] / b.vector[0]; \
            t.vector[1] = a.vector[1] / b.vector[1]; \
            t.vector[2] = a.vector[2] / b.vector[2]; \
        }


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
VEC vNormalize( VEC v );
//MAT vTranspose  ( VEC v );
VEC vClone( VEC v );
void vDestroy( VEC v );

//
float axisRot( VEC v1, VEC v2, int axis );

#endif
