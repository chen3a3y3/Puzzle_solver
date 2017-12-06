// viewer.cpp
#include "viewer.h"
#include "input_process.h"
#include "tile.h"

void Viewer::init(int types, Tile *board)
{
	n_types = types;
	nrows = board->down + 1;
	ncols = board->right + 1;
	this->board = board;
	img = cv::Mat(cv::Size(ncols, nrows), CV_8UC3);
	for (int i = 0; i < n_types; i++) {
		color_map[i] = cv::Vec3b(static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 255.f,
			static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 255.f,
			static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 255.f);
	}
	inited = true;
}

void Viewer::save(const vector<int> &sol, unordered_map<int, position_set>*row2position, std::string filename) {
	if (!inited) return;
	// clear the img
	img = cv::Scalar(0, 0, 0);
	for (auto p : board->point_set) {
		img.at<cv::Vec3b>(p->row_index, p->col_index) = cv::Vec3b(40, 40, 40);
	}
	for (int i = 0; i < sol.size(); i++) {
		auto position_set = (*row2position)[sol[i]];
		for (const auto &t : position_set.tile->point_set) {
			int r_idx = t->row_index + position_set.offset.first;
			int c_idx = t->col_index + position_set.offset.second;
			img.at<cv::Vec3b>(r_idx, c_idx) = color_map[position_set.index];
		}
	}
	cv::Mat full_img;
	cv::resize(img, full_img, cv::Size(res_x, res_y), 0, 0, cv::INTER_NEAREST);
	cv::imwrite(filename, full_img);
}

void Viewer::update(const vector<int> &sol, int ms, unordered_map<int, position_set>*row2position)
{
	if (!inited) return;
	// clear the img
	img = cv::Scalar(0, 0, 0);
	for (auto p : board->point_set) {
		img.at<cv::Vec3b>(p->row_index, p->col_index) = cv::Vec3b(40, 40, 40);
	}
	for (int i = 0; i < sol.size(); i++) {
		auto position_set = (*row2position)[sol[i]];
		for (const auto &t : position_set.tile->point_set) {
			int r_idx = t->row_index + position_set.offset.first;
			int c_idx = t->col_index + position_set.offset.second;
			img.at<cv::Vec3b>(r_idx, c_idx) = color_map[position_set.index];
		}
	}
	cv::Mat full_img;
	cv::resize(img, full_img, cv::Size(res_x, res_y), 0, 0, cv::INTER_NEAREST);
	cv::imshow("Progress", full_img);
	// std::this_thread::sleep_for(std::chrono::milliseconds(500));
	cv::waitKey(ms);
}
