#ifndef LIBBITCOIN_STORAGE_POSTGRESQL_BLOCKCHAIN_H
#define LIBBITCOIN_STORAGE_POSTGRESQL_BLOCKCHAIN_H

#include <tuple>
#include <cppdb/frontend.h>

#include <bitcoin/messages.hpp>
#include <bitcoin/types.hpp>
#include <bitcoin/verify.hpp>

namespace libbitcoin {

using boost::posix_time::milliseconds;
using boost::posix_time::seconds;
using boost::posix_time::time_duration;
using std::placeholders::_1;

data_chunk deserialize_bytes(std::string byte_stream);
hash_digest deserialize_hash(std::string byte_stream);

class postgresql_organizer
{
protected:
    postgresql_organizer(cppdb::session sql);
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
        size_t parent_span_right, size_t new_child_depth, size_t child_width);
    void position_child_branch(size_t old_space, size_t new_space,
        size_t new_depth, size_t new_span_left);

    cppdb::session sql_;
};

class postgresql_reader
{
public:
    postgresql_reader(cppdb::session sql);

    message::block read_block(cppdb::result block_result);
    script select_script(size_t script_id);

private:
    message::transaction_input_list select_inputs(size_t transaction_id);
    message::transaction_output_list select_outputs(size_t transaction_id);

    message::transaction_list read_transactions(cppdb::result result);

    cppdb::session sql_;
};

class postgresql_verifier
  : public postgresql_reader
{
protected:
    postgresql_verifier(cppdb::session sql);
    void verify();
private:
    cppdb::session sql_;
};

class postgresql_blockchain
  : public postgresql_organizer,
    public postgresql_verifier,
    public std::enable_shared_from_this<postgresql_blockchain>
{
public:
    postgresql_blockchain(cppdb::session sql, service_ptr service);

    void set_clearance(size_t clearance);
    void set_timeout(time_duration timeout);

    void raise_barrier();
    
private: 
    void reset_state();
    void start_exec(const boost::system::error_code& ec);
    void start();

    size_t barrier_clearance_level_;
    time_duration barrier_timeout_;

    deadline_timer_ptr timeout_;
    bool timer_started_;
    size_t barrier_level_;
};

} // libbitcoin

#endif

