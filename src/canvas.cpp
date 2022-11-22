#include <QMouseEvent>

#include <cmath>

#include "canvas.h"
#include "backdrop.h"
#include "axis.h"
#include "glmesh.h"
#include "mesh.h"

const float Canvas::P_PERSPECTIVE = 0.25f;
const float Canvas::P_ORTHOGRAPHIC = 0.0f;

const QString Canvas::AMBIENT_COLOR = "ambientColor";
const QString Canvas::AMBIENT_FACTOR = "ambientFactor";
const QString Canvas::DIRECTIVE_COLOR = "directiveColor";
const QString Canvas::DIRECTIVE_FACTOR = "directiveFactor";
const QString Canvas::CURRENT_LIGHT_DIRECTION = "currentLightDirection";

const QColor Canvas::defaultAmbientColor = QColor::fromRgbF(0.22,0.8,1.0);
const QColor Canvas::defaultDirectiveColor = QColor(255,255,255);
const float Canvas::defaultAmbientFactor = 0.67;
const float Canvas::defaultDirectiveFactor = 0.5;
const int Canvas::defaultCurrentLightDirection = 1;

Canvas::Canvas(const QSurfaceFormat& format, QWidget *parent)
    : QOpenGLWidget(parent), mesh(nullptr),
      scale(1), zoom(1),
      anim(this, "perspective"), status(" "),
      meshInfo("")
{
    setFormat(format);
    QFile styleFile(":/qt/style.qss");
    styleFile.open( QFile::ReadOnly );
    setStyleSheet(styleFile.readAll());
    currentTransform = QMatrix4x4();
    resetTransform();

    QSettings settings;
    ambientColor = settings.value(AMBIENT_COLOR,defaultAmbientColor).value<QColor>();
    directiveColor = settings.value(DIRECTIVE_COLOR,defaultDirectiveColor).value<QColor>();
    ambientFactor = settings.value(AMBIENT_FACTOR,defaultAmbientFactor).value<float>();
    directiveFactor = settings.value(DIRECTIVE_FACTOR,defaultDirectiveFactor).value<float>();

    // Fill direction list
    // Fill in directions
    nameDir.clear();
    listDir.clear();
    QList<QString> xname, yname, zname;
    xname << "right " << " " << "left ";
    yname << "top " << " " << "bottom ";
    zname << "rear " << " " << "front ";
    for (int i=-1; i<2 ; i++) {
        for (int j=-1; j<2; j++) {
            for (int k=-1; k<2; k++) {
                QString current = xname.at(i+1) + yname.at(j+1) + zname.at(k+1);
                if (!(i==0 && j==0 && k==0)) {
                    nameDir << current.simplified();
                    listDir << QVector3D((double)i,(double)j,(double)k);
                }
            }
        }
    }
    currentLightDirection = settings.value(CURRENT_LIGHT_DIRECTION,defaultCurrentLightDirection).value<int>();
    if (currentLightDirection < 0 || currentLightDirection >= nameDir.length()) {
        currentLightDirection = defaultCurrentLightDirection;
    }

    anim.setDuration(100);
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
    anim.setStartValue(perspective);
    anim.setEndValue(v);
    anim.start();
}

