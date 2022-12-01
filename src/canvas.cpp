#include <QMouseEvent>

#include <cmath>
#include <vector>

#include "canvas.h"
#include "backdrop.h"
#include "axis.h"
#include "glmesh.h"
#include "mesh.h"

const float Canvas::P_PERSPECTIVE = 0.25f;
const float Canvas::P_ORTHOGRAPHIC = 0.0f;

Canvas::Canvas(const QSurfaceFormat& format, QWidget *parent)
    : QOpenGLWidget(parent), mesh(nullptr),
      scale(1), default_scale(1), zoom(1), tilt(90), yaw(0),
      perspective_animation(this, "perspective"),
      status(" "),
      meshInfo("")
{
    setFormat(format);
    QFile styleFile(":/qt/style.qss");
    styleFile.open( QFile::ReadOnly );
    setStyleSheet(styleFile.readAll());

    perspective_animation.setDuration(100);
}

Canvas::~Canvas()
{
    makeCurrent();
    delete mesh;
    delete mesh_vertshader;
    delete backdrop;
    delete axis;
    doneCurrent();
}

void Canvas::view_anim(float v)
{
    perspective_animation.setStartValue(perspective);
    perspective_animation.setEndValue(v);
    perspective_animation.start();
}

void Canvas::animate_properties_as_group(std::vector<std::tuple<QByteArray, QVariant, int>> prop_list)
{
    common_view_animation.clear();
    for(const auto &value : prop_list)
    {
        const auto& prop_name = std::get<QByteArray>(value);
        const auto anim = new QPropertyAnimation(this, prop_name);
        anim->setStartValue(property(prop_name));
        anim->setEndValue(std::get<QVariant>(value));
        anim->setDuration(std::get<int>(value));

        common_view_animation.addAnimation(anim);
    }

    common_view_animation.start();
}

void Canvas::common_view_change(enum ViewPoint c){
    switch (c)
    {
    case centerview:
        {
            animate_properties_as_group(
                {
                    std::make_tuple("scale", default_scale, 100),
                    std::make_tuple("center", default_center, 100),
                    std::make_tuple("zoom", 1., 100)
                }
            );
        }
        break;
    case topview:
        {
            animate_properties_as_group(
                {
                    std::make_tuple("yaw", 0.f, 100),
                    std::make_tuple("tilt", 0.f, 100)
                }
            );
        }
        break;
    case bottomview:
        {
            animate_properties_as_group(
                {
                    std::make_tuple("yaw", 0.f, 100),
                    std::make_tuple("tilt", 180.f, 100)
                }
            );
        }

        break;
    case leftview:
        {
            animate_properties_as_group(
                {
                    std::make_tuple("yaw", -90.f, 100),
                    std::make_tuple("tilt", 90.f, 100)
                }
            );
        }
        break;
    case rightview:
        {
            animate_properties_as_group(
                {
                    std::make_tuple("yaw", 90.f, 100),
                    std::make_tuple("tilt", 90.f, 100)
                }
            );
        }
        break;
    case frontview:
        {
            animate_properties_as_group(
                {
                    std::make_tuple("yaw", 0.f, 100),
                    std::make_tuple("tilt", 90.f, 100)
                }
            );
        }
        break;
    case backview:
        {
            animate_properties_as_group(
                {
                    std::make_tuple("yaw", 180.f, 100),
                    std::make_tuple("tilt", 90.f, 100)
                }
            );
        }
    }
}

void Canvas::view_perspective(float p, bool animate){
    if(animate)
    {
        view_anim(p);
    }
    else
    {
        set_perspective(p);
    }
}

void Canvas::draw_axes(bool d)
{
    drawAxes = d;
    update();
}

void Canvas::invert_zoom(bool d)
{
    invertZoom = d;
    update();
}

void Canvas::load_mesh(Mesh* m, bool is_reload)
{
    delete mesh;
    mesh = new GLMesh(m);
    QVector3D lower(m->xmin(), m->ymin(), m->zmin());
    QVector3D upper(m->xmax(), m->ymax(), m->zmax());
    if (!is_reload)
    {
        default_center = center = (lower + upper) / 2;
        default_scale = scale = 2 / (upper - lower).length();

        // Reset other camera parameters
        zoom = 1;
        yaw = 0;
        tilt = 90;
    }
    meshInfo = QStringLiteral("Triangles: %1\nX: [%2, %3]\nY: [%4, %5]\nZ: [%6, %7]").arg(m->triCount());
    for(int dIdx = 0; dIdx < 3; dIdx++) meshInfo = meshInfo.arg(lower[dIdx]).arg(upper[dIdx]);
    axis->setScale(lower, upper);
    update();

    delete m;
}

void Canvas::set_status(const QString &s)
{
    status = s;
    update();
}

void Canvas::set_perspective(float p)
{
    perspective = p;
    update();
}

void Canvas::set_center(QVector3D cen)
{
    center = cen;
    update();
}

void Canvas::set_scale(float s)
{
    scale = s;
    update();
}

void Canvas::set_zoom(float z)
{
    zoom = z;
    update();
}

void Canvas::set_tilt(float t)
{
    tilt = t;
    update();
}

void Canvas::set_yaw(float y)
{
    yaw = y;
    update();
}

void Canvas::set_drawMode(enum DrawMode mode)
{
    drawMode = mode;
    update();
}

void Canvas::clear_status()
{
    status = "";
    update();
}

