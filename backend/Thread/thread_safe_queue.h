//thread_safe_queue.h
#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H

#include <deque>
#include <mutex>
#include <condition_variable>
#include <memory>

template<typename T, class Container = std::deque<T> >
class thread_safe_queue {
private:
    mutable std::mutex mut;
    Container data_container;
    std::condition_variable data_cond;

public:
    explicit thread_safe_queue( const Container& cont ) : data_container( cont ) {}
    explicit thread_safe_queue( Container&& cont = Container() ) : data_container( std::move( cont ) ) {}
    thread_safe_queue( thread_safe_queue&& other ) = delete;
    thread_safe_queue( thread_safe_queue const & other ) {
        std::lock_guard<std::mutex> lk( other.mut );
        data_container = other.data_container;
    }

    thread_safe_queue operator=( thread_safe_queue const & queue ) = delete;

    void push( T new_value ) {
        std::lock_guard<std::mutex> lk( mut );
        data_container.push_back(new_value);
        data_cond.notify_one();
    }

    bool try_pop( T& value ){
        std::lock_guard<std::mutex> lk( mut );
        if( data_container.empty() ) {
            return false;
        }
        value = std::move( data_container.front() );
        data_container.pop_front();
        return true;
    }

    std::shared_ptr<T> try_pop() {
        std::lock_guard<std::mutex> lk( mut );
        if( empty() ) {
            return nullptr;
        }
        std::shared_ptr<T> res = std::make_shared<T>( data_container.front() );
        data_container.pop_front();
        return res;
    }

    bool empty() const noexcept {
        std::lock_guard<std::mutex> lk( mut );
        return data_container.empty();
    }

};

#endif //THREADSAFE_QUEUE
