#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

#include <iostream>
#include <vector>
#include <ctime>

#include <windows.h>
#include <gl/gluit.h>
#include <gl.hh>

typedef OpenMesh::TriMesh_ArrayKernelT<>  Mesh;

Mesh mesh;
Mesh mesh2;
int gw, gh;

//Main Func
int mainwin;
int viewer;

std::vector<Mesh::FaceHandle>  fhandles;


void readMesh( Mesh *_mesh, char* path, float center[3], float radius ){

    OpenMesh::IO::read_mesh( (*_mesh), path, false );

    Mesh::ConstVertexIter	v_it( (*_mesh).vertices_begin() ),
                            v_end( (*_mesh).vertices_end() );

	Mesh::Point             bbMin, bbMax;

	bbMin = bbMax = (*_mesh).point(v_it);  // accessing the Point of a vertex
	for (; v_it!=v_end; ++v_it){
		bbMin.minimize((*_mesh).point(v_it));
		bbMax.maximize((*_mesh).point(v_it));
	}

	for (int i = 0; i < 3; i++)
		center[i] = (bbMin[i] +bbMax[i])/2;
	radius = (bbMax-bbMin).norm()/2;  // set radius as half of bbox diagonal

	(*_mesh).request_face_normals();
	(*_mesh).update_normals();

}

void edgeSplit( Mesh *_mesh, int eidx ){

    Mesh::EdgeHandle eh = (*_mesh).edge_handle( eidx );
    Mesh::VertexHandle p1 = (*_mesh).from_vertex_handle( (*_mesh).halfedge_handle( eh, 0 ) );
    Mesh::VertexHandle p2 = (*_mesh).to_vertex_handle( (*_mesh).halfedge_handle( eh, 0 ) );
    Mesh::VertexHandle cv;
    Mesh::Point center;

    center = (*_mesh).point( p1 ) + (*_mesh).point(  p2 );
    center[0] /= 2;
    center[1] /= 2;
    center[2] /= 2;

    cv = (*_mesh).new_vertex( center );
    //(*_mesh).new_edge( p1, cv );

    (*_mesh).set_vertex_handle( (*_mesh).halfedge_handle( eh, 0 ), cv );
    (*_mesh).new_edge( cv, p2 );

}

void edgeMeshSplit( Mesh *_mesh ){

    Mesh::ConstVertexIter       v_it( (*_mesh).vertices_begin() ),
                                v_end( (*_mesh).vertices_end() );
    Mesh::ConstVertexEdgeIter   ve_it;
    Mesh::ConstEdgeIter         e_it( (*_mesh).edges_begin() ),
                                e_end( (*_mesh).edges_end() );
    std::vector<Mesh::VertexHandle>  tmp_face_vhandles;
/*
    for( ; f_it != f_end ; ++f_it )
        (*_mesh).delete_face( f_it.handle(), false );
     (*_mesh).garbage_collection();
*/
    for( ; e_it != e_end ; ++e_it )
        edgeSplit( _mesh, e_it.handle().idx() );

    for( ; v_it != v_end ; ++v_it ){

        tmp_face_vhandles.clear();
        ve_it = (*_mesh).cve_iter( v_it.handle() );
        tmp_face_vhandles.push_back( v_it.handle() );
        tmp_face_vhandles.push_back( (*_mesh).to_vertex_handle( (*_mesh).halfedge_handle( ve_it.handle(), 0 ) ) );
        ++ve_it;
        tmp_face_vhandles.push_back( (*_mesh).to_vertex_handle( (*_mesh).halfedge_handle( ve_it.handle(), 0 ) ) );
        fhandles.push_back( (*_mesh).add_face( tmp_face_vhandles ) );

    }

}

void checkEdge( Mesh *_mesh ){

    Mesh::ConstEdgeIter         e_it( (*_mesh).edges_begin() ),
                                e_end( (*_mesh).edges_end() );
    bool flag = true;


    for( ; e_it != e_end && flag ; ++e_it ){

        if( (*_mesh).point( (*_mesh).to_vertex_handle( (*_mesh).halfedge_handle( e_it, 0 ) ) ) != (*_mesh).point( (*_mesh).from_vertex_handle( (*_mesh).halfedge_handle( e_it, 1 ) ) ) ||
            (*_mesh).point( (*_mesh).from_vertex_handle( (*_mesh).halfedge_handle( e_it, 0 ) ) ) != (*_mesh).point( (*_mesh).to_vertex_handle( (*_mesh).halfedge_handle( e_it, 1 ) ) ) )
            flag = false;

    }

    std::cout << flag << std::endl;

}