void Canvas::initializeGL()
{
    initializeOpenGLFunctions();

    mesh_vertshader = new QOpenGLShader(QOpenGLShader::Vertex);
    mesh_vertshader->compileSourceFile(":/gl/mesh.vert");
    mesh_shader.addShader(mesh_vertshader);
    mesh_shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/gl/mesh.frag");
    mesh_shader.link();
    mesh_wireframe_shader.addShader(mesh_vertshader);
    mesh_wireframe_shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/gl/mesh_wireframe.frag");
    mesh_wireframe_shader.link();
    mesh_surfaceangle_shader.addShader(mesh_vertshader);
    mesh_surfaceangle_shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/gl/mesh_surfaceangle.frag");
    mesh_surfaceangle_shader.link();

    backdrop = new Backdrop();
    axis = new Axis();
}


void Canvas::paintGL()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    backdrop->draw();
    if (mesh)  draw_mesh();
    if (drawAxes) axis->draw(transform_matrix(), view_matrix(),
        orient_matrix(), aspect_matrix(), width() / float(height()));

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    float textHeight = painter.fontInfo().pointSize();
    if (drawAxes) painter.drawText(QRect(10, textHeight, width(), height()), meshInfo);
    painter.drawText(10, height() - textHeight, status);
}

void Canvas::draw_mesh()
{
    QOpenGLShaderProgram* selected_mesh_shader = NULL;
    if(drawMode == wireframe)
    {
        selected_mesh_shader = &mesh_wireframe_shader;
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        if(drawMode == shaded)
        {
            selected_mesh_shader = &mesh_shader;
        }
        else
        {
            selected_mesh_shader = &mesh_surfaceangle_shader;
        }
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    selected_mesh_shader->bind();

    // Load the transform and view matrices into the shader
    glUniformMatrix4fv(
                selected_mesh_shader->uniformLocation("transform_matrix"),
                1, GL_FALSE, transform_matrix().data());
    glUniformMatrix4fv(
                selected_mesh_shader->uniformLocation("view_matrix"),
                1, GL_FALSE, view_matrix().data());

    // Compensate for z-flattening when zooming
    glUniform1f(selected_mesh_shader->uniformLocation("zoom"), 1/zoom);

    // Find and enable the attribute location for vertex position
    const GLuint vp = selected_mesh_shader->attributeLocation("vertex_position");
    glEnableVertexAttribArray(vp);

    // Then draw the mesh with that vertex position
    mesh->draw(vp);

    // Reset draw mode for the background and anything else that needs to be drawn
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Clean up state machine
    glDisableVertexAttribArray(vp);
    selected_mesh_shader->release();
}
QMatrix4x4 Canvas::orient_matrix() const
{
    QMatrix4x4 m;
    m.rotate(tilt, QVector3D(1, 0, 0));
    m.rotate(yaw,  QVector3D(0, 0, 1));
    //We want the x axis to the right, and the z axis up
    m.scale(-1, 1, -1);
    return m;
}
QMatrix4x4 Canvas::transform_matrix() const
{
    QMatrix4x4 m = orient_matrix();
    m.scale(scale);
    m.translate(-center);
    return m;
}
QMatrix4x4 Canvas::aspect_matrix() const
{
    QMatrix4x4 m;
    if (width() > height())
    {
        m.scale(-height() / float(width()), 1, 0.5);
    }
    else
    {
        m.scale(-1, width() / float(height()), 0.5);
    }
    return m;
}
QMatrix4x4 Canvas::view_matrix() const
{
    QMatrix4x4 m = aspect_matrix();
    m.scale(zoom, zoom, 1);
    m(3, 2) = perspective;
    return m;
}

void Canvas::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton ||
        event->button() == Qt::RightButton)
    {
        mouse_pos = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton ||
        event->button() == Qt::RightButton)
    {
        unsetCursor();
    }
}

void Canvas::mouseMoveEvent(QMouseEvent* event)
{
    auto p = event->pos();
    auto d = p - mouse_pos;


    if (event->buttons() & Qt::LeftButton)
    {
        yaw = fmod(yaw - d.x(), 360);
        tilt = fmod(tilt - d.y(), 360);
        update();
    }
    else if (event->buttons() & Qt::RightButton)
    {
        center = transform_matrix().inverted() *
                 view_matrix().inverted() *
                 QVector3D(-d.x() / (0.5*width()),
                            d.y() / (0.5*height()), 0);
        update();
    }
    mouse_pos = p;
}

void Canvas::wheelEvent(QWheelEvent *event)
{
    // Find GL position before the zoom operation
    // (to zoom about mouse cursor)
    auto p = event->position();
    QVector3D v(1 - p.x() / (0.5*width()),
                p.y() / (0.5*height()) - 1, 0);
    QVector3D a = transform_matrix().inverted() *
                  view_matrix().inverted() * v;

    if (event->angleDelta().y() < 0)
    {
        for (int i=0; i > event->angleDelta().y(); --i)
            if (invertZoom)
                zoom /= 1.001;
            else 
                zoom *= 1.001;
    }
    else if (event->angleDelta().y() > 0)
    {
        for (int i=0; i < event->angleDelta().y(); ++i)
            if (invertZoom) 
                zoom *= 1.001;
            else 
                zoom /= 1.001;
    }

    // Then find the cursor's GL position post-zoom and adjust center.
    QVector3D b = transform_matrix().inverted() *
                  view_matrix().inverted() * v;
    center += b - a;
    update();
}

void Canvas::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
}
