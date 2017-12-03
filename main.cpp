#include <iostream>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include "solver.hpp"
#include "input_process.hpp"
using namespace std;

int main() {

	vector<vector<int>> test;
	Input input = Input();
	input.input_process("test.txt", test);
	cout << "input processing done" << endl;
	cout << "row:" << test.size() << " col:" << test[0].size() << endl;
	//for (vector<int> vec : test) {
	//	for (int a : vec) {
	//		cout << a;
	//	}
	//	cout << endl;
	//}
	Solver S = Solver(test.size(), test[0].size());
	vector<vector<int>> result = S.solve(test);
	cout << endl;
	for (auto i : result) {
		for (auto j : i) {
			vector<int> r = (*input.row2position)[j];
			cout << r[0] << r[1] << r[2];
		}
		cout << endl;
	}
	system("pause");
	
	return 0;
}


