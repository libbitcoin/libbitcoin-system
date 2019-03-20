#ifndef LIBBITCOIN__MESSAGE_HEADER_LIST_HPP
#define LIBBITCOIN__MESSAGE_HEADER_LIST_HPP

#include <bitcoin/bitcoin/message/header.hpp>

namespace libbitcoin {
namespace api {

class message_header_list {
public:
	message::header::list getValue() {
        return value;
    }

    void setValue(message::header::list value) {
        this->value = value;
    }
private:
    message::header::list value;
};

} // namespace api
} // namespace libbitcoin
#endif
