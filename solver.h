#pragma once
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <stack>
#include <queue>
#include <unordered_map>
#include "node.h"


using namespace std;

class position_set;
class Viewer;
class Solver {
private:
	int row;
	int col;
	Node* head = new Node(-1, -1);
	vector<vector<int>> final_result;
	Viewer *viewer = nullptr;
	unordered_map<int, position_set> row2pos;
	
public:
	bool show_details = true;
	Solver(int row, int col, const unordered_map<int, position_set> &row2pos, Viewer *viewer) {
		this->row = row;
		this->col = col;
		this->viewer = viewer;
		this->row2pos = row2pos;
	}
	vector<vector<int>> solve(vector<vector<int>>& input);
	void create_link_table(vector<vector<int>>& input);
	void dlx(vector<int>& result);
	void covering(Node* node, int index);
	void uncovering(Node* node);
};


