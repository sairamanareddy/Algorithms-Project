#pragma once

#include "util.hh"

template <typename _Key, typename _Value>
class map {
	class node;
	using pointer = node *;

   private:
	pointer root = nullptr;
	size_t _count = 0;

	static int get_bf(pointer _root) {
		int lh = _root->left ? _root->left->height : 0;
		int rh = _root->right ? _root->right->height : 0;
		return lh - rh;
	}

	static pointer right_rotate(pointer _root) {
		assert(_root->left != nullptr);
		pointer x = _root->left;
		pointer y = x->right;

		x->right = _root;
		_root->left = y;

		_root->fix_height();
		x->fix_height();

		x->parent = _root->parent;
		_root->parent = x;
		if (y) y->parent = _root;

		return x;
	}

	static pointer left_rotate(pointer _root) {
		assert(_root->right != nullptr);
		pointer x = _root->right;
		pointer y = x->left;

		x->left = _root;
		_root->right = y;

		_root->fix_height();
		x->fix_height();

		x->parent = _root->parent;
		_root->parent = x;
		if (y) y->parent = _root;

		return x;
	}

	static pointer find_successor(pointer _root) {
		if (_root->right) {
			pointer tmp = _root->right;
			while (tmp->left) tmp = tmp->left;
			return tmp;
		}
		pointer tmp1 = _root->parent, tmp2 = _root;
		while (tmp1) {
			if (tmp1->left == tmp2) return tmp1;
			tmp2 = tmp1;
			tmp1 = tmp1->parent;
		}
		return nullptr;
	}

	static pointer find_predecessor(pointer _root) {
		if (_root->left) {
			pointer tmp = _root->left;
			while (tmp->right) tmp = tmp->right;
			return tmp;
		}
		pointer tmp1 = _root->parent, tmp2 = _root;
		while (tmp1) {
			if (tmp1->right == tmp2) return tmp1;
			tmp2 = tmp1;
			tmp1 = tmp1->parent;
		}
		return nullptr;
	}

	static pointer _insert(pointer _root, pair<const _Key, _Value> const &elem, pointer _parent) {
		if (_root == nullptr) {
			_root = new node(elem, _parent);
			return _root;
		}
		if (_root->data.first < elem.first) _root->right = _insert(_root->right, elem, _root);
		else if (_root->data.first > elem.first)
			_root->left = _insert(_root->left, elem, _root);
		else
			return _root;
		_root->fix_height();
		int balance = get_bf(_root);

		// rotate if not balanced
		// Left Left Case
		if (balance > 1 && elem.first < _root->left->data.first) return right_rotate(_root);
		// Right Right Case
		if (balance < -1 && elem.first > _root->right->data.first) return left_rotate(_root);
		// Left Right Case
		if (balance > 1 && elem.first > _root->left->data.first) {
			_root->left = left_rotate(_root->left);
			return right_rotate(_root);
		}
		// Right Left Case
		if (balance < -1 && elem.first < _root->right->data.first) {
			_root->right = right_rotate(_root->right);
			return left_rotate(_root);
		}
		return _root;
	}

	static pointer _min_node(pointer _root) {
		pointer tmp = _root;
		while (tmp->left) tmp = tmp->left;
		return tmp;
	}

	static pointer _erase(pointer _root, _Key const &elem) {
		if (_root == nullptr) return _root;
		if (_root->data.first < elem.first) _root->right = _erase(_root->right, elem);
		else if (_root->data.first > elem.first)
			_root->left = _erase(_root->left, elem);
		else {
			if (!_root->left || !_root->right) {
				// single or no child
				pointer tmp = _root->left ? _root->left : _root->right;
				if (tmp != nullptr) tmp->parent = _root->parent;
				delete _root;
				return tmp;
			} else {
				pointer successor = find_successor(_root->right);
				_root->data = successor->data;
				_root->right = _erase(_root->right, successor->data);
			}
		}
		if (_root == NULL) return _root;

		_root->fix_height();

		int balance = get_bf(_root);

		// rotate the tree if unbalanced

		// Left Left Case
		if (balance > 1 && get_bf(_root->left) >= 0) return right_rotate(_root);

		// Left Right Case
		if (balance > 1 && get_bf(_root->left) < 0) {
			_root->left = left_rotate(_root->left);
			return right_rotate(_root);
		}
		// Right Right Case
		if (balance < -1 && get_bf(_root->right) <= 0) return left_rotate(_root);

		// Right Left Case
		if (balance < -1 && get_bf(_root->right) > 0) {
			_root->right = right_rotate(_root->right);
			return left_rotate(_root);
		}
		return _root;
	}

