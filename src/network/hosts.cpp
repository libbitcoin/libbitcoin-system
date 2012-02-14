#include <bitcoin/network/hosts.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <bitcoin/data_helpers.hpp>
#include <bitcoin/error.hpp>

namespace libbitcoin {

hosts::hosts()
  : buffer_(26000)
{
    srand(time(nullptr));
}

void hosts::load(const std::string& filename, load_handler handle_load)
{
    service()->post(
        std::bind(&hosts::do_load, shared_from_this(),
            filename, handle_load));
}
void hosts::do_load(const std::string& filename, load_handler handle_load)
{
    std::ifstream file_handle(filename);
    std::string line;
    while (std::getline(file_handle, line))
    {
        std::vector<std::string> parts;
        boost::split(parts, line, boost::is_any_of(" "));
        if (parts.size() != 2)
            continue;
        data_chunk raw_ip = bytes_from_pretty(parts[0]);
        hosts_field field;
        if (raw_ip.size() != field.ip.size())
            continue;
        std::copy(raw_ip.begin(), raw_ip.end(), field.ip.begin());
        field.port = boost::lexical_cast<uint16_t>(parts[1]);
        strand()->dispatch(
            [&buffer_, field]()
            {
                buffer_.push_back(field);
            });
    }
    handle_load(std::error_code());
}

void hosts::save(const std::string& filename, save_handler handle_save)
{
    strand()->post(
        std::bind(&hosts::do_save, shared_from_this(),
            filename, handle_save));
}
void hosts::do_save(const std::string& filename, save_handler handle_save)
{
    std::ofstream file_handle(filename);
    for (const hosts_field& field: buffer_)
        file_handle << pretty_hex(field.ip) << ' '
            << field.port << std::endl;
    handle_save(std::error_code());
}

void hosts::store(const message::network_address address,
    store_handler handle_store)
{
    strand()->post(
        [&buffer_, address]()
        {
            buffer_.push_back(hosts_field{address.ip, address.port});
        });
}

void hosts::fetch_address(fetch_address_handler handle_fetch)
{
    strand()->post(
        std::bind(&hosts::do_fetch_address, shared_from_this(),
            handle_fetch));
}
void hosts::do_fetch_address(fetch_address_handler handle_fetch)
{
    if (buffer_.empty())
    {
        handle_fetch(error::missing_object, message::network_address());
        return;
    }
    size_t index = rand() % buffer_.size();
    message::network_address address;
    address.timestamp = 0;
    address.services = 0;
    address.ip = buffer_[index].ip;
    address.port = buffer_[index].port;
    handle_fetch(std::error_code(), address);
}

void hosts::fetch_count(fetch_count_handler handle_fetch)
{
    strand()->post(
        std::bind(&hosts::do_fetch_count, shared_from_this(),
            handle_fetch));
}
void hosts::do_fetch_count(fetch_count_handler handle_fetch)
{
    handle_fetch(std::error_code(), buffer_.size());
}

} // libbitcoin

