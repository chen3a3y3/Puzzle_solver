#include "tile.h"
#include <algorithm>

void Tile::init() {
	for (Point *p : point_set) {
		left = min(left, p->col_index);
		up = min(up, p->row_index);
	}
	for (Point *p : point_set) {
		p->row_index -= up;
		p->col_index -= left;
	}
	for (Point *p : point_set) {
		left = min(left, p->col_index);
		right = max(right, p->col_index);
		up = min(up, p->row_index);
		down = max(down, p->row_index);
	}
	point_num = point_set.size();
}