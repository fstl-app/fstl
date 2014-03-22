#include "glmesh.h"
#include "mesh.h"

GLMesh::GLMesh(const Mesh* const mesh)
    : vertices(QGLBuffer::VertexBuffer), indices(QGLBuffer::IndexBuffer)
{
    initializeGLFunctions();

    vertices.create();
    indices.create();

    vertices.setUsagePattern(QGLBuffer::StaticDraw);
    indices.setUsagePattern(QGLBuffer::StaticDraw);

    vertices.bind();
    vertices.allocate(mesh->vertices.data(),
                      mesh->vertices.size() * sizeof(float));
    vertices.release();

    indices.bind();
    indices.allocate(mesh->indices.data(),
                     mesh->indices.size() * sizeof(uint32_t));
    indices.release();
}

void GLMesh::draw(GLuint vp)
{
    vertices.bind();
    indices.bind();

    glVertexAttribPointer(vp, 3, GL_FLOAT, false, 3*sizeof(float), NULL);
    glDrawElements(GL_TRIANGLES, indices.size() / sizeof(uint32_t),
                   GL_UNSIGNED_INT, NULL);

    vertices.release();
    indices.release();
}
