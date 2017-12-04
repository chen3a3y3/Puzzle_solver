#include "node.h"
Node* Node::move_node(string dir, int times) {
	try {
		Node* cur = this;
		for (int i = 0; i < times; i++) {
			if (dir == "right")
				cur = cur->right;
			else if (dir == "left")
				cur = cur->left;
			else if (dir == "up")
				cur = cur->up;
			else if (dir == "down")
				cur = cur->down;
		}
		return cur;
	}
	catch (...) {
		cout << "error" << endl;
		cout << row_index << " " << col_index << endl;
		system("pause");
		exit(-1);
	}
}

Node* Node::go_to_end() {
	Node* cur = this;
	while (cur->down != nullptr) {
		cur = cur->down;
	}
	return cur;
}