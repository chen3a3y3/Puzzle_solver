// myglwidget.cpp

#include <QtWidgets>
#include <qopengl.h>
#include <qopenglfunctions_3_2_core.h>

#include "GLWidget.h"
const float PI = 3.141592653589793f;

GLWidget::GLWidget(QWidget *parent)
	: QOpenGLWidget(parent)
{
	xRot = 0;
	yRot = 0;
	zRot = 0;
	m_frame = 0;
}

GLWidget::~GLWidget()
{
}

QSize GLWidget::minimumSizeHint() const
{
	return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
	return QSize(400, 400);
}

static void qNormalizeAngle(int &angle)
{
	while (angle < 0)
		angle += 360 * 16;
	while (angle > 360)
		angle -= 360 * 16;
}

void GLWidget::setXRotation(int angle)
{
	qNormalizeAngle(angle);
	if (angle != xRot) {
		xRot = angle;
		//emit xRotationChanged(angle);
		update();
	}
}

void GLWidget::setYRotation(int angle)
{
	qNormalizeAngle(angle);
	if (angle != yRot) {
		yRot = angle;
		m_program->bind();
		update();
	}
}

void GLWidget::setZRotation(int angle)
{
	qNormalizeAngle(angle);
	if (angle != zRot) {
		zRot = angle;
		update();
	}
}

void GLWidget::initializeGL()
{
	initializeOpenGLFunctions();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	GLfloat vertices[] = {
		0.0f, 0.f, 0.f, 1.f,
		0.0f, 1.f, 0.f, 1.f,
		1.0f, 0.f, 0.f, 1.f,
		1.0f, 1.f, 0.f, 1.f,
		0.0f, 0.f, 1.f, 1.f,
		0.0f, 1.f, 1.f, 1.f,
		1.0f, 0.f, 1.f, 1.f,
		1.0f, 1.f, 1.f, 1.f
	};

	GLfloat colors[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f
	};


	m_vao.create();
	if (m_vao.isCreated()) {
		m_vao.bind();
	}

	m_program = new QOpenGLShaderProgram(this);
	m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, "..\\QtAlgorithmX\\simple.vert");
	m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, "..\\QtAlgorithmX\\simple.frag");
	m_program->link();

	m_posAttr = m_program->attributeLocation("posAttr");
	m_colAttr = m_program->attributeLocation("colAttr");
	m_matrixUniform = m_program->uniformLocation("perspective");
	m_scaleUniform = m_program->uniformLocation("scale");
	m_rotationxUniform = m_program->uniformLocation("rotationx");
	m_rotationyUniform = m_program->uniformLocation("rotationy");
	m_rotationzUniform = m_program->uniformLocation("rotationz");

	m_vbo_vert.create();
	m_vbo_vert.bind();
	m_vbo_col.setUsagePattern(QOpenGLBuffer::StaticDraw);
	m_vbo_vert.allocate(vertices, sizeof(vertices));
	m_vbo_vert.release();

	m_vbo_col.create();
	m_vbo_col.bind();
	m_vbo_col.setUsagePattern(QOpenGLBuffer::StaticDraw);
	m_vbo_col.allocate(colors, sizeof(colors));
	m_vbo_col.release();

	m_program->bind();

	m_vbo_vert.bind();
	m_program->enableAttributeArray(m_posAttr);
	m_program->setAttributeBuffer(m_posAttr, GL_FLOAT, 0, 4);
	m_vbo_vert.release();

	m_vbo_col.bind();
	m_program->enableAttributeArray(m_colAttr);
	m_program->setAttributeBuffer(m_colAttr, GL_FLOAT, 0, 3);
	m_vbo_col.release();

	QMatrix4x4 matrix;
	matrix.perspective(60.0f, float(width()) / height(), 0.1f, 100.0f);
	matrix.translate(0, 0, -3);

	m_program->setUniformValue(m_matrixUniform, matrix);

	m_program->release();
	
	m_vao.release();


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	/*glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	static GLfloat lightPosition[4] = { 0, 0, 10, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);*/
}

