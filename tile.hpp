#pragma once
#include <iostream>
#include <vector>

using namespace std;

class Point {
public:
	int row_index;
	int col_index;
	char value;
	Point(int row, int col, char v) {
		row_index = row;
		col_index = col;
		value = v;
	}
};

class Tile {
public:
	int left = INT_MAX;
	int right = 0;
	int up = INT_MAX;
	int down = 0;
	int point_num = 0;

	vector<Point*> point_set;
	Tile() {}

	void init();

};

void Tile::init() {
	for (Point *p : point_set) {
		left = min(left, p->col_index);
		right = max(right, p->col_index);
		up = min(up, p->row_index);
		down = max(down, p->row_index);
	}
	point_num = point_set.size();
}
