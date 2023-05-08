#pragma once

#include <mutex>
#include <optional>
#include <exception>
#include <iostream>
#include <thread_pool.h>

template<typename T>
class SharedState {
public:
    SharedState(ThreadPool* threadPool) : threadPool(threadPool) {}
    SharedState() = default;

    void setValue(const T& value) {
        std::lock_guard guard(mutex);
        valueOpt.emplace(value);
        if (callback != nullptr) {
            processCallback();
        }
    }

    void setValue(T&& value) {
        std::lock_guard guard(mutex);
        valueOpt.emplace(std::move(value));
        if (callback != nullptr) {
            processCallback();
        }
    }

    T getValue() {
        std::lock_guard guard(mutex);
        return valueOpt.value();
    }

    bool hasValue() {
        std::lock_guard guard(mutex);
        return valueOpt.has_value();
    }

    void setException(std::exception_ptr exc) {
        std::lock_guard guard(mutex);
        exceptionPtr = exc;
    }

    std::exception_ptr getException() {
        std::lock_guard guard(mutex);
        return exceptionPtr;
    }

    bool hasException() {
        std::lock_guard guard(mutex);
        return exceptionPtr != nullptr;
    }

    struct BaseCallback {
        virtual void call(const T& value) = 0;
        virtual ~BaseCallback() = default;
    };

    template<typename F>
    struct Callback: public BaseCallback {
        Callback(F&& func) : func(std::move(func)) {}
        virtual void call(const T& value) override {
            func(value);
        }

        virtual ~Callback() = default;
        F func;
    };

    template<typename F>
    void setCallback(F&& func) {
        mutex.lock();
        callback = std::make_shared<Callback<F>>(std::move(func));
        if (valueOpt && exceptionPtr == nullptr) {
            mutex.unlock();
            processCallback();
        }
        mutex.unlock();
    }

    void processCallback() {
        if (threadPool == nullptr) {
            callback->call(*valueOpt);
        } else {
            threadPool->Exec([callback=callback, value=*valueOpt](){
                callback->call(value);
            });
        }
    }

private:
    ThreadPool* threadPool;
    std::mutex mutex;
    std::optional<T> valueOpt;
    std::exception_ptr exceptionPtr;
    std::shared_ptr<BaseCallback> callback = nullptr;
};
