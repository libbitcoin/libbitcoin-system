bitcoin_includedir = $(includedir)/bitcoin
bitcoin_include_HEADERS = \
    satoshi_serialize.hpp \
    deterministic_wallet.hpp \
	getx_responder.hpp \
	error.hpp \
	validate.hpp \
	primitives.hpp \
	script.hpp \
	address.hpp \
	transaction.hpp \
	constants.hpp \
	block.hpp \
	bitcoin.hpp \
	types.hpp \
	session.hpp \
	format.hpp \
	transaction_pool.hpp \
	threadpool.hpp \
	poller.hpp \
    version.hpp

bitcoin_blockchain_includedir = $(includedir)/bitcoin/blockchain
bitcoin_blockchain_include_HEADERS = \
	blockchain/blockchain.hpp \
	blockchain/organizer.hpp

if DO_LEVELDB
bitcoin_blockchain_include_HEADERS += \
	blockchain/leveldb_blockchain.hpp
endif

bitcoin_network_includedir = $(includedir)/bitcoin/network
bitcoin_network_include_HEADERS = \
	network/channel.hpp \
	network/network.hpp \
	network/handshake.hpp \
	network/shared_const_buffer.hpp \
	network/hosts.hpp \
	network/protocol.hpp

bitcoin_util_includedir = $(includedir)/bitcoin/utility
bitcoin_util_include_HEADERS = \
	utility/elliptic_curve_key.hpp \
	utility/sha256.hpp \
	utility/serializer.hpp \
	utility/assert.hpp \
	utility/big_number.hpp \
	utility/base58.hpp \
	utility/ripemd.hpp \
	utility/subscriber.hpp \
	utility/logger.hpp \
	utility/weak_bind.hpp \
	utility/key_formats.hpp

