/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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
#include "../test.hpp"

BOOST_AUTO_TEST_SUITE(errorno_t_tests)

BOOST_AUTO_TEST_CASE(errorno_t__clear_errno__always__cleared)
{
    errno = 42;
    error::clear_errno();
    BOOST_REQUIRE(is_zero(errno));
}

BOOST_AUTO_TEST_CASE(errorno_t__set_errno__always__expected)
{
    errno = error::errorno_t::argument_out_of_domain;
    BOOST_REQUIRE(errno == error::errorno_t::argument_out_of_domain);
    error::set_errno(error::errorno_t::bad_address);
    BOOST_REQUIRE(errno == error::errorno_t::bad_address);
}

BOOST_AUTO_TEST_CASE(errorno_t__get_errno__no_code__no_error)
{
    error::clear_errno();
    BOOST_REQUIRE_EQUAL(error::get_errno(), error::errorno_t::no_error);
}

BOOST_AUTO_TEST_CASE(errorno_t__get_errno__code__expected)
{
    errno = error::errorno_t::bad_address;
    BOOST_REQUIRE_EQUAL(error::get_errno(), error::errorno_t::bad_address);
}

BOOST_AUTO_TEST_CASE(errorno_t__code__no_error__false_exected_message)
{
    constexpr auto value = error::no_error;
    const auto ec = code(value);
    BOOST_REQUIRE(!ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "success");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__address_family_not_supported__true_exected_message)
{
    constexpr auto value = error::address_family_not_supported;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "address family not supported");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__address_in_use__true_exected_message)
{
    constexpr auto value = error::address_in_use;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "address in use");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__address_not_available__true_exected_message)
{
    constexpr auto value = error::address_not_available;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "address not available");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__already_connected__true_exected_message)
{
    constexpr auto value = error::already_connected;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "already connected");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__argument_list_too_long__true_exected_message)
{
    constexpr auto value = error::argument_list_too_long;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "argument list too long");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__argument_out_of_domain__true_exected_message)
{
    constexpr auto value = error::argument_out_of_domain;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "argument out of domain");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__bad_address__true_exected_message)
{
    constexpr auto value = error::bad_address;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "bad address");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__bad_file_descriptor__true_exected_message)
{
    constexpr auto value = error::bad_file_descriptor;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "bad file descriptor");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__bad_message__true_exected_message)
{
    constexpr auto value = error::bad_message;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "bad message");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__broken_pipe__true_exected_message)
{
    constexpr auto value = error::broken_pipe;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "broken pipe");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__connection_aborted__true_exected_message)
{
    constexpr auto value = error::connection_aborted;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "connection aborted");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__connection_already_in_progress__true_exected_message)
{
    constexpr auto value = error::connection_already_in_progress;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "connection already in progress");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__connection_refused__true_exected_message)
{
    constexpr auto value = error::connection_refused;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "connection refused");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__connection_reset__true_exected_message)
{
    constexpr auto value = error::connection_reset;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "connection reset");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__cross_device_link__true_exected_message)
{
    constexpr auto value = error::cross_device_link;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "cross device link");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__destination_address_required__true_exected_message)
{
    constexpr auto value = error::destination_address_required;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "destination address required");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__device_or_resource_busy__true_exected_message)
{
    constexpr auto value = error::device_or_resource_busy;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "device or resource busy");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__directory_not_empty__true_exected_message)
{
    constexpr auto value = error::directory_not_empty;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "directory not empty");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__executable_format_error__true_exected_message)
{
    constexpr auto value = error::executable_format_error;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "executable format error");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__file_exists__true_exected_message)
{
    constexpr auto value = error::file_exists;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "file exists");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__file_too_large__true_exected_message)
{
    constexpr auto value = error::file_too_large;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "file too large");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__filename_too_long__true_exected_message)
{
    constexpr auto value = error::filename_too_long;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "filename too long");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__function_not_supported__true_exected_message)
{
    constexpr auto value = error::function_not_supported;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "function not supported");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__host_unreachable__true_exected_message)
{
    constexpr auto value = error::host_unreachable;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "host unreachable");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__identifier_removed__true_exected_message)
{
    constexpr auto value = error::identifier_removed;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "identifier removed");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__illegal_byte_sequence__true_exected_message)
{
    constexpr auto value = error::illegal_byte_sequence;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "illegal byte sequence");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__inappropriate_io_control_operation__true_exected_message)
{
    constexpr auto value = error::inappropriate_io_control_operation;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "inappropriate io control operation");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__interrupted__true_exected_message)
{
    constexpr auto value = error::interrupted;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "interrupted");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__invalid_argument__true_exected_message)
{
    constexpr auto value = error::invalid_argument;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "invalid argument");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__ainvalid_seek__true_exected_message)
{
    constexpr auto value = error::invalid_seek;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "invalid seek");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__io_error__true_exected_message)
{
    constexpr auto value = error::io_error;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "io error");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__is_a_directory__true_exected_message)
{
    constexpr auto value = error::is_a_directory;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "is a directory");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__message_size__true_exected_message)
{
    constexpr auto value = error::message_size;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "message size");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__network_down__true_exected_message)
{
    constexpr auto value = error::network_down;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "network down");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__network_reset__true_exected_message)
{
    constexpr auto value = error::network_reset;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "network reset");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__network_unreachable__true_exected_message)
{
    constexpr auto value = error::network_unreachable;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "network unreachable");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__no_buffer_space__true_exected_message)
{
    constexpr auto value = error::no_buffer_space;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "no buffer space");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__no_child_process__true_exected_message)
{
    constexpr auto value = error::no_child_process;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "no child process");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__no_link__true_exected_message)
{
    constexpr auto value = error::no_link;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "no link");
}

