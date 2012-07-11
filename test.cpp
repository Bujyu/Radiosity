#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

#include <iostream>
#include <ctime>


#include <windows.h>
#include <gl/gluit.h>
#include <gl.hh>

typedef OpenMesh::TriMesh_ArrayKernelT<>  Mesh;
//typedef OpenMesh::PolyMesh_ArrayKernelT<>  Mesh;

Mesh _mesh;
int gw, gh;

//Main Func
int mainwin;
int viewer;


void readMesh( char* path, float center[3], float radius ){

    OpenMesh::IO::read_mesh( _mesh, path );

    Mesh::ConstVertexIter	v_it( _mesh.vertices_begin() ),
                            v_end( _mesh.vertices_end() );

	Mesh::Point             bbMin, bbMax;


	bbMin = bbMax = _mesh.point(v_it);  // accessing the Point of a vertex
	for (; v_it!=v_end; ++v_it){
		bbMin.minimize(_mesh.point(v_it));
		bbMax.maximize(_mesh.point(v_it));
	}

	for (int i = 0; i < 3; i++)
		center[i] = (bbMin[i] +bbMax[i])/2;
	radius = (bbMax-bbMin).norm()/2;  // set radius as half of bbox diagonal

	_mesh.request_face_normals();
	_mesh.update_normals();

}

//2 to 4
void triMeshSplit(){

    Mesh::ConstEdgeIter         e_it( _mesh.edges_begin() ),
                                e_end( _mesh.edges_end() );
    Mesh::Point                 center;

    for( ; e_it != e_end ; ++e_it ){

        center[0] = 0;
        center[2] = 0;
        center[1] = 0;

        center += _mesh.point( _mesh.to_vertex_handle( _mesh.halfedge_handle( e_it.handle(), 0 ) ) );
        center += _mesh.point( _mesh.to_vertex_handle( _mesh.halfedge_handle( e_it.handle(), 1 ) ) );

        center[0] /= 2;
        center[1] /= 2;
        center[2] /= 2;

        _mesh.split( e_it.handle(), center );

    }

}

void meshSplit(){

    static int visited = 0;

    std::cout << ++visited << std::endl;

    Mesh::ConstFaceIter	        f_it( _mesh.faces_begin() ),
                                f_end( _mesh.faces_end() );
    Mesh::ConstFaceVertexIter	fv_it;

    Mesh::Point                 center;

    for( ; f_it != f_end ; ++f_it ){

        center[0] = 0;
        center[1] = 0;
        center[2] = 0;

        fv_it = _mesh.cfv_iter( f_it.handle() );
        center += _mesh.point( fv_it.handle() );
        ++fv_it;
        center += _mesh.point( fv_it.handle() );
        ++fv_it;
        center += _mesh.point( fv_it.handle() );

        center[0] /= 3;
        center[1] /= 3;
        center[2] /= 3;

        _mesh.split( f_it.handle(), center );

    }

    //Face normal reupdate
    _mesh.request_face_normals();
	_mesh.update_normals();

}

void drawMeshAnime(){

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
/*
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
            meshSplit();
            break;
        case 'e':
        case 'E':
            triMeshSplit();
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

void init(){

    float center[3], radius = 0.0;


    //Depth
    glEnable( GL_DEPTH_TEST );


    readMesh( (char*) "Models/five-face.off", center, radius );


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

    drawMesh();

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
