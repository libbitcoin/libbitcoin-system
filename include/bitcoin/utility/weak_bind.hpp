#ifndef LIBBITCOIN_UTILITY_WEAK_BIND_HPP
#define LIBBITCOIN_UTILITY_WEAK_BIND_HPP

#include <functional>
#include <memory>

namespace libbitcoin {

template <typename WeakPtr, typename Handler>
struct weak_wrapper_impl
{
    WeakPtr weak_ref;
    Handler handler;

    template <typename... Args>
    void operator()(Args&&... args)
    {
        auto raii_lock = weak_ref.lock();
        if (raii_lock)
            handler(std::forward<Args>(args)...);
    }
};

template <typename WeakPtr, typename Handler>
weak_wrapper_impl<
    WeakPtr,
    typename std::decay<Handler>::type
>
weak_wrapper(WeakPtr weak_ref, Handler handler)
{
    return {weak_ref, handler};
}

template <typename MemberFunction, typename Class, typename... Args>
auto weak_bind(MemberFunction member_func,
    std::shared_ptr<Class> ref, Args&&... args)
    -> decltype(
        weak_wrapper(
            std::weak_ptr<Class>(ref),
            std::bind(member_func, ref.get(), std::forward<Args>(args)...)
        ))
{
    std::weak_ptr<Class> weak_ref = ref;
    auto handler =
        std::bind(member_func, ref.get(), std::forward<Args>(args)...);
    return weak_wrapper(weak_ref, handler);
}

} // libbitcoin

#endif

