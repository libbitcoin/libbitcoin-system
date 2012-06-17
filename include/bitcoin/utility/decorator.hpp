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

template <typename Wrapper, typename Handler>
struct decorator_dispatch
{
    Wrapper wrapper;
    Handler handler;

    template <typename... Args>
    auto operator()(Args&&... args)
        -> decltype(wrapper(handler)(std::forward<Args>(args)...))
    {
        return wrapper(handler)(std::forward<Args>(args)...);
    }
};

template <typename Wrapper, typename Handler>
decorator_dispatch<
    Wrapper,
    typename std::decay<Handler>::type
>
decorator(Wrapper&& wrapper, Handler&& handler)
{
    return {wrapper, handler};
}

#endif

