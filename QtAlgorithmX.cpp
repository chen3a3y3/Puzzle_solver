#include "QtAlgorithmX.h"
#include <qdesktopwidget.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <future>
#include "solver.h"
#include "viewer.h"
#include "input_process.h"

extern long beginTime, endTime, endFirstTime;

void solve(QtAlgorithmX *qt, shared_ptr<vector<vector<vector<int>>>> test, 
	shared_ptr<Input> input, shared_ptr<Viewer> viewer) {
	auto beginTime = clock();
	int total_result = 0;
	for (int i = 0; i < test->size(); i++) {
		auto &single = (*test)[i];
		Solver S(single.size(), single[0].size(), input->row2positions[i++], viewer.get(), input.get());
		S.show_details = qt->ui.detailBox->isChecked();
		S.qt = qt;
		vector<vector<int>> result = S.solve(single);
		total_result += result.size();
	}

	auto endTime = clock();
	qt->onFinished(total_result);
	QString s = QString::number(endTime - beginTime);
	qt->ui.t2Label->setText("Time to find all solutions: " + s + "ms");
}

void QtAlgorithmX::onFinished(int num_of_sols) {
	auto s = QString::number(num_of_sols);
	emit answerGot("Total number of solutions: " + s);
}

void QtAlgorithmX::onDetailChecked(bool checked) {
	if (checked) ui.singleBox->setChecked(false);
}

void QtAlgorithmX::onSingleChecked(bool checked) {
	if (checked) ui.detailBox->setChecked(false);
}

QtAlgorithmX::QtAlgorithmX(QWidget *parent)
	: QMainWindow(parent)
{
	QRect rec = QApplication::desktop()->screenGeometry();
	desktop_height = rec.height();
	desktop_width = rec.width();
	
	
	ui.setupUi(this);
	this->setGeometry(QRect(desktop_width / 4, desktop_height / 4,
		desktop_width / 2, desktop_height / 2));
	//QPixmap bkgnd("..\\AlgorithmX\\q.jpg");
	//bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
	//QPalette palette;
	//palette.setBrush(QPalette::Background, bkgnd);
	//this->setPalette(palette);
}

void QtAlgorithmX::onButtonClicked()
{
	auto fileName = QFileDialog::getOpenFileName(this,
		tr("Open File"), "/", tr("Text files (*.txt)"));
	emit fileSelected(fileName);
}

void QtAlgorithmX::on_stopButton_clicked() {
	requestedStop = true;
}

void QtAlgorithmX::on_startButton_clicked() {
	requestedStop = false;
	beginTime = clock();
	std::ifstream infile(selected_file);
	if (!infile.good()) return;

	// input process
	auto test = make_shared<vector<vector<vector<int>>>>();
	auto input = make_shared<Input>();
	if (!input->input_process(selected_file, *test, ui.frBox->isChecked())) return;
	int num = input->total_tile_number;
	int c = input->board->right + 1;
	int r = input->board->down + 1;

	auto viewer = make_shared<Viewer>(c * desktop_height / 20, r * desktop_height / 20);
	viewer->init(num, input->board);

	// save solution
	/*if (result.empty()) return;
	int count = 0;
	for (; count < 5; count++) {
		string path = "C:\\Users\\41359\\Desktop\\" + to_string(count) + ".jpg";
		int number = rand() % result.size();
		viewer.save(result[number], &input.row2positions.back(), path);
	}*/

	auto th = std::thread(solve, this, test, input, viewer);
	th.detach();
}

void QtAlgorithmX::onFileSelected(QString qs) {
	selected_file = qs.toLocal8Bit().constData();
}
