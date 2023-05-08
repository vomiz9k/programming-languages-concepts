#include <async.h>
#include <thread_pool.h>
#include <iostream>

int main() {

    static ThreadPool threadPool(4);
    bool f3_called = false;

    auto f1 = async(threadPool, [](int a, int b) -> int {
        std::cout << "F1\n";
        return a + b;
    }, 1, 2);
    auto f2 = f1.then([](int x) {
        std::cout << "F2\n";
        return x * x * x;
    });
    auto f3 = f2.then([&f3_called](int x) {
        f3_called = true;
        std::cout << "F3\n";
        throw std::runtime_error("f3 failed successfully");
        return 0;
    });
    auto f4 = f3.then([](int x) {
        std::cout << "F4\n";
        return 0;
    });

    while (!f3_called) {
        std::this_thread::yield();
    }

    std::cout
        << "f1: " << f1.get() << '\n'
        << "f2: " << f2.get() << '\n'
        << "f3 has exception: " << f3.hasException() << '\n'
        << "f4 has exception: " << f4.hasException() << '\n'
        << "f4 has value    : " << f4.hasValue() << '\n';

    try {
        f3.get();
    } catch(const std::runtime_error& err) {
        std::cout << "exception: " << err.what() << '\n';
    }

    auto f5 = async(threadPool, [](int a, int b) {
        for(int i = 0; i < 100000; ++i) {
            a += i % b;
        }
        std::cout << "F5\n";
        return a;
    }, 100, 5);

    std::cout << "waiting for f5";
    while (f5.tryGet() == std::nullopt) {
        std::cout << ".";
    }
    std::cout << '\n' << "f5: " << f5.get() << '\n';

    auto f5_sync = async([](int a, int b) {
        for(int i = 0; i < 100000; ++i) {
            a += i % b;
        }
        std::cout << "F5_sync\n";
        return a;
    }, 100, 5);

    std::cout << "f5_sync: ";
    std::cout << f5_sync.get() << '\n';



}