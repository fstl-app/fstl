#ifndef AXIS_H
#define AXIS_H

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>

class Axis : protected QOpenGLFunctions
{
public:
    Axis();
    void setScale(QVector3D min, QVector3D max);
    void draw(QMatrix4x4 transMat, QMatrix4x4 viewMat,
        QMatrix4x4 orientMat, QMatrix4x4 aspectMat, float aspectRatio);
private:
    QOpenGLShaderProgram shader;
    QOpenGLBuffer vertices, //GL Buffer for model-space coords
        flowerAxisVertices; //GL Buffer for hud-space axis lines
    QOpenGLBuffer flowerLabelVertices[3];//Buffer for hud-space label lines
};

#endif // AXIS_H
