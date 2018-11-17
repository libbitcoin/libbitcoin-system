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

#ifndef LIBBITCOIN_SYSTEM_LOG_FILE_CHAR_TRAITS_HPP
#define LIBBITCOIN_SYSTEM_LOG_FILE_CHAR_TRAITS_HPP

#include <cctype>
#include <string>
#include <bitcoin/bitcoin/define.hpp>

namespace libbitcoin {
namespace log {

// modified from class extracted from boost/log/sinks/text_file_backend.*pp
struct BC_API file_char_traits
{
#ifdef _MSC_VER
    typedef wchar_t char_type;
#else
    typedef char char_type;
#endif

    static const char_type percent = '%';
    static const char_type number_placeholder = 'N';
    static const char_type day_placeholder = 'd';
    static const char_type month_placeholder = 'm';
    static const char_type year_placeholder = 'y';
    static const char_type full_year_placeholder = 'Y';
    static const char_type frac_sec_placeholder = 'f';
    static const char_type seconds_placeholder = 'S';
    static const char_type minutes_placeholder = 'M';
    static const char_type hours_placeholder = 'H';
    static const char_type space = ' ';
    static const char_type plus = '+';
    static const char_type minus = '-';
    static const char_type zero = '0';
    static const char_type dot = '.';
    static const char_type newline = '\n';

    static bool is_digit(char_type c)
    {
        return (std::isdigit(c) != 0);
    }

    static std::string default_file_name_pattern()
    {
        return "%5N.log";
    }
};

} // namespace log
} // namespace libbitcoin

#endif
