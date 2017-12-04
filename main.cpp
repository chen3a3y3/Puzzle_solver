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
	input.input_process("test.txt", test, true);
	cout << "input processing done" << endl;
	cout << "row:" << test.size() << " col:" << test[0].size() << endl;
	//for (vector<int> vec : test) {
	//	for (int a : vec) {
	//		cout << a;
	//	}
	//	cout << endl;
	//}
	Solver S = Solver(test.size(), test[0].size());
	vector<vector<int>> result = S.solve(test, input);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
	cout << endl;
	for (auto i : result) {
		for (auto j : i) {
			position_set r = (*input.row2position)[j];
			cout << r.index << " " << r.offset.first << " " << r.offset.second << endl;
		}
		cout << endl;
	}
	cout << "The number of solution is:" << result.size() << endl;
	system("pause");
	
	return 0;
}


