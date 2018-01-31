#pragma once

#include "ui_QtOpenGL.h"

class QtOpenGL : public QDialog {
	Q_OBJECT
public:
	Ui::glDialog ui;
	QtOpenGL(QWidget *parent = Q_NULLPTR);
};