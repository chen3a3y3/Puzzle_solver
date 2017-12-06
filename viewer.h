#pragma once
#include <unordered_map>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;

class position_set;
class Tile;
class Viewer {
	int n_types;
	int nrows, ncols;
	int res_x, res_y;
	unordered_map<int, cv::Vec3b> color_map;
	bool inited = false;
	cv::Mat img;
	Tile *board;
	
public:
	Viewer(int x, int y) : res_x(x), res_y(y) {}
	void init(int types, Tile *board);
	void save(const vector<int> &sol, unordered_map<int, position_set>*row2position, std::string filename);
	void update(const vector<int> &sol, int ms, unordered_map<int, position_set>*);
};