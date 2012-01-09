#ifndef  LIBBITCOIN_UTILITY_SUBSCRIBER_H
#define  LIBBITCOIN_UTILITY_SUBSCRIBER_H

#include <bitcoin/types.hpp>

namespace libbitcoin {

template <typename... Args>
class subscriber
  : public std::enable_shared_from_this<subscriber<Args...>>
{
public:
    typedef std::function<void (Args...)> handler_type;

    subscriber(strand_ptr async_strand)
      : strand_(async_strand)
    {
    }

    void subscribe(handler_type handle)
    {
        strand_->dispatch(
            std::bind(&subscriber<Args...>::do_subscribe,
                this->shared_from_this(), handle));
    }

    void relay(Args... params)
    {
        strand_->dispatch(
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
            notify_copy.top()(std::forward<Args>(params)...);
            notify_copy.pop();
        }
        BITCOIN_ASSERT(notify_copy.empty());
    }

    strand_ptr strand_;
    registry_stack registry_;
};

// C++11 - not in g++ yet!
//template <typename Handler>
//using subscriber_ptr = std::shared_ptr<subscribe<Handler>>;

} // libbitcoin

#endif

