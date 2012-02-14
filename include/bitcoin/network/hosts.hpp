#ifndef LIBBITCOIN_HOSTS_H
#define LIBBITCOIN_HOSTS_H

#include <string>
#include <fstream>
#include <functional>

#include <boost/circular_buffer.hpp>

#include <bitcoin/utility/threads.hpp>
#include <bitcoin/messages.hpp>

namespace libbitcoin {

class hosts
  : public std::enable_shared_from_this<hosts>,
    public threaded_service
{
public:
    typedef std::function<void (const std::error_code&)> load_handler;

    typedef std::function<void (const std::error_code&)> save_handler;

    typedef std::function<void (const std::error_code&)> store_handler;

    typedef std::function<
        void (const std::error_code&, const message::network_address&)>
            fetch_address_handler;

    typedef std::function<void (const std::error_code&, size_t)>
        fetch_count_handler;

    hosts(size_t capacity=26000);

    void load(const std::string& filename, load_handler handle_load);
    void save(const std::string& filename, save_handler handle_save);

    void store(const message::network_address address,
        store_handler handle_store);
    void fetch_address(fetch_address_handler handle_fetch);
    void fetch_count(fetch_count_handler handle_fetch);

private:
    struct hosts_field
    {
        message::ip_address ip;
        uint16_t port;
    };

    void do_load(const std::string& filename, load_handler handle_load);
    void do_save(const std::string& filename, save_handler handle_save);

    void do_store(const message::network_address address,
        store_handler handle_store);
    void do_fetch_address(fetch_address_handler handle_fetch_address);
    void do_fetch_count(fetch_count_handler handle_fetch);

    boost::circular_buffer<hosts_field> buffer_;
};

} // libbitcoin

#endif

