#ifndef LIBBITCOIN_VERIFY_H
#define LIBBITCOIN_VERIFY_H

#include <thread>
#include <memory>

#include <bitcoin/messages.hpp>
#include <bitcoin/types.hpp>
#include <bitcoin/util/threaded_service.hpp>

namespace libbitcoin {

class verify_block
  : public threaded_service,
    public std::enable_shared_from_this<verify_block>
{
public:
    typedef std::function<void (std::error_code, bool)> status_handler;

    verify_block(storage_ptr storage);
    void start(message::block current_block, status_handler handle_status);

private:
    void find_duplicate(std::error_code ec, message::block);

    storage_ptr storage_;
    message::block current_block_;
    status_handler handle_status_;
};

} // libbitcoin

#endif

