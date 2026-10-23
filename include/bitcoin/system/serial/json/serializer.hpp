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
#ifndef LIBBITCOIN_SYSTEM_SERIAL_JSON_SERIALIZER_HPP
#define LIBBITCOIN_SYSTEM_SERIAL_JSON_SERIALIZER_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/serial/json/value.hpp>

namespace libbitcoin {
namespace system {
namespace json {

/// Resumable compact serializer of a value tree into caller buffers.
/// Each read fills the buffer unless the text completes within it, so memory
/// is bounded by the buffer and the nesting depth. Doubles are written in
/// shortest round-trip scientific form (e.g. 1.5E0), non-finite as null or
/// [-]1e99999, and strings escape only quote, reverse solidus and controls.
class BC_API serializer
{
public:
    DELETE_COPY_MOVE(serializer);

    serializer() NOEXCEPT;
    ~serializer() NOEXCEPT;

    /// Begin serializing the value, which must outlive the serialization.
    void reset(const value* model) NOEXCEPT;

    /// Write up to size bytes to data, return the written view.
    std::string_view read(char* data, size_t size) THROWS;

    /// The value is fully written.
    bool done() const NOEXCEPT;

private:
    struct frame
    {
        const value* container;
        size_t index;
        bool comma;
        bool keyed;
    };

    static size_t format(char* data, const value& element) NOEXCEPT;

    void pend(std::string_view text) NOEXCEPT;
    void begin(const value& element) THROWS;
    void begin_text(std::string_view text, bool is_key) NOEXCEPT;
    void next() THROWS;
    char* write_pending(char* out, const char* end) NOEXCEPT;
    char* write_text(char* out, const char* end) NOEXCEPT;

    std::vector<frame> stack_{};
    const value* model_{};
    std::string_view text_{};
    size_t offset_{};
    size_t pending_size_{};
    size_t pending_offset_{};
    bool started_{};
    bool done_{ true };
    bool texting_{};
    bool is_key_{};
    char pending_[32]{};
};

/// Serialize the value to a string.
BC_API string serialize(const value& model) THROWS;

/// Serialize the value to a stream.
BC_API std::ostream& operator<<(std::ostream& stream, const value& model) THROWS;

} // namespace json
} // namespace system
} // namespace libbitcoin

#endif
