#include "viewer.h"
#ifdef AC2ME
#include "input_process.h"
#endif

void Viewer::init(int types, int rows, int cols)
{
	n_types = types;
	nrows = rows;
	ncols = cols;
	img = cv::Mat(cv::Size(ncols, nrows), CV_8UC3);
	for (int i = 0; i < n_types; i++) {
		color_map[i] = cv::Vec3b(static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 255.f,
			static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 255.f,
			static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 255.f);
	}
	inited = true;
}

void Viewer::update(const vector<vector<int> > &sol, int ms)
{
	if (!inited) return;
	// clear the img
	img = cv::Scalar(0, 0, 0);
	for (int i = 0; i < sol.size(); i++) {
		int type = 0;
		while (!sol[i][type++]);
		type--;
		assert(type < n_types);
		for (int j = n_types; j < sol[i].size(); j++) {
			if (sol[i][j]) {
				int idx = j - n_types;
				int r_idx = idx / ncols;
				int c_idx = idx % ncols;
				img.at<cv::Vec3b>(r_idx, c_idx) = color_map[type];
			}
		}
	}
	cv::Mat full_img;
	cv::resize(img, full_img, cv::Size(res_x, res_y), 0, 0, cv::INTER_NEAREST);
	cv::imshow("test", full_img);
	// std::this_thread::sleep_for(std::chrono::milliseconds(500));
	cv::waitKey(ms);
}

#ifdef AC2ME
void Viewer::update(vector<int> &sol, int ms, unordered_map<int, position_set>*row2position)
{
	if (!inited) return;
	// clear the img
	img = cv::Scalar(0, 0, 0);
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
	cv::imshow("test", full_img);
	// std::this_thread::sleep_for(std::chrono::milliseconds(500));
	cv::waitKey(ms);
}
#endif