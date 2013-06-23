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

#define EPSILON 0.000001
bool intersectionTri( SURFACE_3D &f, POINT_3D &st, const VEC &dir, double &t ){

    VEC edge1, edge2;
    VEC pvec, tvec, qvec;
    double det, inv_det;
    double u, v;

    /* find vectors for two edges sharing vert0 */
    edge1 = vectorPP( f.plist[0], f.plist[1] );
    edge2 = vectorPP( f.plist[0], f.plist[2] );

    /* begin calculating determinant - also used to calculate U parameter */
    pvec = vCross( dir, edge2 );

    /* if determinant is near zero, ray lies in plane of triangle */
    det = vDot(edge1, pvec);

    if(det > -EPSILON && det < EPSILON){
        vDestroy( edge1 );
        vDestroy( edge2 );
        vDestroy( pvec );
        return false;
    }
    inv_det = 1.0 / det;

    /* calculate distance from vert0 to ray origin */
    tvec = vectorPP(f.plist[0], st);

    /* calculate U parameter and test bounds */
    u = vDot(tvec, pvec) * inv_det;
    if(u < 0.0 || u > 1.0){
        vDestroy( edge1 );
        vDestroy( edge2 );
        vDestroy( pvec );
        vDestroy( tvec );
        return false;
    }

    /* prepare to test V parameter */
    qvec = vCross(tvec, edge1);

    /* calculate V parameter and test bounds */
    v = vDot(dir, qvec) * inv_det;
    if(v < 0.0 || u + v > 1.0){
        vDestroy( edge1 );
        vDestroy( edge2 );
        vDestroy( pvec );
        vDestroy( tvec );
        vDestroy( qvec );
        return false;
    }

    /* calculate t, ray intersects triangle */
    t = vDot(edge2, qvec) * inv_det;

    vDestroy( edge1 );
    vDestroy( edge2 );
    vDestroy( pvec );
    vDestroy( tvec );
    vDestroy( qvec );

    return true;

}

bool intersectionSqr( SURFACE_3D &f, POINT_3D &st, const VEC &dir, double &t ){

    SURFACE_3D tri_1, tri_2;
    bool flag;

    // Split square to two triangles
    tri_1 = addSurface3D( 3, f.plist[0], f.plist[1], f.plist[2]  );
    tri_2 = addSurface3D( 3, f.plist[2], f.plist[3], f.plist[0]  );

    flag = intersectionTri( tri_1, st, dir, t ) || intersectionTri( tri_2, st, dir, t );

    free( tri_1.plist );
    free( tri_2.plist );

    return flag;

}

bool intersection( SURFACE_3D &f, POINT_3D &st, const VEC &dir, double &t ){

    switch( f.n_point ){
        case 3:
            return intersectionTri( f, st, dir, t );
            break;
        case 4:
            return intersectionSqr( f, st, dir, t );
            break;
        default:
            return false;
            break;
    }

}

// check whether any scene object is between point and light
bool ShadowFeeler (const VEC &point, const VEC &light, const int faceid){

	// dir = norm (light - point);
	// distance = len (light - point);
	// for each object in scene
	//    if object is same as self
    //        continue
	//    if intersect (point, dir, distance, object_i)
	//        return TRUE;
	// return FALSE (nothing in the way to light)

	VEC tmp = vCreate( 3 );             // vector create

	VSUB3( tmp, light, point );
    VEC dir = vNormalize( tmp );
    //double distance = vLength( tmp );

    vDestroy( tmp );                    // vector destroy

    extern int searchSceneSurface( const SCENE &scene, int count, int *m, int *p, int *f );

    int mid, pid, fid;
    searchSceneSurface( scene, faceid, &mid, &pid, &fid );

    int m, p, f;
    double t;

    POINT_3D  st;
    st = addPoint3D( point.vector[0], point.vector[1], point.vector[2] );

    for( int i = 0 ; i < scene.n_face ; i++ ) {

        searchSceneSurface( scene, i, &m, &p, &f );
        //cout << faceid << " " << i << " " << intersection( scene.list[m].plist[p].flist[f], st, dir, t ) << endl;
        if( intersection( scene.list[m].plist[p].flist[f], st, dir, t ) && ( m != mid && p != pid && f != fid )  )
            return true;

    }
    return false;

}

double clamp( double value, const double min, const double max ){

	return (value > max) ? max : (value < min) ? min : value;

}

// ambient, diffuse, specular
// also return reflect ray for later use
struct _material {
	VEC ambient;
	VEC diffuse;
	VEC specular;
	int shininess;
};

