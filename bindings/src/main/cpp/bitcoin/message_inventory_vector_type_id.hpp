#ifndef LIBBITCOIN__MESSAGE_INVENTORY_VECTOR_TYPE_ID_HPP
#define LIBBITCOIN__MESSAGE_INVENTORY_VECTOR_TYPE_ID_HPP

//#include <bitcoin/bitcoin/message/inventory_vector.hpp>
#include <cstdint>

namespace libbitcoin {
namespace api {

//enum message_inventory_vector_type_id : message::inventory_vector::type_id {
//};

enum class message_inventory_vector_type_id : uint32_t
{
    error = 0,
    transaction = 1,
    block = 2,
    filtered_block = 3,
    compact_block = 4,
    witness = (1u << 30),
    witness_transaction = witness | transaction,
    witness_block = witness | block,
    reserved = witness | filtered_block
};


} // namespace api
} // namespace libbitcoin
#endif
