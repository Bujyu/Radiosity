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

    // Vertex rad
    pt.rad[0] = 0.0;
    pt.rad[1] = 0.0;
    pt.rad[2] = 0.0;
    pt.count = 0;

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

double areaTri( POINT_3D pa, POINT_3D pb, POINT_3D pc ){

    double a, b, c;
    double s;

    a = lengthPP( pa, pb );
    b = lengthPP( pb, pc );
    c = lengthPP( pc, pa );
    s = ( a + b + c ) / 2;

    return sqrt( s * ( s - a ) * ( s - b ) * ( s - c ) );

}

double lengthPP( POINT_3D a, POINT_3D b ){
    return sqrt( pow( b.x - a.x, 2 ) + pow( b.y - a.y, 2 ) + pow( b.z - a.z, 2 ) );
}

void ptPrint( POINT_3D p ){
    printf("[%.4f %.4f %.4f]\n", p.x, p.y, p.z );
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

    // Refine Clip use only
    face.ne = NULL;
    face.nw = NULL;
    face.se = NULL;
    face.sw = NULL;

    for( i = 0 ; i < amount ; i++ )
        face.plist[i] = va_arg( val, POINT_3D );

    face.center = surfaceCenter( face );

    va_end( val );

    return face;

}

void addSurface3DNode( SURFACE_3D *face, int amount, ... ){

    int i;

    va_list val;
    va_start( val, amount );

    (*face).plist = ( POINT_3D* ) malloc( sizeof( POINT_3D ) * amount );
    (*face).n_point = amount;
    (*face).FF = 0.0;
    (*face).visited = 0;

    // Refine Clip use only
    (*face).ne = NULL;
    (*face).nw = NULL;
    (*face).se = NULL;
    (*face).sw = NULL;

    for( i = 0 ; i < amount ; i++ )
        (*face).plist[i] = va_arg( val, POINT_3D );

    (*face).center = surfaceCenter( (*face) );

    va_end( val );

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
    double a = lengthPP( face.plist[1], face.plist[2] );

    if( a < lengthPP( face.plist[0], face.plist[1] ) )
        n = 2;
    if( a < lengthPP( face.plist[0], face.plist[2] ) )
        n = 1;

    (*ipt).x = ( 1.0 - u ) * ( 1.0 - v ) * face.plist[n].x +
               ( 1.0 - u ) * v * face.plist[(n+1)%3].x +
               u * ( 1.0 - v ) * face.plist[(n+2)%3].x;

    (*ipt).y = ( 1.0 - u ) * ( 1.0 - v ) * face.plist[0].y +
               ( 1.0 - u ) * v * face.plist[(n+1)%3].y +
               u * ( 1.0 - v ) * face.plist[(n+2)%3].y;

    (*ipt).z = ( 1.0 - u ) * ( 1.0 - v ) * face.plist[0].z +
               ( 1.0 - u ) * v * face.plist[(n+1)%3].z +
               u * ( 1.0 - v ) * face.plist[(n+2)%3].z;

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

    double a, b, c;
    double s;

    a = lengthPP( face.plist[0], face.plist[1] );
    b = lengthPP( face.plist[1], face.plist[2] );
    c = lengthPP( face.plist[2], face.plist[0] );
    s = ( a + b + c ) / 2.0;

    return sqrt( s * ( s - a ) * ( s - b ) * ( s - c ) );

}

double triangleAreaPP( double a, double b, double c ){

    double s;
    s = ( a + b + c ) / 2.0;

    return sqrt( s * ( s - a ) * ( s - b ) * ( s - c ) );

}

double squareArea( SURFACE_3D face ){

    double s;
    double a, b, c, d;
    double angleB, angleD;
    double cos_halfBD;

    VEC vBA, vBC, vDC, vDA;

    a = lengthPP( face.plist[0], face.plist[1] );
    b = lengthPP( face.plist[1], face.plist[2] );
    c = lengthPP( face.plist[2], face.plist[3] );
    d = lengthPP( face.plist[3], face.plist[0] );

    s = ( a + b + c + d ) / 2;

    vBA = vectorPP( face.plist[1], face.plist[0] );
    vBC = vectorPP( face.plist[1], face.plist[2] );
    vDC = vectorPP( face.plist[3], face.plist[2] );
    vDA = vectorPP( face.plist[3], face.plist[0] );

    angleB = acos( vCos( vBA, vBC ) );
    angleD = acos( vCos( vDC, vDA ) );
    cos_halfBD = cos( ( angleB + angleD ) / 2 );

    vDestroy( vBA );
    vDestroy( vBC );
    vDestroy( vDC );
    vDestroy( vDA );

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

    return p;

}

