#ifndef LIBBITCOIN_GETX_RESPONDER_H
#define LIBBITCOIN_GETX_RESPONDER_H

#include <bitcoin/types.hpp>
#include <bitcoin/async_service.hpp>
#include <bitcoin/messages.hpp>

namespace libbitcoin {

class getx_responder
  : public std::enable_shared_from_this<getx_responder>
{
public:
    getx_responder(async_service& service,
        blockchain_ptr chain, transaction_pool_ptr txpool);
    void monitor(channel_ptr node);

private:
    void receive_get_data(const std::error_code& ec,
        const message::get_data packet, channel_ptr node);

    void pool_tx(const std::error_code& ec, const message::transaction& tx,
        const hash_digest& tx_hash, channel_ptr node);
    void chain_tx(const std::error_code& ec,
        const message::transaction& tx, channel_ptr node);

    io_service& service_;
    blockchain_ptr chain_;
    transaction_pool_ptr txpool_;
};

} // libbitcoin

#endif

