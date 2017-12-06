#include "solver_yy.h"
#include "viewer.h"
#include "input_process.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace yy {
	void Solver::init(const vector<vector<int>> &mat, const unordered_map<int, position_set> &row2pos, Viewer *viewer) {
		if (inited) return;
		this->viewer = viewer;
		this->row2pos = row2pos;
		sols.clear();
		h = new ColumnObject();
		all_nodes.push_back(h);
		int rows = mat.size();
		if (rows == 0) return;
		int cols = mat[0].size();
		if (cols == 0) return;
		map = mat;

		vector<Object*> rows_obj(rows, nullptr);

		auto iter_c = h;
		for (int i = 0; i < cols; i++) {
			auto c = new ColumnObject();
			all_nodes.push_back(c);
			iter_c->r = c;
			c->l = iter_c;
			c->c = c;

			int one_cnt = 0;
			Object *iter_r = c;
			for (int j = 0; j < rows; j++) {
				if (mat[j][i]) {
					one_cnt++;
					auto r = new DataObject();
					all_nodes.push_back(r);
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

	void Solver::search(int &sol, long long int& ccc, int k) {
		ccc++;
		if (h->r == h) {
			// print solution
			/*cout << "found one solution: ";
			for (size_t i = 0; i < path.size(); i++) {
				cout << static_pointer_cast<DataObject>(path[i])->r_idx << ", ";
			}
			cout << endl;*/

			/*const auto &rows = path2rows(path);
			if (this->viewer && show_details) this->viewer->update(path2rows(path), 0, &this->row2pos);
			sols.push_back(rows);*/
			sols.push_back(vector<int>());
			sol++;
			return;
		}
		if (sol >= max_sol) return;

		int min_cnt = std::numeric_limits<int>::max();
		ColumnObject *cand_c;
		for (auto r = h->r; r != h;  r = r->r) {
			auto c = static_cast<ColumnObject*>(r);
			int cnt = c->s;
			if (cnt < min_cnt) {
				min_cnt = cnt;
				cand_c = c;
			}
		}

		coverCol(cand_c);
		for (auto r = cand_c->d; r != cand_c; r = r->d) {
			path.push_back(r);
			if(this->viewer && show_details) this->viewer->update(path2rows(path), 30, &this->row2pos);
			for (auto j = r->r; j != r; j = j->r) {
				coverCol(j->c);
			}
			search(sol, ccc, k + 1);
			for (auto j = r->l; j != r; j = j->l) {
				uncoverCol(j->c);
			}
			path.pop_back();
			if (this->viewer && show_details) this->viewer->update(path2rows(path), 30, &this->row2pos);
		}
		uncoverCol(cand_c);
	}

	void Solver::solve(int num_of_sol) {
		if (show_details) cv::namedWindow("Progress", cv::WINDOW_AUTOSIZE);
		if (!inited) {
			cerr << "Please init first" << endl;
			return;
		}
		if (this->viewer && show_details) this->viewer->update(path2rows(path), 30, &this->row2pos);
		int sol = 0;
		max_sol = num_of_sol;
		long long int ccc = 0;
		search(sol, ccc);
		cout << ccc << endl;
		if (show_details) cv::destroyWindow("Progress");
	}

	const vector<vector<int>>&Solver::getSols()
	{
		return sols;
	}

	vector<int> Solver::path2rows(const vector<Object*> &p) {
		vector<int> result;
		for (size_t i = 0; i < p.size(); i++) {
			result.push_back(static_cast<DataObject*>(p[i])->r_idx);
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