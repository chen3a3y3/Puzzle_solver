#include "solver_yy.h"
#include "viewer.h"
#include "input_process.h"
#include "QtAlgorithmX.h"
#include <random>
#include <opencv2/opencv.hpp>
#include <future>
#include <opencv2/imgproc/imgproc.hpp>

namespace yy {

	Object *Solver::createHead(const vector<vector<int>> &mat, const unordered_map<int, position_set> &row2pos) {
		auto h = new ColumnObject();
		int rows = mat.size();
		if (rows == 0) return nullptr;
		int cols = mat[0].size();
		if (cols == 0) return nullptr;
		vector<Object*> rows_obj(rows, nullptr);

		auto iter_c = h;
		for (int i = 0; i < cols; i++) {
			auto c = new ColumnObject();
			iter_c->r = c;
			c->l = iter_c;
			c->c = c;

			int one_cnt = 0;
			Object *iter_r = c;
			for (int j = 0; j < rows; j++) {
				if (mat[j][i]) {
					one_cnt++;
					auto r = new DataObject();
					iter_r->d = r;
					r->u = iter_r;
					r->c = c;
					r->r_idx = j;

					iter_r = r;

					if (!rows_obj[j]) {
						rows_obj[j] = r;
						r->r = r;
						r->l = r;
					}
					else {
						rows_obj[j]->r->l = r;
						r->r = rows_obj[j]->r;
						rows_obj[j]->r = r;
						r->l = rows_obj[j];
						rows_obj[j] = r;
					}
				}
			}
			iter_r->d = c;
			c->u = iter_r;
			c->s = one_cnt;
			char name[128];
			sprintf_s(name, 128, "Col %d", i);
			c->name = name;

			iter_c = c;
		}
		iter_c->r = h;
		h->l = iter_c;
		return h;
	}

	void Solver::init(const vector<vector<int>> &mat, const unordered_map<int, position_set> &row2pos, Viewer *viewer, QtAlgorithmX *qt, int br, int bc) {
		this->qt = qt;
		if (inited) return;
		nrows = mat.size();
		ncols = mat[0].size();
		this->viewer = viewer;
		this->row2pos = row2pos;
		sols.clear();
		paths.clear();
		sols_map.clear();
		shuffle_map.clear();
		this->map = mat;
		boardrows = br;
		boardcols = bc;
		h = static_cast<ColumnObject *>(createHead(mat, row2pos));

		inited = true;
	}

	void Solver::coverCol(Object* c) {
		c->l->r = c->r;
		c->r->l = c->l;

		auto i = c->d;
		while (i != c) {
			auto j = i->r;
			while (j != i) {
				j->u->d = j->d;
				j->d->u = j->u;
				static_cast<ColumnObject*>(j->c)->s -= 1;

				j = j->r;
			}
			i = i->d;
		}
	}

	void Solver::uncoverCol(Object* c) {
		auto i = c->u;
		while (i != c) {
			auto j = i->l;
			while (j != i) {
				static_cast<ColumnObject*>(j->c)->s += 1;
				j->d->u = j;
				j->u->d = j;

				j = j->l;
			}
			i = i->u;
		}

		c->r->l = c;
		c->l->r = c;
	}

