#ifndef  LIBBITCOIN_UTILITY_SUBSCRIBER_HPP
#define  LIBBITCOIN_UTILITY_SUBSCRIBER_HPP

#include <stack>

#include <bitcoin/types.hpp>
#include <bitcoin/async_service.hpp>
#include <bitcoin/utility/assert.hpp>

namespace libbitcoin {

template <typename... Args>
class subscriber
  : public std::enable_shared_from_this<subscriber<Args...>>
{
public:
    typedef std::function<void (Args...)> handler_type;
    typedef std::shared_ptr<subscriber<Args...>> ptr;

    subscriber(async_service& service)
      : strand_(service.get_service())
    {
    }

    void subscribe(handler_type handle)
    {
        strand_.dispatch(
            std::bind(&subscriber<Args...>::do_subscribe,
                this->shared_from_this(), handle));
    }

    void relay(Args... params)
    {
        strand_.dispatch(
            std::bind(&subscriber<Args...>::do_relay,
                this->shared_from_this(), std::forward<Args>(params)...));
    }

private:
    typedef std::stack<handler_type> registry_stack;

    void do_subscribe(handler_type handle)
    {
        registry_.push(handle);
    }

    void do_relay(Args... params)
    {
        registry_stack notify_copy = registry_;
        registry_ = registry_stack();
        while (!notify_copy.empty())
        {
            notify_copy.top()(params...);
            notify_copy.pop();
        }
        BITCOIN_ASSERT(notify_copy.empty());
    }

    io_service::strand strand_;
    registry_stack registry_;
};

// C++11 - not in g++ yet!
//template <typename Handler>
//using subscriber_ptr = std::shared_ptr<subscribe<Handler>>;

} // namespace libbitcoin

#endif

