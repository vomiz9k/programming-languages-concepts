#pragma once

#include <future.h>
#include <vector>
#include <thread>
#include <set>
#include <queue>
#include <mutex>
#include <memory>

#include <thread_pool.h>

template<typename F, typename... Args>
Future<std::invoke_result_t<F, Args...>> async(ThreadPool& threadPool, F&& func, Args&&... args) {
    Future<std::invoke_result_t<F, Args...>> future(threadPool);
    Promise<std::invoke_result_t<F, Args...>> promise = future.makePromise();

    threadPool.Exec([func = std::move(func), tup=std::make_tuple(std::move(args)...), promise = std::move(promise)]() mutable {
        promise.setValue(std::apply([func = std::move(func)](auto const&... args) -> decltype(auto) {
            return func(args...);
        }, tup));
    });
    return future;
}

template<typename F, typename... Args>
Future<std::invoke_result_t<F, Args...>> async(F&& func, Args&&... args) {
    Future<std::invoke_result_t<F, Args...>> future;
    Promise<std::invoke_result_t<F, Args...>> promise = future.makePromise();

    promise.setValue(func(args...));
    return future;
}