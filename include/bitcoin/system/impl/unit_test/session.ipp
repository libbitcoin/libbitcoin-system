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
#ifndef LIBBITCOIN_SYSTEM_UNIT_TEST_SESSION_IPP
#define LIBBITCOIN_SYSTEM_UNIT_TEST_SESSION_IPP

namespace libbitcoin {
namespace system {
namespace unit_test {

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

inline session& session::instance() NOEXCEPT
{
    static session instance{};
    return instance;
}

inline session::session() NOEXCEPT
{
}

inline unit_test::settings& session::settings() NOEXCEPT
{
    return settings_;
}

inline std::string session::case_name() const NOEXCEPT
{
    std::unique_lock lock{ mutex_ };
    return name_;
}

inline void session::start(const std::string& name,
    const std::string& path) NOEXCEPT
{
    std::unique_lock lock{ mutex_ };
    name_ = name;
    path_ = path;
    contexts_.clear();
    passed_ = zero;
    failed_ = zero;
    warned_ = zero;
    aborted_ = false;
}

inline unit_test::result session::finish() NOEXCEPT
{
    std::unique_lock lock{ mutex_ };
    name_.clear();
    path_.clear();
    return { passed_, failed_, warned_, aborted_ };
}

inline void session::passed(level, const char* file, size_t line,
    const char* expression) NOEXCEPT
{
    ++passed_;
    if (settings_.log == log_level::all)
        write(log_level::all, location(file, line) + "info: check " +
            expression + " has passed");
}

inline void session::failed(level severity, const char* file, size_t line,
    const std::string& text) NOEXCEPT
{
    std::unique_lock lock{ mutex_ };
    const auto in = "in \"" + path_ + "\": ";
    const auto where = location(file, line);
    lock.unlock();

    switch (severity)
    {
        case level::warn:
            ++warned_;
            write(log_level::warning, where + "warning: " + in + text +
                contexts());
            break;
        case level::check:
            ++failed_;
            write(log_level::error, where + "error: " + in + text +
                contexts());
            break;
        case level::require:
            ++failed_;
            write(log_level::fatal_error, where + "fatal error: " + in +
                text + contexts());
            break;
    }
}

inline void session::exception(const std::string& text) NOEXCEPT
{
    ++failed_;
    std::unique_lock lock{ mutex_ };
    const auto in = "in \"" + path_ + "\": ";
    lock.unlock();
    write(log_level::cpp_exception, "unknown location(0): fatal error: " +
        in + text);
}

inline void session::abort() NOEXCEPT
{
    aborted_ = true;
}

inline void session::log(log_level at, const std::string& text) NOEXCEPT
{
    write(at, text);
}

inline void session::push_context(const std::string& text) NOEXCEPT
{
    std::unique_lock lock{ mutex_ };
    contexts_.push_back(text);
}

inline void session::pop_context() NOEXCEPT
{
    std::unique_lock lock{ mutex_ };
    if (!contexts_.empty())
        contexts_.pop_back();
}

// private
// ----------------------------------------------------------------------------

inline std::string session::location(const char* file,
    size_t line) const NOEXCEPT
{
#if defined(HAVE_MSC)
    return std::string{ file } + "(" + std::to_string(line) + "): ";
#else
    return std::string{ file } + ":" + std::to_string(line) + ": ";
#endif
}

inline std::string session::contexts() const NOEXCEPT
{
    std::unique_lock lock{ mutex_ };
    if (contexts_.empty())
        return {};

    std::string out{ "\nFailure occurred in a following context:" };
    for (const auto& context: contexts_)
        out += "\n    " + context;

    return out;
}

inline void session::write(log_level at, const std::string& text) NOEXCEPT
{
    if (at < settings_.log)
        return;

    std::unique_lock lock{ mutex_ };
    std::cout << text << std::endl;
}

BC_POP_WARNING()

} // namespace unit_test
} // namespace system
} // namespace libbitcoin

#endif
