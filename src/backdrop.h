#ifndef BACKDROP_H
#define BACKDROP_H

#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

class Backdrop : protected QOpenGLFunctions
{
public:
    Backdrop();
    void draw();

private:
    QOpenGLShaderProgram shader;
    QOpenGLBuffer vertices;
};

#endif // BACKDROP_H
