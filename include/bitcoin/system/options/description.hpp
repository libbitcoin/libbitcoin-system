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
#ifndef LIBBITCOIN_SYSTEM_OPTIONS_DESCRIPTION_HPP
#define LIBBITCOIN_SYSTEM_OPTIONS_DESCRIPTION_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/options/value.hpp>

namespace libbitcoin {
namespace system {
namespace options {

/// Not thread safe, non-virtual.
/// The names, value and description of one option.
class BC_API option_description
{
public:
    DEFAULT_COPY_MOVE_DESTRUCT(option_description);

    /// Names are "long,s", "long" or ",s", taking ownership of the value.
    option_description(const std::string& names,
        const value_semantic* semantic,
        const std::string& description) NOEXCEPT;

    /// The long name, without dashes.
    const std::string& long_name() const NOEXCEPT;

    /// The short name, with its dash ("-s"), or empty.
    const std::string& short_name() const NOEXCEPT;

    /// The description.
    const std::string& description() const NOEXCEPT;

    /// The value semantic.
    std::shared_ptr<const value_semantic> semantic() const NOEXCEPT;

    /// The variable name, which is the long name, else the short name.
    const std::string& key() const NOEXCEPT;

    /// The name for display as "-s [ --long ]", "--long" or "-s".
    std::string format_name() const NOEXCEPT;

    /// The value name for display, empty for an option without tokens.
    std::string format_parameter() const NOEXCEPT;

    /// The name in messages, dashed as on the command line if dashed.
    std::string display_name(bool dashed) const NOEXCEPT;

    /// True if name is the long or short name.
    bool matches(const std::string& name) const NOEXCEPT;

    /// True if name is a strict prefix of the long name.
    bool abbreviates(const std::string& name) const NOEXCEPT;

private:
    std::string long_name_;
    std::string short_name_;
    std::string description_;
    std::shared_ptr<const value_semantic> semantic_;
};

/// Not thread safe, non-virtual.
/// A captioned collection of option descriptions.
class BC_API options_description
{
public:
    using option_ptr = std::shared_ptr<option_description>;
    using option_ptrs = std::vector<option_ptr>;

    /// Adds options by successive function calls.
    class BC_API initializer
    {
    public:
        initializer(options_description& owner) NOEXCEPT;

        /// Add an option (see option_description).
        initializer& operator()(const std::string& names,
            const value_semantic* semantic,
            const std::string& description) NOEXCEPT;

        /// Add a switch, which takes no tokens.
        initializer& operator()(const std::string& names,
            const std::string& description) NOEXCEPT;

    private:
        options_description& owner_;
    };

    options_description(const std::string& caption={}) NOEXCEPT;

    /// Add options by successive function calls on the result.
    initializer add_options() NOEXCEPT;

    /// Add an option.
    void add(const option_ptr& option) NOEXCEPT;

    /// The caption.
    const std::string& caption() const NOEXCEPT;

    /// The options, in order of addition.
    const option_ptrs& options() const NOEXCEPT;

    /// The option of the name, or of the long name it uniquely abbreviates if
    /// abbreviation is allowed, or null. Throws if ambiguous.
    const option_description* find_nothrow(const std::string& name,
        bool abbreviated) const THROWS;

    /// The option, as find_nothrow, throwing if not found.
    const option_description& find(const std::string& name,
        bool abbreviated) const THROWS;

private:
    std::string caption_;
    option_ptrs options_{};
};

/// Not thread safe, non-virtual.
/// The option names of positional arguments, in order.
class BC_API positional_options_description
{
public:
    /// Assign the next count positions to name, unlimited if count is -1.
    positional_options_description& add(const std::string& name,
        int count) NOEXCEPT;

    /// The number of positions, max_size_t if unlimited.
    size_t max_total_count() const NOEXCEPT;

    /// The option name for the position (position < max_total_count()).
    const std::string& name_for_position(size_t position) const NOEXCEPT;

private:
    string_list names_{};
    std::string trailing_{};
};

} // namespace options
} // namespace system

typedef system::options::option_description option_metadata;
typedef system::options::options_description options_metadata;
typedef system::options::positional_options_description arguments_metadata;

} // namespace libbitcoin

#endif
