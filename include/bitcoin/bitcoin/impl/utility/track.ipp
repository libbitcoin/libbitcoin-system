/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_TRACK_IPP
#define LIBBITCOIN_TRACK_IPP

#include <atomic>
#include <cstddef>
#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/log/source.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>

// libbitcoin defines the log and tracking but does not use them.
// These are defined in bc so that they can be used in network and blockchain.

// Log name.
#define LOG_SYSTEM "system"

template <class Shared>
std::atomic<size_t> track<Shared>::instances(0);

template <class Shared>
track<Shared>::track(const std::string& DEBUG_ONLY(class_name))
#ifndef NDEBUG
  : class_(class_name)
#endif
{
#ifndef NDEBUG
    LOG_DEBUG(LOG_SYSTEM) << class_ << "(" << ++instances << ")";
#endif
}

template <class Shared>
track<Shared>::~track()
{
#ifndef NDEBUG
    LOG_DEBUG(LOG_SYSTEM) << "~" << class_ << "(" << --instances << ")";
#endif
}

#endif
