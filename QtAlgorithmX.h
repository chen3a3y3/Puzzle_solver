#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets\qfiledialog.h>
#include "ui_QtAlgorithmX.h"

extern long beginTime, firstEndTime, endTime;

class QtAlgorithmX : public QMainWindow
{
	Q_OBJECT

public:
	void onFinished(int);
	bool requestedStop = false;
	Ui::QtAlgorithmXClass ui;
	QtAlgorithmX(QWidget *parent = Q_NULLPTR);
public slots:
	void onButtonClicked();
	void onFileSelected(QString);
	void on_startButton_clicked();
	void on_stopButton_clicked();
	void onDetailChecked(bool);
	void onSingleChecked(bool);
	
signals:
	void fileSelected(QString);
	void answerGot(QString);
	
private:
	std::string selected_file;
	int desktop_width;
	int desktop_height;
};
