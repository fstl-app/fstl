#include "axis.h"

const float xLet[] = {
    -0.1, -0.2, 0,
    0.1, 0.2, 0,
    0.1, -0.2, 0,
    -0.1, 0.2, 0
};
const float yLet[] = {
    0, -0.2, 0,
    0, 0, 0,
    0, 0, 0,
    0.1, 0.2, 0,
    0, 0, 0,
    -0.1, 0.2, 0
};
const float zLet[] = {
    -0.1, -0.2, 0,
    0.1, -0.2, 0,
    0.1, -0.2, 0,
    -0.1, 0.2, 0,
    -0.1, 0.2, 0,
    0.1, 0.2, 0
};
const int axisSegCount[] = {2, 3, 3};
const float* axisLabels[] = {xLet, yLet, zLet};

Axis::Axis()
{
    initializeOpenGLFunctions();

    shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/gl/colored_lines.vert");
    shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/gl/colored_lines.frag");
    shader.link();
    const int ptSize = 6*sizeof(float);
    for(int lIdx = 0; lIdx < 3; lIdx++)
    {
        const float* l = axisLabels[lIdx];
        const int ptCount = axisSegCount[lIdx]*2;
        float c[3] = {0.0};
        c[lIdx] = 1.0;//set color
        QOpenGLBuffer b = flowerLabelVertices[lIdx];
        b.create();
        b.bind();
        b.allocate(ptCount*ptSize);
        for(int pIdx = 0; pIdx < ptCount; pIdx++)
        {
            b.write(pIdx*ptSize, &(l[pIdx*3]), ptSize/2);//write coords
            b.write(pIdx*ptSize + ptSize/2, c, ptSize/2);//write color
        }
        b.release();
    }
    //Axis buffer: 6 floats per vertex, 2 vert per line, 3 lines
    float aBuf[6*2*3] = {0.0};
    for(int aIdx = 0; aIdx < 3; aIdx++)
    {
        aBuf[(2*aIdx)*6+3+aIdx] = 1.0;//Set color (last 3 floats)
        aBuf[(2*aIdx+1)*6+3+aIdx] = 1.0;//Set color (last 3 floats)
        aBuf[(2*aIdx+1)*6+aIdx] = 1.0;//Extend line in axis
    }
    //The lines which form the 'axis-flower' in the corner
    flowerAxisVertices.create();
    flowerAxisVertices.bind();
    flowerAxisVertices.allocate(aBuf, sizeof(aBuf));
    flowerAxisVertices.release();
    //The lines which form the model-space axes
    vertices.create();
    vertices.bind();
    vertices.allocate(aBuf, sizeof(aBuf));
    vertices.release();
}
void Axis::setScale(QVector3D min, QVector3D max)
{
    //Max function. not worth importing <algorithm> just for max
    auto Max = [](float a, float b)
    {
        return (a > b) ? a : b;
    };
    //This is how much the axes extend beyond the model
    //We want it to be dependent on the model's size, but uniform on all axes.
    const float axismargin = 0.25*Max(Max(max[0]-min[0], max[1]-min[1]), max[2]-min[2]);
    vertices.bind();
    //Manually rewrite coordinates to control axis draw lengths
    float s = sizeof(float);
    //aIdx*12+aIdx gets us to the set of 2 points of the axis line, plus the offset for that dimension
    //+6 gets us to the other end of the line in that dimension
    for(int aIdx = 0; aIdx < 3; aIdx++)
    {
        float t = min[aIdx]-axismargin;
        vertices.write(s*(aIdx*12+aIdx), &t, s);
        t = max[aIdx]+axismargin;
        vertices.write(s*(aIdx*12+aIdx+6), &t, s);
    }
    vertices.release();
}
void Axis::draw(QMatrix4x4 transMat, QMatrix4x4 viewMat,
    QMatrix4x4 orientMat, QMatrix4x4 aspectMat, float aspectRatio)
{
    shader.bind();
    vertices.bind();
    // Load the transform and view matrices into the shader
    auto loadMatrixUniforms = [&](QMatrix4x4 transform, QMatrix4x4 view)
    {
        glUniformMatrix4fv(
                    shader.uniformLocation("transform_matrix"),
                    1, GL_FALSE, transform.data());
        glUniformMatrix4fv(
                    shader.uniformLocation("view_matrix"),
                    1, GL_FALSE, view.data());
    };
    const GLuint vp = shader.attributeLocation("vertex_position");
    const GLuint vc = shader.attributeLocation("vertex_color");
    glEnableVertexAttribArray(vp);
    glEnableVertexAttribArray(vc);
    auto loadAttribPtr = [&]()
    {
        glVertexAttribPointer(vp, 3, GL_FLOAT, false,
                        6 * sizeof(GLfloat), 0);
        glVertexAttribPointer(vc, 3, GL_FLOAT, false,
                        6 * sizeof(GLfloat),
                        (GLvoid*)(3 * sizeof(GLfloat)));
    };
    loadMatrixUniforms(transMat, viewMat);
    loadAttribPtr();

    glDrawArrays(GL_LINES, 0, 3*6);

    vertices.release();
    //Next, we draw the hud axis-flower
    flowerAxisVertices.bind();
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
    loadMatrixUniforms(orientMat, aspectMat*hudMat);
    loadAttribPtr();
    glDrawArrays(GL_LINES, 0, 3*6);
    flowerAxisVertices.release();
    for(int aIdx = 0; aIdx < 3; aIdx++){
        QVector3D transVec = QVector3D();
        transVec[aIdx] = 1.25;//This is how far we want the letters to be extended out
        QOpenGLBuffer b = flowerLabelVertices[aIdx];
        //The only transform we want is to translate the letters to the ends of the axis lines
        QMatrix4x4 labelTransMat = QMatrix4x4();
        labelTransMat.translate(orientMat * transVec);
        b.bind();
        loadMatrixUniforms(labelTransMat, aspectMat * hudMat);
        loadAttribPtr();
        glDrawArrays(GL_LINES, 0, axisSegCount[aIdx]*2*6);
        b.release();
    }
    shader.release();
}
