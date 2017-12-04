#pragma once
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <stack>
#include <queue>
#include "node.hpp"
#include "input_process.hpp"
using namespace std;

class Solver {
private:
	int row;
	int col;
	Node* head = new Node(-1, -1, true);
	vector<vector<int>> final_result;
public:
	Solver(int row, int col) {
		this->row = row;
		this->col = col;
	}
	vector<vector<int>> solve(vector<vector<int>>& input, Input& in);
	void create_link_table(vector<vector<int>>& input);
	void dlx(vector<vector<int>>& input, vector<int>& result, int steps, Input& in);
	void remove_link(Node* node, string mode);
	Node* find_next(Node* node, string mode);
};


void Solver::create_link_table(vector<vector<int>>& input) {
	// create assistant node
	Node* cur = head;
	for (int i = 0; i < col; i++) {
		Node* node = new Node(-1, i, true);
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
			Node* node = new Node(i, j, false);
			if (count == 0) first_node = node;
			count++;
			node->left = cur;
			if (cur != nullptr) cur->right = node;
			cur = node;

			Node* assist = head->move_node("right", j + 1);
			Node* upper_node = assist->go_to_end();
			upper_node->down = node;
			node->up = upper_node;
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


Node* Solver::find_next(Node* node, string mode) {
	Node* cur = nullptr;
	if (mode == "right") {
		cur = node->right;
		while ((cur->o_mark != 0 || cur->p_mark != 0) && cur != node) {
			cur = cur->right;
		}
	}
	else if (mode == "left") {
		cur = node->left;
		while ((cur->o_mark != 0 || cur->p_mark != 0) && cur != node) {
			cur = cur->left;
		}
	}
	else if (mode == "up") {
		cur = node->up;
		while ((cur->o_mark != 0 || cur->p_mark != 0) && cur != node) {
			cur = cur->up;
		}
	}
	else if (mode == "down") {
		cur = node->down;
		while ((cur->o_mark != 0 || cur->p_mark != 0) && cur != node) {
			cur = cur->down;
		}
	}
	return cur;
}


void Solver::remove_link(Node* node, string mode) {
	// v = vertical; h = horizontal
	if (mode == "v") {
		Node* cur = node;
		do {
			cur->left->right = cur->right;
			cur->right->left = cur->left;
			cur = cur->down;
		} while (cur != node);
	}
	else if (mode == "h") {
		Node* cur = node;
		do {
			cur->up->down = cur->down;
			cur->down->up = cur->up;
			cur = cur->right;
		} while (cur != node);
	}
}


void Solver::dlx(vector<vector<int>>& input, vector<int>& result, int steps, Input& in) {
	if (find_next(head, "right") == head) {

		for (auto j : result) {
			position_set r = (*in.row2position)[j];
			cout << r.index << " " << r.offset.first << " " << r.offset.second << endl;
		}
		cout << endl;
		final_result.push_back(result);
		return;
	}
	Node* next_to_head = find_next(head, "right");
	if (find_next(next_to_head, "down") == next_to_head) {
		return;
	}

	// 插紫色点的合集，用于恢复
	vector<Node*> p_set;

	// next_to_head is C1， cur is 4
	Node* cur = find_next(next_to_head, "down");
	queue<Node*> node_queue;
	while (cur != next_to_head) {
		node_queue.push(cur);
		// cur_2 is 5
		// 把 4,10 一整行都弄成紫色
		Node* cur_2 = find_next(cur, "right");
		while (cur_2 != cur) {
			cur_2->p_mark = steps;
			p_set.push_back(cur_2);
			cur_2 = find_next(cur_2, "right");
			
		}
		cur->p_mark = steps;
		p_set.push_back(cur);
		cur = find_next(cur, "down");
	}
	next_to_head->p_mark = steps;
	p_set.push_back(next_to_head);

	while (!node_queue.empty()) {
		// 插橙色的点，用于恢复
		vector<Node*> o_set;

		// critical_node is 4
		Node* critical_node = node_queue.front();
		node_queue.pop();
		result.push_back(critical_node->row_index);

		// cur2 is 5
		Node* cur2 = critical_node->right;
		while (cur2 != critical_node) {
			// 把5一整列弄成橙色
			Node* ver_iter = cur2->down;
			while (ver_iter != cur2) {
				if (ver_iter->o_mark != 0 || ver_iter->p_mark != 0) {
					ver_iter = ver_iter->down;
					continue;
				}
				if (!ver_iter->is_head) {
					//把经过的节点的横行弄成橙色
					// hori_iter is 14
					Node* hori_iter = ver_iter->right;
					while (hori_iter != ver_iter) {
						if (hori_iter->p_mark == 0 && hori_iter->o_mark == 0) {
							hori_iter->o_mark = steps;
							o_set.push_back(hori_iter);
						}
						hori_iter = hori_iter->right;
					}
				}
				ver_iter->o_mark = steps;
				o_set.push_back(ver_iter);
				ver_iter = ver_iter->down;
			}
			cur2 = cur2->right;
		}
		
		// 开始递归调用
		dlx(input, result, steps + 1, in);

		// 开始恢复橙色的点
		result.pop_back();
		for (Node* o : o_set) {
			o->o_mark = 0;
		}
		o_set.clear();
	}

	//开始恢复紫色的点
	for (Node* p : p_set) {
		p->p_mark = 0;
	}
}

vector<vector<int>> Solver::solve(vector<vector<int>>& input, Input& in) {
	create_link_table(input);
	vector<int> result;
	dlx(input, result, 1, in);
	return final_result;
}

