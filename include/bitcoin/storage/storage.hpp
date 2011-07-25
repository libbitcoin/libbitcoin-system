#ifndef LIBBITCOIN_STORAGE_STORAGE_H
#define LIBBITCOIN_STORAGE_STORAGE_H

#include <boost/utility.hpp>
#include <functional>

#include <bitcoin/messages.hpp>

namespace libbitcoin {
namespace storage {

class storage : boost::noncopyable
{
public:
    typedef std::function<void (bool)> operation_handler;

    // Not supported in g++ yet
    //template <typename Message>
    //using fetch_handler = std::function<void (Message)>;
    typedef std::function<void (message::inv_list, bool)>
            fetch_handler_inventories;
    typedef std::function<void (message::block, bool)>
            fetch_handler_block;

    virtual void store(message::inv inv, 
            operation_handler handle_store) = 0;
    virtual void store(message::transaction transaction,
            operation_handler handle_store) = 0;
    virtual void store(message::block block, 
            operation_handler handle_store) = 0;

    virtual void fetch_inventories(fetch_handler_inventories handle_fetch) = 0;
    virtual void fetch_block_number(size_t block_number,
            fetch_handler_block handle_fetch) = 0;
};

} // storage
} // libbitcoin

#endif

