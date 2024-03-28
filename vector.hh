#pragma once
// for memcpy and memmove
#include <cstring>
// for dynamic memory
#include <memory>
// for template checks
#include <type_traits>

template <typename T>
class vector {
   private:
	using pointer = T *;
	pointer _start = nullptr;
	pointer _finish = nullptr;
	pointer _end_of_storage = nullptr;
	std::allocator<T> _alloc;
	// Destroy allocated memory
	void _Destroy(pointer begin, pointer end) {
		if (!std::is_trivially_destructible<T>::value) std::destroy_n(begin, end - begin);
		_alloc.deallocate(begin, end - begin);
	}
	// reallocate an uninitialized chunk of extra memory
	void _realloc() {
		if (_finish == _end_of_storage) {
			pointer _old_start = _start;
			pointer _old_finish = _finish;
			pointer _old_end_of_storage = _end_of_storage;
			size_t _n_elems = _old_finish - _old_start;
			_start = _alloc.allocate(_n_elems + 1, _start);
			_end_of_storage = _start + _n_elems + 1;
			_finish = _start + _n_elems;
			if (_old_start != _start) {
				std::memmove(_start, _old_start, _n_elems * sizeof(T));
				_Destroy(_old_start, _old_end_of_storage);
			}
		}
	}
	// copy from other vector
	void _copy(vector<T> const &other) {
		if (_start != nullptr) _Destroy(_start, _end_of_storage);
		size_t capacity = other._end_of_storage - other._start;
		size_t n_elems = other._finish - other._start;
		_start = _alloc.allocate(capacity);
		_finish = _start + n_elems;
		_end_of_storage = _start + capacity;
		for (pointer tmp1 = _start, tmp2 = other._start; tmp2 != other._finish;) *tmp1++ = *tmp2++;
	}
	// move from another vector
	void _move(vector<T> &&other) {
		if (_start != nullptr) _Destroy(_start, _end_of_storage);
		_start = other._start;
		_finish = other._finish;
		_end_of_storage = other._end_of_storage;
		other._start = nullptr;
		other._finish = nullptr;
		other._end_of_storage = nullptr;
	}

   public:
	class iterator;
	vector() : _start(nullptr), _finish(nullptr), _end_of_storage(nullptr) {}
	vector(size_t capacity) {
		_start = _alloc.allocate(capacity);
		_finish = _start + capacity;
		_end_of_storage = _finish;
		std::uninitialized_default_construct_n(_start, capacity);
	}
	size_t size() const { return _finish - _start; }
	vector(size_t capacity, T const &elem) : vector(capacity) {
		for (pointer _tmp = _start; _tmp != _finish; _tmp++) { *_tmp = elem; }
	}
	vector(vector<T> const &other) { _copy(other); }
	vector(vector<T> &&other) { _move(std::forward<vector<T>>(other)); }

	// copy assignment
	vector<T> &operator=(vector<T> const &other) {
		_copy(other);
		return *this;
	}

	// move assignment
	vector<T> &operator=(vector<T> &&other) {
		_move(std::forward<vector<T>>(other));
		return *this;
	}

	bool operator==(vector<T> const &other) const {
		if (this->size() != other.size()) return false;
		for (pointer t1 = _start, t2 = other._start; t1 != _finish;) {
			if (*t1++ != *t2++) return false;
		}
		return true;
	}
	bool operator!=(vector<T> const &other) const { return !(*this == other); }
	// index access
	T &operator[](size_t const &_i) const { return *(_start + _i); }
	void push_back(T const &elem) {
		// realloc if out of space
		_realloc();
		// placement new
		new (_finish++) T(elem);
	}
	void reserve(size_t capacity) {
		pointer _old_start = _start;
		pointer _old_finish = _finish;
		pointer _old_end_of_storage = _end_of_storage;
		size_t n_elems = _old_finish - _old_start;
		_start = _alloc.allocate(capacity, _start);
		_end_of_storage = _start + capacity;
		_finish = _start + n_elems;
		if (_old_start != _start) {
			std::memmove(_start, _old_start, n_elems * sizeof(T));
			_Destroy(_old_start, _old_end_of_storage);
		}
	}
	void push_back(T &&elem) {
		// realloc if out of space
		_realloc();
		// placement new with move constructor
		new (_finish++) T(std::forward<T>(elem));
	}
	iterator begin() const { return iterator(_start); }
	iterator end() const { return iterator(_finish); }
	~vector() { _Destroy(_start, _finish); }
};

template <typename T>
class vector<T>::iterator {
   public:
	using iterator_category = std::random_access_iterator_tag;
	using difference_type = std::ptrdiff_t;
	using value_type = T;
	using pointer = value_type *;
	using reference = value_type &;
	iterator(pointer p) { _t = p; }
	reference operator*() { return *_t; }
	friend bool operator!=(iterator const &a, iterator const &b) { return a._t != b._t; }
	friend bool operator==(iterator const &a, iterator const &b) { return a._t == b._t; }
	bool operator<(iterator const &other) { return _t < other._t; }
	bool operator>(iterator const &other) { return _t > other._t; }
	difference_type operator-(iterator const &other) { return _t - other._t; }
	iterator operator-(difference_type _n) { return iterator(_t - _n); }
	iterator operator+(difference_type _n) { return iterator(_t + _n); }
	pointer operator->() { return _t; }
	iterator &operator++() {
		++_t;
		return *this;
	}
	iterator operator++(int) {
		iterator _res = *this;
		++_t;
		return _res;
	}
	iterator &operator--() {
		--_t;
		return *this;
	}
	iterator operator--(int) {
		iterator _res = *this;
		--_t;
		return *this;
	}

   private:
	pointer _t = nullptr;
};
