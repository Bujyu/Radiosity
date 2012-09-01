#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <cmath>

#include "geometric.h"

// Point
POINT_3D addPoint3D( double x, double y, double z ){

    POINT_3D pt;

    pt.x = x;
    pt.y = y;
    pt.z = z;

    return pt;

}

VEC vectorPP( const POINT_3D st, const POINT_3D ed ){

    VEC v = vCreate( 3 );

    v.vector[0] = ed.x - st.x;
    v.vector[1] = ed.y - st.y;
    v.vector[2] = ed.z - st.z;

    return v;

}

POINT_3D centerPP( POINT_3D a, POINT_3D b ){

    POINT_3D center = addPoint3D( ( a.x + b.x ) / 2, ( a.y + b.y ) / 2, ( a.z + b.z ) / 2 );
    return center;

}

double lengthPP( POINT_3D a, POINT_3D b ){
    return sqrt( pow( b.x - a.x, 2 ) + pow( b.y - a.y, 2 ) + pow( b.z - a.z, 2 ) );
}

void ptPrint( POINT_3D p ){
    printf("[%.4lf %.4lf %.4lf]\n", p.x, p.y, p.z );
}

// Surface
SURFACE_3D addSurface3D( int amount, ... ){

    int i;
    SURFACE_3D face;

    va_list val;
    va_start( val, amount );

    face.plist = ( POINT_3D* ) malloc( sizeof( POINT_3D ) * amount );
    face.n_point = amount;
    face.FF = 0.0;
    face.visited = 0;

    for( i = 0 ; i < amount ; i++ )
        face.plist[i] = va_arg( val, POINT_3D );

    va_end( val );

    return face;

}

void setSurface3DNormal( SURFACE_3D *face, double x, double y, double z ){

    (*face).normal = vCreate( 3 );

    (*face).normal.vector[0] = x;
    (*face).normal.vector[1] = y;
    (*face).normal.vector[2] = z;

}

POINT_3D surfaceCenter( const SURFACE_3D &face ){

    int n;
    POINT_3D center = addPoint3D( 0, 0, 0 );

    for( n = 0 ; n < face.n_point ; n++ ){
        center.x += face.plist[n].x;
        center.y += face.plist[n].y;
        center.z += face.plist[n].z;
    }

    center.x /= face.n_point;
    center.y /= face.n_point;
    center.z /= face.n_point;

    return center;

}

void interpolationSqr( POINT_3D *ipt, const SURFACE_3D &face, double u, double v ){

    (*ipt).x = ( 1.0 - u ) * ( 1.0 - v ) * face.plist[0].x +
            ( 1.0 - u ) * v * face.plist[1].x +
            u * v * face.plist[2].x +
            u * ( 1.0 - v ) * face.plist[3].x;

    (*ipt).y = ( 1.0 - u ) * ( 1.0 - v ) * face.plist[0].y +
            ( 1.0 - u ) * v * face.plist[1].y +
            u * v * face.plist[2].y +
            u * ( 1.0 - v ) * face.plist[3].y;

    (*ipt).z = ( 1.0 - u ) * ( 1.0 - v ) * face.plist[0].z +
            ( 1.0 - u ) * v * face.plist[1].z +
            u * v * face.plist[2].z +
            u * ( 1.0 - v ) * face.plist[3].z;

}

void interpolationTri( POINT_3D *ipt, const SURFACE_3D &face, double u, double v ){

    int n = 0;
    double a = lengthPP( face.plist[1], face.plist[1] );

    if( a < lengthPP( face.plist[0], face.plist[1] ) )
        n = 2;
    if( a < lengthPP( face.plist[0], face.plist[2] ) )
        n = 1;

    (*ipt).x = ( 1.0 - u - v ) * face.plist[n].x + u * face.plist[(n+1)%3].x + v * face.plist[(n+2)%3].x;
    (*ipt).y = ( 1.0 - u - v ) * face.plist[n].y + u * face.plist[(n+1)%3].y + v * face.plist[(n+2)%3].y;
    (*ipt).z = ( 1.0 - u - v ) * face.plist[n].z + u * face.plist[(n+1)%3].z + v * face.plist[(n+2)%3].z;

}

void interpolation( POINT_3D *ipt, const SURFACE_3D &face, double u, double v ){

    switch( face.n_point ){
        case 3:
            interpolationTri( ipt, face, u, v );
            break;
        case 4:
            interpolationSqr( ipt, face, u, v );
            break;
        default:
            break;
    }

}

double triangleArea( SURFACE_3D face ){

    return -1.0;

}

double squareArea( SURFACE_3D face ){

    double s;
    double a, b, c, d;
    double angleB, angleD;
    double cos_halfBD;

    a = lengthPP( face.plist[0], face.plist[1] );
    b = lengthPP( face.plist[1], face.plist[2] );
    c = lengthPP( face.plist[2], face.plist[3] );
    d = lengthPP( face.plist[3], face.plist[0] );

    s = ( a + b + c + d ) / 2;

    angleB = acos( vCos( vectorPP( face.plist[1], face.plist[0] ), vectorPP( face.plist[1], face.plist[2] ) ) );
    angleD = acos( vCos( vectorPP( face.plist[3], face.plist[2] ), vectorPP( face.plist[3], face.plist[0] ) ) );
    cos_halfBD = cos( ( angleB + angleD ) / 2 );

    return sqrt( ( ( s - a ) * ( s - b ) * ( s - c ) * ( s - d ) ) - ( a * b * c * d * cos_halfBD * cos_halfBD ) );

}