	void Solver::search(int idx, Object * head, int k) {
		if (qt->requestedStop) return;
		if (head->r == head) {

			const auto &rows = path2rows(paths[idx]);
			const auto &board = rows2board(rows);
			bool valid = false;
			if (this->viewer && (show_details || single_step)) 
				this->viewer->update(rows, 0, &this->row2pos);
			{
				std::shared_lock<std::shared_mutex> lock(rw_mutex);
				valid = isValid(board);
			}
			if (valid) {
				std::unique_lock<std::shared_mutex> lock(rw_mutex);
				// recheck
				if (isValid(board)) {
					sols.push_back(rows);
					sols_map.push_back(board);
				}
			}
			/*{
				lock_guard<mutex> guard(update_mutex);
				sols.push_back(rows);
			}*/
			return;
		}
		// if (sol >= max_sol) return;

		int min_cnt = std::numeric_limits<int>::max();
		ColumnObject *cand_c;
		for (auto r = head->r; r != head;  r = r->r) {
			auto c = static_cast<ColumnObject*>(r);
			int cnt = c->s;
			if (cnt < min_cnt) {
				min_cnt = cnt;
				cand_c = c;
			}
		}

		coverCol(cand_c);
		for (auto r = cand_c->d; r != cand_c; r = r->d) {
			paths[idx].push_back(r);
			if (this->viewer && (show_details || single_step) && idx == 0)
				this->viewer->update(path2rows(paths[idx]), show_details ? 30 : 0, &this->row2pos);
			for (auto j = r->r; j != r; j = j->r) {
				coverCol(j->c);
			}
			search(idx, head, k + 1);
			for (auto j = r->l; j != r; j = j->l) {
				uncoverCol(j->c);
			}
			paths[idx].pop_back();
			if (this->viewer && (show_details || single_step) && idx == 0)
				this->viewer->update(path2rows(paths[idx]), show_details ? 30 : 0, &this->row2pos);
		}
		uncoverCol(cand_c);
	}

	void Solver::searchThread(int idx, int start, int nrows, Object *head, vector<Object *> rhs) {
		for (int i = start; i < start + nrows; i++) {
			auto r = rhs[shuffle_map[i]];
			paths[idx].push_back(r);
			if (this->viewer && (show_details || single_step) && idx == 0)
				this->viewer->update(path2rows(paths[idx]), show_details ? 30 : 0, &this->row2pos);
			for (auto j = r->r; j != r; j = j->r) {
				coverCol(j->c);
			}
			search(idx, head, 1);
			for (auto j = r->l; j != r; j = j->l) {
				uncoverCol(j->c);
			}
			paths[idx].pop_back();
			if (this->viewer && (show_details || single_step) && idx == 0)
				this->viewer->update(path2rows(paths[idx]), show_details ? 30 : 0, &this->row2pos);
		}
	}

	void Solver::solve(int n_threads) {
		if (show_details || single_step) cv::namedWindow("Progress", cv::WINDOW_AUTOSIZE);
		if (!inited) {
			cerr << "Please init first" << endl;
			return;
		}
		
		int sol = 0;
		int min_cnt = std::numeric_limits<int>::max();
		int offset = 1, min_offset = 1;
		Object *cand_c;
		for (auto r = h->r; r != h; r = r->r) {
			auto c = static_cast<ColumnObject*>(r);
			int cnt = c->s;
			if (cnt < min_cnt) {
				min_cnt = cnt;
				cand_c = c;
				min_offset = offset;
			}
			offset++;
		}
		coverCol(cand_c);
		int one_cnt = static_cast<ColumnObject*>(cand_c)->s;

		vector<thread> threads;
		paths.resize(n_threads);
		
		std::random_device rd;
		std::mt19937 g(rd());

		// generate shuffle map
		vector<int>vals(one_cnt, 0);
		for (int i = 0; i < vals.size(); i++) vals[i] = i;

		// uncomment next line to shuffle
		// std::shuffle(vals.begin(), vals.end(), g);
		for (int i = 0; i < one_cnt; i++) {
			shuffle_map[i] = vals[i];
		}

		for (int i = 0; i < n_threads; i++) {
			auto h_thread = createHead(this->map, this->row2pos);
			auto rc_thread = h_thread;
			for (int k = 0; k < min_offset; k++) {
				rc_thread = rc_thread->r;
			}
			coverCol(rc_thread);
			
			
			for (int k = 0; k < one_cnt; k++) {
				shuffle_map[k] = vals[k];
			}
			auto rh_thread = rc_thread->d;
			vector<Object *> rhs;
			for (auto rh_iter = rh_thread; rh_iter != rc_thread; rh_iter = rh_iter->d) {
				rhs.push_back(rh_iter);
			}
			/*for (int j = 0; j < one_cnt / n_threads * i; j++) {
				rh_thread = rh_thread->d;
			}*/
			thread th(&Solver::searchThread, this, i, one_cnt / n_threads * i, one_cnt / n_threads + ((i == n_threads - 1) ? one_cnt % n_threads : 0),
				h_thread, rhs);
			threads.push_back(move(th));
		}
		for (int i = 0; i < n_threads; i++) threads[i].join();
		if (show_details || single_step) cv::destroyWindow("Progress");
	}

