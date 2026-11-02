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
#ifndef LIBBITCOIN_SYSTEM_UNIT_TEST_RUNNER_HPP
#define LIBBITCOIN_SYSTEM_UNIT_TEST_RUNNER_HPP

#include <chrono>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/unit_test/filter.hpp>
#include <bitcoin/system/unit_test/registry.hpp>
#include <bitcoin/system/unit_test/session.hpp>
#include <bitcoin/system/unit_test/settings.hpp>

namespace libbitcoin {
namespace system {
namespace unit_test {

/// Runs the registered test cases selected by the command line.
class runner
{
public:
    DELETE_COPY_MOVE(runner);

    /// The module names the master suite in logs and reports.
    runner(const std::string& module) NOEXCEPT;

    /// Run the tests, returns the process exit code.
    int run(int argc, const char* const argv[]) NOEXCEPT;

private:
    using path = filter::path;
    using clock = std::chrono::steady_clock;

    struct tally
    {
        size_t cases{};
        size_t cases_passed{};
        size_t cases_failed{};
        size_t cases_aborted{};
        size_t passed{};
        size_t failed{};
        size_t warned{};
    };

    static size_t enable(unit& suite, path& units,
        const filter& selection) NOEXCEPT;
    static std::string join(const path& units) NOEXCEPT;
    static std::string location(const unit& test) NOEXCEPT;
    static std::string elapsed(clock::time_point start) NOEXCEPT;
    static std::string plural(size_t count, const std::string& noun) NOEXCEPT;
    static tally total(const unit& suite) NOEXCEPT;

    void list(const unit& suite, size_t depth) const NOEXCEPT;
    void execute(unit& suite, path& units) NOEXCEPT;
    void invoke(unit& test, const path& units) NOEXCEPT;
    void advance() NOEXCEPT;
    void report(const unit& suite, const std::string& kind,
        size_t depth) const NOEXCEPT;
    void build_info() const NOEXCEPT;
    void configure_runtime() const NOEXCEPT;

    const std::string module_;
    session& session_;
    size_t selected_{};
    size_t completed_{};
    size_t stars_{};
};

} // namespace unit_test
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/unit_test/runner.ipp>

#endif