double surfaceArea( SURFACE_3D face ){

    switch( face.n_point ){
        case 3:
            return triangleArea( face );
        case 4:
            return squareArea( face );
        default:
            return -1.0;
    }

}

// Patch
PATCH createPatch(){

    PATCH p;

    p.n_face = 0;
    memset( p.emission, 0, sizeof(double) * 3 );
    memset( p.reflection, 0, sizeof(double) * 3 );

    return p;

}

void addPatch( PATCH *patch, SURFACE_3D face ){

    (*patch).flist.push_back( face );
    (*patch).n_face++;

}

void setEmission( PATCH *patch, double r, double g, double b ){
    (*patch).emission[0] = r;
    (*patch).emission[1] = g;
    (*patch).emission[2] = b;
}

void setReflection( PATCH *patch, double r, double g, double b ){
    (*patch).reflection[0] = r;
    (*patch).reflection[1] = g;
    (*patch).reflection[2] = b;
}

void drawPatch( const PATCH &patch, float color[4] ){

    for( int i = 0 ; i < patch.n_face ; i++ ){
        glColor4fv( color );
        //glBegin( GL_LINE_LOOP );
        glPolygonMode( GL_BACK, GL_LINE );
        glBegin( GL_POLYGON );
            for( int j = 0 ; j < patch.flist[i].n_point ; j++  ){
                glVertex3fp( patch.flist[i].plist[j] );
            }
        glEnd();
    }

}

void clipQuadSurface( PATCH *patch ){

    std::vector<SURFACE_3D> fclone = (*patch).flist;

    POINT_3D center;
    SURFACE_3D face;

    (*patch).n_face = 0;
    (*patch).flist.clear();

    for( int i = 0 ; i < (int) fclone.size() ; i++ ){

        center = surfaceCenter( fclone[i] );

        for( int j = 0 ; j < fclone[i].n_point ; j++ ){

            face = addSurface3D( 4,
                                 fclone[i].plist[j], centerPP( fclone[i].plist[j], fclone[i].plist[(j+1)%fclone[i].n_point] ),
                                 center, centerPP( fclone[i].plist[(j+3)%fclone[i].n_point], fclone[i].plist[j] ) );
            face.normal = fclone[i].normal;
            //setSurface3DNormal( &face, fclone[i].normal[0], fclone[i].normal[1], fclone[i].normal[2] );
            addPatch( patch, face );

        }

    }

    fclone.clear();

}

// SCENE
SCENE createScene(){

    SCENE s;

    s.n_face = 0;
    s.n_patch = 0;

    return s;

}

void addScene( SCENE *scene, PATCH patch ){

    (*scene).list.push_back( patch );
    (*scene).n_patch++;
    (*scene).n_face += patch.n_face;

}

int searchScene( const SCENE &scene, int count, int *p, int *f ){

    if( count > scene.n_face )
        return 0;

    for( int i = 0 ; i < scene.n_patch ; i++ ){
        if( count < scene.list[i].n_face ){
            *p = i;
            *f = count;
            return 1;
        }
        else
            count -= scene.list[i].n_face;
    }

    return 0;

}

/// Point of intersection
double clap( double num, double mboundary, double pboundary ){
    return ( num > pboundary ) ? pboundary : ( num < mboundary ) ? mboundary : num;
}

int vIntersection( VEC v, int plane, POINT_3D *ipt ){

    float t;

    // Top
    if( plane == 0 ){

        if( v.vector[1] < 0.707 )
            return 0;

        t = 1.0 / v.vector[1];

        (*ipt).x = v.vector[0] * t;
        (*ipt).y = 1.0;
        (*ipt).z = v.vector[2] * t;

        return 1;

    }
    else if( plane == 1 || plane == 3 ){

        if( fabs( v.vector[2] ) < 0.707 )
            return 0;

        t = 1.0 / v.vector[2];

        if( t < 0.0 )
            return 0;

        (*ipt).x = v.vector[0] * t;
        (*ipt).y = v.vector[1] * t;
        (*ipt).z = plane == 1 ? -1.0 : 1.0;

        return 1;
    }
    else if( plane == 2 || plane == 4 ){

        if( fabs( v.vector[0] ) < 0.707 )
            return 0;

        t = 1.0 / v.vector[0];

        if( t < 0.0 )
            return 0;

        (*ipt).x = plane == 2 ? -1.0 : 1.0;
        (*ipt).y = v.vector[1] * t;
        (*ipt).z = v.vector[2] * t;

        return 1;
    }

    return 0;

}

/*
int main(){

    POINT_3D a, b, c, d;
    SURFACE_3D f;
    PATCH p;

    a = addPoint3D( 0, 0, 0 );
    b = addPoint3D( 10, 0, 0 );
    c = addPoint3D( 10, 10, 0 );
    d = addPoint3D( 0, 10, 0 );

    f = addSurface3D( 4, a, b, c, d );
    printf( "%lf\n", surfaceArea( f ) );

    p = createPatch();
    addPatch( p, f );


    return 0;

}
*/

