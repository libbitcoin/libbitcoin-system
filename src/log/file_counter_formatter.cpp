/*
 *          Copyright Andrey Semashev 2007 - 2014.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */
/*!
 * \file   text_file_backend.cpp
 * \author Andrey Semashev
 * \date   09.06.2009
 *
 * \brief  This header is the Boost.Log library implementation, see the library documentation
 *         at http://www.boost.org/doc/libs/release/libs/log/doc/html/index.html.
 */

// Modification of boost implementation to alter log file rotation naming.

#include <bitcoin/bitcoin/log/file_counter_formatter.hpp>
#include <bitcoin/bitcoin/log/file_char_traits.hpp>

namespace libbitcoin {
namespace log {

file_counter_formatter::file_counter_formatter(unsigned int width)
  : width_(width)
{
    stream_.fill(file_char_traits::zero);
}

file_counter_formatter::file_counter_formatter(
    file_counter_formatter const& that)
  : width_(that.width_)
{
    stream_.fill(that.stream_.fill());
}

file_counter_formatter::path_string_type file_counter_formatter::operator()(
    path_string_type const& stem, path_string_type const& extension,
    unsigned int counter) const
{
    stream_.str(path_string_type());
    stream_.width(width_);
    stream_ << counter;

    path_string_type result = stem + file_char_traits::minus + stream_.str()
        + extension;

    return result;

}

} // namespace log
} // namespace libbitcoin
