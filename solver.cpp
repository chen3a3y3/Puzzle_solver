#include "solver.h"
#include "viewer.h"

void Solver::init(const vector<vector<int>> &mat, Viewer *viewer) {
	if (inited) return;
	if (viewer) this->viewer = viewer;
	sols.clear();
	h = make_shared<ColumnObject>();
	all_nodes.push_back(h);
	int rows = mat.size();
	if (rows == 0) return;
	int cols = mat[0].size();
	if (cols == 0) return;
	map = mat;

	vector<shared_ptr<Object>> rows_obj(rows, nullptr);

	auto iter_c = h;
	for (int i = 0; i < cols; i++) {
		auto c = make_shared<ColumnObject>();
		all_nodes.push_back(c);
		iter_c->r = c;
		c->l = iter_c;
		c->c = c;

		int one_cnt = 0;
		shared_ptr<Object> iter_r = c;
		for (int j = 0; j < rows; j++) {
			if (mat[j][i]) {
				one_cnt++;
				auto r = make_shared<DataObject>();
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
					rows_obj[j]->r.lock()->l = r;
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

void Solver::coverCol(const shared_ptr<Object> &c) {
	c->l.lock()->r = c->r;
	c->r.lock()->l = c->l;

	auto i = c->d.lock();
	while (i != c) {
		auto j = i->r.lock();
		while (j != i) {
			j->u.lock()->d = j->d;
			j->d.lock()->u = j->u;
			static_pointer_cast<ColumnObject>(j->c.lock())->s -= 1;

			j = j->r.lock();
		}
		i = i->d.lock();
	}
}

void Solver::uncoverCol(const shared_ptr<Object> &c) {
	auto i = c->u.lock();
	while (i != c) {
		auto j = i->l.lock();
		while (j != i) {
			static_pointer_cast<ColumnObject>(j->c.lock())->s += 1;
			j->d.lock()->u = j;
			j->u.lock()->d = j;

			j = j->l.lock();
		}
		i = i->u.lock();
	}

	c->r.lock()->l = c;
	c->l.lock()->r = c;
}

void Solver::search(int &sol, int k) {
	if (h->r.lock() == h) {
		// print solution
		cout << "found one solution: ";
		for (size_t i = 0; i < path.size(); i++) {
			cout << static_pointer_cast<DataObject>(path[i])->r_idx << ", ";
		}
		cout << endl;
		const auto &rows = path2rows(path);
		if (this->viewer && show_details) this->viewer->update(rows2map(path2rows(path)), 0);
		sols.push_back(rows);
		sol++;
		return;
	}
	if (sol >= max_sol) return;

	int min_cnt = std::numeric_limits<int>::max();
	shared_ptr<ColumnObject> cand_c;
	for (auto r = h->r.lock(); r != h;  r = r->r.lock()) {
		auto c = static_pointer_cast<ColumnObject>(r);
		int cnt = c->s;
		if (cnt < min_cnt) {
			min_cnt = cnt;
			cand_c = c;
		}
	}
	coverCol(cand_c);
	for (auto r = cand_c->d.lock(); r != cand_c; r = r->d.lock()) {
		path.push_back(r);
		if(this->viewer && show_details) this->viewer->update(rows2map(path2rows(path)), 30);
		for (auto j = r->r.lock(); j != r; j = j->r.lock()) {
			coverCol(j->c.lock());
		}
		search(sol, k + 1);
		for (auto j = r->l.lock(); j != r; j = j->l.lock()) {
			uncoverCol(j->c.lock());
		}
		path.pop_back();
		if (this->viewer && show_details) this->viewer->update(rows2map(path2rows(path)), 30);
	}
	uncoverCol(cand_c);
}

void Solver::solve(int num_of_sol) {
	if (!inited) {
		cerr << "Please init first" << endl;
		return;
	}
	if (this->viewer && show_details) this->viewer->update(rows2map(path2rows(path)), 30);
	int sol = 0;
	max_sol = num_of_sol;
	search(sol);
}

const vector<vector<int>>&Solver::getSols()
{
	return sols;
}

vector<int> Solver::path2rows(const vector<shared_ptr<Object>> &p) {
	vector<int> result;
	for (size_t i = 0; i < p.size(); i++) {
		result.push_back(static_pointer_cast<DataObject>(p[i])->r_idx);
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