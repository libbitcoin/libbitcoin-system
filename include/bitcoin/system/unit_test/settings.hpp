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
#ifndef LIBBITCOIN_SYSTEM_UNIT_TEST_SETTINGS_HPP
#define LIBBITCOIN_SYSTEM_UNIT_TEST_SETTINGS_HPP

#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace unit_test {

/// Log verbosity, from most to least verbose.
enum class log_level
{
    all,
    test_suite,
    message,
    warning,
    error,
    cpp_exception,
    system_error,
    fatal_error,
    nothing
};

/// Final report verbosity.
enum class report_level
{
    no,
    confirm,
    summary,
    detailed
};

/// Runner settings, populated from the command line.
struct settings
{
    /// Parse the command line, false with error text if invalid.
    bool parse(std::string& error, int argc,
        const char* const argv[]) NOEXCEPT;

    log_level log{ log_level::error };
    report_level report{ report_level::confirm };
    bool show_progress{};
    bool build_info{};
    bool list_content{};
    bool help{};
    size_t detect_memory_leaks{ one };
    std::vector<std::string> run_test{};

private:
    static bool parse_flag(bool& out, const std::string& text) NOEXCEPT;
    static bool parse_log(log_level& out, const std::string& text) NOEXCEPT;
    static bool parse_report(report_level& out,
        const std::string& text) NOEXCEPT;
    static bool parse_number(size_t& out, const std::string& text) NOEXCEPT;
    static void parse_filters(std::vector<std::string>& out,
        const std::string& text) NOEXCEPT;
};

} // namespace unit_test
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/unit_test/settings.ipp>

#endif
