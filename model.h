#ifndef MODEL_H
#define MODEL_H

#include <vector>

/// Vertex
typedef struct vertex{

    double x;
    double y;
    double z;

    //struct vertex* next;

} VERTEX;

typedef struct mesh{

    int n_points;

    std::vector<VERTEX> vertex;

    //VERTEX **vertex;
    //struct mesh* nest;

} MESH;

/// Surface
typedef struct surface{

    int n_polygons;

    std::vector<VERTEX> vertex;
    std::vector<MESH> vertex;
    //VERTEX** vertex;
    //MESH** mesh;

}

#endif
