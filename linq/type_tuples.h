#pragma once


namespace type_tuples
{

template<class... Ts>
struct TTuple {
};

template<class TT>
concept TypeTuple = requires(TT t) { []<class... Ts>(TTuple<Ts...>){}(t); };

namespace detail {

template <typename, typename>
struct AppendImpl {};

template <typename T, class... Ts>
struct AppendImpl<T, type_tuples::TTuple<Ts...>> {
  using Type = type_tuples::TTuple<T, Ts...>;
};

template <typename T>
struct AppendImpl<T, type_tuples::TTuple<>> {
  using Type = type_tuples::TTuple<T>;
};

}

template <typename T, type_tuples::TypeTuple U>
using Append = typename detail::AppendImpl<T, U>::Type;


} // namespace type_tuples