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
#ifndef LIBBITCOIN_SYSTEM_UNIT_TEST_REGISTRY_HPP
#define LIBBITCOIN_SYSTEM_UNIT_TEST_REGISTRY_HPP

#include <list>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace unit_test {

/// The outcome of a test case.
struct result
{
    size_t passed{};
    size_t failed{};
    size_t warned{};
    bool aborted{};
};

/// A test suite (no invoker) or a test case.
struct unit
{
    using invoker = void(*)();

    bool is_suite() const NOEXCEPT;

    std::string name{};
    std::string file{};
    size_t line{};
    invoker invoke{};
    std::list<unit> children{};

    /// Set by the runner.
    bool enabled{};
    unit_test::result result{};
};

/// The tree of test units, populated by registrars at static initialization.
class registry
{
public:
    DELETE_COPY_MOVE(registry);

    static registry& instance() NOEXCEPT;

    /// An empty registry (the instance is populated by registrars).
    registry() NOEXCEPT;

    /// The master suite, parent of all top level units.
    unit& master() NOEXCEPT;

    /// Open a suite in the open suite (merged with a same named suite).
    void begin_suite(const char* name, const char* file, size_t line) NOEXCEPT;

    /// Close the open suite.
    void end_suite() NOEXCEPT;

    /// Add a test case to the open suite.
    void add_case(unit::invoker invoke, const char* name, const char* file,
        size_t line) NOEXCEPT;

private:
    unit master_{};
    std::vector<unit*> open_{};
};

/// Registers a test unit at static initialization.
struct registrar
{
    /// Close the open suite.
    registrar() NOEXCEPT;

    /// Open a suite.
    registrar(const char* name, const char* file, size_t line) NOEXCEPT;

    /// Add a test case to the open suite.
    registrar(unit::invoker invoke, const char* name, const char* file,
        size_t line) NOEXCEPT;
};

/// Base of test cases outside of a fixture suite.
struct nil_fixture
{
};

} // namespace unit_test
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/unit_test/registry.ipp>

#endif
