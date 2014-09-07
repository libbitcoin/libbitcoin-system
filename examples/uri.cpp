/*
 * Copyright (c) 2011-2013 libwallet developers (see AUTHORS)
 *
 * This file is part of libwallet.
 *
 * libwallet is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
/*
  Demonstration of URI utilities.
*/
#include <iostream>
#include <string>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

/**
 * Example class to demonstrate handling custom URI parameters.
 */
struct custom_result
    : public uri_parse_result
{
    optional_string myparam;

protected:
    virtual bool got_param(std::string& key, std::string& value)
    {
        if ("myparam" == key)
            myparam.reset(value);
        return uri_parse_result::got_param(key, value);
    }
};

int main()
{
    // Write a URI:
    uri_writer writer;
    writer.write_address(std::string("113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD"));
    writer.write_amount(120000);
    writer.write_amount(10000000000);
    writer.write_label("&=\n");
    writer.write_message("hello bitcoin");
    writer.write_param("myparam", "result");
    writer.write_r("http://example.com?purchase=shoes&user=bob");

    std::cout << writer.string() << std::endl;

    // Parse the URI back out, using our result parameter handler:
    custom_result result;
    bool success = uri_parse(writer.string(), result);
    if (!success)
        return 1;

    // Display the results:
    if (result.address)
        std::cout << "address: " << result.address.get().encoded() << std::endl;
    if (result.amount)
        std::cout << "amount: " << result.amount.get() << std::endl;
    if (result.label)
        std::cout << "label: " << result.label.get() << std::endl;
    if (result.message)
        std::cout << "message: " << result.message.get() << std::endl;
    if (result.r)
        std::cout << "r: " << result.r.get() << std::endl;
    if (result.myparam)
        std::cout << "myparam: " << result.myparam.get() << std::endl;

    return 0;
}

