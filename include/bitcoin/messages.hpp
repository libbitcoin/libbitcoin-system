#ifndef LIBBITCOIN_MESSAGES_HPP
#define LIBBITCOIN_MESSAGES_HPP

#include <bitcoin/primitives.hpp>

namespace libbitcoin {
namespace message {

typedef block_locator_type block_locator;
typedef ip_address_type ip_address;
typedef network_address_type network_address;
typedef inventory_type_id inventory_type_id;
typedef inventory_vector_type inventory_vector;
typedef inventory_list inventory_list;
typedef header_type header;
typedef version_type version;
typedef verack_type verack;
typedef get_address_type get_address;
typedef get_blocks_type get_blocks;
typedef output_point output_point;
typedef input_point input_point;
typedef input_point_list input_point_list;
typedef output_point_list output_point_list;
typedef transaction_input_type transaction_input;
typedef transaction_output_type transaction_output;
typedef transaction_input_list transaction_input_list;
typedef transaction_output_list transaction_output_list;
typedef transaction_type transaction;
typedef transaction_list transaction_list;
typedef block_type block;
typedef network_address_list network_address_list;
typedef address_type address;
typedef get_data_type get_data;
typedef inventory_type inventory;
typedef ping_type ping;
typedef pong_type pong;

} // message
} // namespace libbitcoin

#endif

