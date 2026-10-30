/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#ifndef LIBBITCOIN_SYSTEM_EXCEPTIONS_HPP
#define LIBBITCOIN_SYSTEM_EXCEPTIONS_HPP

#include <exception>
#include <iostream>
#include <new>
#include <stdexcept>
#include <string>
#include <bitcoin/system/boost.hpp>

namespace libbitcoin {

/// We do not use exceptions for general purpose error handling.

/// Dependencies, including boost, may throw.
/// We try to avoid the use of external API calls that throw.
/// If an external API is documented to throw for non-exceptional results, we
/// catch and squash. If an external API throws unepxectedly, our pervasize use
/// of NOEXCEPT will cause a call to std::terminate.

/// We throw for istream >> read failures, given the nature of streams.

/// We throw for division by zero and other basic math parameterization errors
/// because the functions are public and are otherwise unguarded. When calling
/// them internally we first verify parameters to prevent hitting the exception.
/// This behavior is no different than when using C++ math operators directly.

/// We do not generally catch or throw for out of memory conditions as they are
/// pervasive and it would be impractical to attempt to inject an allocator for
/// all possible external API memory allocations. Long term objective: inject
/// all allocation.

/// UTF8 Everywhere initialization failure.
using runtime_exception = std::runtime_error;

/// Maths.
using overflow_exception = std::overflow_error;

/// Streams.
using ostream_exception = std::iostream::failure;

/// Options.
class option_exception
  : public std::logic_error
{
public:
    explicit option_exception(const std::string& message)
      : std::logic_error(message)
    {
    }
};

/// The token is not a valid value for its option.
class istream_exception
  : public option_exception
{
public:
    explicit istream_exception(const std::string& token,
        const std::string& option={}, const std::string& detail={})
      : option_exception("the argument ('" + token + "') for option " +
            (option.empty() ? std::string{} : "'" + option + "' ") +
            "is invalid" + detail),
        token_(token)
    {
    }

    const std::string& token() const NOEXCEPT
    {
        return token_;
    }

private:
    std::string token_;
};

/// The configuration file cannot be read.
class ifstream_exception
  : public option_exception
{
public:
    explicit ifstream_exception(const std::string& path)
      : option_exception("can not read options configuration file '" +
            path + "'")
    {
    }
};

/// The options are inconsistent with their declarations.
class validation_exception
  : public option_exception
{
public:
    explicit validation_exception(const std::string& message)
      : option_exception(message)
    {
    }
};

/// Allocation.
using allocation_exception = std::bad_alloc;
using bad_array_new_length = std::bad_array_new_length;


} // namespace libbitcoin

#endif
