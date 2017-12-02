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

Node* Node::move_node(string dir, int times) {
	try {
		Node* cur = this;
		for (int i = 0; i < times; i++) {
			if (dir == "right")
				cur = cur->right;
			else if (dir == "left")
				cur = cur->left;
			else if (dir == "up")
				cur = cur->up;
			else if (dir == "down")
				cur = cur->down;
		}
		return cur;
	}
	catch(...){
		cout << "error" << endl;
		cout << row_index << " " << col_index << endl;
		system("pause");
		exit(-1);
	}
}

Node* Node::go_to_end() {
	Node* cur = this;
	while (cur->down != nullptr) {
		cur = cur->down;
	}
	return cur;
}