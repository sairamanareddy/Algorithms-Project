#include "util.hh"
#include "vector.hh"
// Heap implementation using vector
// H, D, P as described in problem statement
template <typename _Value>
class heap {
   private:
	vector<int> H;
	vector<_Value> D;
	vector<int> P;
	int _size = 0;
	int parent(int i) { return (i - 1) / 2; }
	int left(int i) { return 2 * i + 1; }
	int right(int i) { return 2 * (i + 1); }

   public:
	heap(int n) : H(n), D(n), P(n, -1) {}
	heap(vector<_Value> const& data)
		: H(data.size()), D(data), P(data.size(), -1), _size(data.size()) {
		for (int i = 0; i < _size; ++i) { H[i] = P[i] = i; }
		for (int i = _size / 2; i >= 0; --i) {
			int tmp = i;
			while (tmp < _size) {
				int max_tmp = tmp;
				if (left(tmp) < _size && D[H[left(tmp)]] > D[H[max_tmp]]) max_tmp = left(tmp);
				if (right(tmp) < _size && D[H[right(tmp)]] > D[H[max_tmp]]) max_tmp = right(tmp);
				if (max_tmp == tmp) break;
				std::swap(P[H[tmp]], P[H[max_tmp]]);
				std::swap(H[tmp], H[max_tmp]);
				tmp = max_tmp;
			}
		}
	}
	pair<int, _Value> peek() { return {H[0], D[H[0]]}; }
	pair<int, _Value> extract_max() {
		int res = H[0];
		_Value val = D[H[0]];
		erase(res);
		return {res, val};
	}
	int size() { return this->_size; }
	void increase_key(int x, _Value value) {
		int index = P[x];
		D[x] = value;
		while (index != 0 && D[H[index]] > D[H[parent(index)]]) {
			std::swap(P[H[index]], P[H[parent(index)]]);
			std::swap(H[index], H[parent(index)]);
			index = parent(index);
		}
	}
	_Value get_value(int index) { return D[index]; }
	int get_index(int vertex) { return P[vertex]; }
	void insert(int x, _Value value) {
		++_size;
		H[_size - 1] = x;
		D[x] = value;
		P[x] = _size - 1;
		int index = _size - 1;
		while (index != 0 && D[H[parent(index)]] < D[H[index]]) {
			std::swap(P[H[index]], P[H[parent(index)]]);
			std::swap(H[index], H[parent(index)]);
			index = parent(index);
		}
	}
	void erase(int x) {
		int index = P[x];
		std::swap(P[x], P[H[_size - 1]]);
		std::swap(H[index], H[_size - 1]);
		--_size;
		while (index != 0 && D[H[index]] > D[H[parent(index)]]) {
			std::swap(P[H[index]], P[H[parent(index)]]);
			std::swap(H[index], H[parent(index)]);
			index = parent(index);
		}
		while (index < _size) {
			int max_index = index;
			if (left(index) < _size && D[H[left(index)]] > D[H[max_index]]) max_index = left(index);
			if (right(index) < _size && D[H[right(index)]] > D[H[max_index]])
				max_index = right(index);
			if (max_index == index) break;
			std::swap(P[H[index]], P[H[max_index]]);
			std::swap(H[index], H[max_index]);
			index = max_index;
		}
		P[x] = -1;
	}
};