#pragma once

#include <type_lists.h>
#include <value_types.h>

namespace value_types {
template <int curr>
struct Nats {
  using Head = ValueTag<curr>;
  using Tail = Nats<curr + 1>;
};

template <int curr=0>
struct Fib {
  using Head =
      ValueTag<Fib<curr - 1>::Head::Value + Fib<curr - 2>::Head::Value>;
  using Tail = Fib<curr + 1>;
};

template <>
struct Fib<0> {
  using Head = ValueTag<0>;
  using Tail = Fib<1>;
};

template <>
struct Fib<1> {
  using Head = ValueTag<1>;
  using Tail = Fib<2>;
};

using Fibonacci = Fib<0>;

template <int num, int curr>
struct CheckPrime {
  static constexpr bool Value =
      (num % curr != 0) && CheckPrime<num, curr - 1>::Value;
};

template <int num>
struct CheckPrime<num, 1> {
  static constexpr bool Value = true;
};

template <int num>
using isPrime = CheckPrime<num, num - 1>;

template <int number, int last>
struct PrimeList {};

template <int number, int last>
requires(isPrime<last>::Value) struct PrimeList<number, last> {
  using Head = ValueTag<last>;
  using Tail = PrimeList<number + 1, last + 1>;
};

template <int number, int last>
requires(!isPrime<last>::Value) struct PrimeList<number, last> {
  using Head = typename PrimeList<number, last + 1>::Head;
  using Tail = typename PrimeList<number, last + 1>::Tail;
};

}  // namespace value_types

using Nats = value_types::Nats<0>;
using Fib = value_types::Fib<0>;
using Primes = value_types::PrimeList<0, 2> /*???*/;