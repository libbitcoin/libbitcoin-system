/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_PENDING_HPP
#define LIBBITCOIN_SYSTEM_PENDING_HPP

#include <atomic>
#include <cstddef>
#include <functional>
#include <memory>
#include <vector>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/utility/noncopyable.hpp>
#include <bitcoin/bitcoin/utility/thread.hpp>

namespace libbitcoin {

/// A managed collection of object pointers.
template <class Element>
class pending
  : noncopyable
{
public:
    typedef std::shared_ptr<Element> element_ptr;
    typedef std::vector<element_ptr> elements;
    using finder = std::function<bool(const element_ptr& element)>;

    pending(size_t initial_capacity);
    virtual ~pending();

    /// Safely copy the member collection.
    elements collection() const;

    /// The number of elements in the collection.
    size_t size() const;

    /// Determine if there exists an element that satisfies the finder.
    bool exists(finder match) const;

    /// Store a unique element in the collection (fails if stopped).
    code store(element_ptr element, finder match);

    /// Store an element in the collection (fails if stopped).
    code store(element_ptr element);

    /// Remove an element from the collection.
    void remove(element_ptr element);

    /// Stop all elements of the collection (idempotent).
    void stop(const code& ec);

    /// Close and erase all elements of the collection (blocking).
    void close();

private:

    // This is thread safe.
    std::atomic<bool> stopped_;

    // This is protected by mutex.
    elements elements_;
    mutable upgrade_mutex mutex_;
};

} // namespace libbitcoin

#include <bitcoin/bitcoin/impl/utility/pending.ipp>

#endif
