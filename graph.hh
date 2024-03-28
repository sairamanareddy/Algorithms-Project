#pragma once
// only for std::shuffle
#include <algorithm>
// for std::cout
#include <iostream>
// only for std::uniform_int_distribution<> and std::default_random_engine
#include <random>

#include "heap.hh"
#include "map.hh"
#include "queue.hh"
#include "util.hh"
#include "vector.hh"

class graph {
   private:
	std::default_random_engine _engine;
	vector<vector<pair<int, int64_t>>> G;
	std::uniform_int_distribution<> _vertex_gen;
	std::uniform_int_distribution<int64_t> _weight_gen;
	vector<pair<int, int>> edge_index;
	vector<int64_t> weights;
	class dsu;
	int _n = 0;
	int _m = 0;
	// create a graph with a single cycle using a permutation
	void _create_with_cycle(map<pair<int, int>, int64_t>& edges) {
		vector<int> permutation(_n);
		for (int i = 0; i < _n; ++i) permutation[i] = i;

		std::shuffle(permutation.begin(), permutation.end(), _engine);

		for (int i = 0; i < _n; ++i) {
			int u = min(permutation[i], permutation[(i + 1) % _n]);
			int v = max(permutation[i], permutation[(i + 1) % _n]);
			int w = _weight_gen(_engine);
			if (u > v) std::swap(u, v);
			G[u].push_back({v, w});
			G[v].push_back({u, w});
			_m += 2;
			edges[{u, v}] = w;
		}
	}
	// In the first type, we sample pairs of vertices randomly and assign weights
	void _create_1(map<pair<int, int>, int64_t>& edges) {
		_create_with_cycle(edges);
		while (edges.size() < 3 * _n) {
			int u = _vertex_gen(_engine);
			int v = _vertex_gen(_engine);
			if (u > v) std::swap(u, v);
			int64_t w = _weight_gen(_engine);
			if (u != v && !edges.search({u, v})) {
				G[u].push_back({v, w});
				G[v].push_back({u, w});
				_m += 2;
				edges[{u, v}] = w;
			}
		}
	}
	// In the second type, we sample number of neighbours and neighbours for each vertex
	int _create_2(map<pair<int, int>, int64_t>& edges) {
		_create_with_cycle(edges);
		int lo = (_n / 5 - 50), hi = (_n / 5 + 50);
		std::uniform_int_distribution<> _n_gen(max(lo, 0), hi);
		vector<int> n_neighs(_n), permutation(_n);
		for (int i = 0; i < _n; ++i) {
			n_neighs[i] = _n_gen(_engine);
			permutation[i] = i;
		}
		while (true) {
			std::shuffle(permutation.begin(), permutation.end(), _engine);
			int flag = -1;
			for (int i = 0; i < _n && flag == -1; ++i)
				if (G[permutation[i]].size() < n_neighs[permutation[i]]) flag = i;
			if (flag == -1) break;
			int u = permutation[flag];
			for (int j = 1; j < _n && G[u].size() < n_neighs[u]; ++j) {
				int v = permutation[(flag + j) % _n];
				if (G[v].size() < hi && u != v && !edges.search({min(u, v), max(u, v)})) {
					int64_t w = _weight_gen(_engine);
					G[u].push_back({v, w});
					G[v].push_back({u, w});
					_m += 2;
					edges[{min(u, v), max(u, v)}] = w;
				}
			}
			if (G[u].size() < n_neighs[u]) return -1;
		}
		return 0;
	}
	// find max fringer using a linear scan (without heap)
	int _get_max_fringer(vector<int> const& status, vector<int64_t> const& b_width) {
		int64_t max_bw = 0, max_fringer = -1;
		for (int i = 0; i < _n; ++i) {
			if (status[i] == 1 && max_bw < b_width[i]) {
				max_bw = b_width[i];
				max_fringer = i;
			}
		}
		return max_fringer;
	}
	// construct path by using dad array
	vector<int> _retrace_path(vector<int> const& dad, const int s, const int t) {
		vector<int> ans;
		for (int c = t; c != s; c = dad[c]) { ans.push_back(c); }
		ans.push_back(s);
		reverse(ans.begin(), ans.end());
		return ans;
	}
	// BFS to find path in MST from s to t
	pair<vector<int>, int64_t> _bfs(vector<vector<pair<int, int64_t>>> const& tree, const int s,
									const int t) {
		vector<int> dad(_n, -1);
		vector<int64_t> b_width(_n, 0);
		queue<int> q;
		dad[s] = s;
		b_width[s] = INT32_MAX;
		q.push(s);
		while (q.size() > 0) {
			int u = q.pop();
			if (u == t) break;
			for (auto e : tree[u]) {
				if (dad[e.first] == -1) {
					dad[e.first] = u;
					b_width[e.first] = min(b_width[u], e.second);
					q.push(e.first);
				}
			}
		}
		pair<vector<int>, int64_t> ans(_retrace_path(dad, s, t), b_width[t]);
		return ans;
	}

