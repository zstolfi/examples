#pragma once
#include <optional>
#include <ranges>
namespace ranges = std::ranges;

template <class T>
class Extrema {
	bool m_empty = true;
	std::optional<T> m_min {};
	std::optional<T> m_max {};

public:
	Extrema() {}
	Extrema(ranges::input_range auto&& v) { for (auto&& value : v) add(value); }

	Extrema& add(const T& value) {
		if (m_empty || value < *m_min) m_min = value;
		if (m_empty || value > *m_max) m_max = value;
		m_empty = false;
		return *this;
	}

	bool empty() const { return m_empty; }
	const T& getMin() const { return *m_min; }
	const T& getMax() const { return *m_max; }
};

template <ranges::input_range IR>
Extrema(IR&&) -> Extrema<ranges::range_value_t<IR>>;
