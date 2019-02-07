#ifndef LIBBITCOIN__MATH_SECRET_LIST_HPP
#define LIBBITCOIN__MATH_SECRET_LIST_HPP

#include <bitcoin/bitcoin/math/elliptic_curve.hpp>

namespace libbitcoin {
namespace api {

class math_secret_list : public std::vector<ec_secret> {
};

} // namespace api
} // namespace libbitcoin
#endif
