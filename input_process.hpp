#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include "tile.hpp"

using namespace std;



void input_process(string path, vector<vector<int>>& input) {
	path = "test.txt";
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
	


	vector<vector<char>> raw_data(row, vector<char>(col, ' '));
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < input_string[i].size(); j++) {
			raw_data[i][j] = input_string[i][j];
		}
	}

	for (vector<char> a : raw_data) {
		for (char s : a) {
			cout << s;
		}
		cout << endl;
	}
	

	vector<Tile*> tile_set;
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
				tile->point_set.push_back(point);
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

	for (Tile* t : tile_set) {
		cout << t->point_num << endl;
	}
	







	infile.close();

}