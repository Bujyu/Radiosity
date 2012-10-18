#include <cstdio>

#include "geometric.h"
#include "vector.hpp"

extern SCENE scene;

int betweenCheck( SURFACE_3D i, SURFACE_3D j, SURFACE_3D f ){

    if( ( i.normal.vector[0] * f.center.x + i.normal.vector[1] * f.center.y + i.normal.vector[2] * f.center.z ) < 0 ||
        ( j.normal.vector[0] * f.center.x + j.normal.vector[1] * f.center.y + j.normal.vector[2] * f.center.z ) < 0 )
        return 0;

    return 1;

}

int checkOcclusion( int n, VEC *ray, SURFACE_3D face ){

    VEC *fnormal = (VEC*) malloc( sizeof( VEC ) * n );
    double d;
    int flag = 0;

    for( int i = 0 ; i < n ; i++ )
        fnormal[i] = vCross( ray[(i+1)%n], ray[i] );

    for( int i = 0 ; i < n ; i++ ){
        d = ( face.center.x * fnormal[i].vector[0] ) +
            ( face.center.y * fnormal[i].vector[1] ) +
            ( face.center.z * fnormal[i].vector[2] ) ;
        if( d < 0.0 ){
            flag = 0;
            break;
        }
    }

    for( int i = 0 ; i < n ; i++ )
        vDestroy( fnormal[i] );
    free( fnormal );

    return flag;

}

float occlusion( SURFACE_3D i, SURFACE_3D j, int isn, int jsn ){

    double visibility = 1.0;

    // For pass1
    VEC aTob;
    VEC bToa;
    double cosA, cosB;

    // For pass2
    VEC *ray;
    int im, ip, iface;
    double r;

    // Pass 1
    aTob = vectorPP( i.center, j.center );
    bToa = vectorPP( j.center, i.center );

    cosA = vCos( i.normal, aTob );
    cosB = vCos( j.normal, bToa );

    if( cosA < 0.000001 || cosB < 0.000001 )
        visibility = 0.0;

    // Pass 2
    if( visibility > 0.0 ){

        ray = (VEC*) malloc( sizeof( VEC ) * ( i.n_point > j.n_point ? i.n_point : j.n_point ) );

        // the center point of i to the terminal points of j
        for( int n = 0 ; n < j.n_point ; n++ ){
            ray[n] = vCreate( 3 );
            r = lengthPP( i.center, j.plist[n] );
            ray[n].vector[0] = ( j.plist[n].x - i.center.x ) / r;
            ray[n].vector[1] = ( j.plist[n].y - i.center.y ) / r;
            ray[n].vector[2] = ( j.plist[n].z - i.center.z ) / r;
        }

        for( int n = 0 ; n < scene.n_face ; n++ ){

            searchSceneSurface( scene, n, &im, &ip, &iface );

            if( n == isn || n == jsn )
                continue;
/*
            if( !betweenCheck( i, j, scene.list[im].plist[ip].flist[iface] ) )
                continue;
*/
            if( checkOcclusion( j.n_point, ray, scene.list[im].plist[ip].flist[iface] ) ){
                visibility = 0.0;
                break;
            }

        }

        for( int n = 0 ; n < j.n_point ; n++ )
            vDestroy( ray[n] );
        free( ray );

    }

    vDestroy( aTob );
    vDestroy( bToa );

    return visibility;

}
