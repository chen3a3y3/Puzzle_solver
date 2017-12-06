// solver.cpp
#include "solver.h"
#include "viewer.h"
#include "input_process.h"
#include <time.h>
#include <assert.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "QtAlgorithmX.h"

long beginTime, firstEndTime, endTime;

void Solver::create_link_table(vector<vector<int>>& input) {
	// create assistant node
	Node* cur = head;
	for (int i = 0; i < col; i++) {
		Node* node = new Node(-1, i);
		node->C = node;
		node->count = 0;
		cur->right = node;
		node->left = cur;
		cur = node;
	}
	cur->right = head;
	head->left = cur;

	// create link table
	for (int i = 0; i < row; i++) {
		Node* cur = nullptr;
		Node* first_node = nullptr;
		int count = 0;
		for (int j = 0; j < col; j++) {
			if (input[i][j] == 0) continue;
			Node* node = new Node(i, j);
			if (count == 0) first_node = node;
			count++;
			node->left = cur;
			if (cur != nullptr) cur->right = node;
			cur = node;

			Node* assist = head->move_node("right", j + 1);
			Node* upper_node = assist->go_to_end();
			upper_node->down = node;
			node->up = upper_node;

			assist->count++;
			node->C = assist;
		}
		if (first_node != nullptr) {
			cur->right = first_node;
			first_node->left = cur;
		}
	}

	// set the link list is vertical circulatory
	for (int i = 0; i < col; i++) {
		Node* assist = head->move_node("right", i + 1);
		Node* last = assist->go_to_end();
		last->down = assist;
		assist->up = last;
	}
}


// uncovering operation
void Solver::uncovering(Node* node) {
	Node* i = node->up;
	while (i != node) {
		Node* j = i->left;
		while (j != i) {
			j->C->count++;
			j->down->up = j;
			j->up->down = j;
			j = j->left;
		}
		i = i->up;
	}
	node->right->left = node;
	node->left->right = node;
}


// covering operation
void Solver::covering(Node* node, int index = INT_MAX) {
	node->left->right = node->right;
	node->right->left = node->left;
	Node* i = node->down;
	while (i != node) {
		Node* j = i->right;
		while (j != i) {
			j->down->up = j->up;
			j->up->down = j->down;
			j->C->count--;
			j = j->right;
		}
		i = i->down;
	}
}


bool Solver::_compare(vector<vector<int>>& a, vector<vector<int>>& b) {
	assert(a.size() == b.size());
	assert(a[0].size() == b[0].size());

	for (int i = 0; i < a.size(); i++) {
		for (int j = 0; j < a[0].size(); j++) {
			if (a[i][j] != b[i][j]) return false;
		}
	}
	return true;
}


void Solver::rotate_90(vector<vector<int>>* input, vector<vector<int>>& output) {
	int row = (*input).size(), col = (*input)[0].size();
	assert(row == col);
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			output[i][j] = (*input)[j][col - 1 - i];
		}
	}
}


void Solver::rotate_180(vector<vector<int>>* input, vector<vector<int>>& output) {
	int row = (*input).size(), col = (*input)[0].size();
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			output[i][j] = (*input)[row - i - 1][col - j - 1];
		}
	}
}


bool Solver::check_is_valid_solution(vector<int>& result) {
	bool is_square = (input->board->right == input->board->down);
	vector<vector<int>> *this_solution = new vector<vector<int>>(input->board->down + 1, vector<int>(input->board->right + 1, -1));
	for (int row : result) {
		position_set a = row2pos[row];
		pair<int, int> pa = a.offset;
		int index = a.index;
		Tile* tile = a.tile;
		for (Point* p : tile->point_set) {
			(*this_solution)[p->row_index + pa.first][p->col_index + pa.second] = index;
		}
	}
	vector<vector<vector<int>>> this_solution_set;
	this_solution_set.push_back(*this_solution);
	if (is_square) {
		vector<vector<int>> output_1(input->board->down + 1, vector<int>(input->board->right + 1, -1));
		vector<vector<int>> output_2(input->board->down + 1, vector<int>(input->board->right + 1, -1));
		vector<vector<int>> output_3(input->board->down + 1, vector<int>(input->board->right + 1, -1));
		rotate_90(this_solution, output_1);
		rotate_180(this_solution, output_2);
		rotate_90(&output_2, output_3);
		this_solution_set.push_back(output_1);
		this_solution_set.push_back(output_2);
		this_solution_set.push_back(output_3);
	}
	else {
		vector<vector<int>> output_1(input->board->down + 1, vector<int>(input->board->right + 1, -1));
		rotate_180(this_solution, output_1);
		this_solution_set.push_back(output_1);
	}
	for (auto solution : (*solution_set)) {
		for (auto a : this_solution_set) {
			if (_compare(*solution, a)) {
				delete this_solution;
				this_solution = nullptr;
				return false;
			}
		}
	}
	(*solution_set).push_back(this_solution);
	return true;
}


// dancing link
void Solver::dlx(vector<int>& result) {
	if (qt->requestedStop) return;
	if (head->right == head) {
		if (check_is_valid_solution(result)) {
			if (this->viewer && show_details) {
				viewer->update(result, 0, &this->row2pos);
			}
			if (final_result.empty()) {
				firstEndTime = clock();
				updateFirstTime(firstEndTime - beginTime);
			}
			final_result.push_back(result);
		}
		return;
	}

	int min_count = INT_MAX;
	Node* _cur = head->right;
	Node* next_to_head = nullptr;
	while (_cur != head) {
		if (_cur->count < min_count) {
			min_count = _cur->count;
			next_to_head = _cur;
		}
		_cur = _cur->right;
	}

	covering(next_to_head);
	Node* cur = next_to_head->down;
	while (cur != next_to_head) {
		result.push_back(cur->row_index);
		if (this->viewer && (show_details || qt->ui.singleBox->isChecked())) {
			viewer->update(result, show_details ? 10 : 0, &this->row2pos);
		}

		// covering
		for (auto j = cur->right; j != cur; j = j->right) {
			covering(j->C);
		}

		dlx(result);
		result.pop_back();
		// uncovering
		for (auto j = cur->left; j != cur; j = j->left) {
			uncovering(j->C);
		}
		if (this->viewer && (show_details || qt->ui.singleBox->isChecked())) {
			viewer->update(result, show_details ? 10 : 0, &this->row2pos);
		}
		cur = cur->down;
	}
	uncovering(next_to_head);

}


vector<vector<int>> Solver::solve(vector<vector<int>>& input) {
	if (show_details) cv::namedWindow("Progress", cv::WINDOW_AUTOSIZE);
	create_link_table(input);
	vector<int> result;
	dlx(result);
	if (show_details) cv::destroyWindow("Progress");
	
	return final_result;
}

void Solver::updateFirstTime(long time) {
	QString s = QString::number(time);
	qt->ui.t1Label->setText("Time to find the first solution: "  + s + "ms");
}

void Solver::updateAllTime(long time) {
	QString s = QString::number(time);
	qt->ui.t2Label->setText("Time to find all solutions: " + s + "ms");
}
