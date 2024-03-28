#pragma once
#include <stdexcept>

template <typename T>
inline T const& max(T const& a, T const& b) {
	return (a > b) ? a : b;
}

template <typename T>
inline T const& min(T const& a, T const& b) {
	return (a < b) ? a : b;
}

struct generic_exception : std::exception {
	const char* message;
	generic_exception(const char* message) noexcept : message(message) {}
	virtual const char* what() const noexcept { return message; }
};

template <typename T, typename U>
class pair {
   public:
	T first;
	U second;
	pair() {}
	pair(T const& a, U const& b) : first(a), second(b) {}
	pair(T&& a, U&& b) : first(std::forward<T>(a)), second(std::forward<U>(b)) {}
	bool operator<(pair<T, U> const& a) const {
		return (first < a.first || (first == a.first && second < a.second));
	}
	bool operator>(pair<T, U> const& a) const {
		return (first > a.first || (first == a.first && second > a.second));
	}
	bool operator==(pair<T, U> const& a) const { return first == a.first && second == a.second; }
	bool operator!=(pair<T, U> const& a) const { return !(*this == a); }
};

#define assert(...) \
	if (!(__VA_ARGS__)) throw generic_exception(#__VA_ARGS__)

template <typename _iter>
void reverse(_iter a, _iter b) {
	for (_iter ta = a, tb = b - 1; ta < tb; ++ta, --tb) { std::swap(*ta, *tb); }
}
