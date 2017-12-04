#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include "tile.h"

using namespace std;

typedef struct position_set {
	Tile* tile;
	pair<int, int> offset;
	int index = 0;
} position_set;


class Input {
private:
	struct hash_func {
		size_t operator()(const pair<int, int>& p) const {
			return p.first * 9999 + p.second;
		}
	};
	struct cmp_func {
		bool operator()(const pair<int, int>& p1, const pair<int, int>& p2) const {
			return (p1.first == p2.first && p1.second == p2.second);
		}
	};
	unordered_map<pair<int, int>, char, hash_func, cmp_func> index2value;
	unordered_map<pair<int, int>, int, hash_func, cmp_func> position2col;
public:

	int tile_number = 0;
	vector<Tile*> tile_set;
	unordered_map<int, position_set>* row2position;

	Tile* board = nullptr;
	Input() {
		row2position = new unordered_map<int, position_set>;
	};
	~Input() {
		for (Tile* t : tile_set) {
			delete t;
			t = nullptr;
		}
		delete row2position;
		row2position = nullptr;
		board = nullptr;
	};

	bool check(Tile* cur, int row_offset, int col_offset);
	bool check_is_diff(vector<Tile*>& single, Tile* tt);
	bool is_contain_point(Point* target, Tile* t);
	void input_process(string path, vector<vector<int>>& input, bool f_and_r);
	void create_index2value();
};