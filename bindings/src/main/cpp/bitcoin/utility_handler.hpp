#ifndef LIBBITCOIN__UTILITY_HANDLER_HPP
#define LIBBITCOIN__UTILITY_HANDLER_HPP

#include <functional>
//#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
//#include <bitcoin/bitcoin/utility/deadline.hpp>
//#include <boost/asio.hpp>
//#include <bitcoin/bitcoin/utility/asio.hpp>
//#include <bitcoin/bitcoin/utility/enable_shared_from_base.hpp>
//#include <bitcoin/bitcoin/utility/assert.hpp>
//#include <bitcoin/bitcoin/utility/noncopyable.hpp>
//#include <bitcoin/bitcoin/utility/thread.hpp>
//#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace api {

class utility_handler : public std::function<void(const libbitcoin::code&)>  {
};

} // namespace api
} // namespace libbitcoin
#endif