//2 to 4
void triMeshSplit( Mesh *_mesh ){

    Mesh::ConstEdgeIter         e_it( (*_mesh).edges_begin() ),
                                e_end( (*_mesh).edges_end() );
    Mesh::Point                 center;

    for( ; e_it != e_end ; ++e_it ){

        center[0] = 0;
        center[2] = 0;
        center[1] = 0;

        center += (*_mesh).point( (*_mesh).to_vertex_handle( (*_mesh).halfedge_handle( e_it.handle(), 0 ) ) );
        center += (*_mesh).point( (*_mesh).to_vertex_handle( (*_mesh).halfedge_handle( e_it.handle(), 1 ) ) );

        center[0] /= 2;
        center[1] /= 2;
        center[2] /= 2;

        (*_mesh).split( e_it.handle(), center );

    }

}

void meshSplit( Mesh *_mesh ){

    static int visited = 0;

    std::cout << ++visited << std::endl;

    Mesh::ConstFaceIter	        f_it( (*_mesh).faces_begin() ),
                                f_end( (*_mesh).faces_end() );
    Mesh::ConstFaceVertexIter	fv_it;

    Mesh::Point                 center;

    for( ; f_it != f_end ; ++f_it ){

        center[0] = 0;
        center[1] = 0;
        center[2] = 0;

        fv_it = (*_mesh).cfv_iter( f_it.handle() );
        center += (*_mesh).point( fv_it.handle() );
        ++fv_it;
        center += (*_mesh).point( fv_it.handle() );
        ++fv_it;
        center += (*_mesh).point( fv_it.handle() );

        center[0] /= 3;
        center[1] /= 3;
        center[2] /= 3;

        (*_mesh).split( f_it.handle(), center );

    }

    //Face normal reupdate
    (*_mesh).request_face_normals();
	(*_mesh).update_normals();

}

void drawMeshAnime( Mesh _mesh ){

    Mesh::ConstFaceIter         f_it( _mesh.faces_begin() ),
                                f_end( _mesh.faces_end() );
    Mesh::ConstFaceVertexIter   fv_it;

    //glBegin( GL_TRIANGLES );
    glColor3f( 1.0, 0.0, 0.0 );

    for( ; f_it != f_end ; ++f_it ){
        glBegin( GL_TRIANGLES );
            fv_it = _mesh.cfv_iter( f_it.handle() );
            GL::glVertex( _mesh.point( fv_it ) );
            ++fv_it;
            GL::glVertex( _mesh.point( fv_it ) );
            ++fv_it;
            GL::glVertex( _mesh.point( fv_it ) );
        glEnd();
        glFlush();
        int i = 0;
        while( i < 10000000 )
            i++;
    }
    //glEnd();

}

void drawMesh( Mesh _mesh ){

    Mesh::ConstEdgeIter     e_it( _mesh.edges_begin() ),
                            e_end( _mesh.edges_end() );

    //checkEdge( _mesh );
    //std::cout << _mesh.n_vertices() << std::endl;
    //std::cout << _mesh.n_edges() << std::endl;
    //std::cout << _mesh.n_halfedges() << std::endl;

    glColor3f( 1.0, 1.0, 1.0 );
    for( ; e_it != e_end ; ++e_it ){

        glBegin( GL_LINES );
            GL::glVertex( _mesh.point( _mesh.to_vertex_handle( _mesh.halfedge_handle( e_it.handle(), 0 ) ) ) );
            GL::glVertex( _mesh.point( _mesh.to_vertex_handle( _mesh.halfedge_handle( e_it.handle(), 1 ) ) ) );
        glEnd();
    }

}

