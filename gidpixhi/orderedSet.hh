#pragma once
#include <vector>
#include <iterator>
#include <algorithm>

template <class T>
struct Set : public std::vector<T> {
	Set() = default;
	Set(Set const&) = default;
	Set& operator=(Set const&) = default;
	template<std::input_iterator I>
	Set(I first, I last): std::vector<T>::vector(first, last) { normalize(); }
	Set(std::initializer_list<T> il): Set(il.begin(), il.end()) { normalize(); }
	Set(std::size_t count, T const& val = T()) { if (count) *this = {val}; }

	template <std::input_iterator I>
	void insert(I first, I last) {
		std::vector<T>::insert(this->end(), first, last);
		normalize();
	}

	void insert(T const& val) {
		std::vector<T>::insert(this->end(), val);
		normalize();
	}

	void insert(std::size_t count, T const& val) {
		if (count) insert(val);
	}

	const T& operator[](std::size_t n) const {
		return std::vector<T>::operator[](n);
	}

	bool contains(T const& val) {
		return std::binary_search(this->begin(), this->end(), val);
	}

private:
	void normalize() {
		std::sort(this->begin(), this->end());
		auto last = std::unique(this->begin(), this->end());
		this->erase(last, this->end());
	}
};
