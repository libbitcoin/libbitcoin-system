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
#ifndef LIBBITCOIN_SYSTEM_UNIT_TEST_SESSION_HPP
#define LIBBITCOIN_SYSTEM_UNIT_TEST_SESSION_HPP

#include <atomic>
#include <mutex>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/unit_test/registry.hpp>
#include <bitcoin/system/unit_test/settings.hpp>

namespace libbitcoin {
namespace system {
namespace unit_test {

/// Assertion severity.
enum class level
{
    warn,
    check,
    require
};

/// Thrown by a failed require, ends the current test case.
struct aborted
{
};

/// The state of a test run, shared by assertions on any thread.
class session
{
public:
    static session& instance() NOEXCEPT;

    /// The command line settings.
    unit_test::settings& settings() NOEXCEPT;

    /// The name of the running test case (empty if none).
    std::string case_name() const NOEXCEPT;

    /// Begin a test case (path is its suites and name, joined by '/').
    void start(const std::string& name, const std::string& path) NOEXCEPT;

    /// End the test case and obtain its outcome.
    unit_test::result finish() NOEXCEPT;

    /// Record an assertion that held.
    void passed(level severity, const char* file, size_t line,
        const char* expression) NOEXCEPT;

    /// Record an assertion that failed (the caller ends a require).
    void failed(level severity, const char* file, size_t line,
        const std::string& text) NOEXCEPT;

    /// Record an exception that escaped the test case.
    void exception(const std::string& text) NOEXCEPT;

    /// Record that the test case was ended by a require.
    void abort() NOEXCEPT;

    /// Log text at the given level.
    void log(log_level at, const std::string& text) NOEXCEPT;

    /// Add or remove context text shown with failures.
    void push_context(const std::string& text) NOEXCEPT;
    void pop_context() NOEXCEPT;

private:
    session() NOEXCEPT;
    std::string location(const char* file, size_t line) const NOEXCEPT;
    std::string contexts() const NOEXCEPT;
    void write(log_level at, const std::string& text) NOEXCEPT;

    unit_test::settings settings_{};
    mutable std::mutex mutex_{};
    std::string name_{};
    std::string path_{};
    std::vector<std::string> contexts_{};
    std::atomic<size_t> passed_{};
    std::atomic<size_t> failed_{};
    std::atomic<size_t> warned_{};
    std::atomic<bool> aborted_{};
};

} // namespace unit_test
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/unit_test/session.ipp>

#endif
