#pragma once
#include <vector>
#include <string>
#include <memory>
#include <limits>
#include <iostream>
#include <chrono>
#include <thread>
#include <unordered_map>
using namespace std;

class Viewer;
class position_set;
class Object {
public:
	Object *l, *r, *u, *d, *c;
};

class DataObject : public Object {
public:
	int r_idx;
};

class ColumnObject : public Object {
public:
	int s;
	string name;
};

namespace yy {
	class Solver {
		vector<vector<int>> map;
		ColumnObject *h;
		// for maintaining strong reference
		vector<Object*> all_nodes;
		// for solution printing
		vector<Object*> path;
		// r_idx sols
		vector<vector<int>> sols;
		void coverCol(Object* c);
		void uncoverCol(Object* c);
		bool inited = false;
		void search(int &sol, long long int&, int k = 0);
		Viewer *viewer = nullptr;
		unordered_map<int, position_set> row2pos;
		int max_sol = 0;
		vector<int> path2rows(const vector<Object*> &);
		vector<vector<int>> rows2map(const vector<int> &);

	public:
		bool show_details = false;
		const vector<vector<int>>&getSols();
		void init(const vector<vector<int>> &mat, const unordered_map<int, position_set> &row2pos, Viewer *viewer);
		void solve(int num_of_sol);
	};
}
