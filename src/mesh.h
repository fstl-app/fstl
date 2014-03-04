#ifndef MESH_H
#define MESH_H

#include <QString>

#include <Eigen/Dense>

class Mesh
{
public:
    Mesh(const Eigen::Matrix3Xf &vertices, const Eigen::Matrix3Xi &indices);
    static Mesh* load_stl(const QString& filename);

    float xmin() const { return vertices.row(0).minCoeff(); }
    float xmax() const { return vertices.row(0).maxCoeff(); }
    float ymin() const { return vertices.row(1).minCoeff(); }
    float ymax() const { return vertices.row(1).maxCoeff(); }
    float zmin() const { return vertices.row(2).minCoeff(); }
    float zmax() const { return vertices.row(2).maxCoeff(); }

private:
    const Eigen::Matrix3Xf vertices;
    const Eigen::Matrix3Xi indices;

    friend class GLMesh;
};

#endif // MESH_H
