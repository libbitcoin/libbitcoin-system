#ifndef LIBBITCOIN_STORAGE_TYPES_H
#define LIBBITCOIN_STORAGE_TYPES_H

#include <memory>

namespace libbitcoin {

class storage;

using std::shared_ptr;

typedef shared_ptr<storage> storage_ptr;

} // libbitcoin

#endif