void GLWidget::paintGL()
{
	QOpenGLFunctions_3_2_Core *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_2_Core>();
	//f->glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	
	const qreal retinaScale = devicePixelRatio();
	glViewport(0, 0, width() * retinaScale, height() * retinaScale);

	glClear(GL_COLOR_BUFFER_BIT);

	m_program->bind();

	m_program->setUniformValue(m_scaleUniform, 0.3f);
	m_program->setUniformValue(m_rotationxUniform, xRot / 180.f * PI);
	m_program->setUniformValue(m_rotationyUniform, yRot / 180.f * PI);
	m_program->setUniformValue(m_rotationzUniform, zRot / 180.f * PI);

	m_vao.bind();

	GLubyte indices[] = {
		0, 1, 2, 2, 1, 3,
		4, 6, 5, 5, 6, 7,
		0, 4, 1, 1, 4, 5,
		2 ,3, 6, 6, 3, 7,
		0, 2, 4, 4, 2, 6,
		1, 5, 3, 3, 5, 7
	};
	f->glDrawElementsInstanced(GL_TRIANGLES, sizeof(indices) / sizeof(GLubyte), GL_UNSIGNED_BYTE, indices, 27);

	m_vao.release();

	m_program->release();

	//++m_frame;
	//update();
	/*glLoadIdentity();
	glTranslatef(0.0, 0.0, -10.0);
	glRotatef(xRot / 16.0, 1.0, 0.0, 0.0);
	glRotatef(yRot / 16.0, 0.0, 1.0, 0.0);
	glRotatef(zRot / 16.0, 0.0, 0.0, 1.0);
	draw();*/
}

void GLWidget::resizeGL(int width, int height)
{
	/*int side = qMin(width, height);
	glViewport((width - side) / 2, (height - side) / 2, side, side);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
#ifdef QT_OPENGL_ES_1
	glOrthof(-2, +2, -2, +2, 1.0, 15.0);
#else
	glOrtho(-2, +2, -2, +2, 1.0, 15.0);
#endif
	glMatrixMode(GL_MODELVIEW);*/
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
	lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
	int dx = event->x() - lastPos.x();
	int dy = event->y() - lastPos.y();

	if (event->buttons() & Qt::LeftButton) {
		setXRotation(xRot + 4 * dy);
		setYRotation(yRot + 4 * dx);
	}
	else if (event->buttons() & Qt::RightButton) {
		setXRotation(xRot + 4 * dy);
		setZRotation(zRot + 4 * dx);
	}

	lastPos = event->pos();
}

void GLWidget::draw()
{
	glBegin(GL_QUADS);
	glNormal3f(0, 0, -1);
	glVertex3f(-1, -1, 0);
	glVertex3f(-1, 1, 0);
	glVertex3f(1, 1, 0);
	glVertex3f(1, -1, 0);

	glEnd();
	glBegin(GL_TRIANGLES);
	glNormal3f(0, -1, 0.707);
	glVertex3f(-1, -1, 0);
	glVertex3f(1, -1, 0);
	glVertex3f(0, 0, 1.2);
	glEnd();
	glBegin(GL_TRIANGLES);
	glNormal3f(1, 0, 0.707);
	glVertex3f(1, -1, 0);
	glVertex3f(1, 1, 0);
	glVertex3f(0, 0, 1.2);
	glEnd();
	glBegin(GL_TRIANGLES);
	glNormal3f(0, 1, 0.707);
	glVertex3f(1, 1, 0);
	glVertex3f(-1, 1, 0);
	glVertex3f(0, 0, 1.2);
	glEnd();
	glBegin(GL_TRIANGLES);
	glNormal3f(-1, 0, 0.707);
	glVertex3f(-1, 1, 0);
	glVertex3f(-1, -1, 0);
	glVertex3f(0, 0, 1.2);
	glEnd();
}