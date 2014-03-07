#ifndef MESH_H
#define MESH_H

#include <QString>
#include <QtOpenGL/QtOpenGL>

#include <vector>

class Mesh
{
public:
    Mesh(std::vector<GLfloat> vertices, std::vector<GLuint> indices);
    static Mesh* load_stl(const QString& filename);
/*
    float xmin() const { return vertices.row(0).minCoeff(); }
    float xmax() const { return vertices.row(0).maxCoeff(); }
    float ymin() const { return vertices.row(1).minCoeff(); }
    float ymax() const { return vertices.row(1).maxCoeff(); }
    float zmin() const { return vertices.row(2).minCoeff(); }
    float zmax() const { return vertices.row(2).maxCoeff(); }
*/
private:
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

    friend class GLMesh;
};

#endif // MESH_H
