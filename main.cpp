#include <chrono>
#include <iostream>
#include <random>

#include "graph.hh"

int main() {
	const int N = 5000;
	std::uniform_int_distribution<> vertex_gen(0, N - 1);
	auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	std::default_random_engine engine(42);

	const int type = 1;

	graph G(5000, type, engine);

	for (int pairs = 0; pairs < 5; ++pairs) {
		int u = vertex_gen(engine), v = vertex_gen(engine);
		if (u == v) continue;
		auto t1 = std::chrono::high_resolution_clock::now();
		auto r1 = G.dijkstra_1(u, v);
		auto t2 = std::chrono::high_resolution_clock::now();
		auto r2 = G.dijkstra_2(u, v);
		auto t3 = std::chrono::high_resolution_clock::now();
		auto r3 = G.kruskal(u, v);
		auto t4 = std::chrono::high_resolution_clock::now();
		std::cout << "Running Times:" << std::endl;
		std::cout << "Dijkstra without heap: "
				  << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()
				  << std::endl;
		std::cout << "Dijkstra with heap: "
				  << std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2).count()
				  << std::endl;
		std::cout << "Kruskal's: "
				  << std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count()
				  << std::endl;

		std::cout << r1.second << std::endl;
		std::cout << r2.second << std::endl;
		std::cout << r3.second << std::endl;
	}
}