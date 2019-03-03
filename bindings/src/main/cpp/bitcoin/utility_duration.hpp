#ifndef LIBBITCOIN__UTILITY_DURATION_HPP
#define LIBBITCOIN__UTILITY_DURATION_HPP

#include <bitcoin/bitcoin/utility/asio.hpp>

namespace libbitcoin {
namespace api {

class utility_duration {
public:
	libbitcoin::asio::duration getValue() {
        return value;
    }

    void setValue(libbitcoin::asio::duration value) {
        this->value = value;
    }
private:
    libbitcoin::asio::duration value;
};

} // namespace api
} // namespace libbitcoin
#endif
