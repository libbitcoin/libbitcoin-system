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
#ifndef LIBBITCOIN_SYSTEM_UNIT_TEST_REGISTRY_IPP
#define LIBBITCOIN_SYSTEM_UNIT_TEST_REGISTRY_IPP

namespace libbitcoin {
namespace system {
namespace unit_test {

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

// unit
// ----------------------------------------------------------------------------

inline bool unit::is_suite() const NOEXCEPT
{
    return is_null(invoke);
}

// registry
// ----------------------------------------------------------------------------

inline registry& registry::instance() NOEXCEPT
{
    static registry instance{};
    return instance;
}

inline registry::registry() NOEXCEPT
  : open_{ &master_ }
{
}

inline unit& registry::master() NOEXCEPT
{
    return master_;
}

inline void registry::begin_suite(const char* name, const char* file,
    size_t line) NOEXCEPT
{
    auto& children = open_.back()->children;
    for (auto& child: children)
    {
        if (child.is_suite() && child.name == name)
        {
            open_.push_back(&child);
            return;
        }
    }

    children.push_back({ name, file, line });
    open_.push_back(&children.back());
}

inline void registry::end_suite() NOEXCEPT
{
    if (open_.size() > one)
        open_.pop_back();
}

inline void registry::add_case(unit::invoker invoke, const char* name,
    const char* file, size_t line) NOEXCEPT
{
    open_.back()->children.push_back({ name, file, line, invoke });
}

// registrar
// ----------------------------------------------------------------------------

inline registrar::registrar() NOEXCEPT
{
    registry::instance().end_suite();
}

inline registrar::registrar(const char* name, const char* file,
    size_t line) NOEXCEPT
{
    registry::instance().begin_suite(name, file, line);
}

inline registrar::registrar(unit::invoker invoke, const char* name,
    const char* file, size_t line) NOEXCEPT
{
    registry::instance().add_case(invoke, name, file, line);
}

BC_POP_WARNING()

} // namespace unit_test
} // namespace system
} // namespace libbitcoin

#endif
