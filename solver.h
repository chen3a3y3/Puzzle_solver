#pragma once
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <stack>
#include <queue>
#include "node.h"


using namespace std;

class Input;
class Viewer;
class Solver {
private:
	int row;
	int col;
	Node* head = new Node(-1, -1, true);
	vector<vector<int>> final_result;
	Input *input = nullptr;
	Viewer *viewer = nullptr;
	
public:
	bool show_details = true;
	Solver(int row, int col, Input *inp, Viewer *viewer) {
		this->row = row;
		this->col = col;
		this->input = inp;
		this->viewer = viewer;
	}
	vector<vector<int>> solve(vector<vector<int>>& input, Input& in);
	void create_link_table(vector<vector<int>>& input);
	void dlx(vector<vector<int>>& input, vector<int>& result, int steps, Input& in);
	void remove_link(Node* node, string mode);
	Node* find_next(Node* node, string mode);
};


