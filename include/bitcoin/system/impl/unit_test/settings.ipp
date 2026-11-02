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
#ifndef LIBBITCOIN_SYSTEM_UNIT_TEST_SETTINGS_IPP
#define LIBBITCOIN_SYSTEM_UNIT_TEST_SETTINGS_IPP

#include <charconv>

namespace libbitcoin {
namespace system {
namespace unit_test {

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

inline bool settings::parse_flag(bool& out, const std::string& text) NOEXCEPT
{
    if (text.empty() || text == "yes" || text == "y" || text == "true" ||
        text == "on" || text == "1")
    {
        out = true;
        return true;
    }

    if (text == "no" || text == "n" || text == "false" || text == "off" ||
        text == "0")
    {
        out = false;
        return true;
    }

    return false;
}

inline bool settings::parse_log(log_level& out,
    const std::string& text) NOEXCEPT
{
    static const std::vector<std::pair<std::string, log_level>> levels
    {
        { "all", log_level::all },
        { "success", log_level::all },
        { "test_suite", log_level::test_suite },
        { "message", log_level::message },
        { "warning", log_level::warning },
        { "error", log_level::error },
        { "cpp_exception", log_level::cpp_exception },
        { "system_error", log_level::system_error },
        { "fatal_error", log_level::fatal_error },
        { "nothing", log_level::nothing }
    };

    for (const auto& level: levels)
    {
        if (level.first == text)
        {
            out = level.second;
            return true;
        }
    }

    return false;
}

inline bool settings::parse_report(report_level& out,
    const std::string& text) NOEXCEPT
{
    static const std::vector<std::pair<std::string, report_level>> levels
    {
        { "no", report_level::no },
        { "confirm", report_level::confirm },
        { "short", report_level::summary },
        { "detailed", report_level::detailed }
    };

    for (const auto& level: levels)
    {
        if (level.first == text)
        {
            out = level.second;
            return true;
        }
    }

    return false;
}

inline bool settings::parse_number(size_t& out,
    const std::string& text) NOEXCEPT
{
    const auto end = std::next(text.data(), text.size());
    const auto result = std::from_chars(text.data(), end, out);
    return result.ec == std::errc{} && result.ptr == end;
}

// Test unit filters are separated by colons.
inline void settings::parse_filters(std::vector<std::string>& out,
    const std::string& text) NOEXCEPT
{
    size_t start{};
    while (start <= text.size())
    {
        const auto colon = text.find(':', start);
        const auto stop = (colon == std::string::npos) ? text.size() : colon;
        if (stop > start)
            out.push_back(text.substr(start, stop - start));

        start = add1(stop);
    }
}

// Options are --name=value, --name (a flag) or -t value (run_test).
inline bool settings::parse(std::string& error, int argc,
    const char* const argv[]) NOEXCEPT
{
    for (auto index = one; index < possible_narrow_sign_cast<size_t>(argc);
        ++index)
    {
        const std::string argument{ argv[index] };
        std::string name{ argument };
        std::string value{};

        if (argument == "-t")
        {
            if (++index == possible_narrow_sign_cast<size_t>(argc))
            {
                error = "missing value for option '-t'";
                return false;
            }

            parse_filters(run_test, argv[index]);
            continue;
        }

        const auto equal = argument.find('=');
        if (equal != std::string::npos)
        {
            name = argument.substr(zero, equal);
            value = argument.substr(add1(equal));
        }

        auto valid = true;
        if (name == "--run_test")
            parse_filters(run_test, value);
        else if (name == "--log_level")
            valid = parse_log(log, value);
        else if (name == "--report_level")
            valid = parse_report(report, value);
        else if (name == "--show_progress")
            valid = parse_flag(show_progress, value);
        else if (name == "--build_info")
            valid = parse_flag(build_info, value);
        else if (name == "--list_content")
            valid = value.empty() || value == "HRF";
        else if (name == "--detect_memory_leaks" ||
            name == "--detect_memory_leak")
            valid = parse_number(detect_memory_leaks, value);
        else if (name == "--help" || name == "-h" || name == "-?")
            help = true;
        else
        {
            error = "unrecognized option '" + argument + "'";
            return false;
        }

        if (name == "--list_content")
            list_content = true;

        if (!valid)
        {
            error = "invalid value for option '" + argument + "'";
            return false;
        }
    }

    return true;
}

BC_POP_WARNING()

} // namespace unit_test
} // namespace system
} // namespace libbitcoin

#endif
