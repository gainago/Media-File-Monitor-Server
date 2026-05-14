#ifndef GLOBAL_THREAD_PULL_H
#define GLOBAL_THREAD_PULL_H

#include <atomic>
#include <future>
#include <iostream>
#include <vector>

#include "thread_safe_queue.h"
#include "function_wrapper.h"

class thread_pool {
    // use shared_ptr because try pop makes copy
    thread_safe_queue<std::shared_ptr<function_wrapper>> pool_work_queue;
    std::atomic<bool> done{false};
    std::vector<std::thread> threads;

    std::mutex queue_mutex;
    std::condition_variable condition;


    void worker_thread() {
        while( !done ) {
            std::shared_ptr<function_wrapper> task;
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                condition.wait(lock, [this]()->bool {
                    return done == true || !pool_work_queue.empty();
                });
                if (done.load()) {
                    return;
                }
                if (!pool_work_queue.try_pop(task)) {
                    continue; // if suprious wake up
                }
            }
            //std::cout << "got task and start evaluate" << std::endl;
            (*task)();
        }
    }

public:

    thread_pool(size_t count_of_threads = std::thread::hardware_concurrency()) {
        for(size_t i = 0; i < count_of_threads; ++i) {
            threads.push_back(std::thread(&thread_pool::worker_thread, this));
        }
    }

    template<typename FunctionType>
    std::future<typename std::invoke_result<FunctionType()>::type>
    submit( FunctionType f ) {
        typedef typename std::invoke_result<FunctionType>::type result_type;

        std::packaged_task<result_type()> task(f);
        std::future<result_type> res( task.get_future() );
        pool_work_queue.push( std::make_shared<function_wrapper>(std::move( task)) );
        return res;
    }

    void push_task(std::shared_ptr<function_wrapper> task) {
        pool_work_queue.push(task);
        condition.notify_one();
    }
    void push_task(function_wrapper task) {
        pool_work_queue.push(std::make_shared<function_wrapper>(std::move(task)));
        condition.notify_one();
        std::cout << "pushed task" << std::endl;
    }

    void run_pending_task() { //this method aimed to call from other thrdeam, not into thread
        std::shared_ptr<function_wrapper> ptr_to_task;
        if(pool_work_queue.try_pop(ptr_to_task)) {    
            (*ptr_to_task)();
        }
        else{
            std::this_thread::yield();
        }
    }

    ~thread_pool() {
        done = true;
        condition.notify_all();
        for(size_t i = 0; i < threads.size(); ++i) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
    }
};

#endif // GLOBAL_THREAD_PULL_H