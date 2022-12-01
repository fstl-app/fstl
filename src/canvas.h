#ifndef CANVAS_H
#define CANVAS_H

#include <QtOpenGL>
#include <QSurfaceFormat>
#include <QOpenGLShaderProgram>

#include <tuple>
#include <vector>

class GLMesh;
class Mesh;
class Backdrop;
class Axis;

enum ViewPoint {centerview, topview, bottomview, leftview, rightview, frontview, backview};
enum DrawMode {shaded, wireframe, surfaceangle, DRAWMODECOUNT};

class Canvas : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
    Q_PROPERTY(float perspective MEMBER perspective WRITE set_perspective);
    Q_PROPERTY(QVector3D center MEMBER center WRITE set_center);
    Q_PROPERTY(float scale MEMBER scale WRITE set_scale);
    Q_PROPERTY(float zoom MEMBER zoom WRITE set_zoom);
    Q_PROPERTY(float tilt MEMBER tilt WRITE set_tilt);
    Q_PROPERTY(float yaw MEMBER yaw WRITE set_yaw);

public:
    explicit Canvas(const QSurfaceFormat& format, QWidget* parent=0);
    ~Canvas();

    const static float P_PERSPECTIVE;
    const static float P_ORTHOGRAPHIC;

    void view_perspective(float p, bool animate);
    void draw_axes(bool d);
    void invert_zoom(bool d);
    void set_drawMode(enum DrawMode mode);
    void common_view_change(enum ViewPoint c);

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
    void set_center(QVector3D cen);
    void set_scale(float s);
    void set_zoom(float z);
    void set_tilt(float t);
    void set_yaw(float y);
    void view_anim(float v);

    /**
     * @brief Animates a list of properties of the Canvas class in a parallel animation group.
     * @param prop_list List of tuples of the property name, the target value and the property animation time.
     */
    void animate_properties_as_group(std::vector<std::tuple<QByteArray, QVariant, int>> prop_list);
private:
    void draw_mesh();

    QMatrix4x4 orient_matrix() const;
    QMatrix4x4 transform_matrix() const;
    QMatrix4x4 aspect_matrix() const;
    QMatrix4x4 view_matrix() const;

    QOpenGLShader* mesh_vertshader;
    QOpenGLShaderProgram mesh_shader;
    QOpenGLShaderProgram mesh_wireframe_shader;
    QOpenGLShaderProgram mesh_surfaceangle_shader;

    GLMesh* mesh;
    Backdrop* backdrop;
    Axis* axis;

    QVector3D center, default_center;
    float scale, default_scale;
    float zoom;
    float tilt;
    float yaw;

    float perspective;
    enum DrawMode drawMode;
    bool drawAxes;
    bool invertZoom;
    
    QPropertyAnimation perspective_animation;

    QParallelAnimationGroup common_view_animation;

    QPoint mouse_pos;
    QString status;
    QString meshInfo;
};

#endif // CANVAS_H
