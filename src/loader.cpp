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

Mesh* mesh_from_verts(uint32_t tri_count, QVector<Vec3i>& verts)
{
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

////////////////////////////////////////////////////////////////////////////////

Mesh* Loader::load_stl()
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        emit error_missing_file();
        return NULL;
    }

    // First, try to read the stl as an ASCII file
    if (file.read(6) == "solid ")
    {
        file.readLine(); // skip solid name
        const auto line = file.readLine().trimmed();
        if (line.startsWith("facet") ||
            line.startsWith("endsolid"))
        {
            file.seek(0);
            return read_stl_ascii(file);
        }
        confusing_stl = true;
    }
    else
    {
        confusing_stl = false;
    }

    // Otherwise, skip the rest of the header material and read as binary
    file.seek(0);
    return read_stl_binary(file);
}

Mesh* Loader::read_stl_binary(QFile& file)
{
    QDataStream data(&file);
    data.setByteOrder(QDataStream::LittleEndian);
    data.setFloatingPointPrecision(QDataStream::SinglePrecision);

    // Load the triangle count from the .stl file
    file.seek(80);
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

    if (confusing_stl)
    {
        emit warning_confusing_stl();
    }

    return mesh_from_verts(tri_count, verts);
}

Mesh* Loader::read_stl_ascii(QFile& file)
{
    file.readLine();
    uint32_t tri_count = 0;
    QVector<Vec3i> verts(tri_count*3);

    bool okay = true;
    while (!file.atEnd() && okay)
    {
        const auto line = file.readLine().simplified();
        if (line.startsWith("endsolid"))
        {
            break;
        }
        else if (!line.startsWith("facet normal") ||
                 !file.readLine().simplified().startsWith("outer loop"))
        {
            okay = false;
            break;
        }

        for (int i=0; i < 3; ++i)
        {
            auto line = file.readLine().simplified().split(' ');
            if (line[0] != "vertex")
            {
                okay = false;
                break;
            }
            const float x = line[1].toFloat(&okay);
            const float y = line[2].toFloat(&okay);
            const float z = line[3].toFloat(&okay);
            verts.push_back({{x, y, z}, 0});
        }
        if (!file.readLine().trimmed().startsWith("endloop") ||
            !file.readLine().trimmed().startsWith("endfacet"))
        {
            okay = false;
            break;
        }
        tri_count++;
    }

    if (okay)
    {
        return mesh_from_verts(tri_count, verts);
    }
    else
    {
        emit error_bad_stl();
        return NULL;
    }
}

