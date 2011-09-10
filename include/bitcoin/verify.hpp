#ifndef LIBBITCOIN_VERIFY_H
#define LIBBITCOIN_VERIFY_H

#include <thread>
#include <memory>

#include <bitcoin/messages.hpp>
#include <bitcoin/types.hpp>
#include <bitcoin/util/threaded_service.hpp>

namespace libbitcoin {

class verify_block
{
protected:
    verify_block(dialect_ptr dialect, const message::block& current_block);
    bool check_block();

private:
    bool check_proof_of_work(hash_digest hash, uint32_t bits);
    bool check_transaction(const message::transaction& tx);
    size_t number_script_operations();

    dialect_ptr dialect_;
    clock_ptr clock_;

    const message::block& current_block_;
};

} // libbitcoin

#endif

