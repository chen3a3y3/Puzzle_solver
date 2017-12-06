// input_process.cpp
// Input processing process in included in this file.
// Transfer the ASKII to a matrix

#include "input_process.h"

bool Input::is_contain_point(Point* target, Tile* t) {
	for (Point* p : t->point_set) {
		if (target->row_index == p->row_index && target->col_index == p->col_index && target->value == p->value) {
			return true;
		}
	}
	return false;
}

// check whether the new tile after rotating or fliping is the same as the old tiles
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


void Input::create_index2value(int idx, int tile_num) {
	for (Point* p : board->point_set) {
		index2values[idx][make_pair(p->row_index, p->col_index)] = p->value;
	}
	int start_col = tile_num;
	for (Point* p : board->point_set) {
		position2cols[idx][make_pair(p->row_index, p->col_index)] = start_col;
		start_col++;
	}
}


bool Input::check(int idx, Tile* cur, int row_offset, int col_offset) {
	for (Point* p : cur->point_set) {
		if (!index2values[idx].count(make_pair(p->row_index + row_offset, p->col_index + col_offset)))
			return false;
		char board_value = index2values[idx][make_pair(p->row_index + row_offset, p->col_index + col_offset)];
		if (board_value != p->value) return false;
	}
	return true;
}

// the main function to process the input data
bool Input::input_process(string path, vector<vector<vector<int>>>& input, bool f_and_r) {
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
	// transfer the raw_data to tile object
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

	// give the index to each tile
	for (int i = 0; i < (int)tile_set.size(); i++) {
		tile_set[i]->index = i;
	}

	int total_sum = 0;
	for (Tile* t : tile_set) {
		total_sum += t->point_set.size();
	}
	if (total_sum < board->point_set.size()) {
		cerr << "tile size does not match to board size" << endl;
		return false;
	}

	total_tile_number = tile_set.size();

	vector<vector<Tile *>> all_tile_sets;
	if (total_sum == board->point_set.size()) {
		all_tile_sets.push_back(tile_set);
	}
	else {
		all_tile_sets = get_sub_tile_set(tile_set, board->point_set.size());
	}

	for (int i = 0; i < all_tile_sets.size(); i++) {
		tile_numbers.push_back(all_tile_sets[i].size());
		index2values.push_back(unordered_map<pair<int, int>, char, hash_func, cmp_func>());
		position2cols.push_back(unordered_map<pair<int, int>, int, hash_func, cmp_func>());
		row2positions.push_back(unordered_map<int, position_set>());

		// create the map between index and point value of board
		create_index2value(i, tile_numbers[i]);
	}
	

	int tile_set_idx = 0;
	for (auto &tile_set : all_tile_sets) {
		vector<vector<int>> single_input;
		// if user checks flip and rotate box
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

					Point* p7 = new Point(-p_col, -p_row, val);
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

		// generate the matrix which used in Algorithm X
		int matrix_col_size = tile_numbers[tile_set_idx] + board->point_set.size();
		int row_index = 0;
		for (int index = 0; index < (int)tile_set.size(); index++) {
			Tile* cur = tile_set[index];
			// get the valid position of one tile
			for (int i = 0; i <= board->down - cur->down; i++) {
				for (int j = 0; j <= board->right - cur->right; j++) {
					vector<int> temp(matrix_col_size, 0);
					if (check(tile_set_idx, cur, i, j)) {
						temp[cur->index] = 1;
						for (Point* p : cur->point_set) {
							int col_index = position2cols[tile_set_idx][make_pair(p->row_index + i, p->col_index + j)];
							temp[col_index] = 1;
						}

						position_set position;
						position.index = cur->index;
						position.offset = make_pair(i, j);
						position.tile = cur;
						row2positions[tile_set_idx][row_index] = position;
						single_input.push_back(temp);
						row_index++;
					}
				}
			}
		}
		input.push_back(single_input);
		tile_set_idx++;
	}
	return true;
}


vector<vector<Tile *>> Input::get_sub_tile_set(const vector<Tile*> &tile_set, int target) {
	// brute force
	vector<vector<Tile *>> results;
	unsigned int total = 1 << tile_set.size();
	int iter = 0;
	while (iter < total) {
		int sum = 0;
		vector<Tile *> tiles;
		for (int i = 0; i < tile_set.size(); i++) {
			if (iter & (1 << i)) {
				tiles.push_back(tile_set[i]);
				sum += tile_set[i]->point_num;
				if (sum > target) break;
			}
		}
		iter++;
		if (sum != target) continue;
		results.push_back(tiles);
	}
	return results;
}