void addPatch( PATCH *patch, SURFACE_3D face ){

    (*patch).flist.push_back( face );
    (*patch).n_face++;

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

            face = fclone[i].n_point != 3 ?
                   addSurface3D( 4,
                                 fclone[i].plist[j], centerPP( fclone[i].plist[j], fclone[i].plist[(j+1)%fclone[i].n_point] ),
                                 center, centerPP( fclone[i].plist[(j+3)%fclone[i].n_point], fclone[i].plist[j] ) ) :
                   addSurface3D( 3,
                                 fclone[i].plist[j],
                                 centerPP( fclone[i].plist[j], fclone[i].plist[(j+1)%fclone[i].n_point] ),
                                 centerPP( fclone[i].plist[(j+2)%fclone[i].n_point], fclone[i].plist[j] ) );
            face.normal = fclone[i].normal;
            //setSurface3DNormal( &face, fclone[i].normal[0], fclone[i].normal[1], fclone[i].normal[2] );
            addPatch( patch, face );

        }

        if( fclone[i].n_point == 3 ){
            face = addSurface3D( 3, centerPP( fclone[i].plist[0], fclone[i].plist[1] ),
                                    centerPP( fclone[i].plist[1], fclone[i].plist[2] ),
                                    centerPP( fclone[i].plist[0], fclone[i].plist[2] ) );
            face.normal = fclone[i].normal;
            //setSurface3DNormal( &face, fclone[i].normal[0], fclone[i].normal[1], fclone[i].normal[2] );
            addPatch( patch, face );
        }

    }

    fclone.clear();

}

int subdiv( SURFACE_3D *f, double Aeps ){

    if( ( surfaceArea( *f ) / 4 ) < Aeps )
        return 0;

    if( f->ne != NULL )
        return 1;

    f->ne = (SURFACE_3D*) malloc( sizeof( SURFACE_3D ) );
    f->nw = (SURFACE_3D*) malloc( sizeof( SURFACE_3D ) );
    f->se = (SURFACE_3D*) malloc( sizeof( SURFACE_3D ) );
    f->sw = (SURFACE_3D*) malloc( sizeof( SURFACE_3D ) );

    // Only for square
    for( int i = 0 ; i < (int) (*f).n_point ; i++ ){
        switch( i ){
            case 0:
                addSurface3DNode( f->ne, 4,
                                 (*f).plist[i], centerPP( (*f).plist[i], (*f).plist[(i+1)%(*f).n_point] ),
                                 (*f).center, centerPP( (*f).plist[(i+3)%(*f).n_point], (*f).plist[i] ) );
                f->ne->normal = (*f).normal;
                break;
            case 1:
                addSurface3DNode( f->nw, 4,
                                 (*f).plist[i], centerPP( (*f).plist[i], (*f).plist[(i+1)%(*f).n_point] ),
                                 (*f).center, centerPP( (*f).plist[(i+3)%(*f).n_point], (*f).plist[i] ) );
                f->nw->normal = (*f).normal;
                break;
            case 2:
                addSurface3DNode( f->se, 4,
                                 (*f).plist[i], centerPP( (*f).plist[i], (*f).plist[(i+1)%(*f).n_point] ),
                                 (*f).center, centerPP( (*f).plist[(i+3)%(*f).n_point], (*f).plist[i] ) );
                f->se->normal = (*f).normal;
                break;
            case 3:
                addSurface3DNode( f->sw, 4,
                                 (*f).plist[i], centerPP( (*f).plist[i], (*f).plist[(i+1)%(*f).n_point] ),
                                 (*f).center, centerPP( (*f).plist[(i+3)%(*f).n_point], (*f).plist[i] ) );
                f->sw->normal = (*f).normal;
                break;
        }
    }

    return 1;

}