	static void _destroy(pointer _root) {
		if (!_root) return;
		_destroy(_root->left);
		_destroy(_root->right);
		delete _root;
	}

   public:
	class iterator;
	inline static const iterator npos{nullptr};
	iterator search(_Key const &elem) {
		pointer tmp = root;
		while (tmp) {
			if (tmp->data.first == elem) return iterator(tmp);
			else if (tmp->data.first < elem)
				tmp = tmp->right;
			else
				tmp = tmp->left;
		}
		return iterator(nullptr);
	}
	_Value &operator[](_Key const &key) {
		pointer tmp = root, tmp1 = nullptr;
		while (tmp) {
			tmp1 = tmp;
			if (tmp->data.first == key) return tmp->data.second;
			else if (tmp->data.first < key)
				tmp = tmp->right;
			else
				tmp = tmp->left;
		}
		auto elem = new node(pair<const _Key, _Value>(key, _Value()), tmp1);
		if (tmp1 == nullptr) root = elem;
		else if (key < tmp1->data.first)
			tmp1->left = elem;
		else
			tmp1->right = elem;
		++this->_count;
		return elem->data.second;
	}
	void erase(pair<const _Key, _Value> &elem) {
		if (!search(elem)) {
			--this->_count;
			root = _erase(root, elem);
		}
	}
	void insert(pair<const _Key, _Value> &elem) {
		if (!search(elem)) ++this->_count;
		root = _insert(root, elem, nullptr);
	}
	size_t size() { return this->_count; }
	iterator begin() { return iterator(_min_node(root)); }
	iterator end() { return npos; }
	~map() { _destroy(root); }
};

template <typename _Key, typename _Value>
class map<_Key, _Value>::node {
	friend class map<_Key, _Value>;

   private:
	using pointer = node *;
	pointer left = nullptr, right = nullptr, parent = nullptr;
	int height = 1;
	void fix_height() { height = max(left ? left->height : 0, right ? right->height : 0) + 1; }

   public:
	pair<const _Key, _Value> data;
	node(pair<const _Key, _Value> const &data) : data(data) {}
	node(pair<const _Key, _Value> const &data, pointer parent) : parent(parent), data(data) {}
};

template <typename _Key, typename _Value>
class map<_Key, _Value>::iterator {
   public:
	using iterator_category = std::bidirectional_iterator_tag;
	using difference_type = std::ptrdiff_t;
	using value_type = pair<const _Key, _Value>;
	using pointer = value_type *;
	using reference = value_type &;
	iterator(map<_Key, _Value>::node *p) { _t = p; }
	reference operator*() { return _t->data; }
	friend bool operator!=(iterator const &a, iterator const &b) { return a._t != b._t; }
	friend bool operator==(iterator const &a, iterator const &b) { return a._t == b._t; }
	pointer operator->() { return &_t->data; }
	iterator &operator++() {
		assert(_t, "Reached end of container");
		_t = find_successor(_t);
		return *this;
	}
	iterator operator++(int) {
		iterator _res = *this;
		_t = find_successor(_t);
		return _res;
	}
	iterator &operator--() {
		_t = find_predecessor(_t);
		return *this;
	}
	iterator operator--(int) {
		iterator _res = *this;
		_t = find_predecessor(_t);
		return *this;
	}
	operator bool() const { return _t != nullptr; }

   private:
	map<_Key, _Value>::node *_t = nullptr;
};
