#include <QFile>
#include <QDataStream>

#include <algorithm>

#include "mesh.h"

Mesh::Mesh(const Eigen::Matrix3Xf& v, const Eigen::Matrix3Xi& i)
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

    data.skipRawData(80);
    uint32_t tri_count;
    data >> tri_count;

    // Extract vertices into a vector of Vector4d objects
    std::vector<Eigen::Vector4d> verts(tri_count*3);
    for (unsigned i=0; i < tri_count; ++i)
    {
        data.skipRawData(3*sizeof(float));
        for (int j=0; j < 3; ++j)
        {
            float x, y, z;
            data >> x >> y >> z;
            verts[3*i + j] << x, y, z, 3*i + j;
        }
        data.skipRawData(sizeof(uint16_t));
    }

    // Sort the set of vertices (to deduplicate)
    std::sort(verts.begin(), verts.end(),
              [](const Eigen::Vector4d& lhs, const Eigen::Vector4d& rhs)
        {
            if      (lhs[0] != rhs[0])  return lhs[0] < rhs[0];
            else if (lhs[1] != rhs[1])  return lhs[1] < rhs[1];
            else if (lhs[2] != rhs[2])  return lhs[2] < rhs[2];
            else                        return false;
        }
    );

    // This list will store unique vertices
    std::list<Eigen::Vector3f> unique;

    // This vector will store triangles as rows of indices
    Eigen::Matrix3Xi indices;
    indices.resize(Eigen::NoChange, tri_count);

    // Go through the sorted vertex list, deduplicating and creating
    // an indexed geometry representation for the triangles.
    for (auto v : verts)
    {
        if (!unique.size() || v[0] != unique.back()[0] ||
                              v[1] != unique.back()[1] ||
                              v[2] != unique.back()[2])
        {
            // Switch to a float vector and save in the list.
            Eigen::Vector3f v_;
            v_ << v[0], v[1], v[2];
            unique.push_back(v_);
        }
        indices(int(v[3]) % 3, int(v[3]) / 3) = unique.size() - 1;
    }

    // Finally, pack unique vertices into a matrix.
    Eigen::Matrix3Xf unique_verts;
    unique_verts.resize(Eigen::NoChange, unique.size());
    {
        auto v = unique.begin();
        for (unsigned i=0; i < unique.size(); ++i)
        {
            unique_verts.col(i) = *(v++);
        }
    }

    return new Mesh(unique_verts, indices);
}
