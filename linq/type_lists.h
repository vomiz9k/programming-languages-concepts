#pragma once

#include <concepts>
#include <type_tuples.h>

namespace type_lists {

template <class TL>
concept TypeSequence = requires {
  typename TL::Head;
  typename TL::Tail;
};

struct Nil {};

template <class TL>
concept Empty = std::derived_from<TL, Nil>;

template <class TL>
concept TypeList = Empty<TL> || TypeSequence<TL>;

template <typename T, typename U>
struct Cons {
  using Head = T;
  using Tail = U;
};

namespace detail {

template <TypeList TL>
struct ToTupleImpl {
  using Type =
      typename type_tuples::Append<typename TL::Head,
                          typename ToTupleImpl<typename TL::Tail>::Type>;
};

template <Empty TL>
struct ToTupleImpl<TL> {
  using Type = type_tuples::TTuple<>;
};

}


template <TypeList TL>
using ToTuple = typename detail::ToTupleImpl<TL>::Type;

template <class>
struct FromTuple {};

template <class T, class... Ts>
struct FromTuple<type_tuples::TTuple<T, Ts...>> {
  using Head = T;
  using Tail = FromTuple<type_tuples::TTuple<Ts...>>;
};

template <class T>
struct FromTuple<type_tuples::TTuple<T>> {
  using Head = T;
  using Tail = Nil;
};

template <int count, typename>
struct TakeImpl {};

template <int count, TypeList TL>
struct Take {
  using Head = typename TL::Head;
  using Tail = Take<count - 1, typename TL::Tail>;
};

template <TypeList U>
struct Take<0, U> : Nil {};

template <int count, Empty TL>
struct Take<count, TL> : Nil {};


template <template <class> typename func, TypeList TL>
struct Map {
  using Head = func<typename TL::Head>;
  using Tail = Map<func, typename TL::Tail>;
};

template <template <class> typename func, Empty TL>
struct Map<func, TL> : Nil {};

template <typename T>
struct Repeat {
  using Head = T;
  using Tail = Repeat<T>;
};

template <int count, TypeList TL>
struct Drop : Drop<count - 1, typename TL::Tail> {};

template <TypeSequence TL>
struct Drop<0, TL> : TL {};

template <int count, Empty TL>
struct Drop<count, TL> : Nil {};


template <int count, typename T>
struct Replicate {
  using Head = T;
  using Tail = Replicate<count - 1, T>;
};

template <typename T>
struct Replicate<0, T> : Nil {};

template <template <class> typename func, typename T>
struct Iterate {
  using Head = T;
  using Tail = Iterate<func, func<T>>;
};

template<class...>
struct Cycle {};

template <TypeList TL, TypeList Start>
struct Cycle<TL, Start> {
  using Head = typename TL::Head;
  using Tail = Cycle<typename TL::Tail, Start>;
};

template <TypeList Start>
struct Cycle<Nil, Start> {
  using Head = typename Start::Head;
  using Tail = Cycle<typename Start::Tail, Start>;
};

template <TypeList TL>
struct Cycle<TL> : Cycle<TL, TL> {};

template <template <class> typename, TypeList>
struct Filter {};

template <template <class> typename func, TypeList TL>
requires(func<typename TL::Head>::Value == true) struct Filter<func, TL> {
  using Head = typename TL::Head;
  using Tail = Filter<func, typename TL::Tail>;
};

template <template <class> typename func, TypeList TL>
requires(func<typename TL::Head>::Value == false) struct Filter<func, TL> {
  using Head = typename Filter<func, typename TL::Tail>::Head;
  using Tail = typename Filter<func, typename TL::Tail>::Tail;
};

template <template <class> typename func, Empty TL>
struct Filter<func, TL> : Nil {};

template <template <class, class> typename OP, typename T, TypeList TL>
struct Scanl {
  using Head = T;
  using Tail = Scanl<OP, OP<T, typename TL::Head>, typename TL::Tail>;
};

template <template <class, class> typename OP, typename T, Empty TL>
struct Scanl<OP, T, TL> {
  using Head = T;
  using Tail = Nil;
};

namespace detail {

template <template <class, class> typename OP, typename T, TypeList TL>
struct FoldlImpl {
  using Type =
      typename FoldlImpl<OP, OP<T, typename TL::Head>, typename TL::Tail>::Type;
};

template <template <class, class> typename OP, typename T, Empty TL>
struct FoldlImpl<OP, T, TL> : Nil {
  using Type = T;
};

}

template <template <class, class> typename OP, typename T, TypeList TL>
using Foldl = typename detail::FoldlImpl<OP, T, TL>::Type;


namespace detail {

template <int count, TypeList TL, TypeList Left>
struct InitsImpl {
  using Head = Take<count, TL>;
  using Tail = InitsImpl<count + 1, TL, typename Left::Tail>;
};

template <int count, TypeList TL, Empty Left>
struct InitsImpl<count, TL, Left> {
  using Head = Take<count, TL>;
  using Tail = Nil;
};

}
template <TypeList TL>
using Inits = detail::InitsImpl<0, TL, TL>;

template <TypeList TL>
struct Tails {
  using Head = TL;
  using Tail = Tails<typename TL::Tail>;
};

template <Empty TL>
struct Tails<TL> {
  using Head = TL;
  using Tail = Nil;
};

template <TypeList TL1, TypeList TL2>
struct Zip2 {
  using Head = type_tuples::TTuple<typename TL1::Head, typename TL2::Head>;
  using Tail = Zip2<typename TL1::Tail, typename TL2::Tail>;
};

template <Empty TL1, TypeList TL2>
struct Zip2<TL1, TL2> : Nil {};

template <TypeList TL1, Empty TL2>
struct Zip2<TL1, TL2> : Nil {};


template <TypeList... TLs>
struct Zip {
using Head = type_tuples::TTuple<typename TLs::Head...>;
using Tail = Zip<typename TLs::Tail...>;
};

template <template <class, class> typename EQ, TypeList TL>
struct GroupBy {};

template <template <class, class> typename EQ, TypeList TL>
requires Empty<typename TL::Tail>
struct GroupBy<EQ, TL> {
  using Head = Cons<typename TL::Head, Nil>;
  using Tail = Nil;
};

template <template <class, class> typename EQ, TypeList TL>
requires EQ<typename TL::Head,
            typename TL::Tail::Head>::Value
struct GroupBy<EQ, TL> {
  using Head =
      Cons<typename TL::Head, typename GroupBy<EQ, typename TL::Tail>::Head>;
  using Tail = typename GroupBy<EQ, typename TL::Tail>::Tail;
};

template <template <class, class> typename EQ, TypeList TL>
requires(!EQ<typename TL::Head,
             typename TL::Tail::Head>::Value)
struct GroupBy<EQ, TL> {
  using Head = Cons<typename TL::Head, Nil>;
  using Tail = GroupBy<EQ, typename TL::Tail>;
};

}  // namespace type_lists