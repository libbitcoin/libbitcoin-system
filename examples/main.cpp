/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>

#include <boost/json.hpp>
 
 // This file must be manually included when
// using basic_parser to implement a parser.
#include <boost/json/basic_parser_impl.hpp>

#include <bitcoin/system.hpp>

BC_USE_LIBBITCOIN_MAIN

namespace json = boost::json;

void pretty_print(std::ostream& os, const json::value& value, std::string indent)
{
    constexpr auto offset = 4;

    switch (value.kind())
    {
        case json::kind::object:
        {
            os << "{\n";
            indent.append(offset, ' ');
            auto const& obj = value.get_object();

            if (!obj.empty())
            {
                auto it = obj.begin();
                while (true)
                {
                    os << indent << json::serialize(it->key()) << " : ";
                    pretty_print(os, it->value(), indent);
                    if (++it == obj.end())
                        break;
                    os << ",\n";
                }
            }

            os << "\n";
            indent.resize(indent.size() - offset);
            os << indent << "}";
            break;
        }

        case json::kind::array:
        {
            os << "[\n";
            indent.append(offset, ' ');
            auto const& arr = value.get_array();

            if (!arr.empty())
            {
                auto it = arr.begin();
                while (true)
                {
                    os << indent;
                    pretty_print(os, *it, indent);
                    if (++it == arr.end())
                        break;
                    os << ",\n";
                }
            }

            os << "\n";
            indent.resize(indent.size() - offset);
            os << indent << "]";
            break;
        }

        case json::kind::string:
            os << json::serialize(value.get_string());
            break;

        case json::kind::uint64:
            os << value.get_uint64();
            break;

        case json::kind::int64:
            os << value.get_int64();
            break;

        case json::kind::double_:
            os << value.get_double();
            break;

        case json::kind::bool_:
            os << (value.get_bool() ? "true" : "false");
            break;

        case json::kind::null:
            os << "null";
            break;
    }

    if (indent.empty())
        os << "\n";
}

// The null parser discards all the data
class null_parser
{
    struct handler
    {
        constexpr static size_t max_object_size = size_t(-1);
        constexpr static size_t max_array_size = size_t(-1);
        constexpr static size_t max_key_size = size_t(-1);
        constexpr static size_t max_string_size = size_t(-1);

        bool on_document_begin(json::error_code&) { return true; }
        bool on_document_end(json::error_code&) { return true; }
        bool on_object_begin(json::error_code&) { return true; }
        bool on_object_end(size_t, json::error_code&) { return true; }
        bool on_array_begin(json::error_code&) { return true; }
        bool on_array_end(size_t, json::error_code&) { return true; }
        bool on_key_part(json::string_view, size_t, json::error_code&) { return true; }
        bool on_key(json::string_view, size_t, json::error_code&) { return true; }
        bool on_string_part(json::string_view, size_t, json::error_code&) { return true; }
        bool on_string(json::string_view, size_t, json::error_code&) { return true; }
        bool on_number_part(json::string_view, json::error_code&) { return true; }
        bool on_int64(int64_t, json::string_view, json::error_code&) { return true; }
        bool on_uint64(uint64_t, json::string_view, json::error_code&) { return true; }
        bool on_double(double, json::string_view, json::error_code&) { return true; }
        bool on_bool(bool, json::error_code&) { return true; }
        bool on_null(json::error_code&) { return true; }
        bool on_comment_part(json::string_view, json::error_code&) { return true; }
        bool on_comment(json::string_view, json::error_code&) { return true; }
    };

    json::basic_parser<handler> parser_;

public:
    null_parser()
      : parser_(json::parse_options())
    {
    }

    ~null_parser()
    {
    }

    std::size_t write_some(const json::string_view& text, json::error_code& ec)
    {
        return parser_.write_some(false, text.data(), text.size(), ec);
    }
};

int bc::system::main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: pretty <json>" << std::endl;
        return EXIT_FAILURE;
    }

    json::error_code ec;
    const auto text = json::string_view(argv[1]);

    null_parser validate;
    auto consumed = validate.write_some(text, ec);

    if (ec || consumed < text.size())
    {
        std::cerr << "Invalid <json> null_parser" << std::endl;
        return EXIT_FAILURE;
    }

    json::stream_parser parse;
    consumed = parse.write_some(text, ec);

    if (ec || consumed < text.size())
    {
        std::cerr << "Invalid <json> stream_parser" << std::endl;
        return EXIT_FAILURE;
    }

    const auto value = parse.release();
    pretty_print(std::cout, value, "");

    return EXIT_SUCCESS;
}