BOOST_AUTO_TEST_CASE(errorno_t__code_no_lock_available__true_exected_message)
{
    constexpr auto value = error::no_lock_available;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "no lock available");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__no_message_available__true_exected_message)
{
    constexpr auto value = error::no_message_available;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "no message available");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__no_message__true_exected_message)
{
    constexpr auto value = error::no_message;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "no message");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__no_protocol_option__true_exected_message)
{
    constexpr auto value = error::no_protocol_option;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "no protocol option");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__no_space_on_device__true_exected_message)
{
    constexpr auto value = error::no_space_on_device;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "no space on device");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__no_stream_resources__true_exected_message)
{
    constexpr auto value = error::no_stream_resources;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "no stream resources");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__no_such_device_or_address__true_exected_message)
{
    constexpr auto value = error::no_such_device_or_address;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "no such device or address");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__errorno_t__code__no_such_device__true_exected_message)
{
    constexpr auto value = error::no_such_device;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "no such device");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__no_such_file_or_directory__true_exected_message)
{
    constexpr auto value = error::no_such_file_or_directory;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "no such file or directory");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__no_such_process__true_exected_message)
{
    constexpr auto value = error::no_such_process;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "no such process");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__not_a_directory__true_exected_message)
{
    constexpr auto value = error::not_a_directory;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "not a directory");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__not_a_socket__true_exected_message)
{
    constexpr auto value = error::not_a_socket;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "not a socket");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__not_a_stream__true_exected_message)
{
    constexpr auto value = error::not_a_stream;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "not a stream");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__not_connected__true_exected_message)
{
    constexpr auto value = error::not_connected;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "not connected");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__not_enough_memory)
{
    constexpr auto value = error::not_enough_memory;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "not enough memory");
}

// sourceware.org/bugzilla/show_bug.cgi?id=2363
BOOST_AUTO_TEST_CASE(errorno_t__code__not_supported__true_exected_message)
{
    constexpr auto value = error::not_supported;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "not supported");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__operation_canceled__true_exected_message)
{
    constexpr auto value = error::operation_canceled;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "operation canceled");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__operation_in_progress__true_exected_message)
{
    constexpr auto value = error::operation_in_progress;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "operation in progress");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__operation_not_permitted__true_exected_message)
{
    constexpr auto value = error::operation_not_permitted;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "operation not permitted");
}

// sourceware.org/bugzilla/show_bug.cgi?id=2363
BOOST_AUTO_TEST_CASE(errorno_t__code__operation_not_supported__true_exected_message)
{
    constexpr auto value = error::operation_not_supported;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "not supported");
}

// stackoverflow.com/questions/7003234/which-systems-define-eagain-and-ewouldblock-as-different-values
BOOST_AUTO_TEST_CASE(errorno_t__code__operation_would_block__true_exected_message)
{
    constexpr auto value = error::operation_would_block;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "resource unavailable try again");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__owner_dead__true_exected_message)
{
    constexpr auto value = error::owner_dead;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "owner dead");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__permission_denied__true_exected_message)
{
    constexpr auto value = error::permission_denied;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "permission denied");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__protocol_error__true_exected_message)
{
    constexpr auto value = error::protocol_error;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "protocol error");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__protocol_not_supported__true_exected_message)
{
    constexpr auto value = error::protocol_not_supported;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "protocol not supported");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__read_only_file_system__true_exected_message)
{
    constexpr auto value = error::read_only_file_system;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "read only file system");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__resource_deadlock_would_occur__true_exected_message)
{
    constexpr auto value = error::resource_deadlock_would_occur;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "resource deadlock would occur");
}

// stackoverflow.com/questions/7003234/which-systems-define-eagain-and-ewouldblock-as-different-values
BOOST_AUTO_TEST_CASE(errorno_t__code__resource_unavailable_try_again__true_exected_message)
{
    constexpr auto value = error::resource_unavailable_try_again;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "resource unavailable try again");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__result_out_of_range__true_exected_message)
{
    constexpr auto value = error::result_out_of_range;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "result out of range");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__state_not_recoverable__true_exected_message)
{
    constexpr auto value = error::state_not_recoverable;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "state not recoverable");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__stream_timeout__true_exected_message)
{
    constexpr auto value = error::stream_timeout;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "stream timeout");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__text_file_busy__true_exected_message)
{
    constexpr auto value = error::text_file_busy;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "text file busy");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__timed_out__true_exected_message)
{
    constexpr auto value = error::timed_out;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "timed out");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__too_many_files_open_in_system__true_exected_message)
{
    constexpr auto value = error::too_many_files_open_in_system;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "too many files open in system");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__too_many_files_open__true_exected_message)
{
    constexpr auto value = error::too_many_files_open;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "too many files open");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__too_many_links__true_exected_message)
{
    constexpr auto value = error::too_many_links;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "too many links");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__too_many_symbolic_link_levels__true_exected_message)
{
    constexpr auto value = error::too_many_symbolic_link_levels;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "too many symbolic link levels");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__value_too_large__true_exected_message)
{
    constexpr auto value = error::value_too_large;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "value too large");
}

BOOST_AUTO_TEST_CASE(errorno_t__code__wrong_protocol_type__true_exected_message)
{
    constexpr auto value = error::wrong_protocol_type;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "wrong protocol type");
}

BOOST_AUTO_TEST_SUITE_END()
