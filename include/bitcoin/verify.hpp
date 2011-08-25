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
    typedef std::function<void (const std::error_code&, bool)> status_handler;

    verify_block(storage_ptr storage, dialect_ptr dialect);
    void start(const message::block& current_block, 
            status_handler handle_status);

private:
    void find_duplicate(std::error_code ec, bool block_exists);
    void find_previous(std::error_code ec, message::block);

    bool check_block();
    bool check_proof_of_work(hash_digest hash, uint32_t bits);
    bool check_transaction(const message::transaction& tx);
    size_t number_script_operations();

    storage_ptr storage_;
    dialect_ptr dialect_;
    clock_ptr clock_;

    message::block current_block_;
    hash_digest current_block_hash_;
    status_handler handle_status_;
};

} // libbitcoin

#endif

