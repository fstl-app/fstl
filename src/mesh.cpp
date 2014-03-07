#include <QFile>
#include <QDataStream>
#include <QVector3D>

#include <algorithm>

#include "mesh.h"

struct Vec3
{
    GLfloat x, y, z;
    bool operator!=(const Vec3& rhs) const
    {
        return x != rhs.x || y != rhs.y || z != rhs.z;
    }
    bool operator<(const Vec3& rhs) const
    {
        if      (x != rhs.x)    return x < rhs.x;
        else if (y != rhs.y)    return y < rhs.y;
        else if (z != rhs.z)    return z < rhs.z;
        else                    return false;
    }
};


Mesh::Mesh(std::vector<GLfloat> v, std::vector<GLuint> i)
    : vertices(v), indices(i)
{
    // Nothing to do here
}

Mesh* Mesh::load_stl(const QString& filename)
{
    QFile file(filename);
    file.open(QIODevice::ReadOnly);

    QDataStream data(&file);
    data.setByteOrder(QDataStream::LittleEndian);
    data.setFloatingPointPrecision(QDataStream::SinglePrecision);

    // Skip .stl file header
    data.skipRawData(80);

    // Load the triangle count from the .stl file
    uint32_t tri_count;
    data >> tri_count;

    // Extract vertices into an array of xyz, unsigned pairs
    QVector<std::pair<Vec3, GLuint>> verts(tri_count*3);

    // Store vertices in the array, processing one triangle at a time.
    for (auto v=verts.begin(); v != verts.end(); v += 3)
    {
        // Skip face's normal vector
        data.skipRawData(3*sizeof(float));

        // Load vertex data from .stl file into vertices
        data >> v[0].first.x >> v[0].first.y >> v[0].first.z;
        data >> v[1].first.x >> v[1].first.y >> v[1].first.z;
        data >> v[2].first.x >> v[2].first.y >> v[2].first.z;

        // Skip face attribute
        data.skipRawData(sizeof(uint16_t));
    }

    // Save indicies as the second element in the array
    // (so that we can reconstruct triangle order after sorting)
    for (size_t i=0; i < tri_count*3; ++i)
    {
        verts[i].second = i;
    }

    // Sort the set of vertices (to deduplicate)
    std::sort(verts.begin(), verts.end());

    // This vector will store triangles as sets of 3 indices
    std::vector<GLuint> indices(tri_count*3);

    // Go through the sorted vertex list, deduplicating and creating
    // an indexed geometry representation for the triangles.
    // Unique vertices are moved so that they occupy the first vertex_count
    // positions in the verts array.
    size_t vertex_count = 0;
    for (auto v : verts)
    {
        if (!vertex_count || v.first != verts[vertex_count-1].first)
        {
            verts[vertex_count++] = v;
        }
        indices[v.second] = vertex_count - 1;
    }

    // Finally, pack unique vertices into a flat array.
    std::vector<GLfloat> unique_verts(vertex_count*3);
    for (size_t i=0; i < vertex_count; ++i)
    {
        unique_verts[3*i]     = verts[i].first.x;
        unique_verts[3*i + 1] = verts[i].first.y;
        unique_verts[3*i + 2] = verts[i].first.z;
    }

    return new Mesh(unique_verts, indices);
}
