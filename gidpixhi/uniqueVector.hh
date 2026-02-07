#pragma once
#include <vector>
#include <iterator>
#include <algorithm>
#include <functional>

template <class T, class Equivalence, bool Sorted>
struct UniqueVector : std::vector<T> {
	using value_equivalence = Equivalence;
	static constexpr bool is_sorted = Sorted;

	UniqueVector() = default;
	UniqueVector(UniqueVector const&) = default;
	UniqueVector& operator=(UniqueVector const&) = default;

	template<std::input_iterator I>
	UniqueVector(I first, I last)
	: std::vector<T>::vector(first, last) { normalize(); }

	UniqueVector(std::initializer_list<T> il)
	: UniqueVector(il.begin(), il.end()) { normalize(); }

	UniqueVector(std::size_t count, T const& val = T())
	{ if (count) *this = {val}; }

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

	bool contains(T const& val) const {
		return std::binary_search(this->begin(), this->end(), val);
	}

private:
	std::size_t normalUntil {0};

	void normalize() {
		if constexpr (Sorted) {
			std::sort(this->begin(), this->end());
			auto last = std::unique(this->begin(), this->end(), Equivalence {});
			this->erase(last, this->end());
		}
		else {
			for (auto it=this->begin()+normalUntil; it!=this->end(); /**/) {
				if (std::find(this->begin(), it-1, *it) != it-1) {
					it = this->erase(it);
				}
				else ++it;
			}
			normalUntil = this->size();
		}
	}
};

template <class T, class Equivalence = std::equal_to<>>
struct Set : UniqueVector<T, Equivalence, true> {
    using UniqueVector<T, Equivalence, true>::UniqueVector;
};

template <class T, class Equivalence = std::equal_to<>>
struct UnorderedSet : UniqueVector<T, Equivalence, false> {
    using UniqueVector<T, Equivalence, false>::UniqueVector;
};
