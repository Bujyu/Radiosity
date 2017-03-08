#include <GL/GL.h>
#include "vector.hpp"
#include "matrix.hpp"
#include "geometric.h"

#include <cmath>
#include <cstring>
#include <iostream>
using namespace std;

// Screen size is 600 * 600
extern GLubyte image[600][600][3];
extern SCENE scene;

extern VEC raytrace( const VEC &c, const VEC &dir, int &level );
extern double clamp( double value, const double min, const double max );

void GetCameraFrame( VEC& c, VEC& point, VEC& up ){

	double m[16];
	glGetDoublev (GL_MODELVIEW_MATRIX, m);

	// explicit formulation: does not speed up as much as I expected ..
	c.vector[0] = -m[0]*m[12]-m[1]*m[13]-m[2]*m[14];
	c.vector[1] = -m[4]*m[12]-m[5]*m[13]-m[6]*m[14];
	c.vector[2] = -m[8]*m[12]-m[9]*m[13]-m[10]*m[14];

	point.vector[0] = -m[0]*m[12]-m[1]*m[13]-m[2]*(m[14]+1);
	point.vector[1] = -m[4]*m[12]-m[5]*m[13]-m[6]*(m[14]+1);
	point.vector[2] = -m[8]*m[12]-m[9]*m[13]-m[10]*(m[14]+1);

	//point -= c;
	VSUB3( point, point, c )

	// up: [0,1,0]
	up.vector[0] = -m[0]*m[12]-m[1]*(m[13]-1)-m[2]*m[14];
	up.vector[1] = -m[4]*m[12]-m[5]*(m[13]-1)-m[6]*m[14];
	up.vector[2] = -m[8]*m[12]-m[9]*(m[13]-1)-m[10]*m[14];

	//up -= c;
	VSUB3( up, up, c )

}

#define SetRGB(i,j,rgb) {\
    image[i][j][0] = rgb.vector[0]*255;\
    image[i][j][1] = rgb.vector[1]*255;\
    image[i][j][2] = rgb.vector[2]*255;\
}

void DoRayTrace( void ){

	int i, j;

	// clear canvas
	memset( image, 0, 600*600*3 );

	// get camera position
	VEC c = vCreate( 3 );
	VEC point = vCreate( 3 );
	VEC up = vCreate( 3 );

	GetCameraFrame( c, point, up );

    // camera = c
    extern VEC camera;
    VASSIGN3( camera, c );

	// op: origin of projection plane (lower left corner)
	const double glnear = 1.0;
	const double fovy = 65.0;

	VEC op = vCreate( 3 );
    // op = c + glnear * point;
    op.vector[0] = c.vector[0] + ( glnear * point.vector[0] );
    op.vector[1] = c.vector[1] + ( glnear * point.vector[1] );
    op.vector[2] = c.vector[2] + ( glnear * point.vector[2] );

	double w = 2 * glnear * tan( fovy / 360 * M_PI);

	VEC V = vClone( up );
	VEC U = vCross( point, V );

	// op = op - w / 2 * U - w / 2 * V;
	op.vector[0] = op.vector[0] - w / 2 * U.vector[0] - w / 2 * V.vector[0];
	op.vector[1] = op.vector[1] - w / 2 * U.vector[1] - w / 2 * V.vector[1];
	op.vector[2] = op.vector[2] - w / 2 * U.vector[2] - w / 2 * V.vector[2];

	VEC dir = vCreate( 3 );
	VEC gridpt = vCreate( 3 );
	//VEC p = vCreate( 3 );
	//VEC normal = vCreate( 3 );
	VEC tmp = vCreate( 3 );

	double div = w / 600;
    int level;

    extern void progressBar( int total, int now, int barLength );

	for (i = 0; i < 600; i++) {

        progressBar( 600, i, 25 );

		for (j = 0; j < 600; j++) {
			// gridpt = op + ( i + 0.5 ) * div * V + ( j + 0.5 ) * div * U;
			gridpt.vector[0] = op.vector[0] + ( i + 0.5 ) * div * V.vector[0] + ( j + 0.5 ) * div * U.vector[0];
			gridpt.vector[1] = op.vector[1] + ( i + 0.5 ) * div * V.vector[1] + ( j + 0.5 ) * div * U.vector[1];
			gridpt.vector[2] = op.vector[2] + ( i + 0.5 ) * div * V.vector[2] + ( j + 0.5 ) * div * U.vector[2];

			// r = norm( gridpt - c );
			VSUB3( tmp, gridpt, c );
			dir = vNormalize( tmp );

			level = 1;

            //cout << i << "-" << j <<endl;
			VEC intensity = raytrace( c, dir, level );  // level 1 for eye ray
            intensity.vector[0] = clamp( intensity.vector[0], 0.0, 1.0 );
            intensity.vector[1] = clamp( intensity.vector[1], 0.0, 1.0 );
            intensity.vector[2] = clamp( intensity.vector[2], 0.0, 1.0 );

            //cout << intensity << endl;
			SetRGB( i, j, intensity );

			vDestroy( intensity );
			vDestroy( dir );

		}
	}

    vDestroy( tmp );
    vDestroy( gridpt );

    vDestroy( U );
    vDestroy( V );
    vDestroy( op );

    vDestroy( c );
	vDestroy( point );
	vDestroy( up );

}