/*
void drawMesh(){

    Mesh::ConstFaceIter         f_it( _mesh.faces_begin() ),
                                f_end( _mesh.faces_end() );
    Mesh::ConstFaceVertexIter   fv_it;

    glColor3f( 1.0, 1.0, 1.0 );
    //glBegin( GL_TRIANGLES );
    for( ; f_it != f_end ; ++f_it ){
        glBegin( GL_LINE_LOOP );
        fv_it = _mesh.cfv_iter( f_it.handle() );
        GL::glVertex( _mesh.point( fv_it ) );
        ++fv_it;
        GL::glVertex( _mesh.point( fv_it ) );
        ++fv_it;
        GL::glVertex( _mesh.point( fv_it ) );
        glEnd();
    }
    //glEnd();
    //glFlush();

}

void display( void ){

    glClear( GL_COLOR_BUFFER_BIT );

    drawMesh();
    drawMeshAnime();

}

void reshape( int w, int h ){

    gw = w;
    gh = h;

}
*/
void keyboard( unsigned char key, int x, int y ){

    switch( key ){
        case 's':
        case 'S':
            meshSplit( &mesh );
            meshSplit( &mesh2 );
            break;
        case 'e':
        case 'E':
            edgeMeshSplit( &mesh );
            edgeMeshSplit( &mesh2 );
            break;
        default:
            break;
    }

    //glClear( GL_COLOR_BUFFER_BIT );
    glutPostRedisplay();

}
/*
void init( void ){

    float center[3], radius = 0.0;

    readMesh( (char*) "Models/mushroom.off", center, radius );

    glClearColor( 0.0, 0.0, 0.0, 1.0 );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( -1.0, 1.0, -1.0, 1.0, -1.0, 1.0 );

}

int main( int argc, char **argv ){

    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_SINGLE | GLUT_RGBA );
    glutInitWindowSize( 600, 600 );
    glutCreateWindow( "TEST" );

    init();

    glutDisplayFunc( display );
    glutReshapeFunc( reshape );
    glutKeyboardFunc( keyboard );
    glutMainLoop();

    return 0;

}
*/

void axis(){

    glPushAttrib( GL_ALL_ATTRIB_BITS );

    glLineWidth( 3.0 );

    glColor3f( 1.0, 0.0, 0.0 );
    glBegin( GL_LINES );
        glVertex3f( 0.0, 0.0, 0.0 );
        glVertex3f( 0.1, 0.0, 0.0 );
    glEnd();

    glColor3f( 0.0, 1.0, 0.0 );
    glBegin( GL_LINES );
        glVertex3f( 0.0, 0.0, 0.0 );
        glVertex3f( 0.0, 0.1, 0.0 );
    glEnd();

    glColor3f( 0.0, 0.0, 1.0 );
    glBegin( GL_LINES );
        glVertex3f( 0.0, 0.0, 0.0 );
        glVertex3f( 0.0, 0.0, 0.1 );
    glEnd();

    glPopAttrib();

}

void interpolation( double p0[], double p1[], double p2[], double u, double v ){

    double ipt[3];

    ipt[0] = ( 1.0 - u - v ) * p0[0] + u * p1[0] + v * p2[0];
    ipt[1] = ( 1.0 - u - v ) * p0[1] + u * p1[1] + v * p2[1];
    ipt[2] = ( 1.0 - u - v ) * p0[2] + u * p1[2] + v * p2[2];

    glBegin( GL_POINTS );
        glVertex3f( ipt[0], ipt[1], ipt[2] );
    glEnd();

}

double len( double p0[], double p1[] ){

    return sqrt( pow( p1[0] - p0[0], 2 ) + pow( p1[1] - p0[1], 2 ) + pow( p1[2] - p0[2], 2 ) );


}

void init(){

    float center[3], radius = 0.0;

    //Depth
    glEnable( GL_DEPTH_TEST );

    //readMesh( &mesh, (char*) "Models/five-face.off", center, radius );
    //readMesh( &mesh2, (char*) "Models/mushroom.off", center, radius );

    //Initial color
    glClearColor( 0.0, 0.0, 0.0, 1.0 );

}

void reshape( int w, int h ){

	glViewport( 0, 0, w, h );
	ViewerReshape( viewer );

}

void display(){}

