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
#include <bitcoin/system/error/errorno_t.hpp>

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/macros.hpp>

namespace libbitcoin {
namespace system {
namespace error {

// on gcc/clang EWOULDBLOCK == EAGAIN so use same description.
// on gcc/clang EOPNOTSUPP == ENOTSUP so use same description.
DEFINE_ERROR_T_MESSAGE_MAP(errorno)
{
    { no_error, "success"},
    { address_family_not_supported, "address family not supported" },
    { address_in_use, "address in use" },
    { address_not_available, "address not available" },
    { already_connected, "already connected" },
    { argument_list_too_long, "argument list too long" },
    { argument_out_of_domain, "argument out of domain" },
    { bad_address, "bad address" },
    { bad_file_descriptor, "bad file descriptor" },
    { bad_message, "bad message" },
    { broken_pipe, "broken pipe" },
    { connection_aborted, "connection aborted" },
    { connection_already_in_progress, "connection already in progress" },
    { connection_refused, "connection refused" },
    { connection_reset, "connection reset" },
    { cross_device_link, "cross device link" },
    { destination_address_required, "destination address required" },
    { device_or_resource_busy, "device or resource busy" },
    { directory_not_empty, "directory not empty" },
    { executable_format_error, "executable format error" },
    { file_exists, "file exists" },
    { file_too_large, "file too large" },
    { filename_too_long, "filename too long" },
    { function_not_supported, "function not supported" },
    { host_unreachable, "host unreachable" },
    { identifier_removed, "identifier removed" },
    { illegal_byte_sequence, "illegal byte sequence" },
    { inappropriate_io_control_operation, "inappropriate io control operation" },
    { interrupted, "interrupted" },
    { invalid_argument, "invalid argument" },
    { invalid_seek, "invalid seek" },
    { io_error, "io error" },
    { is_a_directory, "is a directory" },
    { message_size, "message size" },
    { network_down, "network down" },
    { network_reset, "network reset" },
    { network_unreachable, "network unreachable" },
    { no_buffer_space, "no buffer space" },
    { no_child_process, "no child process" },
    { no_link, "no link" },
    { no_lock_available, "no lock available" },
    { no_message_available, "no message available" },
    { no_message, "no message" },
    { no_protocol_option, "no protocol option" },
    { no_space_on_device, "no space on device" },
    { no_stream_resources, "no stream resources" },
    { no_such_device_or_address, "no such device or address" },
    { no_such_device, "no such device" },
    { no_such_file_or_directory, "no such file or directory" },
    { no_such_process, "no such process" },
    { not_a_directory, "not a directory" },
    { not_a_socket, "not a socket" },
    { not_a_stream, "not a stream" },
    { not_connected, "not connected" },
    { not_enough_memory, "not enough memory" },
    { not_supported, "not supported" },
    { operation_canceled, "operation canceled" },
    { operation_in_progress, "operation in progress" },
    { operation_not_permitted, "operation not permitted" },
    { operation_not_supported, "not supported" },
    { operation_would_block, "resource unavailable try again" },
    { owner_dead, "owner dead" },
    { permission_denied, "permission denied" },
    { protocol_error, "protocol error" },
    { protocol_not_supported, "protocol not supported" },
    { read_only_file_system, "read only file system" },
    { resource_deadlock_would_occur, "resource deadlock would occur" },
    { resource_unavailable_try_again, "resource unavailable try again" },
    { result_out_of_range, "result out of range" },
    { state_not_recoverable, "state not recoverable" },
    { stream_timeout, "stream timeout" },
    { text_file_busy, "text file busy" },
    { timed_out, "timed out" },
    { too_many_files_open_in_system, "too many files open in system" },
    { too_many_files_open, "too many files open" },
    { too_many_links, "too many links" },
    { too_many_symbolic_link_levels, "too many symbolic link levels" },
    { value_too_large, "value too large" },
    { wrong_protocol_type, "wrong protocol type" }
};

DEFINE_ERROR_T_CATEGORY(errorno, "os", "unknown")

void clear_errno() NOEXCEPT
{
    set_errno(errorno_t::no_error);
}

void set_errno(errorno_t value) NOEXCEPT
{
    errno = value;
}

std::error_code get_errno() NOEXCEPT
{
    return make_error_code(static_cast<errorno_t>(errno));
}

#undef UNKNOWN_MESSAGE

} // namespace error
} // namespace system
} // namespace libbitcoin
