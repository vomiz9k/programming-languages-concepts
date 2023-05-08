#pragma once

#include <shared_state.h>
#include <promise.h>
#include <thread>
#include <iostream>

template<typename T>
class Future {
public:
    Future() {
        sharedState = std::make_shared<SharedState<T>>();
    }

    Future(ThreadPool& threadPool) {
        sharedState = std::make_shared<SharedState<T>>(&threadPool);
    }

    T get() {
        while (!sharedState->hasValue() && !sharedState->hasException()) {
            std::this_thread::yield();
        }
        if (sharedState->hasException()) {
            std::rethrow_exception(sharedState->getException());
        }
        return sharedState->getValue();
    }

    std::optional<T> tryGet() {
        if (sharedState->hasException()) {
            std::rethrow_exception(sharedState->getException());
        }
        if (sharedState->hasValue()) {
            return sharedState->getValue();
        }
        return std::nullopt;
    }

    bool hasValue() {
        return sharedState->hasValue();
    }

    bool hasException() {
        return sharedState->hasException();
    }

    Promise<T> makePromise() {
        return Promise<T>(sharedState);
    }

    template<typename F>
    Future<std::invoke_result_t<F, T>> then(F&& func) {
        auto future = Future<std::invoke_result_t<F, T>>();
        auto promise = future.makePromise();
        if (sharedState->hasException()) {
            promise.setException(sharedState->getException());
        }
        sharedState->setCallback([func = std::move(func), promise=std::move(promise)](const T& value) mutable {
            std::invoke_result_t<F, T> result;
            try {
                result = func(value);
            } catch(...) {
                promise.setException(std::current_exception());
                return;
            }

            promise.setValue(result);
        });
        return future;
    }

private:
    std::shared_ptr<SharedState<T>> sharedState;
};