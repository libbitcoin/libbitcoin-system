bitcoin_includedir = $(includedir)/bitcoin
bitcoin_include_HEADERS = \
	error.hpp \
	validate.hpp \
	messages.hpp \
	script.hpp \
	exporter.hpp \
	address.hpp \
	transaction.hpp \
	constants.hpp \
	block.hpp \
	bitcoin.hpp \
	types.hpp \
	session.hpp \
	format.hpp \
	transaction_pool.hpp \
	async_service.hpp \
	poller.hpp

bitcoin_blockchain_includedir = $(includedir)/bitcoin/blockchain
bitcoin_blockchain_include_HEADERS = \
	blockchain/blockchain.hpp \
	blockchain/organizer.hpp

if DO_BDB
bitcoin_blockchain_include_HEADERS += \
	blockchain/bdb_blockchain.hpp
endif

if DO_POSTGRES
#bitcoin_blockchain_include_HEADERS += \
#	blockchain/postgresql_blockchain.hpp
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
	utility/clock.hpp \
	utility/base58.hpp \
	utility/ripemd.hpp \
	utility/subscriber.hpp \
	utility/logger.hpp \
	utility/coroutine.hpp \
	utility/yield.hpp

