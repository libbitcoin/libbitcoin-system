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
    { no_error, "success" },
    { address_family_not_supported, "address_family_not_supported" },
    { address_in_use, "address_in_use" },
    { address_not_available, "address_not_available" },
    { already_connected, "already_connected" },
    { argument_list_too_long, "argument_list_too_long" },
    { argument_out_of_domain, "argument_out_of_domain" },
    { bad_address, "bad_address" },
    { bad_file_descriptor, "bad_file_descriptor" },
    { bad_message, "bad_message" },
    { broken_pipe, "broken_pipe" },
    { connection_aborted, "connection_aborted" },
    { connection_already_in_progress, "connection_already_in_progress" },
    { connection_refused, "connection_refused" },
    { connection_reset, "connection_reset" },
    { cross_device_link, "cross_device_link" },
    { destination_address_required, "destination_address_required" },
    { device_or_resource_busy, "device_or_resource_busy" },
    { directory_not_empty, "directory_not_empty" },
    { executable_format_error, "executable_format_error" },
    { file_exists, "file_exists" },
    { file_too_large, "file_too_large" },
    { filename_too_long, "filename_too_long" },
    { function_not_supported, "function_not_supported" },
    { host_unreachable, "host_unreachable" },
    { identifier_removed, "identifier_removed" },
    { illegal_byte_sequence, "illegal_byte_sequence" },
    { inappropriate_io_control_operation, "inappropriate_io_control_operation" },
    { interrupted, "interrupted" },
    { invalid_argument, "invalid_argument" },
    { invalid_seek, "invalid_seek" },
    { io_error, "io_error" },
    { is_a_directory, "is_a_directory" },
    { message_size, "message_size" },
    { network_down, "network_down" },
    { network_reset, "network_reset" },
    { network_unreachable, "network_unreachable" },
    { no_buffer_space, "no_buffer_space" },
    { no_child_process, "no_child_process" },
    { no_link, "no_link" },
    { no_lock_available, "no_lock_available" },
    { no_message_available, "no_message_available" },
    { no_message, "no_message" },
    { no_protocol_option, "no_protocol_option" },
    { no_space_on_device, "no_space_on_device" },
    { no_stream_resources, "no_stream_resources" },
    { no_such_device_or_address, "no_such_device_or_address" },
    { no_such_device, "no_such_device" },
    { no_such_file_or_directory, "no_such_file_or_directory" },
    { no_such_process, "no_such_process" },
    { not_a_directory, "not_a_directory" },
    { not_a_socket, "not_a_socket" },
    { not_a_stream, "not_a_stream" },
    { not_connected, "not_connected" },
    { not_enough_memory, "not_enough_memory" },
    { not_supported, "not_supported" },
    { operation_canceled, "operation_canceled" },
    { operation_in_progress, "operation_in_progress" },
    { operation_not_permitted, "operation_not_permitted" },
    { operation_not_supported, "not_supported" },
    { operation_would_block, "resource_unavailable_try_again" },
    { owner_dead, "owner_dead" },
    { permission_denied, "permission_denied" },
    { protocol_error, "protocol_error" },
    { protocol_not_supported, "protocol_not_supported" },
    { read_only_file_system, "read_only_file_system" },
    { resource_deadlock_would_occur, "resource_deadlock_would_occur" },
    { resource_unavailable_try_again, "resource_unavailable_try_again" },
    { result_out_of_range, "result_out_of_range" },
    { state_not_recoverable, "state_not_recoverable" },
    { stream_timeout, "stream_timeout" },
    { text_file_busy, "text_file_busy" },
    { timed_out, "timed_out" },
    { too_many_files_open_in_system, "too_many_files_open_in_system" },
    { too_many_files_open, "too_many_files_open" },
    { too_many_links, "too_many_links" },
    { too_many_symbolic_link_levels, "too_many_symbolic_link_levels" },
    { value_too_large, "value_too_large" },
    { wrong_protocol_type, "wrong_protocol_type" }
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
