#ifndef LIBBITCOIN__UTILITY_DURATION_HPP
#define LIBBITCOIN__UTILITY_DURATION_HPP

#include <bitcoin/bitcoin/utility/asio.hpp>

namespace libbitcoin {
namespace api {

class utility_duration {
public:
	libbitcoin::asio::duration* getValue() {
        return value_;
    }

    void setValue(libbitcoin::asio::duration* value) {
        value_ = value;
    }
private:
    libbitcoin::asio::duration* value_;
};

} // namespace api
} // namespace libbitcoin
#endif
