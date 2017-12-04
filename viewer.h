#pragma once
#include "solver.h"
#include <unordered_map>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class Viewer {
	int n_types;
	int nrows, ncols;
	int res_x, res_y;
	unordered_map<int, cv::Vec3b> color_map;
	bool inited = false;
	cv::Mat img;
	
public:
	Viewer(int x, int y) : res_x(x), res_y(y) {}
	void init(int types, int rows, int cols);
	void update(const vector<vector<int> > &sol, int ms);
};