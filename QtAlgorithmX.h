#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets\qfiledialog.h>
#include "ui_QtAlgorithmX.h"

class QtAlgorithmX : public QMainWindow
{
	Q_OBJECT

public:
	Ui::QtAlgorithmXClass ui;
	QtAlgorithmX(QWidget *parent = Q_NULLPTR);
public slots:
	void onButtonClicked();
	void onFileSelected(QString);
	void on_startButton_clicked();
signals:
	void fileSelected(QString);
	void answerGot(QString);
	
private:
	std::string selected_file;
	int desktop_width;
	int desktop_height;
	
};
