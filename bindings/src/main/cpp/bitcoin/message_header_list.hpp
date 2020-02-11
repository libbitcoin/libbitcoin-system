#ifndef LIBBITCOIN__MESSAGE_HEADER_LIST_HPP
#define LIBBITCOIN__MESSAGE_HEADER_LIST_HPP

#include <bitcoin/bitcoin/message/header.hpp>

namespace libbitcoin {
namespace api {

class message_header_list {
public:
	message::header::list* getValue() {
        return value_;
    }

    void setValue(message::header::list* value) {
        value_ = value;
    }
private:
    message::header::list* value_;
};

} // namespace api
} // namespace libbitcoin
#endif
