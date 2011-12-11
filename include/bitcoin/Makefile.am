bitcoin_includedir = $(includedir)/bitcoin
bitcoin_include_HEADERS = \
	error.hpp \
	validate.hpp \
	messages.hpp \
	kernel.hpp \
	script.hpp \
	dialect.hpp \
	address.hpp \
	transaction.hpp \
	constants.hpp \
	block.hpp \
	bitcoin.hpp \
	types.hpp

bitcoin_blockchain_includedir = $(includedir)/bitcoin/blockchain
bitcoin_blockchain_include_HEADERS = \
	blockchain/blockchain.hpp

if DO_BERKDB
bitcoin_blockchain_include_HEADERS += \
	blockchain/bdb_detail.hpp \
	blockchain/bdb_blockchain.hpp
endif

if DO_POSTGRES
bitcoin_blockchain_include_HEADERS += \
	blockchain/postgresql_blockchain.hpp
endif

bitcoin_network_includedir = $(includedir)/bitcoin/network
bitcoin_network_include_HEADERS = \
	network/channel.hpp \
	network/network.hpp \
	network/shared_const_buffer.hpp \
	network/types.hpp

bitcoin_util_includedir = $(includedir)/bitcoin/util
bitcoin_util_include_HEADERS = \
	util/elliptic_curve_key.hpp \
	util/sha256.hpp \
	util/postbind.hpp \
	util/threads.hpp \
	util/serializer.hpp \
	util/assert.hpp \
	util/big_number.hpp \
	util/clock.hpp \
	util/base58.hpp \
	util/ripemd.hpp \
	util/logger.hpp
