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
public:
    bool check();

protected:
    verify_block(dialect_ptr dialect, 
        size_t depth, const message::block& current_block);

    virtual uint32_t previous_block_bits() = 0;
    virtual uint64_t actual_timespan(const uint64_t interval) = 0;
    virtual uint64_t median_time_past() = 0;

private:
    bool check_block();
    bool check_proof_of_work(hash_digest hash, uint32_t bits);
    bool check_transaction(const message::transaction& tx);
    size_t number_script_operations();

    bool accept_block();
    uint32_t work_required();
    bool passes_checkpoints();

    dialect_ptr dialect_;
    clock_ptr clock_;

    const size_t depth_;
    const message::block& current_block_;
};

} // libbitcoin

#endif

