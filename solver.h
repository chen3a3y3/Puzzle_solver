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
class Input;
class QtAlgorithmX;
class Solver {
private:
	int row;
	int col;
	Node* head = new Node(-1, -1);
	vector<vector<int>> final_result;
	vector<vector<int>> input_data;
	Viewer *viewer = nullptr;
	unordered_map<int, position_set> row2pos;
	vector<vector<vector<int>>*>* solution_set;
	Input *input = nullptr;
public:
	QtAlgorithmX *qt = nullptr;
	bool show_details = true;
	Solver(int row, int col, const unordered_map<int, position_set> &row2pos, Viewer *viewer, Input *input) {
		this->row = row;
		this->col = col;
		this->viewer = viewer;
		this->row2pos = row2pos;
		this->input = input;
		solution_set = new vector<vector<vector<int>>*>();
	}
	~Solver() {
		for (auto solution : *solution_set) {
			delete solution;
			solution = nullptr;
		}
		solution_set = nullptr;
		delete head;
		head = nullptr;
	}
	vector<vector<int>> solve(vector<vector<int>>& input);
	Node *create_link_table(const vector<vector<int>>& input);
	void dlx(vector<int>& result, Node *head, int k);
	void solve1(vector<int>& result, Node *head, Node *cur, int k, int r);
	void dlx1(vector<int>& result, Node *head, int k);
	void covering(Node* node, int index);
	void uncovering(Node* node);
	void rotate_180(vector<vector<int>>* input, vector<vector<int>>& output);
	bool check_is_valid_solution(vector<int>& result);
	bool _compare(vector<vector<int>>& a, vector<vector<int>>& b);
	void rotate_90(vector<vector<int>>* input, vector<vector<int>>& output);
	void updateFirstTime(long time);
	void updateAllTime(long time);
};
