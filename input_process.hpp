#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include "tile.hpp"

using namespace std;

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
	vector<Tile*> tile_set;
	unordered_map<int, vector<int>>* row2position;
	
	Tile* board = nullptr;
	Input() {
		row2position = new unordered_map<int, vector<int>>;
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
	void input_process(string path, vector<vector<int>>& input);
	void create_index2value();
};


void Input::create_index2value() {
	for (Point* p : board->point_set) {
		index2value[make_pair(p->row_index, p->col_index)] = p->value;
	}
	int start_col = tile_set.size();
	for (Point* p : board->point_set) {
		position2col[make_pair(p->row_index, p->col_index)] = start_col;
		start_col++;
	}
}


bool Input::check(Tile* cur, int row_offset, int col_offset) {
	for (Point* p : cur->point_set) {
		if (!index2value.count(make_pair(p->row_index + row_offset, p->col_index + col_offset)))
			return false;
		char board_value = index2value[make_pair(p->row_index + row_offset, p->col_index + col_offset)];
		if (board_value != p->value) return false;
	}
	return true;
}

void Input::input_process(string path, vector<vector<int>>& input) {
	char data[256];
	vector<string> input_string;
	ifstream infile;
	infile.open(path);
	int col = 0, row = 0;
	while (!infile.eof()) {
		infile.getline(data, 256);
		string s = data;
		col = max(col, (int)s.size());
		row++;
		input_string.push_back(s);
	}
	infile.close();

	vector<vector<char>> raw_data(row, vector<char>(col, ' '));
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < (int)input_string[i].size(); j++) {
			raw_data[i][j] = input_string[i][j];
		}
	}
	
	queue<Point*> q;
	// 把raw_data 转换成 tile的对象
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			if (raw_data[i][j] == ' ')  continue;
			Tile* tile = new Tile();
			Point* point = new Point(i, j, raw_data[i][j]);

			// BFS
			q.push(point);
			while (!q.empty()) {
				Point* cur = q.front();
				q.pop();
				int r = cur->row_index, c = cur->col_index;
				tile->point_set.push_back(cur);
				raw_data[r][c] = ' ';

				if (r - 1 >= 0 && raw_data[r - 1][c] != ' ') {
					Point* p = new Point(r - 1, c, raw_data[r - 1][c]);
					q.push(p);
					raw_data[r - 1][c] = ' ';
				}
				if (r + 1 < row && raw_data[r + 1][c] != ' ') {
					Point* p = new Point(r + 1, c, raw_data[r + 1][c]);
					q.push(p);
					raw_data[r + 1][c] = ' ';
				}
				if (c - 1 >= 0 && raw_data[r][c - 1] != ' ') {
					Point* p = new Point(r, c - 1, raw_data[r][c - 1]);
					q.push(p);
					raw_data[r][c - 1] = ' ';
				}
				if (c + 1 < col && raw_data[r][c + 1] != ' ') {
					Point* p = new Point(r, c + 1, raw_data[r][c + 1]);
					q.push(p);
					raw_data[r][c + 1] = ' ';
				}
			}

			tile->init();
			tile_set.push_back(tile);
		}
	}

	int max_tile_size = 0, position = 0;
	for (int i = 0; i < (int)tile_set.size(); i++) {
		if (tile_set[i]->point_num > max_tile_size) {
			max_tile_size = tile_set[i]->point_num;
			board = tile_set[i];
			position = i;
		}
	}
	tile_set.erase(tile_set.begin() + position);

	int total_sum = 0;
	for (Tile* t : tile_set) {
		total_sum += t->point_set.size();
	}
	if (total_sum != board->point_set.size()) {
		cout << "tile size does not match to board size" << endl;
		system("pause");
		exit(-1);
	}

	// 建立坐标和board里value的映射表
	create_index2value();

	// 开始生成return的matrix
	int matrix_col_size = tile_set.size() + board->point_set.size();
	int row_index = 0;
	for (int index = 0; index < (int)tile_set.size(); index++) {
		Tile* cur = tile_set[index];
		// 对于其中一块tile，开始计算可行位置
		for (int i = 0; i <= board->down - cur->down; i++) {
			for (int j = 0; j <= board->right - cur->right; j++) {
				vector<int> temp(matrix_col_size, 0);
				if (check(cur, i, j)) {
					temp[index] = 1;
					for (Point* p : cur->point_set) {
						int col_index = position2col[make_pair(p->row_index + i, p->col_index + j)];
						temp[col_index] = 1;
					}

					vector<int> position(3, 0);
					position[0] = index;
					position[1] = i;
					position[2] = j;
					(*row2position)[row_index] = position;
					input.push_back(temp);
					row_index++;
				}
			}
		}
	}
}