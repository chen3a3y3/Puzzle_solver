#pragma once
#include <iostream>
#include <string>

using namespace std;

class Node {
private:
	Node* left = nullptr;
	Node* right = nullptr;
	Node* up = nullptr;
	Node* down = nullptr;
	int row_index;
	int col_index;
	bool is_head;
	int p_mark = 0;
	int o_mark = 0;
public:
	friend class Solver;
	Node(int row_index = -1, int col_index = -1, bool is_head = false) {
		this->row_index = row_index;
		this->col_index = col_index;
		this->is_head = is_head;
	}; 

	Node* move_node(string dir, int times);
	Node* go_to_end();

};

