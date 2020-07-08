#ifndef CANVAS_H
#define CANVAS_H

#include <QtOpenGL>
#include <QSurfaceFormat>
#include <QOpenGLShaderProgram>

class GLMesh;
class Mesh;
class Backdrop;

enum DrawMode {shaded, wireframe};

class Canvas : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit Canvas(const QSurfaceFormat& format, QWidget* parent=0);
    ~Canvas();

    void view_orthographic();
    void view_perspective();
    void draw_shaded();
    void draw_wireframe();
    void draw_axes(bool d);

public slots:
    void set_status(const QString& s);
    void clear_status();
    void load_mesh(Mesh* m, bool is_reload);

protected:
    void paintGL() override;
    void initializeGL() override;
    void resizeGL(int width, int height) override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    
    void set_perspective(float p);
    void set_drawMode(enum DrawMode mode);
    void view_anim(float v);

private:
    void draw_mesh();

    QMatrix4x4 transform_matrix() const;
    QMatrix4x4 view_matrix() const;

    QOpenGLShaderProgram mesh_shader;
    QOpenGLShaderProgram mesh_wireframe_shader;
    QOpenGLShaderProgram quad_shader;

    GLMesh* mesh;
    Backdrop* backdrop;

    QVector3D center;
    float scale;
    float zoom;
    float tilt;
    float yaw;

    float perspective;
    enum DrawMode drawMode;
    bool drawAxes;
    Q_PROPERTY(float perspective MEMBER perspective WRITE set_perspective);
    QPropertyAnimation anim;

    QPoint mouse_pos;
    QString status;
};

#endif // CANVAS_H
