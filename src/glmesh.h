#ifndef GLMESH_H
#define GLMESH_H

#include <QtOpenGL/QGLBuffer>
#include <QtOpenGL/QGLFunctions>

class Mesh;

class GLMesh : protected QGLFunctions
{
public:
    GLMesh(const Mesh* const mesh);
    void draw(GLuint vp);
private:
    QGLBuffer vertices;
    QGLBuffer indices;
};

#endif // GLMESH_H
