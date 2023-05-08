#pragma once

#include <future.h>
#include <vector>
#include <thread>
#include <set>
#include <queue>
#include <mutex>
#include <memory>

class BaseTask {
public:
    virtual void call() = 0;
    virtual ~BaseTask() = default;
};

template<typename F>
class Task: public BaseTask {
public:
    Task(F&& func) : func(std::move(func)) {}

    virtual void call() override {
        func();
    }

    virtual ~Task() = default;

private:
    F func;
};

class TaskQueue {
public:
    template <typename F>
    void push(F&& task) {
        std::lock_guard guard(queueMutex);
        queue.push(std::make_shared<Task<F>>(std::move(task)));
    }

    std::shared_ptr<BaseTask> get() {
        std::lock_guard guard(queueMutex);
        if (!queue.empty()) {
            auto result = queue.back();
            queue.pop();
            return result;
        }
        return nullptr;
    }

    bool empty() {
        return queue.empty();
    }

private:
    std::mutex queueMutex;
    std::queue<std::shared_ptr<BaseTask>> queue;
};

class ThreadPool {
public:
    ThreadPool(int n) :  threads(std::vector<std::thread>(n)), freeThreadsCnt(n) {
        for (int i = 0; i < n; ++i) {
            threads[i] = std::thread([this](){
                while (alive) {
                    while (queue.empty() && alive) {
                        std::this_thread::yield();
                    }
                    std::shared_ptr<BaseTask> taskPtr = nullptr;
                    {
                        std::lock_guard guard(queueMutex);
                        if (!queue.empty()) {
                            queue.get()->call();
                        }
                    }
                    if (taskPtr != nullptr) {
                        taskPtr->call();
                    }
                }
            });
        }
    }

    template<typename F, typename... Args>
    void Exec(F&& func, Args&&... args) {
        if (freeThreadsCnt == 0) {
            func(std::forward(args)...);
        } else {
            queue.push([func = std::move(func), tup=std::make_tuple(std::move(args)...)](){
                std::apply([func = std::move(func)](auto const&... args) mutable {
                    return func(args...);
                }, tup);
            });
        }
    }

    ~ThreadPool() {
        alive.store(false);
        for (int i = 0; i < threads.size(); ++i) {
            threads[i].join();
        }
    }
private:
    std::atomic<bool> alive = true;
    std::vector<std::thread> threads;
    std::atomic<int> freeThreadsCnt;
    TaskQueue queue;
    std::mutex queueMutex;
};