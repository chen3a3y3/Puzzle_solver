#pragma once
#include <vector>
#include <string>
#include <memory>
#include <limits>
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <map>
#include <unordered_map>
using namespace std;

class Viewer;
class position_set;
class QtAlgorithmX;
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
		mutex update_mutex;
		shared_mutex rw_mutex;
		int ncols, nrows;
		int boardrows, boardcols;
		vector<vector<int>> map;
		ColumnObject *h;
		// for maintaining thread heads
		vector<Object*> heads;
		// for solution printing per thread
		vector<vector<Object*>> paths;
		// r_idx sols
		vector<vector<int>> sols;
		vector<vector<vector<int>>> sols_map;
		void coverCol(Object* c);
		void uncoverCol(Object* c);
		bool inited = false;
		void search(int idx, Object * head, int k = 0);
		void searchThread(int k, int start, int nrows, Object *head, vector<Object *> rhs);
		Viewer *viewer = nullptr;
		// for thread random execution
		std::map<int, int> shuffle_map;
		unordered_map<int, position_set> row2pos;
		int max_sol = 0;
		vector<vector<int>> rows2board(const vector<int> &);
		vector<int> path2rows(const vector<Object*> &);
		bool isValid(const vector<vector<int>>&);
		vector<vector<int>> rotate90(const vector<vector<int>>&);
		vector<vector<int>> rotate180(const vector<vector<int>>&);
		vector<vector<int>> rotate270(const vector<vector<int>> &);
		bool isBoardSame(const vector<vector<int>> &b1, const vector<vector<int>> &b2);
		vector<vector<int>> rows2map(const vector<int> &);

	public:
		QtAlgorithmX *qt = nullptr;
		bool single_step = false;
		bool show_details = false;
		bool is_square = false;
		const vector<vector<int>>&getSols();
		Object *createHead(const vector<vector<int>> &mat, const unordered_map<int, position_set> &row2pos);
		void init(const vector<vector<int>> &mat, const unordered_map<int, position_set> &row2pos, Viewer *viewer, QtAlgorithmX *, int br, int bc);
		void solve(int num_of_threads);
	};
}
