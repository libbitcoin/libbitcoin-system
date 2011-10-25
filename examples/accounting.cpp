#include <iostream>

#include <bitcoin/types.hpp>
#include <bitcoin/kernel.hpp>
#include <bitcoin/storage/postgresql_storage.hpp>
#include <bitcoin/util/logger.hpp>

using namespace libbitcoin;

void display_balance(const std::error_code& ec, uint64_t value)
{
    if (ec)
    {
        log_fatal() << ec.message();
        return;
    }
    log_info() << "Balance: " << value;
}

int main()
{
    kernel_ptr app(new kernel());
    storage_ptr app_storage(new postgresql_storage(
        app, "bitcoin", "genjix", ""));
    app->register_storage(app_storage);

    data_chunk address = 
        bytes_from_pretty("12 ab 8d c5 88 ca 9d 57 87 dd "
                          "e7 eb 29 56 9d a6 3c 3a 23 8c");
    app_storage->fetch_balance(address, display_balance);

    std::cin.get();
    return 0;
}

