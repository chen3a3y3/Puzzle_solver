#pragma once
#include <tuple>
#include <vector>
using namespace std;

class Block {
public:
	explicit Block(const vector<tuple<int, int, int>>& c, int oidx, int oriidx) :
		coords(c), object_idx(oidx), ori_idx(oriidx) {}
	vector<tuple<int, int, int>> coords;
	int object_idx = -1;
	int ori_idx = -1;
	tuple<int, int, int> tmin;
	tuple<int, int, int> tmax;
};

class Input3D {
public:
	Input3D(const Block &, bool fr = true);
};