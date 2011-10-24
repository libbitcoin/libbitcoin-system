#ifndef LIBBITCOIN_STORAGE_POSTGRESQL_BLOCKCHAIN_H
#define LIBBITCOIN_STORAGE_POSTGRESQL_BLOCKCHAIN_H

#include <tuple>
#include <cppdb/frontend.h>

#include <bitcoin/messages.hpp>
#include <bitcoin/types.hpp>
#include <bitcoin/validate.hpp>

namespace libbitcoin {

using boost::posix_time::milliseconds;
using boost::posix_time::seconds;
using boost::posix_time::time_duration;
using std::placeholders::_1;

class pq_organizer
{
protected:
    pq_organizer(cppdb::session sql);
    void delete_branch(size_t space, size_t depth, 
        size_t span_left, size_t span_right);
    void organize();

private:
    struct span
    {
        size_t left, right;
    };

    bool load_span(size_t block_id, span& spn);
    bool load_position_info(size_t block_id, 
        size_t& space, size_t& depth, span& spn);
    size_t get_block_width(size_t space, size_t depth, span block_span);
    void reserve_branch_area(size_t parent_space, size_t parent_width,
        const span& parent_span, size_t new_child_depth, size_t child_width);
    void position_child_branch(size_t old_space, size_t new_space,
        size_t new_depth, size_t new_span_left);

    void delete_chains(size_t left, size_t right);
    void unwind_chain(size_t depth, size_t chain_id);

    cppdb::session sql_;
};

struct pq_transaction_info
{
    size_t transaction_id;
    std::vector<size_t> input_ids, output_ids;
};

typedef std::vector<pq_transaction_info> pq_transaction_info_list;

struct pq_block_info
{
    size_t block_id, depth, span_left, span_right;
    pq_transaction_info_list transactions;
};

typedef std::tuple<pq_block_info, message::block> pq_block;

class pq_reader
{
public:
    pq_reader(cppdb::session sql);

    pq_block read_block(cppdb::result block_result);

private:
    message::transaction_input_list select_inputs(
        size_t transaction_id, std::vector<size_t>& input_ids);
    message::transaction_output_list select_outputs(
        size_t transaction_id, std::vector<size_t>& output_ids);

    message::transaction_list read_transactions(
        cppdb::result result, pq_transaction_info_list& tx_infos);

    cppdb::session sql_;
};

class pq_validate_block
  : public validate_block,
    public pq_reader
{
public:
    pq_validate_block(
        cppdb::session sql, dialect_ptr,
        const pq_block_info& block_info,
        const message::block& current_block);
protected:
    uint32_t previous_block_bits();
    uint64_t actual_timespan(const uint64_t interval);
    uint64_t median_time_past();
    bool validate_transaction(const message::transaction& tx, 
        size_t index_in_block, uint64_t& value_in);
private:
    bool connect_input(
        size_t input_id, const message::transaction& current_tx, 
        size_t input_index, uint64_t& value_in);
    size_t previous_block_depth(size_t previous_tx_id);
    bool search_double_spends(
        size_t input_id, const message::transaction_input& input);

    cppdb::session sql_;
    const pq_block_info& block_info_;
    const message::block& current_block_;
};

class pq_blockchain
  : public pq_organizer,
    public pq_reader,
    public std::enable_shared_from_this<pq_blockchain>
{
public:
    pq_blockchain(cppdb::session sql, service_ptr service);
    // Only called during init. Otherwise use raise_barrier()
    void start();

    void set_clearance(size_t clearance);
    void set_timeout(time_duration timeout);

    void buffer_block(const pq_block& buffer_block);
    void raise_barrier();
    
private: 
    void reset_state();
    void start_exec(const boost::system::error_code& ec);

    void validate();
    void finalize_status(
        const pq_block_info& block_info, 
        const message::block& current_block);

    pq_block fetch_or_read_block(cppdb::result result);

    size_t barrier_clearance_level_;
    time_duration barrier_timeout_;

    deadline_timer_ptr timeout_;
    bool timer_started_;
    size_t barrier_level_;

    std::vector<pq_block> blocks_buffer_;

    dialect_ptr dialect_;
    cppdb::session sql_;
};

} // libbitcoin

#endif