void Canvas::common_view_change(enum ViewPoint c){
  switch (c) {
    break; default: return ;
    break; case centerview: scale = default_scale; center = default_center; zoom = 1;
    break; case topview:    yaw = 0;   tilt = 0;
    break; case bottomview: yaw = 0;   tilt = 180;
    break; case leftview:   yaw = -90; tilt = 90;
    break; case rightview : yaw = 90;  tilt = 90;
    break; case frontview:  yaw = 0;   tilt = 90;
    break; case backview:   yaw = 180; tilt = 90;
  }
  update();
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

void Canvas::setResetTransformOnLoad(bool d) {
    resetTransformOnLoad = d;
}

void Canvas::resetTransform() {
    currentTransform.setToIdentity();
    // apply some rotations to define initial orientation
    currentTransform.rotate(-90.0, QVector3D(1, 0, 0));
    currentTransform.rotate(180.0 + 15.0, QVector3D(0, 0, 1));
    currentTransform.rotate(15.0, QVector3D(1, -sin(M_PI/12), 0));
    
    zoom = 1;
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
        if (resetTransformOnLoad) {
            resetTransform();
        }
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
    mesh_meshlight_shader.addShader(mesh_vertshader);
    mesh_meshlight_shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/gl/mesh_light.frag");
    mesh_meshlight_shader.link();

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
        else if (drawMode == surfaceangle)
        {
            selected_mesh_shader = &mesh_surfaceangle_shader;
        }
        else if (drawMode == meshlight)
        {
            selected_mesh_shader = &mesh_meshlight_shader;
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

    // specific meshlight arguments
    if (drawMode == meshlight) {
        // Ambient Light Color, followed by the ambient light coefficient to use
        //glUniform4f(selected_mesh_shader->uniformLocation("ambient_light_color"),0.22f, 0.8f, 1.0f, 0.67f);
        glUniform4f(selected_mesh_shader->uniformLocation("ambient_light_color"),ambientColor.redF(), ambientColor.greenF(), ambientColor.blueF(), ambientFactor);
        // Directive Light Color, followed by the directive light coefficient to use
        //glUniform4f(selected_mesh_shader->uniformLocation("directive_light_color"),1.0f,1.0f,1.0f,0.5f);
        glUniform4f(selected_mesh_shader->uniformLocation("directive_light_color"),directiveColor.redF(),directiveColor.greenF(),directiveColor.blueF(),directiveFactor);

        // Directive Light Direction
        // dir 1,0,0  Light from the left
        // dir -1,0,0 Light from the right
        // dir 0,1,0  Light from bottom
        // dir 0,-1,0 Light from top
        // dir 0,0,1  Light from viewer (front)
        // dir 0,0,-1 Light from behind
        //
        // -1,-1,0 Light from top right
        //glUniform3f(selected_mesh_shader->uniformLocation("directive_light_direction"),-1.0f,-1.0f,0.0f);
        glUniform3f(selected_mesh_shader->uniformLocation("directive_light_direction"),listDir.at(currentLightDirection).x(), listDir.at(currentLightDirection).y(), listDir.at(currentLightDirection).z());
    }

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
    QMatrix4x4 m = currentTransform;
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


// This method change the referential of the mouse point coordinates
// into a referential x=[-1.0,1.0], y=[-1.0,1.0], with 0,0 being the
// center of the widget.
QPointF Canvas::changeMouseCoordinates(QPoint p) {
    QPointF pr;
    // Change coordinates
    double ws2 = this->width() / 2.0;
    double hs2 = this->height() / 2.0;
    pr.setX(p.x() / ws2 - 1.0);
    pr.setY(p.y() / hs2 - 1.0);
    return pr;
}

void Canvas::calcArcballTransform(QPointF p1, QPointF p2) {
    // Calc z1 & z2
    double x1 = p1.x();
    double x2 = p2.x();
    double y1 = p1.y();
    double y2 = p2.y();
    double p1sq = x1 * x1 + y1 * y1;
    double z1;
    if (p1sq <= 1) {
        z1 = sqrt(1.0 - p1sq);
    } else {
        x1 = x1 / sqrt(p1sq);
        y1 = y1 / sqrt(p1sq);
        z1 = 0.0;
    }
    double p2sq = x2 * x2 + y2 * y2;
    double z2;
    if (p2sq <= 1) {
        z2 = sqrt(1.0 - p2sq);
    } else {
        x2 = x2 / sqrt(p2sq);
        y2 = y2 / sqrt(p2sq);
        z2 = 0.0;
    }

    // set v1 and v2
    QVector3D v1(x1, y1, z1);
    QVector3D v2(x2, y2, z2);

    // calc v1 cross v2
    QVector3D v1xv2 = QVector3D::crossProduct(v1, v2);
    QVector3D v1xv2Obj = currentTransform.inverted().mapVector(v1xv2);

    // calc angle
    double angle = acos(std::min(1.0f,QVector3D::dotProduct(v1, v2))) * 180.0 / M_PI;
    
    // apply transform
    currentTransform.rotate(angle,v1xv2Obj);
}

void Canvas::mouseMoveEvent(QMouseEvent* event)
{
    auto p = event->pos();
    auto d = p - mouse_pos;
    

    if (event->buttons() & Qt::LeftButton)
    {
        QPointF p1r = changeMouseCoordinates(mouse_pos);
        QPointF p2r = changeMouseCoordinates(p);
        calcArcballTransform(p1r,p2r);

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

QColor Canvas::getAmbientColor() {
    return ambientColor;
}

void Canvas::setAmbientColor(QColor c) {
    ambientColor = c;
    QSettings settings;
    settings.setValue(AMBIENT_COLOR,c);
}

double Canvas::getAmbientFactor() {
    return (float) ambientFactor;
}

void Canvas::setAmbientFactor(double f) {
    ambientFactor = (float) f;
    QSettings settings;
    settings.setValue(AMBIENT_FACTOR,f);
}

void Canvas::resetAmbientColor() {
    setAmbientColor(defaultAmbientColor);
    setAmbientFactor(defaultAmbientFactor);
}

QColor Canvas::getDirectiveColor() {
    return directiveColor;
}

void Canvas::setDirectiveColor(QColor c) {
    directiveColor = c;
    QSettings settings;
    settings.setValue(DIRECTIVE_COLOR,c);
}

double Canvas::getDirectiveFactor() {
    return (float) directiveFactor;
}

void Canvas::setDirectiveFactor(double f) {
    directiveFactor = (float) f;
    QSettings settings;
    settings.setValue(DIRECTIVE_FACTOR,f);
}

void Canvas::resetDirectiveColor() {
    setDirectiveColor(defaultDirectiveColor);
    setDirectiveFactor(defaultDirectiveFactor);
}

QList<QString> Canvas::getNameDir() {
    return nameDir;
}

int Canvas::getCurrentLightDirection() {
    return currentLightDirection;
}

void Canvas::setCurrentLightDirection(int ind) {
    currentLightDirection = ind;
    QSettings settings;
    settings.setValue(CURRENT_LIGHT_DIRECTION,currentLightDirection);
}

void Canvas::resetCurrentLightDirection() {
    setCurrentLightDirection(defaultCurrentLightDirection);
}
