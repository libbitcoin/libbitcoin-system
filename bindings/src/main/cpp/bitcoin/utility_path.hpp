#ifndef LIBBITCOIN__UTILITY_PATH_HPP
#define LIBBITCOIN__UTILITY_PATH_HPP

#include "boost/filesystem/path.hpp"

namespace libbitcoin {
namespace api {

class utility_path : public boost::filesystem::path {
};

} // api
} // libbitcoin

#endif
