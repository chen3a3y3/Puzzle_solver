#include "QtAlgorithmX.h"
#include <qdesktopwidget.h>
#include <iostream>
#include <fstream>
#ifdef AC2ME
#include "solver.h"
#include "viewer.h"
#include "input_process.h"
#else
#include "solver_yy.h"
#include "viewer.h"
#include "input_process.h"
#endif // AC2ME



QtAlgorithmX::QtAlgorithmX(QWidget *parent)
	: QMainWindow(parent)
{
	QRect rec = QApplication::desktop()->screenGeometry();
	desktop_height = rec.height();
	desktop_width = rec.width();
	
	
	ui.setupUi(this);
	this->setGeometry(QRect(desktop_width / 4, desktop_height / 4,
		desktop_width / 2, desktop_height / 2));
//	auto geo = ui.centralWidget->geometry();
//	ui.gridLayout->setGeometry(ui.centralWidget->geometry());
}

void QtAlgorithmX::onButtonClicked()
{
	auto fileName = QFileDialog::getOpenFileName(this,
		tr("Open File"), "/", tr("Text files (*.txt)"));
	emit fileSelected(fileName);
}

void QtAlgorithmX::on_startButton_clicked() {
	std::ifstream infile(selected_file);
	if (!infile.good()) return;
	vector<vector<int>> test;
	Input input = Input();
	input.input_process(selected_file, test, true);
	int num = input.tile_number;
	int c = input.board->right + 1;
	int r = input.board->down + 1;

	Viewer viewer(c * desktop_height / 20, r * desktop_height / 20);
	viewer.init(num, r, c);

#ifdef AC2ME
	Solver S = Solver(test.size(), test[0].size(), &input, &viewer);
	S.show_details = ui.radioButton->isChecked();
	vector<vector<int>> result = S.solve(test, input);
	QString s = QString::number(result.size());
	emit answerGot(s);
#else


	yy::Solver solver;
	solver.show_details = ui.radioButton->isChecked();
	solver.init(test, &input, &viewer);
	solver.solve(1000000);

	const auto &sol = solver.getSols();
	QString s = QString::number(sol.size());
	emit answerGot(s);
#endif
}

void QtAlgorithmX::onFileSelected(QString qs) {
	selected_file = qs.toLocal8Bit().constData();
}