   public:
	graph(int n, int type, std::default_random_engine& _engine)
		: G(n), _engine(_engine), _vertex_gen(0, n - 1), _weight_gen(1, INT32_MAX), _n(n) {
		map<pair<int, int>, int64_t> edges;
		if (type == 1) {
			_create_1(edges);
		} else {
			assert(type == 2);
			for (int i = 0; i < n; ++i) G[i].reserve(n / 5);
			while (true) {
				int res = _create_2(edges);
				if (res == 0) break;
				else
					edges = decltype(edges)();	// reset
			}
		}
		edge_index = vector<pair<int, int>>(_m / 2);
		weights = vector<int64_t>(_m / 2);
		assert(edges.size() == _m / 2);
		auto it = edges.begin();
		for (int count = 0; count < _m / 2; ++count, ++it) {
			edge_index[count] = it->first;
			weights[count] = it->second;
		}
		assert(!it);
		int min_deg = INT32_MAX, sum_deg = 0, max_deg = 0;
		for (int i = 0; i < n; ++i) {
			min_deg = min(min_deg, (int)G[i].size());
			max_deg = max(max_deg, (int)G[i].size());
			sum_deg += G[i].size();
		}
		std::cout << "Graph has been created" << std::endl;
		std::cout << "Min Degree: " << min_deg << " Max Degree: " << max_deg
				  << " Avg Degree: " << (double)(sum_deg) / n << std::endl;
	}
	// Dijkstra without heap
	pair<vector<int>, int64_t> dijkstra_1(const int s, const int t) {
		vector<int> status(_n, -1), dad(_n, -1);
		vector<int64_t> b_width(_n, 0);
		status[s] = 0;
		b_width[s] = INT32_MAX;
		dad[s] = s;
		int fringer_count = 0;
		auto add_fringer = [&](int v) {
			status[v] = 1;
			++fringer_count;
		};
		auto remove_fringer = [&](int v) {
			status[v] = 0;
			--fringer_count;
		};
		for (auto edge : G[s]) {
			add_fringer(edge.first);
			b_width[edge.first] = edge.second;
			dad[edge.first] = s;
		}
		while (fringer_count > 0) {
			int v = _get_max_fringer(status, b_width);
			remove_fringer(v);
			for (auto edge : G[v]) {
				if (status[edge.first] == -1) {
					dad[edge.first] = v;
					b_width[edge.first] = min(b_width[v], edge.second);
					add_fringer(edge.first);
				} else if (status[edge.first] == 1 &&
						   b_width[edge.first] < min(b_width[v], edge.second)) {
					dad[edge.first] = v;
					b_width[edge.first] = min(b_width[v], edge.second);
				}
			}
		}
		pair<vector<int>, int64_t> ans(_retrace_path(dad, s, t), b_width[t]);
		return ans;
	}
	// Dijikstra with heap
	pair<vector<int>, int64_t> dijkstra_2(const int s, const int t) {
		vector<int> status(_n, -1), dad(_n, -1);
		vector<int64_t> b_width(_n, 0);
		status[s] = 0;
		b_width[s] = INT32_MAX;
		dad[s] = s;
		heap<int64_t> H(_n);
		auto add_fringer = [&](int v) {
			status[v] = 1;
			H.insert(v, b_width[v]);
		};
		auto remove_fringer = [&](int v) {
			status[v] = 0;
			H.erase(v);
		};
		for (auto& edge : G[s]) {
			b_width[edge.first] = edge.second;
			dad[edge.first] = s;
			add_fringer(edge.first);
		}
		while (H.size() > 0) {
			int v = H.peek().first;
			remove_fringer(v);
			for (auto& edge : G[v]) {
				if (status[edge.first] == -1) {
					dad[edge.first] = v;
					b_width[edge.first] = min(b_width[v], edge.second);
					add_fringer(edge.first);
				} else if (status[edge.first] == 1 &&
						   b_width[edge.first] < min(b_width[v], edge.second)) {
					dad[edge.first] = v;
					b_width[edge.first] = min(b_width[v], edge.second);
					H.increase_key(edge.first, b_width[edge.first]);
				}
			}
		}
		pair<vector<int>, int64_t> ans(_retrace_path(dad, s, t), b_width[t]);
		return ans;
	}
	// Kruskal Algorithm
	pair<vector<int>, int64_t> kruskal(const int s, const int t);
};

class graph::dsu {
   private:
	int _n;
	vector<int> parent, rank;

   public:
	dsu(int n) : _n(n), parent(n), rank(n) {
		for (int i = 0; i < _n; ++i) {
			parent[i] = i;
			rank[i] = 1;
		}
	}
	int find(int x) {
		if (parent[x] == x) return x;
		else
			return (parent[x] = find(parent[x]));
	}
	void unify(int a, int b) {
		if (rank[a] >= rank[b]) parent[b] = a;
		else if (rank[b] > rank[a])
			parent[a] = b;
		if (rank[a] == rank[b]) ++rank[a];
	}
};

pair<vector<int>, int64_t> graph::kruskal(const int s, const int t) {
	heap<int64_t> H(weights);
	vector<vector<pair<int, int64_t>>> tree(_n);
	dsu _forest(_n);
	while (H.size() > 0) {
		auto w = H.extract_max();
		pair<int, int> edge = edge_index[w.first];
		int a = _forest.find(edge.first), b = _forest.find(edge.second);
		if (a != b) {
			_forest.unify(a, b);
			tree[edge.first].push_back({edge.second, w.second});
			tree[edge.second].push_back({edge.first, w.second});
		}
	}
	return _bfs(tree, s, t);
}