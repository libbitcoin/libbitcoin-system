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

#ifndef LIBBITCOIN_SYSTEM_LOG_FILE_COUNTER_FORMATTER_HPP
#define LIBBITCOIN_SYSTEM_LOG_FILE_COUNTER_FORMATTER_HPP

#include <sstream>
#include <boost/filesystem/path.hpp>

#include <bitcoin/bitcoin/define.hpp>

namespace libbitcoin {
namespace log {

// modified from class extracted from boost/log/sinks/text_file_backend.*pp
//! The functor formats the file counter into the file name
class BC_API file_counter_formatter
{
public:
    typedef boost::filesystem::path::string_type path_string_type;

public:
    //! Initializing constructor
    file_counter_formatter(unsigned int width);

    //! Copy constructor
    file_counter_formatter(file_counter_formatter const& that);

    //! The function formats the file counter into the file name
    path_string_type operator()(path_string_type const& stem,
        path_string_type const& extension, unsigned int counter) const;

    bool scan_seperator(path_string_type::const_iterator& it,
        path_string_type::const_iterator end);

    BOOST_DELETED_FUNCTION(file_counter_formatter& operator= (file_counter_formatter const&))

private:
    //! File counter width
    std::streamsize width_;
    //! The file counter formatting stream
    mutable std::basic_ostringstream<path_string_type::value_type> stream_;
};

} // namespace log
} // namespace libbitcoin

#endif
