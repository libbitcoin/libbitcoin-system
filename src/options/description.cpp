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
#include <bitcoin/system/options/description.hpp>

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/options/value.hpp>

namespace libbitcoin {
namespace system {
namespace options {

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

// option_description
// ----------------------------------------------------------------------------

option_description::option_description(const std::string& names,
    const value_semantic* semantic, const std::string& description) NOEXCEPT
  : description_(description), semantic_(semantic)
{
    const auto comma = names.find(',');
    if (comma == std::string::npos)
    {
        long_name_ = names;
        return;
    }

    long_name_ = names.substr(zero, comma);
    short_name_ = "-" + names.substr(add1(comma));
}

const std::string& option_description::long_name() const NOEXCEPT
{
    return long_name_;
}

const std::string& option_description::short_name() const NOEXCEPT
{
    return short_name_;
}

const std::string& option_description::description() const NOEXCEPT
{
    return description_;
}

std::shared_ptr<const value_semantic>
option_description::semantic() const NOEXCEPT
{
    return semantic_;
}

const std::string& option_description::key() const NOEXCEPT
{
    return long_name_.empty() ? short_name_ : long_name_;
}

std::string option_description::format_name() const NOEXCEPT
{
    if (short_name_.empty())
        return "--" + long_name_;

    return long_name_.empty() ? short_name_ :
        short_name_ + " [ --" + long_name_ + " ]";
}

std::string option_description::format_parameter() const NOEXCEPT
{
    return is_zero(semantic_->max_tokens()) ? std::string{} :
        semantic_->name();
}

std::string option_description::display_name(bool dashed) const NOEXCEPT
{
    if (long_name_.empty())
        return short_name_;

    return dashed ? "--" + long_name_ : long_name_;
}

bool option_description::matches(const std::string& name) const NOEXCEPT
{
    return (!long_name_.empty() && name == long_name_) ||
        (!short_name_.empty() && name == short_name_);
}

bool option_description::abbreviates(const std::string& name) const NOEXCEPT
{
    return !name.empty() && name.size() < long_name_.size() &&
        long_name_.starts_with(name);
}

// options_description
// ----------------------------------------------------------------------------

options_description::initializer::initializer(
    options_description& owner) NOEXCEPT
  : owner_(owner)
{
}

options_description::initializer&
options_description::initializer::operator()(const std::string& names,
    const value_semantic* semantic, const std::string& description) NOEXCEPT
{
    owner_.add(std::make_shared<option_description>(names, semantic,
        description));

    return *this;
}

options_description::initializer&
options_description::initializer::operator()(const std::string& names,
    const std::string& description) NOEXCEPT
{
    return (*this)(names, value<bool>()->zero_tokens(), description);
}

options_description::options_description(const std::string& caption) NOEXCEPT
  : caption_(caption)
{
}

options_description::initializer options_description::add_options() NOEXCEPT
{
    return { *this };
}

void options_description::add(const option_ptr& option) NOEXCEPT
{
    options_.push_back(option);
}

const std::string& options_description::caption() const NOEXCEPT
{
    return caption_;
}

const options_description::option_ptrs&
options_description::options() const NOEXCEPT
{
    return options_;
}

// An exact match governs, otherwise an abbreviation must be unique.
const option_description* options_description::find_nothrow(
    const std::string& name, bool abbreviated) const THROWS
{
    const option_description* exact{};
    const option_description* abbreviation{};
    size_t exacts{};
    size_t abbreviations{};

    for (const auto& option: options_)
    {
        if (option->matches(name))
        {
            exact = option.get();
            ++exacts;
        }
        else if (abbreviated && option->abbreviates(name))
        {
            abbreviation = option.get();
            ++abbreviations;
        }
    }

    if (exacts > one || (is_zero(exacts) && abbreviations > one))
        throw validation_exception{ "option '" + name + "' is ambiguous" };

    return is_null(exact) ? abbreviation : exact;
}

const option_description& options_description::find(const std::string& name,
    bool abbreviated) const THROWS
{
    const auto option = find_nothrow(name, abbreviated);
    if (is_null(option))
        throw validation_exception{ "unrecognised option '" + name + "'" };

    return *option;
}

// positional_options_description
// ----------------------------------------------------------------------------

positional_options_description& positional_options_description::add(
    const std::string& name, int count) NOEXCEPT
{
    if (is_negative(count))
        trailing_ = name;
    else
        names_.insert(names_.end(), to_unsigned(count), name);

    return *this;
}

size_t positional_options_description::max_total_count() const NOEXCEPT
{
    return trailing_.empty() ? names_.size() : max_size_t;
}

const std::string& positional_options_description::name_for_position(
    size_t position) const NOEXCEPT
{
    return position < names_.size() ? names_.at(position) : trailing_;
}

BC_POP_WARNING()

} // namespace options
} // namespace system
} // namespace libbitcoin
