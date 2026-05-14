#ifndef FUNCTION_WRAPPER_H
#define FUNCTION_WRAPPER_H

#include <memory>
#include <utility>

class function_wrapper {
    struct impl_base {
        virtual void call() = 0;
        virtual ~impl_base() = default;
    };

    template<typename F>
    struct impl_type : impl_base {
        F f;
        impl_type(F&& f_) : f(std::forward<F>(f_)) {}
        void call() override { f(); }
    };

    std::unique_ptr<impl_base> impl;

public:
    template<typename F>
    function_wrapper(F&& f) : impl(new impl_type<F>(std::forward<F>(f))) {}
    function_wrapper() = default;
    function_wrapper(function_wrapper&&) = default;
    function_wrapper& operator=(function_wrapper&&) = default;

    void operator()() { impl->call();}

};

#endif // FUNCTION_WRAPPER_H