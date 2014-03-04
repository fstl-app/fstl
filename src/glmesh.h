#ifndef GLMESH_H
#define GLMESH_H

#include <QtOpenGL/QGLBuffer>

class Mesh;

class GLMesh
{
public:
    GLMesh(const Mesh* const mesh);
    void draw(GLuint vp);
private:
    QGLBuffer vertices;
    QGLBuffer indices;
};

#endif // GLMESH_H
