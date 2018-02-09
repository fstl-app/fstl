#include <QFile>
#include <QDataStream>
#include <QVector3D>

#include <cmath>

#include "mesh.h"

////////////////////////////////////////////////////////////////////////////////

Mesh::Mesh(std::vector<GLfloat>&& v, std::vector<GLuint>&& i)
    : vertices(std::move(v)), indices(std::move(i))
{
    // Nothing to do here
}

float Mesh::min(size_t start) const
{
    if (start >= vertices.size())
    {
        return -1;
    }
    float v = vertices[start];
    for (size_t i=start; i < vertices.size(); i += 3)
    {
        v = fmin(v, vertices[i]);
    }
    return v;
}

float Mesh::max(size_t start) const
{
    if (start >= vertices.size())
    {
        return 1;
    }
    float v = vertices[start];
    for (size_t i=start; i < vertices.size(); i += 3)
    {
        v = fmax(v, vertices[i]);
    }
    return v;
}

bool Mesh::empty() const
{
    return vertices.size() == 0;
}
