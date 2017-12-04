#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets\qfiledialog.h>
#include "ui_QtAlgorithmX.h"

class QtAlgorithmX : public QMainWindow
{
	Q_OBJECT

public:
	QtAlgorithmX(QWidget *parent = Q_NULLPTR);
public slots:
	void onButtonClicked();
	void onFileSelected(QString);
signals:
	void fileSelected(QString);
	void answerGot(QString);

private:
	Ui::QtAlgorithmXClass ui;
};
