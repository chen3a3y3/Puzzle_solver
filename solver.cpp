#include "solver.h"
#include "viewer.h"
#include "input_process.h"
#include <time.h>
#include <assert.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

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


void Solver::dlx(vector<int>& result) {
	if (head->right == head) {
		if (this->viewer && show_details) {
			viewer->update(result, 0, &this->row2pos);
		}
		final_result.push_back(result);
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
		if (this->viewer && show_details) {
			viewer->update(result, 30, &this->row2pos);
		}
		cur = cur->down;
	}
	uncovering(next_to_head);

}


vector<vector<int>> Solver::solve(vector<vector<int>>& input) {
	if (show_details) cv::namedWindow("Progress", cv::WINDOW_AUTOSIZE);
	create_link_table(input);
	vector<int> result;
	long begin_time = clock();
	dlx(result);
	long end_time = clock();
	cout << end_time - begin_time << endl;
	if (show_details) cv::destroyWindow("Progress");
	return final_result;
}