#ifndef LIBBITCOIN__UTILITY_SHARED_THREAD_HPP
#define LIBBITCOIN__UTILITY_SHARED_THREAD_HPP

#include <bitcoin/bitcoin/utility/thread.hpp>

namespace libbitcoin {
namespace api {

class utility_shared_mutex : public libbitcoin::shared_mutex {
};

} // namespace api
} // namespace libbitcoin
#endif
