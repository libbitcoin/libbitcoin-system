#ifndef LIBBITCOIN_STORAGE_STORAGE_H
#define LIBBITCOIN_STORAGE_STORAGE_H

#include <boost/utility.hpp>
#include <functional>

#include <bitcoin/net/messages.hpp>

namespace libbitcoin {
namespace storage {

class storage : boost::noncopyable
{
public:
    typedef std::function<void (net::message::inv_list)>
            accept_inventories_handler;
    virtual void push(net::message::inv inv) = 0;
    virtual void push(net::message::transaction transaction) = 0;
    virtual void push(net::message::block block) = 0;
    virtual void request_inventories(accept_inventories_handler handler) = 0;
};

} // storage
} // libbitcoin

#endif

