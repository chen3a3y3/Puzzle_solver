// main.cpp
// Created time: 2017.12.1
// Created by: Angyang Chen, Yongyi Yang
// Problem Description: The goal of this project is to solving a tiling puzzle. The input is a ASCII file which contains all the input data, including the board to be joint and the tiles we can use. The output should be a set of pictures showing the result of the solutions saved to desktop. In the output, different tiles should be marked with different color and be put into the board with no overlap or gap.  We also build up a friendly user interface using QT development tools to allow user choose input file and select some options. Also the output can be shown using this interface.

#include "QtAlgorithmX.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QtAlgorithmX w;
	w.show();
	return a.exec();
}
