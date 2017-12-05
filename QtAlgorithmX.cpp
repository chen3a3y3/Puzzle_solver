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
	vector<vector<vector<int>>> test;
	Input input = Input();
	if (!input.input_process(selected_file, test, true)) return;
	int num = input.total_tile_number;
	int c = input.board->right + 1;
	int r = input.board->down + 1;

	Viewer viewer(c * desktop_height / 20, r * desktop_height / 20);
	viewer.init(num, input.board);

#ifdef AC2ME
	int i = 0;
	int total_result = 0;
	for (auto &single: test) {
		Solver S = Solver(single.size(), single[0].size(), input.row2positions[i++], &viewer, &input);
		S.show_details = ui.radioButton->isChecked();
		vector<vector<int>> result = S.solve(single);
		total_result += result.size();
	}
	QString s = QString::number(total_result);
	emit answerGot(s);
#else

	int i = 0;
	for (const auto &single : test) {
		yy::Solver solver;
		solver.show_details = ui.radioButton->isChecked();
		solver.init(single, input.row2positions[i++], &viewer);
		solver.solve(1000000);

		const auto &sol = solver.getSols();
		QString s = QString::number(sol.size());
		emit answerGot(s);
	}
	
#endif
}

void QtAlgorithmX::onFileSelected(QString qs) {
	selected_file = qs.toLocal8Bit().constData();
}
