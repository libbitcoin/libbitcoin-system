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

namespace libbitcoin {
namespace system {
namespace chain {

void parse_point()
{
    const auto text_point =
        "{"
            "\"hash\": \"0000000000000000000000000000000000000000000000000000000000000000\","
            "\"index\" : 42"
        "}";

    const auto parsed_value = json::parse(text_point);

    const chain::point const_object
    {
        null_hash,
        42
    };

    const auto parsed_to_object = json::value_to<chain::point>(parsed_value);
    std::cout << json::serialize(parsed_value) << std::endl;
    assert(parsed_to_object == const_object);

    const auto const_object_to_value = json::value_from(const_object);
    std::cout << json::serialize(const_object_to_value) << std::endl;
    assert(parsed_value == const_object_to_value);
}

void parse_input()
{
    const auto text_input =
        "{ \"point\": "
            "{"
                "\"hash\": \"0000000000000000000000000000000000000000000000000000000000000000\","
                "\"index\" : 42"
            "},"
            "\"script\": \"return\","
            "\"witness\": \"[424242]\","
            "\"sequence\": 24"
        "}";

    const auto parsed_value = json::parse(text_input);

    const chain::input const_object
    {
        chain::point{ null_hash, 42 },
        chain::script{ { opcode::op_return } },
        chain::witness{ "[424242]" },
        24 
    };

    const auto parsed_to_object = json::value_to<chain::input>(parsed_value);
    std::cout << json::serialize(parsed_value) << std::endl;
    assert(parsed_to_object == const_object);

    const auto const_object_to_value = json::value_from(const_object);
    std::cout << json::serialize(const_object_to_value) << std::endl;
    assert(parsed_value == const_object_to_value);
}

void parse_output()
{
    const auto text_output =
        "{"
            "\"value\": 24,"
            "\"script\": \"return\""
        "}";

    const auto parsed_value = json::parse(text_output);

    const chain::output const_object
    {
        24,
        chain::script{ { opcode::op_return } }
    };

    const auto parsed_to_object = json::value_to<chain::output>(parsed_value);
    std::cout << json::serialize(parsed_value) << std::endl;
    assert(parsed_to_object == const_object);

    const auto const_object_to_value = json::value_from(const_object);
    std::cout << json::serialize(const_object_to_value) << std::endl;
    assert(parsed_value == const_object_to_value);
}

void parse_transaction()
{
    const auto text_tx =
        "{"
            "\"version\": 42,"
            "\"inputs\": "
            "["
                "{ \"point\": "
                    "{"
                        "\"hash\": \"0000000000000000000000000000000000000000000000000000000000000000\","
                        "\"index\" : 24"
                    "},"
                    "\"script\": \"return\","
                    "\"witness\": \"[242424]\","
                    "\"sequence\": 42"
                "},"
                "{ \"point\": "
                    "{"
                        "\"hash\": \"0000000000000000000000000000000000000000000000000000000000000001\","
                        "\"index\" : 42"
                    "},"
                    "\"script\": \"return\","
                    "\"witness\": \"[424242]\","
                    "\"sequence\": 24"
                "}"
            "],"
            "\"outputs\": "
            "["
                "{"
                    "\"value\": 24,"
                    "\"script\": \"return\""
                "},"
                "{"
                    "\"value\": 42,"
                    "\"script\": \"return\""
                "}"
            "],"
            "\"locktime\": 24"
        "}";

    const auto parsed_value = json::parse(text_tx);

    const chain::transaction const_object
    {
        42,
        chain::inputs
        {
            {
                chain::point{ null_hash, 24 },
                chain::script{ { opcode::op_return } },
                chain::witness{ "[242424]" },
                42
            },
            {
                chain::point{ one_hash, 42 },
                chain::script{ { opcode::op_return } },
                chain::witness{ "[424242]" },
                24
            }
        },
        chain::outputs
        {
            {
                24,
                chain::script{ { opcode::op_return } }
            },
            {
                42,
                chain::script{ { opcode::op_return } }
            }
        },
        24
    };

    const auto parsed_to_object = json::value_to<chain::transaction>(parsed_value);
    std::cout << json::serialize(parsed_value) << std::endl;
    assert(parsed_to_object == const_object);

    const auto const_object_to_value = json::value_from(const_object);
    std::cout << json::serialize(const_object_to_value) << std::endl;
    assert(parsed_value == const_object_to_value);
}

} // chain
} // system
} // libbitcoin

int bc::system::main(int argc, char* argv[])
{
    bc::system::chain::parse_point();
    bc::system::chain::parse_input();
    bc::system::chain::parse_output();
    bc::system::chain::parse_transaction();
    
    ////const std::string test_string
    ////{
    ////    "{"
    ////        "\"pi\": 3.141,"
    ////        "\"happy\": true,"
    ////        "\"name\": \"Boost\","
    ////        "\"nothing\": null,"
    ////        "\"answer\": {\"everything\": 42},"
    ////        "\"list\": [1, 0, 2],"
    ////        "\"object\": {\"currency\": \"USD\", \"value\": 42.99}"
    ////    "}"
    ////};
    ////
    ////const auto text = json::string_view(test_string);
    ////
    ////json::error_code ec;
    ////null_parser validate;
    ////auto consumed = validate.write_some(text, ec);
    ////
    ////if (ec || consumed < text.size())
    ////{
    ////    std::cerr << "Invalid <json> null_parser" << std::endl;
    ////    return EXIT_FAILURE;
    ////}
    ////
    ////json::stream_parser parse;
    ////consumed = parse.write_some(text, ec);
    ////
    ////if (ec || consumed < text.size())
    ////{
    ////    std::cerr << "Invalid <json> stream_parser" << std::endl;
    ////    return EXIT_FAILURE;
    ////}
    ////
    ////const auto value = parse.release();
    ////
    ////std::cout << std::endl;
    ////pretty_print(std::cout, value, "");
    ////
    ////json::value test_value
    ////{
    ////    { "pi", 3.141 },
    ////    { "happy", true },
    ////    { "name", "Boost" },
    ////    { "nothing", nullptr },
    ////    { "answer", { { "everything", 42 } } },
    ////    { "list", { 1, 0, 2 } },
    ////    { "object", { { "currency", "USD" }, { "value", 42.99 } } }
    ////};
    ////
    ////std::cout << std::endl;
    ////pretty_print(std::cout, test_value, "");
    ////
    ////json::object test_object;
    ////test_object["pi"] = 3.141;
    ////test_object["happy"] = true;
    ////test_object["name"] = "Boost";
    ////test_object["nothing"] = nullptr;
    ////test_object["answer"].emplace_object()["everything"] = 42;
    ////test_object["list"] = { 1, 0, 2 };
    ////test_object["object"] = { { "currency", "USD" }, { "value", 42.99 } };
    ////
    ////std::cout << std::endl;
    ////pretty_print(std::cout, test_object, "");
    ////
    ////unsigned char buffer[4096];
    ////json::static_resource memory_resource(buffer);
    ////json::parse_options options;
    ////json::value value = json::parse("[1, 2, 3]", &memory_resource, options);
    ////pretty_print(std::cout, value, "");

    return EXIT_SUCCESS;
}
