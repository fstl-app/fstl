#include <QMouseEvent>
#include <QDebug>

#include <cmath>

#include "canvas.h"
#include "backdrop.h"
#include "glmesh.h"
#include "mesh.h"

Canvas::Canvas(const QGLFormat& format, QWidget *parent)
    : QGLWidget(format, parent), mesh(NULL),
      scale(1), zoom(1), tilt(90), yaw(0), status(" ")
{
    // Nothing to do here
}

Canvas::~Canvas()
{
    delete mesh;
}

void Canvas::load_mesh(Mesh* m)
{
    mesh = new GLMesh(m);
    center = QVector3D(m->xmin() + m->xmax(),
                       m->ymin() + m->ymax(),
                       m->zmin() + m->zmax()) / 2;
    scale = 2 / sqrt(
                pow(m->xmax() - m->xmin(), 2) +
                pow(m->ymax() - m->ymin(), 2) +
                pow(m->zmax() - m->zmin(), 2));

    // Reset other camera parameters
    zoom = 1;
    yaw = 0;
    tilt = 90;

    update();

    delete m;
}

void Canvas::set_status(const QString &s)
{
    status = s;
    update();
}

void Canvas::clear_status()
{
    status = "";
    update();
}

void Canvas::initializeGL()
{
    initializeGLFunctions();

    mesh_shader.addShaderFromSourceFile(QGLShader::Vertex, ":/gl/mesh.vert");
    mesh_shader.addShaderFromSourceFile(QGLShader::Fragment, ":/gl/mesh.frag");
    mesh_shader.link();

    backdrop = new Backdrop();
}

void Canvas::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    backdrop->draw();
    if (mesh)  draw_mesh();

    if (status.isNull())    return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawText(10, height() - 10, status);
}


void Canvas::draw_mesh()
{
    mesh_shader.bind();

    // Load the transform and view matrices into the shader
    glUniformMatrix4fv(
                mesh_shader.uniformLocation("transform_matrix"),
                1, GL_FALSE, transform_matrix().data());
    glUniformMatrix4fv(
                mesh_shader.uniformLocation("view_matrix"),
                1, GL_FALSE, view_matrix().data());

    // Compensate for z-flattening when zooming
    glUniform1f(mesh_shader.uniformLocation("zoom"), 1/zoom);

    // Find and enable the attribute location for vertex position
    const GLuint vp = mesh_shader.attributeLocation("vertex_position");
    glEnableVertexAttribArray(vp);

    // Then draw the mesh with that vertex position
    mesh->draw(vp);

    // Clean up state machine
    glDisableVertexAttribArray(vp);
    mesh_shader.release();
}

QMatrix4x4 Canvas::transform_matrix() const
{
    QMatrix4x4 m;
    m.rotate(tilt, QVector3D(1, 0, 0));
    m.rotate(yaw,  QVector3D(0, 0, 1));
    m.scale(scale);
    m.translate(-center);
    return m;
}

QMatrix4x4 Canvas::view_matrix() const
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
    m.scale(zoom, zoom, 1);
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
        tilt = fmax(0, fmin(180, tilt - d.y()));
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
    auto p = event->pos();
    QVector3D v(1 - p.x() / (0.5*width()),
                p.y() / (0.5*height()) - 1, 0);
    QVector3D a = transform_matrix().inverted() *
                  view_matrix().inverted() * v;

    if (event->delta() < 0)
    {
        for (int i=0; i > event->delta(); --i)
            zoom *= 1.001;
    }
    else if (event->delta() > 0)
    {
        for (int i=0; i < event->delta(); ++i)
            zoom /= 1.001;
    }

    // Then find the cursor's GL position post-zoom and adjust center.
    QVector3D b = transform_matrix().inverted() *
                  view_matrix().inverted() * v;
    center += b - a;
    update();
}