void content( void ){

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
/*
    drawMesh( mesh );
    drawMesh( mesh2 );
*/
/*
    int n_cells = 8;

    double cellSize = (double) 2 / n_cells;
    double halfSize = n_cells / 2;
    int i, j, k;

    //Top
    for( i = -halfSize ; i < halfSize ; i++ ){
        for( j = -halfSize ; j < halfSize ; j++ ){

            glPolygonMode( GL_BACK, GL_LINE );
            glBegin( GL_POLYGON );
            glVertex3f( (double)i * cellSize, halfSize * cellSize, (double)j * cellSize );
            glVertex3f( (double)i * cellSize, halfSize * cellSize, (double)( j + 1 ) * cellSize );
            glVertex3f( (double)( i + 1 ) * cellSize, halfSize * cellSize, (double)( j + 1 ) * cellSize);
            glVertex3f( (double)( i + 1 ) * cellSize, halfSize * cellSize, (double)j * cellSize );
            glEnd();

        }
    }

    //Front and Back
    for( i = -halfSize ; i < halfSize ; i++ ){
        for( j = 0; j < halfSize ; j++ ){

            glBegin( GL_POLYGON );
            glVertex3f( (double)( i + 1 ) * cellSize, (double)j * cellSize        , (double)halfSize * cellSize );
            glVertex3f( (double)( i + 1 ) * cellSize, (double)( j + 1 ) * cellSize, (double)halfSize * cellSize );
            glVertex3f( (double)        i * cellSize, (double)( j + 1 ) * cellSize, (double)halfSize * cellSize );
            glVertex3f( (double)        i * cellSize, (double)j * cellSize        , (double)halfSize * cellSize );
            glEnd();

            glBegin( GL_POLYGON );
            glVertex3f( (double)        i * cellSize, (double)j * cellSize        , (double)-halfSize * cellSize );
            glVertex3f( (double)        i * cellSize, (double)( j + 1 ) * cellSize, (double)-halfSize * cellSize );
            glVertex3f( (double)( i + 1 ) * cellSize, (double)( j + 1 ) * cellSize, (double)-halfSize * cellSize );
            glVertex3f( (double)( i + 1 ) * cellSize, (double)j * cellSize        , (double)-halfSize * cellSize );
            glEnd();

        }
    }

    // Right and Left
    for( i = 0; i < halfSize ; i++ ){
        for( j = -halfSize; j < halfSize ; j++ ){

            glBegin( GL_POLYGON );
            glVertex3f( (double)halfSize * cellSize, (double)( i + 1 ) * cellSize, (double)j * cellSize         );
            glVertex3f( (double)halfSize * cellSize, (double)( i + 1 ) * cellSize, (double)( j + 1 ) * cellSize );
            glVertex3f( (double)halfSize * cellSize, (double)        i * cellSize, (double)( j + 1 ) * cellSize );
            glVertex3f( (double)halfSize * cellSize, (double)        i * cellSize, (double)j * cellSize         );
            glEnd();

            glBegin( GL_POLYGON );
            glVertex3f( (double)-halfSize * cellSize, (double)        i * cellSize, (double)j * cellSize         );
            glVertex3f( (double)-halfSize * cellSize, (double)        i * cellSize, (double)( j + 1 ) * cellSize );
            glVertex3f( (double)-halfSize * cellSize, (double)( i + 1 ) * cellSize, (double)( j + 1 ) * cellSize );
            glVertex3f( (double)-halfSize * cellSize, (double)( i + 1 ) * cellSize, (double)j * cellSize         );
            glEnd();
        }
    }
*/

    double u, v;

    double p0[] = { 0.0, 0.0, 0.5 };
    double p1[] = { 0.1, 0.25, 0.1 };
    double p2[] = { -0.25, 0.3, -1.0 };

    axis();

    u = 0.0;
    while( u < 1.0 ){

        v = 0.0;
        while( v < 1.0 ){

            if( u + v <= 1.0 )
                interpolation( p0, p1, p2, u, v );
            v += len( p0, p1 ) / 500;

        }
        u += len( p0, p2 ) / 500;

    }

    printf("%lf %lf\n", len( p0, p1 ), len( p0, p2 ) );

	glutSwapBuffers();

}

int main( int argc, char** argv ){

    //Initial
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( 600, 600 );

    //Create Window
    mainwin = glutCreateWindow( "test" );

    //Call Back Function
    glutDisplayFunc( display );
    glutReshapeFunc( reshape );

    //Veiwer
    viewer = ViewerNew( mainwin, 0, 0, 600, 600, (void*(*)()) content);
    init();

    ViewerExtent (viewer, 16);
	glutKeyboardFunc (keyboard);

	glutMainLoop();

    return 0;

}
