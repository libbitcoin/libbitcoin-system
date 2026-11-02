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
#ifndef LIBBITCOIN_SYSTEM_UNIT_TEST_MACROS_HPP
#define LIBBITCOIN_SYSTEM_UNIT_TEST_MACROS_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/unit_test/checks.hpp>
#include <bitcoin/system/unit_test/registry.hpp>
#include <bitcoin/system/unit_test/session.hpp>

/// The fixture of test cases outside of a fixture suite.
using libbitcoin_unit_test_fixture =
    ::libbitcoin::system::unit_test::nil_fixture;

#define BC_UNIT_TEST_JOIN_(left, right) left##right
#define BC_UNIT_TEST_JOIN(left, right) BC_UNIT_TEST_JOIN_(left, right)
#define BC_UNIT_TEST ::libbitcoin::system::unit_test
#define BC_UNIT_TEST_WRITER(output) \
    [&](std::ostream& bc_unit_test_stream) { bc_unit_test_stream << output; }

// Suites (a suite is a namespace, merged by name across files).
// ----------------------------------------------------------------------------

#define BC_AUTO_TEST_SUITE(name) \
    namespace name { \
    static const BC_UNIT_TEST::registrar \
        BC_UNIT_TEST_JOIN(bc_unit_test_suite_, __LINE__) \
            { #name, __FILE__, __LINE__ };

#define BC_FIXTURE_TEST_SUITE(name, fixture) \
    BC_AUTO_TEST_SUITE(name) \
    using libbitcoin_unit_test_fixture = fixture;

#define BC_AUTO_TEST_SUITE_END() \
    static const BC_UNIT_TEST::registrar \
        BC_UNIT_TEST_JOIN(bc_unit_test_suite_end_, __LINE__){}; \
    }

// Cases (a case is a fixture-derived class whose test_method is the body).
// ----------------------------------------------------------------------------

#define BC_FIXTURE_TEST_CASE(name, fixture) \
    struct name : public fixture { void test_method(); }; \
    static void BC_UNIT_TEST_JOIN(name, _invoke)() \
    { \
        name instance; \
        instance.test_method(); \
    } \
    static const BC_UNIT_TEST::registrar BC_UNIT_TEST_JOIN(name, _registrar) \
        { &BC_UNIT_TEST_JOIN(name, _invoke), #name, __FILE__, __LINE__ }; \
    void name::test_method()

#define BC_AUTO_TEST_CASE(name) \
    BC_FIXTURE_TEST_CASE(name, libbitcoin_unit_test_fixture)

// Assertions by severity.
// ----------------------------------------------------------------------------

#define BC_UNIT_TEST_CHECK(severity, predicate) \
    BC_UNIT_TEST::check(BC_UNIT_TEST::level::severity, !!(predicate), \
        __FILE__, __LINE__, #predicate)

#define BC_UNIT_TEST_MESSAGE(severity, predicate, output) \
    BC_UNIT_TEST::check_message(BC_UNIT_TEST::level::severity, \
        !!(predicate), __FILE__, __LINE__, #predicate, \
        BC_UNIT_TEST_WRITER(output))

#define BC_UNIT_TEST_COMPARE(severity, comparison, left, right, text) \
    BC_UNIT_TEST::compare<BC_UNIT_TEST::comparison>( \
        BC_UNIT_TEST::level::severity, __FILE__, __LINE__, \
        #left text #right, (left), (right))

#define BC_UNIT_TEST_CLOSE(severity, left, right, percent) \
    BC_UNIT_TEST::compare_close(BC_UNIT_TEST::level::severity, __FILE__, \
        __LINE__, #left " ~= " #right, (left), (right), (percent))

#define BC_UNIT_TEST_COLLECTIONS(severity, left_begin, left_end, \
    right_begin, right_end) \
    BC_UNIT_TEST::compare_collections(BC_UNIT_TEST::level::severity, \
        __FILE__, __LINE__, "{ " #left_begin ", " #left_end " } == { " \
        #right_begin ", " #right_end " }", (left_begin), (left_end), \
        (right_begin), (right_end))

#define BC_UNIT_TEST_THROW(severity, statement, exception) \
    do \
    { \
        auto bc_unit_test_raised = false; \
        try { statement; } \
        catch (const exception&) { bc_unit_test_raised = true; } \
        BC_UNIT_TEST::check_message(BC_UNIT_TEST::level::severity, \
            bc_unit_test_raised, __FILE__, __LINE__, \
            "exception " #exception " raised", BC_UNIT_TEST_WRITER( \
            "exception " #exception " expected but not raised")); \
    } while (false)

#define BC_UNIT_TEST_NO_THROW(severity, statement) \
    do \
    { \
        auto bc_unit_test_raised = false; \
        try { statement; } \
        catch (const BC_UNIT_TEST::aborted&) { throw; } \
        catch (...) { bc_unit_test_raised = true; } \
        BC_UNIT_TEST::check_message(BC_UNIT_TEST::level::severity, \
            !bc_unit_test_raised, __FILE__, __LINE__, \
            "no exception thrown by " #statement, BC_UNIT_TEST_WRITER( \
            "unexpected exception thrown by " #statement)); \
    } while (false)

#define BC_UNIT_TEST_EXCEPTION(severity, statement, exception, predicate) \
    do \
    { \
        auto bc_unit_test_raised = false; \
        auto bc_unit_test_matched = false; \
        try { statement; } \
        catch (const exception& bc_unit_test_exception) \
        { \
            bc_unit_test_raised = true; \
            bc_unit_test_matched = (predicate)(bc_unit_test_exception); \
        } \
        BC_UNIT_TEST::check_message(BC_UNIT_TEST::level::severity, \
            bc_unit_test_raised && bc_unit_test_matched, __FILE__, __LINE__, \
            "exception " #exception " raised", BC_UNIT_TEST_WRITER( \
            (bc_unit_test_raised ? "incorrect exception " #exception \
            " is caught" : "exception " #exception " expected but not " \
            "raised"))); \
    } while (false)

// Assertions (warn records, check fails, require fails and ends the case).
// ----------------------------------------------------------------------------

#define BC_WARN(predicate) BC_UNIT_TEST_CHECK(warn, predicate)
#define BC_CHECK(predicate) BC_UNIT_TEST_CHECK(check, predicate)
#define BC_REQUIRE(predicate) BC_UNIT_TEST_CHECK(require, predicate)

#define BC_WARN_MESSAGE(predicate, output) \
    BC_UNIT_TEST_MESSAGE(warn, predicate, output)
#define BC_CHECK_MESSAGE(predicate, output) \
    BC_UNIT_TEST_MESSAGE(check, predicate, output)
#define BC_REQUIRE_MESSAGE(predicate, output) \
    BC_UNIT_TEST_MESSAGE(require, predicate, output)

#define BC_WARN_EQUAL(left, right) \
    BC_UNIT_TEST_COMPARE(warn, equal, left, right, " == ")
#define BC_CHECK_EQUAL(left, right) \
    BC_UNIT_TEST_COMPARE(check, equal, left, right, " == ")
#define BC_REQUIRE_EQUAL(left, right) \
    BC_UNIT_TEST_COMPARE(require, equal, left, right, " == ")

#define BC_CHECK_NE(left, right) \
    BC_UNIT_TEST_COMPARE(check, not_equal, left, right, " != ")
#define BC_REQUIRE_NE(left, right) \
    BC_UNIT_TEST_COMPARE(require, not_equal, left, right, " != ")

#define BC_CHECK_LT(left, right) \
    BC_UNIT_TEST_COMPARE(check, less, left, right, " < ")
#define BC_REQUIRE_LT(left, right) \
    BC_UNIT_TEST_COMPARE(require, less, left, right, " < ")
#define BC_REQUIRE_LE(left, right) \
    BC_UNIT_TEST_COMPARE(require, less_equal, left, right, " <= ")
#define BC_REQUIRE_GT(left, right) \
    BC_UNIT_TEST_COMPARE(require, greater, left, right, " > ")
#define BC_REQUIRE_GE(left, right) \
    BC_UNIT_TEST_COMPARE(require, greater_equal, left, right, " >= ")

#define BC_REQUIRE_CLOSE(left, right, percent) \
    BC_UNIT_TEST_CLOSE(require, left, right, percent)

#define BC_REQUIRE_EQUAL_COLLECTIONS(left_begin, left_end, right_begin, \
    right_end) \
    BC_UNIT_TEST_COLLECTIONS(require, left_begin, left_end, right_begin, \
        right_end)

#define BC_REQUIRE_THROW(statement, exception) \
    BC_UNIT_TEST_THROW(require, statement, exception)
#define BC_REQUIRE_NO_THROW(statement) \
    BC_UNIT_TEST_NO_THROW(require, statement)
#define BC_REQUIRE_EXCEPTION(statement, exception, predicate) \
    BC_UNIT_TEST_EXCEPTION(require, statement, exception, predicate)

#define BC_FAIL(output) \
    BC_UNIT_TEST::check_message(BC_UNIT_TEST::level::require, false, \
        __FILE__, __LINE__, "", BC_UNIT_TEST_WRITER(output))

// Logging.
// ----------------------------------------------------------------------------

#define BC_TEST_MESSAGE(output) \
    BC_UNIT_TEST::session::instance().log(BC_UNIT_TEST::log_level::message, \
        BC_UNIT_TEST::format(BC_UNIT_TEST_WRITER(output)))

#define BC_TEST_CONTEXT(output) \
    if constexpr (const BC_UNIT_TEST::context \
        BC_UNIT_TEST_JOIN(bc_unit_test_context_, __LINE__) \
        { BC_UNIT_TEST::format(BC_UNIT_TEST_WRITER(output)) }; false) {} \
    else

#endif
