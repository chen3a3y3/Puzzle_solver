#include "QtAlgorithmX.h"
#include "input_process.hpp"
#include <iostream>
#include "solver.h"
// #include "solver.hpp"
#include "viewer.h"

QtAlgorithmX::QtAlgorithmX(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
}

void QtAlgorithmX::onButtonClicked()
{
	auto fileName = QFileDialog::getOpenFileName(this,
		tr("Open File"), "/", tr("Text files (*.txt)"));
	emit fileSelected(fileName);
}

void QtAlgorithmX::onFileSelected(QString qs) {
	std::string file = qs.toLocal8Bit().constData();
	vector<vector<int>> test;
	Input input = Input();
	input.input_process(file, test, true);
	int num = input.tile_number;
	int c = input.board->right + 1;
	int r = input.board->down + 1;

	Viewer viewer(c * 60, r * 60);
	viewer.init(num, r, c);

	Solver solver;
	solver.show_details = ui.radioButton->isChecked();
	solver.init(test, &viewer);
	solver.solve(1000000);

	const auto &sol = solver.getSols();
	QString s = QString::number(sol.size());
	emit answerGot(s);

	/*Solver S = Solver(test.size(), test[0].size());
	vector<vector<int>> result = S.solve(test, input);
	QString s = QString::number(result.size());
	emit answerGot(s);*/
}
