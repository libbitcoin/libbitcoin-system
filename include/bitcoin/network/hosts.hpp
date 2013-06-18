#ifndef LIBBITCOIN_HPPOSTS_HPP
#define LIBBITCOIN_HPPOSTS_HPP

#include <string>
#include <fstream>
#include <functional>
#include <system_error>

#include <boost/circular_buffer.hpp>

#include <bitcoin/primitives.hpp>
#include <bitcoin/threadpool.hpp>

namespace libbitcoin {

class hosts
{
public:
    typedef std::function<void (const std::error_code&)> load_handler;
    typedef std::function<void (const std::error_code&)> save_handler;
    typedef std::function<void (const std::error_code&)> store_handler;
    typedef std::function<void (const std::error_code&)> remove_handler;

    typedef std::function<
        void (const std::error_code&, const network_address_type&)>
            fetch_address_handler;

    typedef std::function<void (const std::error_code&, size_t)>
        fetch_count_handler;

    hosts(threadpool& pool, size_t capacity=1000);

    hosts(const hosts&) = delete;
    void operator=(const hosts&) = delete;

    void load(const std::string& filename, load_handler handle_load);
    void save(const std::string& filename, save_handler handle_save);

    void store(const network_address_type& address,
        store_handler handle_store);
    void remove(const network_address_type& address,
        remove_handler handle_remove);
    void fetch_address(fetch_address_handler handle_fetch);
    void fetch_count(fetch_count_handler handle_fetch);

private:
    struct hosts_field
    {
        bool operator==(const hosts_field& other);
        ip_address_type ip;
        uint16_t port;
    };

    void do_load(const std::string& filename, load_handler handle_load);
    void do_save(const std::string& filename, save_handler handle_save);

    void do_remove(const network_address_type& address,
        remove_handler handle_remove);
    void do_fetch_address(fetch_address_handler handle_fetch_address);
    void do_fetch_count(fetch_count_handler handle_fetch);

    async_strand strand_;
    boost::circular_buffer<hosts_field> buffer_;
};

} // namespace libbitcoin

#endif

