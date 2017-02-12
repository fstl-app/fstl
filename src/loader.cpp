#include "loader.h"

Loader::Loader(QObject* parent, const QString& filename)
    : QThread(parent), filename(filename)
{
    // Nothing to do here
}

void Loader::run()
{
    Mesh* mesh = load_stl();
    if (mesh)
    {
        emit got_mesh(mesh);
        emit loaded_file(filename);
    }
}


////////////////////////////////////////////////////////////////////////////////

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

typedef std::pair<Vec3, GLuint> Vec3i;

////////////////////////////////////////////////////////////////////////////////

Mesh* Loader::load_stl()
{
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    if (file.read(5) == "solid")
    {
        emit error_ascii_stl();
        return NULL;
    }
    // Skip the rest of the header material
    file.read(75);

    QDataStream data(&file);
    data.setByteOrder(QDataStream::LittleEndian);
    data.setFloatingPointPrecision(QDataStream::SinglePrecision);

    // Load the triangle count from the .stl file
    uint32_t tri_count;
    data >> tri_count;

    // Verify that the file is the right size
    if (file.size() != 84 + tri_count*50)
    {
        emit error_bad_stl();
        return NULL;
    }

    // Extract vertices into an array of xyz, unsigned pairs
    QVector<Vec3i> verts(tri_count*3);

    // Dummy array, because readRawData is faster than skipRawData
    char buffer[sizeof(float)*3];

    // Store vertices in the array, processing one triangle at a time.
    for (auto v=verts.begin(); v != verts.end(); v += 3)
    {
        // Skip face's normal vector
        data.readRawData(buffer, 3*sizeof(float));

        // Load vertex data from .stl file into vertices
        data >> v[0].first.x >> v[0].first.y >> v[0].first.z;
        data >> v[1].first.x >> v[1].first.y >> v[1].first.z;
        data >> v[2].first.x >> v[2].first.y >> v[2].first.z;

        // Skip face attribute
        data.readRawData(buffer, sizeof(uint16_t));
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
    verts.resize(vertex_count);

    std::vector<GLfloat> flat_verts;
    flat_verts.reserve(vertex_count*3);
    for (auto v : verts)
    {
        flat_verts.push_back(v.first.x);
        flat_verts.push_back(v.first.y);
        flat_verts.push_back(v.first.z);
    }

    return new Mesh(flat_verts, indices);
}

