#pragma once
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <stack>
#include "node.hpp"
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

	vector<vector<int>> solve(vector<vector<int>>& input);
	void create_link_table(vector<vector<int>>& input);
	void dlx(vector<vector<int>>& input, vector<int> result);
	void remove_link(Node* node, string mode);
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

void Solver::dlx(vector<vector<int>>& input, vector<int> result) {
	if (head->right == head) {
		final_result.push_back(result);
		return;
	}
	Node* next_to_head = head->right;
	remove_link(next_to_head, "v");

	Node* cur = next_to_head->down;
	stack<Node*> node_stack;
	while (cur != next_to_head) {
		node_stack.push(cur);
		cur = cur->down;
	}

	while (!node_stack.empty()) {
		Node* critical_node = node_stack.top();
		node_stack.pop();

	}


}

vector<vector<int>> Solver::solve(vector<vector<int>>& input) {
	create_link_table(input);
	vector<int> result;
	dlx(input, result);
	return final_result;
}

