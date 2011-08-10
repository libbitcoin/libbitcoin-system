#include <bitcoin/verify.hpp>

#include <bitcoin/block.hpp>
#include <bitcoin/storage/storage.hpp>
#include <bitcoin/storage/errors.hpp>
#include <bitcoin/util/logger.hpp>

namespace libbitcoin {

using std::placeholders::_1;
using std::placeholders::_2;

verify_block::verify_block(storage_ptr storage)
  : storage_(storage)
{
}

void verify_block::start(message::block current_block, 
        status_handler handle_status)
{
    handle_status_ = handle_status;
    current_block_ = current_block;
    // Check for duplicate
    hash_digest current_hash = hash_block_header(current_block);
    storage_->fetch_block_by_hash(current_hash, 
            std::bind(&verify_block::find_duplicate, 
                shared_from_this(), _1, _2));
}

void verify_block::find_duplicate(std::error_code ec, message::block)
{
    if (ec != storage_error::block_doesnt_exist)
    {
        // These are error cases
        if (!ec)
            //handle_status_(block_already_exists, false);
            handle_status_(std::error_code(), false);
        else if (ec)
            handle_status_(ec, false);
        return;
    }

    handle_status_(std::error_code(), true);

    // CheckBlock()
    // These are checks that are independent of context
    // that can be verified before saving an orphan block
    // ...
}

} // libbitcoin

