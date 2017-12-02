#pragma once
#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;

vector<vector<int>> create_matrix() {
	vector<vector<int>> m(6, vector<int>(7, 0));
	m[0][2] = 1;
	m[0][4] = 1;
	m[0][5] = 1;
	m[1][0] = 1;
	m[1][3] = 1;
	m[1][6] = 1;
	m[2][1] = 1;
	m[2][2] = 1;
	m[2][5] = 1;
	m[3][0] = 1;
	m[3][3] = 1;
	m[4][1] = 1;
	m[4][6] = 1;
	m[5][3] = 1;
	m[5][4] = 1;
	m[5][6] = 1;
	return m;
}