double FormFactorEstimate( SURFACE_3D *i,  SURFACE_3D *j ){
    return pow( sqrt( surfaceArea( (*j) ) / M_PI ) / lengthPP( i->center, j->center ), 2 );
}

void refineClip( SURFACE_3D *i, SURFACE_3D *j, double Aeps, double Feps ){

    double Fij, Fji;

    Fij = FormFactorEstimate( i, j );
    Fji = FormFactorEstimate( j, i );

    if( Fij < Feps && Fji < Feps )
        return;
    else{
        if( Fij > Fji ){
            if( subdiv( j, Aeps ) ){
                refineClip( i, j->ne, Aeps, Feps );
                refineClip( i, j->nw, Aeps, Feps );
                refineClip( i, j->se, Aeps, Feps );
                refineClip( i, j->sw, Aeps, Feps );
            }
            else
                return;
        }
        else{
            if( subdiv( i, Aeps ) ){
                refineClip( j, i->ne, Aeps, Feps );
                refineClip( j, i->nw, Aeps, Feps );
                refineClip( j, i->se, Aeps, Feps );
                refineClip( j, i->sw, Aeps, Feps );
            }
            else
                return;
        }
    }

}

std::vector<SURFACE_3D> fcolect;
void travelNode( SURFACE_3D *face ){

    if( !face->ne && !face->nw && !face->se  && !face->sw  )
        fcolect.push_back( (*face) );
    else{
        travelNode( face->ne );
        travelNode( face->nw );
        travelNode( face->se );
        travelNode( face->sw );
    }

}

void delNode( SURFACE_3D *face ){

    if( !face->ne && !face->nw && !face->se && !face->sw )
        free( face );
    else{
        delNode( face->ne );
        delNode( face->nw );
        delNode( face->se );
        delNode( face->sw );
    }

}

// Model
MODEL createModel(){

    MODEL m;

    m.n_face = 0;
    m.n_patch = 0;

    memset( m.emission, 0, sizeof(double) * 3 );
    memset( m.reflection, 0, sizeof(double) * 3 );

    return m;

}

void addModel( MODEL *model, PATCH patch ){

    (*model).plist.push_back( patch );
    (*model).n_patch++;
    (*model).n_face += patch.n_face;

}

void setEmission( MODEL *model, double r, double g, double b ){
    (*model).emission[0] = r;
    (*model).emission[1] = g;
    (*model).emission[2] = b;
}

void setReflection( MODEL *model, double r, double g, double b ){
    (*model).reflection[0] = r;
    (*model).reflection[1] = g;
    (*model).reflection[2] = b;
}

void clipPatch( MODEL *model ){

    (*model).n_face = 0;
    for( int i = 0 ; i < (*model).n_patch ; i++ ){
        clipQuadSurface( &((*model).plist[i]) );
        (*model).n_face += (*model).plist[i].n_face;
    }

}

// SCENE
SCENE createScene(){

    SCENE s;

    s.n_face = 0;
    s.n_patch = 0;
    s.n_model = 0;

    return s;

}

void addScene( SCENE *scene, MODEL model ){

    (*scene).list.push_back( model );
    (*scene).n_model++;
    (*scene).n_face += model.n_face;
    (*scene).n_patch += model.n_patch;

}

int searchSceneSurface( const SCENE &scene, int count, int *m, int *p, int *f ){

    if( count > scene.n_face )
        return 0;

    for( int i = 0 ; i < scene.n_model ; i++ ){
        for( int j = 0 ; j < scene.list[i].n_patch ; j++ ){
            if( count < scene.list[i].plist[j].n_face ){
                *m = i;
                *p = j;
                *f = count;
                return 1;
            }
            else
                count -= scene.list[i].plist[j].n_face;
        }
    }

    return 0;

}

int searchScenePatch( const SCENE &scene, int count, int *m, int *p ){

    if( count > scene.n_patch )
        return 0;

    for( int i = 0 ; i < scene.n_model ; i++ ){
        if( count < scene.list[i].n_patch ){
            *m = i;
            *p = count;
            return 1;
        }
        else
            count -= scene.list[i].n_patch;
    }

    return 0;

}

int searchSceneModel( const SCENE &scene, int count, int *m ){

    if( count > scene.n_model )
        return 0;

    *m = count;

    return 1;

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

