#ifndef AXIS_H
#define AXIS_H

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>

class Axis : protected QOpenGLFunctions
{
public:
    Axis();
    void setScale(float xmin, float xmax, float ymin, float ymax,
        float zmin, float zmax);
    void draw(QMatrix4x4 transMat, QMatrix4x4 viewMat,
        QMatrix4x4 orientMat, QMatrix4x4 aspectMat, float aspectRatio);
private:
    void drawHud(QMatrix4x4 transMat, QMatrix4x4 viewMat);
    QOpenGLShaderProgram shader;
    float vbuf[36];
    QOpenGLBuffer vertices, hud_vertices;
};

#endif // AXIS_H
