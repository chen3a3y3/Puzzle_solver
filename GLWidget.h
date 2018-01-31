#pragma once

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <qopenglfunctions.h>

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT
public:
	explicit GLWidget(QWidget *parent = 0);
	~GLWidget();

	void setXRotation(int angle);
	void setYRotation(int angle);
	void setZRotation(int angle);

protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);

	QSize minimumSizeHint() const;
	QSize sizeHint() const;
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

private:
	void draw();

	int xRot;
	int yRot;
	int zRot;
	int m_frame;

	int m_posAttr, m_colAttr, m_matrixUniform;
	int m_scaleUniform, m_rotationxUniform, m_rotationyUniform, m_rotationzUniform;

	QPoint lastPos;
	QOpenGLVertexArrayObject m_vao;
	QOpenGLBuffer m_vbo_vert, m_vbo_col;
	QOpenGLShaderProgram *m_program;
};

#endif // GLWIDGET_H