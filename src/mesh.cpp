#include <QFile>
#include <QDataStream>
#include <QVector3D>

#include <cmath>

#include "mesh.h"

/**
* Constructor for a mesh object. Contains GLfloat vectors and GLuint indicies
* @param v vector of GLfloats
* @param i vector of GLuints
*/
Mesh::Mesh(std::vector<GLfloat> v, std::vector<GLuint> i)
    : vertices(v), indices(i)
{
    // Nothing to do here
}

/**
* Gets the minimum of the mesh. 
* @param start the axis to search on. 0 = x, 1 = y, 2 = z.
*/
float Mesh::min(size_t start) const
{
    float v = vertices[start];
    for (size_t i=start; i < vertices.size(); i += 3)
    {
        v = fmin(v, vertices[i]);
    }
    return v;
}

/**
* Gets the max of the mesh. 
* @param start the axis to search on. 0 = x, 1 = y, 2 = z.
*/
float Mesh::max(size_t start) const
{
    float v = vertices[start];
    for (size_t i=start; i < vertices.size(); i += 3)
    {
        v = fmax(v, vertices[i]);
    }
    return v;
}
