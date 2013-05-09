#include <bitcoin/network/hosts.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <bitcoin/format.hpp>
#include <bitcoin/error.hpp>

namespace libbitcoin {

hosts::hosts(threadpool& pool, size_t capacity)
  : async_strand(pool), buffer_(capacity)
{
    srand(time(nullptr));
}

void hosts::load(const std::string& filename, load_handler handle_load)
{
    queue(
        std::bind(&hosts::do_load,
            this, filename, handle_load));
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
        queue(
            [this, field]()
            {
                buffer_.push_back(field);
            });
    }
    handle_load(std::error_code());
}

void hosts::save(const std::string& filename, save_handler handle_save)
{
    queue(
        std::bind(&hosts::do_save,
            this, filename, handle_save));
}
void hosts::do_save(const std::string& filename, save_handler handle_save)
{
    std::ofstream file_handle(filename);
    for (const hosts_field& field: buffer_)
        file_handle << pretty_hex(field.ip) << ' '
            << field.port << std::endl;
    handle_save(std::error_code());
}

void hosts::store(const network_address_type& address,
    store_handler handle_store)
{
    queue(
        [this, address, handle_store]()
        {
            buffer_.push_back(hosts_field{address.ip, address.port});
            handle_store(std::error_code());
        });
}

void hosts::remove(const network_address_type& address,
    remove_handler handle_remove)
{
    queue(
        std::bind(&hosts::do_remove,
            this, address, handle_remove));
}
bool hosts::hosts_field::operator==(const hosts_field& other)
{
    return ip == other.ip && port == other.port;
}
void hosts::do_remove(const network_address_type& address,
    remove_handler handle_remove)
{
    auto it = std::find(buffer_.begin(), buffer_.end(),
        hosts_field{address.ip, address.port});
    if (it == buffer_.end())
    {
        handle_remove(error::not_found);
        return;
    }
    buffer_.erase(it);
    handle_remove(std::error_code());
}

void hosts::fetch_address(fetch_address_handler handle_fetch)
{
    queue(
        std::bind(&hosts::do_fetch_address,
            this, handle_fetch));
}
void hosts::do_fetch_address(fetch_address_handler handle_fetch)
{
    if (buffer_.empty())
    {
        handle_fetch(error::not_found, network_address_type());
        return;
    }
    size_t index = rand() % buffer_.size();
    network_address_type address;
    address.timestamp = 0;
    address.services = 0;
    address.ip = buffer_[index].ip;
    address.port = buffer_[index].port;
    handle_fetch(std::error_code(), address);
}

void hosts::fetch_count(fetch_count_handler handle_fetch)
{
    queue(
        std::bind(&hosts::do_fetch_count,
            this, handle_fetch));
}
void hosts::do_fetch_count(fetch_count_handler handle_fetch)
{
    handle_fetch(std::error_code(), buffer_.size());
}

} // namespace libbitcoin

