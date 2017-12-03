#include <iostream>
#include <algorithm>
#include <vector>
#include "create_matrix.hpp"
#include "solver.hpp"
#include "input_process.hpp"
using namespace std;

int main() {
	vector<vector<int>> test;
	input_process("123", test);
	/*vector<vector<int>> test = create_matrix();
	Solver S = Solver(test.size(), test[0].size());
	vector<vector<int>> result = S.solve(test);
	for (auto i : result) {
		for (auto j : i) {
			cout << j << " ";
		}
		cout << endl;
	}*/
	system("pause");
	return 0;
}


