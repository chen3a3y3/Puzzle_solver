// tile.h
// build up a class represnting each point
// build up a class representing each tile
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
	int index = -1;

	vector<Point*> point_set;
	Tile() {}
	~Tile() {
		for (Point* p : point_set) {
			delete p;
			p = nullptr;
		}
	}

	void init();

};
