#include <value_lists.h>
#include <type_lists.h>

template<typename T>
using DivisibleBy3 = value_types::ValueTag<T::Value % 3 == 0>;

template<typename T>
using SquareEven = value_types::ValueTag<T::Value % 2 == 0 ? T::Value * T::Value : T::Value>;

int main() {
    using namespace type_lists;
    static_assert(
        std::same_as<
            ToTuple<
                Take<
                    5,
                    Map<
                        SquareEven,
                        Filter<
                            DivisibleBy3,
                            value_types::Fibonacci
                        >
                    >
                >
            >,
            value_types::VTuple<int, 0, 3, 21, 20736, 987>
        >
    );
}

