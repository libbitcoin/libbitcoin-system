#ifndef LIBBITCOIN_STORAGE_POSTGRESQL_BLOCKCHAIN_H
#define LIBBITCOIN_STORAGE_POSTGRESQL_BLOCKCHAIN_H

#include <set>
#include <cppdb/frontend.h>

#include <bitcoin/messages.hpp>
#include <bitcoin/types.hpp>
#include <bitcoin/validate.hpp>

namespace libbitcoin {

using boost::posix_time::milliseconds;
using boost::posix_time::seconds;
using boost::posix_time::time_duration;
using std::placeholders::_1;

class pq_organizer;
class pq_reader;
typedef std::shared_ptr<pq_organizer> pq_organizer_ptr;
typedef std::shared_ptr<pq_reader> pq_reader_ptr;

class pq_organizer
{
public:
    pq_organizer(cppdb::session sql, kernel_ptr kernel);
    void delete_branch(size_t space, size_t depth, 
        size_t span_left, size_t span_right);
    void organize();
    void refresh_block(size_t block_id);

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

    void get_inbetweens(std::set<size_t> block_ids);
    std::set<size_t> recent_blocks_;

    cppdb::session sql_;
    kernel_ptr kernel_;
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

typedef std::pair<pq_block_info, message::block> pq_block;

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
  : public validate_block
{
public:
    pq_validate_block(
        cppdb::session sql, exporter_ptr saver, pq_reader_ptr reader,
        const pq_block_info& block_info,
        const message::block& current_block);
protected:
    uint32_t previous_block_bits();
    uint64_t actual_timespan(const uint64_t interval);
    uint64_t median_time_past();
    bool validate_inputs(const message::transaction& tx, 
        size_t index_in_block, uint64_t& value_in);
private:
    bool connect_input(
        size_t input_id, const message::transaction& current_tx, 
        size_t input_index, uint64_t& value_in);
    size_t previous_block_depth(size_t previous_tx_id);
    bool search_double_spends(
        size_t input_id, const message::transaction_input& input);

    cppdb::session sql_;
    pq_reader_ptr reader_;
    const pq_block_info& block_info_;
    const message::block& current_block_;
};

class pq_blockchain
  : public std::enable_shared_from_this<pq_blockchain>
{
public:
    pq_blockchain(cppdb::session sql, service_ptr service, kernel_ptr kernel);
    void start();

    pq_organizer_ptr organizer();
    pq_reader_ptr reader();

    void validate();
    
private: 
    void finalize_status(
        const pq_block_info& block_info, 
        const message::block& current_block);

    pq_block fetch_or_read_block(cppdb::result result);

    std::vector<pq_block> blocks_buffer_;

    pq_organizer_ptr organizer_;
    pq_reader_ptr reader_;
    exporter_ptr export_;
    cppdb::session sql_;
};

} // namespace libbitcoin

#endif

