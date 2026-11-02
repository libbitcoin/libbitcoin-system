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
#include <bitcoin/system/options/parsers.hpp>

#include <istream>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/options/description.hpp>
#include <bitcoin/system/options/variables.hpp>
#include <bitcoin/system/unicode/unicode.hpp>

#if defined(HAVE_WINDOWS)
    #include <stdlib.h>
#else
    extern char** environ;
#endif

namespace libbitcoin {
namespace system {
namespace options {

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

// command line
// ----------------------------------------------------------------------------

constexpr auto terminator = "--";

static bool is_long(const std::string& argument) NOEXCEPT
{
    return argument.size() > two && argument.starts_with(terminator);
}

static bool is_short(const std::string& argument) NOEXCEPT
{
    return argument.size() > one && argument.front() == '-' &&
        argument.at(one) != '-';
}

// An option argument with an empty value following its equal sign.
static void validate_adjacent(const std::string& argument) THROWS
{
    if (is_long(argument) && argument.ends_with('='))
        throw validation_exception{ "the argument for option '" +
            argument.substr(zero, sub1(argument.size())) +
            "' should follow immediately after the equal sign" };
}

static const option_description& declared_option(const std::string& name,
    const std::string& argument, const options_description& options,
    bool abbreviated) THROWS
{
    const auto option = options.find_nothrow(name, abbreviated);
    if (is_null(option))
        throw validation_exception{ "unrecognised option '" + argument +
            "'" };

    return *option;
}

// Take the least number of tokens of the option from the arguments. An
// argument that is exactly a declared short option name is not a token.
static void take_tokens(parsed_option& parsed,
    const option_description& option, const string_list& arguments,
    size_t& index, const options_description& options) THROWS
{
    const auto semantic = option.semantic();
    const auto minimum = semantic->min_tokens();
    const auto name = option.display_name(true);
    const auto available = parsed.tokens.size() + (arguments.size() - index);

    if (available < minimum)
        throw validation_exception{ "the required argument for option '" +
            name + "' is missing" };

    if (!parsed.tokens.empty() && is_zero(semantic->max_tokens()))
        throw validation_exception{ "option '" + name +
            "' does not take any arguments" };

    auto needed = floored_subtract(minimum, parsed.tokens.size());
    for (; is_nonzero(needed) && index < arguments.size(); --needed, ++index)
    {
        const auto& argument = arguments.at(index);
        validate_adjacent(argument);
        if (is_short(argument) && !is_null(options.find_nothrow(argument,
            false)))
            throw validation_exception{ "the required argument for "
                "option '" + name + "' is missing" };

        parsed.tokens.push_back(argument);
    }
}

// An option that may take more tokens takes following positional arguments.
static std::vector<parsed_option> extend_tokens(
    const std::vector<parsed_option>& parsed,
    const options_description& options) THROWS
{
    std::vector<parsed_option> out{};
    out.reserve(parsed.size());

    for (size_t index{}; index < parsed.size(); ++index)
    {
        out.push_back(parsed.at(index));
        auto& option = out.back();
        if (option.key.empty())
            continue;

        const auto semantic = options.find(option.key, false).semantic();
        const auto maximum = semantic->max_tokens();
        if (semantic->min_tokens() >= maximum ||
            option.tokens.size() >= maximum)
            continue;

        auto more = maximum - option.tokens.size();
        for (; is_nonzero(more) && add1(index) < parsed.size(); --more)
        {
            const auto& next = parsed.at(add1(index));
            if (!next.key.empty() || next.terminated)
                break;

            option.tokens.push_back(next.tokens.front());
            ++index;
        }
    }

    return out;
}

parsed_options parse_command_line(int argc, const char* argv[],
    const options_description& options,
    const positional_options_description& arguments) THROWS
{
    string_list tokens{};
    for (auto index = one; index < to_unsigned(argc); ++index)
        tokens.emplace_back(argv[index]);

    std::vector<parsed_option> parsed{};
    for (size_t index{}; index < tokens.size();)
    {
        const auto argument = tokens.at(index++);

        if (argument == terminator)
        {
            for (; index < tokens.size(); ++index)
                parsed.push_back({ {}, { tokens.at(index) }, true });

            break;
        }

        if (is_long(argument))
        {
            validate_adjacent(argument);
            const auto equals = argument.find('=');
            const auto name = argument.substr(two, equals - two);
            const auto& option = declared_option(name, argument, options,
                true);

            parsed_option occurrence{ option.key(), {} };
            if (equals != std::string::npos)
                occurrence.tokens.push_back(argument.substr(add1(equals)));

            take_tokens(occurrence, option, tokens, index, options);
            parsed.push_back(std::move(occurrence));
            continue;
        }

        if (is_short(argument))
        {
            // Switches may be grouped, the first other option takes the rest
            // of the argument as its token.
            auto name = argument.substr(zero, two);
            auto rest = argument.substr(two);
            while (true)
            {
                const auto& option = declared_option(name, name, options,
                    false);

                parsed_option occurrence{ option.key(), {} };
                const auto zero_tokens = is_zero(option.semantic()->max_tokens());
                if (zero_tokens && !rest.empty())
                {
                    parsed.push_back(std::move(occurrence));
                    name = std::string{ "-" } + rest.front();
                    rest.erase(rest.begin());
                    continue;
                }

                if (!rest.empty())
                    occurrence.tokens.push_back(rest);

                take_tokens(occurrence, option, tokens, index, options);
                parsed.push_back(std::move(occurrence));
                break;
            }

            continue;
        }

        parsed.push_back({ {}, { argument } });
    }

    parsed = extend_tokens(parsed, options);

    size_t position{};
    for (auto& option: parsed)
    {
        if (!option.key.empty())
            continue;

        if (position >= arguments.max_total_count())
            throw validation_exception{ "too many positional options have "
                "been specified on the command line" };

        option.key = arguments.name_for_position(position++);
    }

    return { std::move(parsed), &options, true };
}

// configuration file
// ----------------------------------------------------------------------------

parsed_options parse_config_file(std::istream& stream,
    const options_description& options) THROWS
{
    std::string line{};
    std::string section{};
    std::vector<parsed_option> parsed{};

    while (std::getline(stream, line))
    {
        line = trim_copy(line.substr(zero, line.find('#')));
        if (line.empty())
            continue;

        if (line.front() == '[' && line.back() == ']')
        {
            section = line.substr(one, line.size() - two);
            if (section.empty() || section.back() != '.')
                section.push_back('.');

            continue;
        }

        const auto equals = line.find('=');
        if (equals == std::string::npos)
            throw validation_exception{ "the options configuration file "
                "contains an invalid line '" + line + "'" };

        const auto name = section + trim_copy(line.substr(zero, equals));
        const auto value = trim_copy(line.substr(add1(equals)));
        if (is_null(options.find_nothrow(name, false)))
            throw validation_exception{ "unrecognised option '" + name +
                "'" };

        parsed.push_back({ name, { value } });
    }

    return { std::move(parsed), &options, false };
}

// environment
// ----------------------------------------------------------------------------

static char** environment() NOEXCEPT
{
#if defined(HAVE_WINDOWS)
    BC_PUSH_WARNING(DEPRECATED_DECLARATION)
    return _environ;
    BC_POP_WARNING()
#else
    return environ;
#endif
}

parsed_options parse_environment(const options_description& options,
    const std::string& prefix) THROWS
{
    std::vector<parsed_option> parsed{};

    const auto variables = environment();
    for (size_t index{}; !is_null(variables); ++index)
    {
        BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
        const auto variable = variables[index];
        BC_POP_WARNING()

        if (is_null(variable))
            break;

        const std::string entry{ variable };
        const auto equals = entry.find('=');
        if (equals == std::string::npos || !entry.starts_with(prefix))
            continue;

        const auto name = ascii_to_lower(entry.substr(prefix.size(),
            equals - prefix.size()));

        if (!name.empty() && !is_null(options.find_nothrow(name, false)))
            parsed.push_back({ name, { entry.substr(add1(equals)) } });
    }

    return { std::move(parsed), &options, false };
}

BC_POP_WARNING()

} // namespace options
} // namespace system
} // namespace libbitcoin
