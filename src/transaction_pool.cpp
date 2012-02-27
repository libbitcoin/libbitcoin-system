#include <bitcoin/transaction_pool.hpp>

#include <bitcoin/error.hpp>
#include <bitcoin/exporter.hpp>
#include <bitcoin/transaction.hpp>
#include <bitcoin/validate.hpp>
#include <bitcoin/utility/assert.hpp>

namespace libbitcoin {

using std::placeholders::_1;

transaction_pool_ptr transaction_pool::create(
    async_service& service, blockchain_ptr chain)
{
    // make_shared cannot use private constructors
    transaction_pool_ptr tx_pool(new transaction_pool(service));
    tx_pool->initialize(chain);
    return tx_pool;
}

transaction_pool::transaction_pool(async_service& service)
  : strand_(service.get_service()), pool_(100)
{
}
void transaction_pool::initialize(blockchain_ptr chain)
{
    chain_ = chain;
}

void transaction_pool::store(const message::transaction& stored_transaction,
    store_handler handle_store)
{
    strand_.post(
        std::bind(&transaction_pool::do_store, shared_from_this(),
            stored_transaction, handle_store));
}
void transaction_pool::do_store(
    const message::transaction& stored_transaction,
    store_handler handle_store)
{
    exporter_ptr saver = std::make_shared<satoshi_exporter>();
    validate_transaction_ptr validate =
        std::make_shared<validate_transaction>(chain_, saver,
            stored_transaction, pool_, strand_);
    validate->start(strand_.wrap(std::bind(
        &transaction_pool::handle_delegate,
            shared_from_this(), _1, stored_transaction, handle_store)));
}

void transaction_pool::handle_delegate(const std::error_code& ec,
    const message::transaction& tx, store_handler handle_store)
{
    if (!ec)
        pool_.push_back({hash_transaction(tx), tx});
    handle_store(ec);
}

} // libbitcoin

