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
#ifndef LIBBITCOIN__UTILITY_SYNCHRONIZER_HPP
#define LIBBITCOIN__UTILITY_SYNCHRONIZER_HPP

#include <cstddef>
//#include <memory>
//#include <stdexcept>
#include <string>
//#include <thread>
//#include <type_traits>
//#include <utility>
//#include <bitcoin/bitcoin/error.hpp>
//#include <bitcoin/bitcoin/utility/assert.hpp>
//#include <bitcoin/bitcoin/utility/thread.hpp>
#include <bitcoin/bitcoin/utility/synchronizer.hpp>

namespace libbitcoin {
namespace api {

enum class synchronizer_terminate
{
    /// Terminate on first error or count.
    /// Return code on first error, otherwise success.
    on_error,

    /// Terminate first success (priority) or count.
    /// Return success on first success, otherwise operation_failed.
    on_success,

    /// Terminate on count only.
    /// Return success once count is reached (always).
    on_count
};

template <typename Handler>
class utility_synchronizer
{
public:
	utility_synchronizer(Handler&& handler, size_t clearance_count,
        const std::string& name, synchronizer_terminate mode)
      : value(new synchronizer<Handler>(handler, clearance_count, name, mode))
    {
    }

    // Determine if the code is cause for termination.
    bool complete(const code& ec)
    {
        return value.complete(ec);
    }

    // Assuming we are terminating, generate the proper result code.
    code result(const code& ec)
    {
        return value.result(ec);
    }

    template <typename... Args>
    void operator()(const code& ec, Args&&... args)
    {
    	value(ec, args);
    }

	synchronizer<Handler> getValue() {
		return value;
	}

	void setValue(synchronizer<Handler> value) {
		this->value = value;
	}
private:
	synchronizer<Handler> value;
//    typedef typename std::decay<Handler>::type decay_handler;
//
//    decay_handler handler_;
//    const std::string name_;
//    const size_t clearance_count_;
//    const synchronizer_terminate terminate_;
//
//    // We use pointer to reference the same value/mutex across instance copies.
//    std::shared_ptr<size_t> counter_;
//    upgrade_mutex_ptr mutex_;
};

template <typename Handler>
utility_synchronizer<Handler> synchronize(Handler&& handler, size_t clearance_count,
    const std::string& name, synchronizer_terminate mode=
        synchronizer_terminate::on_error)
{
    return utility_synchronizer<Handler>(std::forward<Handler>(handler),
        clearance_count, name, mode);
}

} // namespace api
} // namespace libbitcoin

#endif
