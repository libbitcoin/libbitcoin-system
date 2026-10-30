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
#ifndef LIBBITCOIN_SYSTEM_OPTIONS_VALUE_HPP
#define LIBBITCOIN_SYSTEM_OPTIONS_VALUE_HPP

#include <any>
#include <optional>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace options {

/// True if the type is constructed from the whole token (otherwise extracted).
template <typename Type>
struct is_whole_token_t : std::false_type {};
template <typename Type>
constexpr bool is_whole_token = is_whole_token_t<Type>::value;

/// Not thread safe, virtual.
/// The type, token arity, default and notification of an option value.
class BC_API value_semantic
{
public:
    virtual ~value_semantic() NOEXCEPT = default;

    /// The value name for display, with any implicit and default text.
    virtual std::string name() const NOEXCEPT = 0;

    /// The least number of tokens of one occurrence.
    virtual size_t min_tokens() const NOEXCEPT = 0;

    /// The greatest number of tokens of one occurrence.
    virtual size_t max_tokens() const NOEXCEPT = 0;

    /// True if occurrences from successive sources accumulate.
    virtual bool is_composing() const NOEXCEPT = 0;

    /// True if the option must be specified.
    virtual bool is_required() const NOEXCEPT = 0;

    /// Parse the tokens of one occurrence into value (option names errors).
    virtual void parse(std::any& value, const string_list& tokens,
        const std::string& option) const THROWS = 0;

    /// Set value to the default, false if there is no default.
    virtual bool apply_default(std::any& value) const NOEXCEPT = 0;

    /// Assign value to the bound store and invoke the notifier.
    virtual void notify(const std::any& value) const THROWS = 0;
};

/// Not thread safe, virtual.
/// An option value of the given type, optionally bound to a store.
template <typename Type>
class typed_value
  : public value_semantic
{
public:
    using handler = std::function<void(const Type&)>;

    typed_value(Type* store) NOEXCEPT;

    /// Declare the default value, displayed as its serialization.
    typed_value* default_value(const Type& value) NOEXCEPT;

    /// Declare the default value, displayed as text.
    typed_value* default_value(const Type& value,
        const std::string& text) NOEXCEPT;

    /// Declare the value of an occurrence without tokens.
    typed_value* implicit_value(const Type& value) NOEXCEPT;

    /// Declare a handler of the value upon notification.
    typed_value* notifier(const handler& notify) NOEXCEPT;

    /// Accumulate occurrences from successive sources.
    typed_value* composing() NOEXCEPT;

    /// Accept any number of tokens per occurrence.
    typed_value* multitoken() NOEXCEPT;

    /// Accept no tokens (a switch).
    typed_value* zero_tokens() NOEXCEPT;

    /// Require that the option is specified.
    typed_value* required() NOEXCEPT;

    std::string name() const NOEXCEPT override;
    size_t min_tokens() const NOEXCEPT override;
    size_t max_tokens() const NOEXCEPT override;
    bool is_composing() const NOEXCEPT override;
    bool is_required() const NOEXCEPT override;
    void parse(std::any& value, const string_list& tokens,
        const std::string& option) const THROWS override;
    bool apply_default(std::any& value) const NOEXCEPT override;
    void notify(const std::any& value) const THROWS override;

private:
    template <typename Element>
    static std::string to_text(const Element& value) NOEXCEPT;

    template <typename Element>
    static Element parse_token(const std::string& token,
        const std::string& option) THROWS;

    Type* store_;
    std::optional<Type> default_{};
    std::optional<Type> implicit_{};
    std::string default_text_{};
    std::string implicit_text_{};
    handler notifier_{};
    bool composing_{};
    bool multitoken_{};
    bool zero_tokens_{};
    bool required_{};
};

/// Declare an option value, bound to store if not null.
template <typename Type>
typed_value<Type>* value(Type* store=nullptr) NOEXCEPT;

} // namespace options
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/options/value.ipp>

#endif
