#ifndef CANVAS_H
#define CANVAS_H

#include <QtOpenGL>
#include <QSurfaceFormat>
#include <QOpenGLShaderProgram>

class GLMesh;
class Mesh;
class Backdrop;
class Axis;

enum DrawMode {shaded, wireframe, surfaceangle, meshlight, DRAWMODECOUNT};

class Canvas : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit Canvas(const QSurfaceFormat& format, QWidget* parent=0);
    ~Canvas();

    const static float P_PERSPECTIVE;
    const static float P_ORTHOGRAPHIC;

    void view_perspective(float p, bool animate);
    void draw_axes(bool d);
    void invert_zoom(bool d);
    void set_drawMode(enum DrawMode mode);

    QColor getAmbientColor();
    void setAmbientColor(QColor c);
    double getAmbientFactor();
    void setAmbientFactor(double f);
    void resetAmbientColor();

    QColor getDirectiveColor();
    void setDirectiveColor(QColor c);
    double getDirectiveFactor();
    void setDirectiveFactor(double f);
    void resetDirectiveColor();

    QList<QString> getNameDir();
    int getCurrentLightDirection();
    void setCurrentLightDirection(int ind);
    void resetCurrentLightDirection();

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
    void view_anim(float v);

private:
    void draw_mesh();

    QMatrix4x4 orient_matrix() const;
    QMatrix4x4 transform_matrix() const;
    QMatrix4x4 aspect_matrix() const;
    QMatrix4x4 view_matrix() const;
    void resetTransform();
    QPointF changeMouseCoordinates(QPoint p);
    void calcArcballTransform(QPointF p1, QPointF p2);

    QOpenGLShader* mesh_vertshader;
    QOpenGLShaderProgram mesh_shader;
    QOpenGLShaderProgram mesh_wireframe_shader;
    QOpenGLShaderProgram mesh_surfaceangle_shader;
    QOpenGLShaderProgram mesh_meshlight_shader;

    QColor ambientColor;
    QColor directiveColor;
    float ambientFactor;
    float directiveFactor;
    QList<QString> nameDir;
    QList<QVector3D> listDir;
    int currentLightDirection;

    const static QColor defaultAmbientColor;
    const static QColor defaultDirectiveColor;
    const static float defaultAmbientFactor;
    const static float defaultDirectiveFactor;
    const static int defaultCurrentLightDirection;
    const static QString AMBIENT_COLOR;
    const static QString AMBIENT_FACTOR;
    const static QString DIRECTIVE_COLOR;
    const static QString DIRECTIVE_FACTOR;
    const static QString CURRENT_LIGHT_DIRECTION;


    GLMesh* mesh;
    Backdrop* backdrop;
    Axis* axis;

    QVector3D center;
    float scale;
    float zoom;
    QMatrix4x4 currentTransform;

    float perspective;
    enum DrawMode drawMode;
    bool drawAxes;
    bool invertZoom;
    Q_PROPERTY(float perspective MEMBER perspective WRITE set_perspective);
    QPropertyAnimation anim;

    QPoint mouse_pos;
    QString status;
    QString meshInfo;
};

#endif // CANVAS_H
