#pragma once
// queue using linked lists
// no headers needed
template <typename T>
class queue {
   private:
	class node;
	using pointer = node*;
	pointer _head = nullptr, _tail = nullptr;
	size_t _size = 0;

   public:
	inline size_t size() const { return _size; }
	void push(T const& a) {
        ++_size;
		if (_tail == nullptr) {
			_head = new node(a);
            _tail = _head;
			return;
		}
		_tail->next = new node(a);
		_tail = _tail->next;
	}
	T pop() {
        --_size;
		assert(_head != nullptr);
		T res = _head->data;
		pointer tmp = _head->next;
		delete _head;
		_head = tmp;
        if (tmp == nullptr) _tail = nullptr;
		return res;
	}
	~queue() {
		for (pointer tmp1 = _head; tmp1 != nullptr;) {
			pointer tmp2 = tmp1->next;
			delete tmp1;
			tmp1 = tmp2;
		}
	}
};

template <typename T>
class queue<T>::node {
   public:
	T data;
	node* next;
	node(T const& _data) : data(_data), next(nullptr) {}
	node(T const& _data, node* _next) : data(_data), next(_next) {}
};