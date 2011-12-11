#ifndef LIBBITCOIN_BITCOIN_H
#define LIBBITCOIN_BITCOIN_H

// Convenience header that includes everything
// Not to be used internally. For API users.
#include <bitcoin/error.hpp>
#include <bitcoin/validate.hpp>
#include <bitcoin/messages.hpp>
#include <bitcoin/util/elliptic_curve_key.hpp>
#include <bitcoin/util/sha256.hpp>
#include <bitcoin/util/postbind.hpp>
#include <bitcoin/util/threads.hpp>
#include <bitcoin/util/serializer.hpp>
#include <bitcoin/util/assert.hpp>
#include <bitcoin/util/big_number.hpp>
#include <bitcoin/util/clock.hpp>
#include <bitcoin/util/base58.hpp>
#include <bitcoin/util/ripemd.hpp>
#include <bitcoin/util/logger.hpp>
#include <bitcoin/kernel.hpp>
#include <bitcoin/script.hpp>
#include <bitcoin/dialect.hpp>
#include <bitcoin/address.hpp>
#include <bitcoin/transaction.hpp>
#include <bitcoin/storage/postgresql_storage.hpp>
#include <bitcoin/storage/storage.hpp>
#include <bitcoin/storage/bdb_storage.hpp>
#include <bitcoin/constants.hpp>
#include <bitcoin/block.hpp>
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/types.hpp>
#include <bitcoin/network/network.hpp>
#include <bitcoin/network/types.hpp>

namespace bc = libbitcoin;

#endif