	const vector<vector<int>>&Solver::getSols()
	{
		return sols;
	}

	vector<vector<int>> Solver::rows2board(const vector<int> &rows) {
		vector<vector<int>> result;
		result.resize(boardrows);
		for (auto &r : result) r.resize(boardcols);
		for (size_t i = 0; i < rows.size(); i++) {
			int r_idx = rows[i];
			const position_set &pos = row2pos[r_idx];
			const auto &pa = pos.offset;
			int index = pos.index;
			Tile* tile = pos.tile;
			for (const auto &p : tile->point_set) {
				result[p->row_index + pa.first][p->col_index + pa.second] = index;
			}
		}
		return result;
	}

	vector<int> Solver::path2rows(const vector<Object*> &p) {
		vector<int> result;
		for (size_t i = 0; i < p.size(); i++) {
			result.push_back(static_cast<DataObject*>(p[i])->r_idx);
		}
		return result;
	}

	bool Solver::isValid(const vector<vector<int>>& sol) {
		if (is_square) {
			const auto &sol90 = rotate90(sol);
			const auto &sol180 = rotate180(sol);
			const auto &sol270 = rotate270(sol);
			for (const auto &before : sols_map) {
				if (isBoardSame(sol, before)) return false;
				if (isBoardSame(sol90, before)) return false;
				if (isBoardSame(sol180, before)) return false;
				if (isBoardSame(sol270, before)) return false;
			}
			return true;
		}
		else {
			const auto &sol180 = rotate180(sol);
			for (const auto &before : sols_map) {
				if (isBoardSame(sol, before)) return false;
				if (isBoardSame(sol180, before)) return false;
			}
			return true;
		}
	}

	bool Solver::isBoardSame(const vector<vector<int>> &b1, const vector<vector<int>> &b2) {
		int nrows = b1.size();
		int ncols = b1[0].size();
		for (int i = 0; i < nrows; i++) {
			for (int j = 0; j < ncols; j++) {
				if (b1[i][j] != b2[i][j]) return false;
			}
		}
		return true;
	}

	vector<vector<int>> Solver::rotate90(const vector<vector<int>>&b) {
		vector<vector<int>> result;
		int nrows = b.size();
		int ncols = b[0].size();
		result.resize(nrows);
		for (auto &r : result) r.resize(ncols);
		for (int i = 0; i < nrows; i++) {
			for (int j = 0; j < ncols; j++) {
				result[i][j] = b[j][ncols - 1 - i];
			}
		}
		return result;
	}

	vector<vector<int>> Solver::rotate180(const vector<vector<int>>&b) {
		vector<vector<int>> result;
		int nrows = b.size();
		int ncols = b[0].size();
		result.resize(nrows);
		for (auto &r : result) r.resize(ncols);
		for (int i = 0; i < nrows; i++) {
			for (int j = 0; j < ncols; j++) {
				result[i][j] = b[nrows - i - 1][ncols - j - 1];
			}
		}
		return result;
	}

	vector<vector<int>> Solver::rotate270(const vector<vector<int>> &b) {
		vector<vector<int>> result;
		int nrows = b.size();
		int ncols = b[0].size();
		result.resize(nrows);
		for (auto &r : result) r.resize(ncols);
		for (int i = 0; i < nrows; i++) {
			for (int j = 0; j < ncols; j++) {
				result[i][j] = b[nrows - 1 - j][i];
			}
		}
		return result;
	}

	vector<vector<int>> Solver::rows2map(const vector<int> &rows) {
		vector<vector<int>> patt;
		for (int i = 0; i < rows.size(); i++) {
			patt.push_back(map[rows[i]]);
		}
		return patt;
	}

}