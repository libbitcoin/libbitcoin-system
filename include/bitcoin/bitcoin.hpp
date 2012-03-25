#ifndef LIBBITCOIN_BITCOIN_H
#define LIBBITCOIN_BITCOIN_H

/**
 * @mainpage libbitcoin API dox
 *
 * @section intro_sec Introduction
 *
 * This is the intro.
 *
 * @section install_sec Installation
 *
 * @subsection step1 Step 1: Opening the box
 *
 */

// Convenience header that includes everything
// Not to be used internally. For API users.
#include <bitcoin/error.hpp>
#include <bitcoin/address.hpp>
#include <bitcoin/network/channel.hpp>
#include <bitcoin/network/shared_const_buffer.hpp>
#include <bitcoin/network/network.hpp>
#include <bitcoin/network/hosts.hpp>
#include <bitcoin/network/handshake.hpp>
#include <bitcoin/block.hpp>
#include <bitcoin/session.hpp>
#include <bitcoin/poller.hpp>
#include <bitcoin/data_helpers.hpp>
#include <bitcoin/exporter.hpp>
#include <bitcoin/validate.hpp>
#include <bitcoin/constants.hpp>
#include <bitcoin/types.hpp>
#include <bitcoin/messages.hpp>
#include <bitcoin/blockchain/organizer.hpp>
#include <bitcoin/blockchain/blockchain.hpp>
#include <bitcoin/utility/threads.hpp>
#include <bitcoin/utility/elliptic_curve_key.hpp>
#include <bitcoin/utility/assert.hpp>
#include <bitcoin/utility/ripemd.hpp>
#include <bitcoin/utility/logger.hpp>
#include <bitcoin/utility/sha256.hpp>
#include <bitcoin/utility/base58.hpp>
#include <bitcoin/utility/big_number.hpp>
#include <bitcoin/utility/serializer.hpp>
#include <bitcoin/utility/subscriber.hpp>
#include <bitcoin/utility/clock.hpp>
#include <bitcoin/script.hpp>
#include <bitcoin/transaction.hpp>
#include <bitcoin/network/protocol.hpp>
#include <bitcoin/async_service.hpp>

namespace bc = libbitcoin;

#endif

