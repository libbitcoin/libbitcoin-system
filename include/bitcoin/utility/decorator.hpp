#ifndef LIBBITCOIN_UTILITY_DECORATOR_H
#define LIBBITCOIN_UTILITY_DECORATOR_H

/*
 * Defines a function decorator ala Python
 *
 *   void foo(int x, int y);
 *   function<void ()> wrapper(function<void (int)> f);
 *
 *   auto f = decorator(wrapper, bind(foo, 110, _1));
 *   f();
 */

#include <functional>

template <typename Wrapper, typename Functor>
struct decorator_dispatch
{
    Wrapper wrapper;
    Functor functor;

    template <typename... Args>
    auto operator()(Args&&... args)
        -> decltype(wrapper(functor)(std::forward<Args>(args)...))
    {
        return wrapper(functor)(std::forward<Args>(args)...);
    }
};

template <typename Wrapper, typename Functor>
decorator_dispatch<
    Wrapper,
    typename std::decay<Functor>::type
>
decorator(Wrapper&& wrapper, Functor&& functor)
{
    return {wrapper, functor};
}

#endif

