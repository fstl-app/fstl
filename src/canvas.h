#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QtOpenGL/QGLWidget>
#include <QtOpenGL/QGLShaderProgram>
#include <QMatrix4x4>

class GLMesh;
class Mesh;
class Backdrop;

class Canvas : public QGLWidget
{
    Q_OBJECT

public:
    Canvas(const QGLFormat& format, QWidget* parent=0);

    void initializeGL();
    void paintEvent(QPaintEvent* event);
    ~Canvas();

public slots:
    void set_status(const QString& s);
    void load_mesh(Mesh* m);


protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);


private:
    void draw_mesh();

    QMatrix4x4 transform_matrix() const;
    QMatrix4x4 view_matrix() const;

    QGLShaderProgram mesh_shader;
    QGLShaderProgram quad_shader;

    GLMesh* mesh;
    Backdrop* backdrop;

    QVector3D center;
    float scale;
    float tilt;
    float yaw;

    QPoint mouse_pos;
    QString status;
};

#endif // CANVAS_H
