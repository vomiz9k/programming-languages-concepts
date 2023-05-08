#pragma once

#include <shared_state.h>

template<typename T>
class Promise {
public:
    Promise(std::shared_ptr<SharedState<T>> sharedState) : sharedState(sharedState) {

    }

    void setValue(const T& value) {
        sharedState->setValue(value);
    }

    void setValue(T&& value) {
        sharedState->setValue(std::forward<T>(value));
    }

    void setException(std::exception_ptr exc) {
        sharedState->setException(exc);
    }

private:
    std::shared_ptr<SharedState<T>> sharedState;
};