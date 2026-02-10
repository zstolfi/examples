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

	std::size_t insert(T const& val) {
		std::vector<T>::insert(this->end(), val);
		normalize();
		// TODO: Avoid having to 'rediscover' the index.
		if constexpr (Sorted) {
			return std::distance(
				this->begin(),
				std::lower_bound(this->begin(), this->end(), val)
			);
		}
		else {
			return (this->size()-1) - std::distance(
				this->rbegin(),
				std::find(this->rbegin(), this->rend(), val)
			);
		}
	}

	void reassign(std::size_t i, T const& val) {
		// TODO: Throw exception if 'val' disrupts set order, or uniqueness.
		std::vector<T>::at(i) = val;
	}

	auto erase(auto&& ... args) {
		auto result = std::vector<T>::erase(args ... );
		normalUntil = this->size();
		return result;
	}

	void resize(auto&& ... args) {
		std::vector<T>::resize(args ... );
		normalUntil = this->size();
	}

	const T& operator[](std::size_t i) const {
		return std::vector<T>::at(i);
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
				if (std::find(this->begin(), it, *it) != it) {
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
