#include "axis.h"
Axis::Axis()
    : vbuf {1, 0, 0, 1, 0, 0, //X Axis
        -1, 0, 0, 1, 0, 0,
        0, 1, 0, 0, 1, 0, //Y Axis
        0, -1, 0, 0, 1, 0,
        0, 0, 1, 0, 0, 1, //Z Axis
        0, 0, -1, 0, 0, 1}
{
    initializeOpenGLFunctions();

    shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/gl/colored_lines.vert");
    shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/gl/colored_lines.frag");
    shader.link();

    float hud_vbuf[] = {0, 0, 0, 1, 0, 0, //X Axis
        1, 0, 0, 1, 0, 0,
        1.1, 0.1, 0, 1, 0, 0, //X Letter
        1.5, -0.1, 0, 1, 0, 0,
        1.1, -0.1, 0, 1, 0, 0,
        1.5, 0.1, 0, 1, 0, 0,

        0, 0, 0, 0, 1, 0, //Y Axis
        0, 1, 0, 0, 1, 0,
        0, 1.1, 0, 0, 1, 0, //Y Letter
        0, 1.3, 0, 0, 1, 0,
        0, 1.3, 0, 0, 1, 0,
        0, 1.5, 0.1, 0, 1, 0,
        0, 1.3, 0, 0, 1, 0,
        0, 1.5, -0.1, 0, 1, 0,

        0, 0, 0, 0, 0, 1, //Z Axis
        0, 0, 1, 0, 0, 1,
        0.1, 0, 1.1, 0, 0, 1, //Z Letter
        -0.1, 0, 1.1, 0, 0, 1,
        -0.1, 0, 1.1, 0, 0, 1,
        0.1, 0, 1.5, 0, 0, 1,
        0.1, 0, 1.5, 0, 0, 1,
        -0.1, 0, 1.5, 0, 0, 1,
    };
    //The lines which form the 'axis-flower' in the corner
    hud_vertices.create();
    hud_vertices.bind();
    hud_vertices.allocate(hud_vbuf, sizeof(hud_vbuf));
    hud_vertices.release();
    //The lines which form the model-space axes
    vertices.create();
    vertices.bind();
    vertices.allocate(vbuf, sizeof(vbuf));
    vertices.release();
}
void Axis::setScale(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax)
{
    //Max function. not worth importing <algorithm> just for max
    auto max = [](float a, float b)
    {
        return (a > b) ? a : b;
    };
    //This is how much the axes extend beyond the model
    //We want it to be dependent on the model's size, but uniform on all axes.
    const float axismargin = 0.25*max(max(xmax-xmin, ymax-ymin), zmax-zmin);
    //Manually rewrite coordinates to control axis draw lengths
    vbuf[0] = xmin-axismargin;
    vbuf[6] = xmax+axismargin;
    vbuf[13] = ymin-axismargin;
    vbuf[19] = ymax+axismargin;
    vbuf[26] = zmin-axismargin;
    vbuf[32] = zmax+axismargin;
    vertices.bind();
    vertices.write(0, vbuf, sizeof(vbuf));
    vertices.release();
}
void Axis::draw(QMatrix4x4 transMat, QMatrix4x4 viewMat,
    QMatrix4x4 orientMat, QMatrix4x4 aspectMat, float aspectRatio)
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

    glDrawArrays(GL_LINES, 0, 3*6);

    vertices.release();
    //Next, we draw the hud axis-flower
    hud_vertices.bind();
    glClear(GL_DEPTH_BUFFER_BIT);//Ensure hud draws over everything
    const float hudSize = 0.2;
    QMatrix4x4 hudMat;
    //Move the hud to the bottom left corner with margin
    if (aspectRatio > 1.0)
    {
        hudMat.translate(aspectRatio-2*hudSize, -1.0+2*hudSize, 0);
    }
    else
    {
        hudMat.translate(1.0-2*hudSize, -1.0/aspectRatio+2*hudSize, 0);
    }
    //Scale the hud to be small
    hudMat.scale(hudSize, hudSize, 1);
    glUniformMatrix4fv(
                shader.uniformLocation("view_matrix"),
                1, GL_FALSE, (aspectMat*hudMat).data());
    glUniformMatrix4fv(
                shader.uniformLocation("transform_matrix"),
                1, GL_FALSE, orientMat.data());

    glVertexAttribPointer(vp, 3, GL_FLOAT, false,
                          6 * sizeof(GLfloat), 0);
    glVertexAttribPointer(vc, 3, GL_FLOAT, false,
                          6 * sizeof(GLfloat),
                          (GLvoid*)(3 * sizeof(GLfloat)));

    glDrawArrays(GL_LINES, 0, 3*22);
    shader.release();
    hud_vertices.release();
}
