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
	Node* C = nullptr;
	int count = -1;
	int row_index;
	int col_index;
public:
	friend class Solver;
	Node(int row_index = -1, int col_index = -1) {
		this->row_index = row_index;
		this->col_index = col_index;
	}; 

	Node* move_node(string dir, int times);
	Node* go_to_end();

};

