#ifndef LIBBITCOIN__MESSAGE_INVENTORY_VECTOR_LIST_HPP
#define LIBBITCOIN__MESSAGE_INVENTORY_VECTOR_LIST_HPP

#include <bitcoin/bitcoin/message/inventory_vector.hpp>

namespace libbitcoin {
namespace api {

class message_inventory_vector_list {
public:
	message::inventory_vector::list getValue() {
        return value;
    }

    void setValue(message::inventory_vector::list value) {
        this->value = value;
    }
private:
    message::inventory_vector::list value;
};

} // namespace api
} // namespace libbitcoin
#endif
