#ifndef BACKDROP_H
#define BACKDROP_H

#include <QtOpenGL/QGLFunctions>
#include <QtOpenGL/QGLShaderProgram>
#include <QtOpenGL/QGLBuffer>

class Backdrop : protected QGLFunctions
{
public:
    Backdrop();
    void draw();
private:
    QGLShaderProgram shader;
    QGLBuffer vertices;
};

#endif // BACKDROP_H