VEC EvaluateIlocal( const VEC &point, const int faceid, const VEC &n ){

	extern VEC lightpos[5];
	//extern VEC camera;
	extern int searchSceneSurface( const SCENE &scene, int count, int *m, int *p, int *f );
	extern int numlights;;

	// compute normal of objectid at point
	//struct _material mat;

    int mid, pid, fid;

	VEC intensity = vCreateArg( 3, 0.0, 0.0, 0.0 );
	for( int i = 0; i < numlights; i++ ) {

        extern VEC b[3];

        searchSceneSurface( scene, faceid, &mid, &pid, &fid );
        //ambient component
        intensity.vector[0] += b[0].vector[faceid]*3;
        intensity.vector[1] += b[1].vector[faceid]*3;
        intensity.vector[2] += b[2].vector[faceid]*3;


		VEC tmp = vCreate( 3 );             // vector create
		VSUB3( tmp, lightpos[i], point )
		VEC l = vNormalize( tmp );
		vDestroy( tmp );                    // vector destroy
		double ndotl = vDot( n, l );

		if ( ndotl > 0.0 && !ShadowFeeler( point, lightpos[i], faceid )  ){ // no block

            //diffuuse component
            intensity.vector[0] += ndotl * b[0].vector[faceid];
            intensity.vector[1] += ndotl * b[1].vector[faceid];
            intensity.vector[2] += ndotl * b[2].vector[faceid];

/*
            //Has edited variable reflect
			VEC reflect = vCreate( 3 );
			tmp = vScalar( n, 2 * ndotl );  // vector create
			VSUB3( tmp, tmp, l );
			reflect = vNormalize( tmp );
			vDestroy( tmp );                // vector destroy

			tmp = vCreate( 3 );             // vector create
			VSUB3( tmp, camera, point );
			VEC v = vNormalize( tmp );
			vDestroy( tmp );                // vector destroy

			//specular component
			double shininessFactor = vDot( reflect, v ) * 250;
            intensity.vector[0] += pow( shininessFactor, 1.0 );
            intensity.vector[1] += pow( shininessFactor, 1.0 );
            intensity.vector[2] += pow( shininessFactor, 1.0 );

            vDestroy( reflect );
*/
		}

        vDestroy( l );

	}

	return intensity;

}

// replace {find first hit}
// with a first-hit guess procedure using OpenGL prerender
//
VEC raytrace( const VEC &c, const VEC &dir, int &level ){

    extern int searchSceneSurface( const SCENE &scene, int count, int *m, int *p, int *f );
	extern int maxlevel;
	//extern MAT visible;

	if( level > maxlevel )
		return vCreateArg( 3, 0.0, 0.0, 0.0 );

	level++;

	// {find first hit (object, point)}
	int first_hit = -1;
	bool hit = false;
	double min_d = 1e10;
	double t;

    POINT_3D st = addPoint3D( c.vector[0], c.vector[1], c.vector[2] );
    int mid, pid, fid;

    /* All patches in the scene */
    for( int i = 0 ; i < scene.n_face ; i++ ){

        searchSceneSurface( scene, i, &mid, &pid, &fid );
		if( intersection( scene.list[mid].plist[pid].flist[fid], st, dir, t ) && t < min_d ) {
			hit = true;
			first_hit = i;
			min_d = t;
		}

    }

    if( hit ){

		VEC point = vCreate( 3 );
		point.vector[0] = c.vector[0] + dir.vector[0] * min_d; // compute first hit point
		point.vector[1] = c.vector[1] + dir.vector[1] * min_d; // compute first hit point
		point.vector[2] = c.vector[2] + dir.vector[2] * min_d; // compute first hit point

        searchSceneSurface( scene, first_hit, &mid, &pid, &fid );       // Get hit face idx
		VEC normal = vClone( scene.list[mid].plist[pid].flist[fid].normal );

		VEC intensity = EvaluateIlocal( point, first_hit, normal );
		double Kr = 0.6;

		extern VEC camera;
		VEC tmp = vCreate( 3 );

		// vNormalize( 2 * vDot( camera - point, normal ) * normal - ( camera - point ) );
		VSUB3( tmp, camera, point );    // Vector camera to point
		double dot = vDot( tmp, normal );
        tmp.vector[0] = 2 * dot * normal.vector[0] - tmp.vector[0];
        tmp.vector[1] = 2 * dot * normal.vector[1] - tmp.vector[1];
        tmp.vector[2] = 2 * dot * normal.vector[2] - tmp.vector[2];

        VEC reflect = vNormalize( tmp );
        vDestroy( tmp );

        tmp = raytrace( point, reflect, ++level );
		intensity.vector[0] += Kr * clamp( tmp.vector[0], 0.0, 1.0 );
		intensity.vector[1] += Kr * clamp( tmp.vector[1], 0.0, 1.0 );
		intensity.vector[2] += Kr * clamp( tmp.vector[2], 0.0, 1.0 );

		vDestroy( tmp );
		vDestroy( reflect );
		vDestroy( normal );
		vDestroy( point );

    //    if (transparent)
    // 		 {trace the ray in the object, until it move out}
    //         (consider total reflection)
    //       intensity += Kt * raytrace(outpoint, outdir, level+1)

		return intensity;
	}
	else{                    // nothing hit
		return vCreateArg( 3, 0.0, 0.0, 0.0 ); // background color
    }

}
