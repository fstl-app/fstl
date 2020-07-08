#include "axis.h"

Axis::Axis()
{
    initializeOpenGLFunctions();

    shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/gl/colored_lines.vert");
    shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/gl/colored_lines.frag");
    shader.link();

    float vbuf[] = {
        1.0, 0.0, 0.0, 0.9, 0.0, 0.0, //X Axis
        -1.0, 0.0, 0.0, 0.9, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0, 0.9, 0.0, //Y Axis
        0.0, -1.0, 0.0, 0.0, 0.9, 0.0,
        0.0, 0.0, 1.0, 0.0, 0.0, 0.9, //Z Axis
        0.0, 0.0, -1.0, 0.0, 0.0, 0.9};

    vertices.create();
    vertices.bind();
    vertices.allocate(vbuf, sizeof(vbuf));
    vertices.release();
}
void Axis::setScale(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax)
{
}
void Axis::draw(QMatrix4x4 transMat, QMatrix4x4 viewMat)
{
    shader.bind();
    vertices.bind();
    // Load the transform and view matrices into the shader
    glUniformMatrix4fv(
                shader.uniformLocation("transform_matrix"),
                1, GL_FALSE, transMat.data());
    glUniformMatrix4fv(
                shader.uniformLocation("view_matrix"),
                1, GL_FALSE, viewMat.data());

    const GLuint vp = shader.attributeLocation("vertex_position");
    const GLuint vc = shader.attributeLocation("vertex_color");

    glEnableVertexAttribArray(vp);
    glEnableVertexAttribArray(vc);

    glVertexAttribPointer(vp, 3, GL_FLOAT, false,
                          6 * sizeof(GLfloat), 0);
    glVertexAttribPointer(vc, 3, GL_FLOAT, false,
                          6 * sizeof(GLfloat),
                          (GLvoid*)(3 * sizeof(GLfloat)));

    glDrawArrays(GL_LINES, 0, 18);

    vertices.release();
    shader.release();
}
