#include "input_process.h"

bool Input::is_contain_point(Point* target, Tile* t) {
	for (Point* p : t->point_set) {
		if (target->row_index == p->row_index && target->col_index == p->col_index && target->value == p->value) {
			return true;
		}
	}
	return false;
}

bool Input::check_is_diff(vector<Tile*>& single, Tile* tt) {
	for (Tile* t : single) {
		bool flag = false;
		for (Point* p : t->point_set) {
			if (!is_contain_point(p, tt)) {
				flag = true;
				break;
			}
		}
		if (!flag) return false;
	}
	return true;
}


void Input::create_index2value() {
	for (Point* p : board->point_set) {
		index2value[make_pair(p->row_index, p->col_index)] = p->value;
	}
	int start_col = tile_number;
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

void Input::input_process(string path, vector<vector<int>>& input, bool f_and_r) {
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

	// 给tile编号
	for (int i = 0; i < (int)tile_set.size(); i++) {
		tile_set[i]->index = i;
	}

	int total_sum = 0;
	for (Tile* t : tile_set) {
		total_sum += t->point_set.size();
	}
	if (total_sum != board->point_set.size()) {
		cout << "tile size does not match to board size" << endl;
		system("pause");
		exit(-1);
	}
	tile_number = tile_set.size();
	cout << tile_number << endl;
	// 判断旋转或者翻转
	if (f_and_r) {
		vector<Tile*> new_tile_set = tile_set;
		for (Tile* t : new_tile_set) {
			vector<Tile*> temp;
			for (int i = 0; i < 7; i++) {
				Tile* t1 = new Tile();
				temp.push_back(t1);
			}
			for (Point* p : t->point_set) {
				int p_row = p->row_index, p_col = p->col_index, val = p->value;
				Point* p1 = new Point(p_row, -p_col, val);
				temp[0]->point_set.push_back(p1);

				Point* p2 = new Point(-p_row, p_col, val);
				temp[1]->point_set.push_back(p2);

				Point* p3 = new Point(-p_row, -p_col, val);
				temp[2]->point_set.push_back(p3);

				Point* p4 = new Point(p_col, p_row, val);
				temp[3]->point_set.push_back(p4);

				Point* p5 = new Point(p_col, -p_row, val);
				temp[4]->point_set.push_back(p5);

				Point* p6 = new Point(-p_col, p_row, val);
				temp[5]->point_set.push_back(p6);

				Point* p7= new Point(-p_col, -p_row, val);
				temp[6]->point_set.push_back(p7);
			}
			int ind = t->index;
			vector<Tile*> single;
			single.push_back(t);
			for (Tile* tt : temp) {
				tt->index = ind;
				tt->init();
				if (check_is_diff(single, tt)) {
					single.push_back(tt);
					tile_set.push_back(tt);
				}
			}
		}
	}

	// 测试
	//for (Tile* t : tile_set) {
	//	cout << t->index << endl;
	//	for (Point* p : t->point_set) {
	//		cout << p->row_index << " " << p->col_index << " " << p->value << endl;
	//	}
	//	cout << endl;
	//}

	//Tile* debug = new Tile();
	//Point* a = new Point(0, 0, 'X');
	//Point* b = new Point(0, 1, 'X');
	//Point* c = new Point(1, 0, 'X');
	//Point* d = new Point(2, 0, 'X');
	//debug->point_set.push_back(a);
	//debug->point_set.push_back(b);
	//debug->point_set.push_back(c);
	//debug->point_set.push_back(d);
	//cout << single.size() << endl;
	//cout << check_is_diff(single, debug) << endl;

	// 建立坐标和board里value的映射表
	create_index2value();

	// 开始生成return的matrix
	int matrix_col_size = tile_number + board->point_set.size();
	int row_index = 0;
	for (int index = 0; index < (int)tile_set.size(); index++) {
		Tile* cur = tile_set[index];
		// 对于其中一块tile，开始计算可行位置
		for (int i = 0; i <= board->down - cur->down; i++) {
			for (int j = 0; j <= board->right - cur->right; j++) {
				vector<int> temp(matrix_col_size, 0);
				if (check(cur, i, j)) {
					temp[cur->index] = 1;
					for (Point* p : cur->point_set) {
						int col_index = position2col[make_pair(p->row_index + i, p->col_index + j)];
						temp[col_index] = 1;
					}

					position_set position;
					position.index = cur->index;
					position.offset = make_pair(i, j);
					position.tile = cur;
					(*row2position)[row_index] = position;
					input.push_back(temp);
					row_index++;
				}
			}
		}
	}
}
