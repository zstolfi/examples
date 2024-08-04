#pragma once
#include <map>
#include <tuple>
#include <type_traits>

template <class T> concept Function_c = std::is_function_v<T>;

template <class T> struct FunctionSig;
template <class R, class... As> struct FunctionSig<R(As...)> {
	using Args = std::tuple<As...>;
	using Ret = R;
};

template <Function_c F>
class Tab {
	using Sig = FunctionSig<F>;
	using Tab_t = std::map<typename Sig::Args, typename Sig::Ret>;
	F* m_function;
	Tab_t m_tab;

public:
	Tab(F* function) : m_function{function} {}
	Tab(Tab_t tab, F* function) : m_function{function}, m_tab{tab} {}

	auto operator()(auto&&... a) {
		auto args = typename Sig::Args {a...};
		if (m_tab.contains(args)) return m_tab.at(args);
		return m_tab[args] = m_function(a...);
	}
};
