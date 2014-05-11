bitcoin_includedir = $(includedir)/bitcoin
bitcoin_include_HEADERS = \
	address.hpp \
	bitcoin.hpp \
	block.hpp \
	constants.hpp \
	define.hpp \
	error.hpp \
	format.hpp \
	getx_responder.hpp \
	poller.hpp \
	primitives.hpp \
	satoshi_serialize.hpp \
	script.hpp \
	session.hpp \
	stealth.hpp \
	threadpool.hpp \
	transaction.hpp \
	transaction_indexer.hpp \
	transaction_pool.hpp \
	types.hpp \
	validate.hpp \
	version.hpp

bitcoin_impl_includedir = $(includedir)/bitcoin/impl/
bitcoin_impl_include_HEADERS = \
	impl/format.ipp

bitcoin_impl_serialize_includedir = $(includedir)/bitcoin/impl/serialize
bitcoin_impl_serialize_include_HEADERS = \
	impl/serialize/block.ipp \
	impl/serialize/misc.ipp

bitcoin_blockchain_includedir = $(includedir)/bitcoin/blockchain
bitcoin_blockchain_include_HEADERS = \
	blockchain/blockchain.hpp \
	blockchain/organizer.hpp

bitcoin_blockchain_database_includedir = $(includedir)/bitcoin/blockchain/database
bitcoin_blockchain_database_include_HEADERS = \
	blockchain/database/stealth_database.hpp

if DO_LEVELDB
bitcoin_blockchain_include_HEADERS += \
	blockchain/leveldb_blockchain.hpp
endif

bitcoin_network_includedir = $(includedir)/bitcoin/network
bitcoin_network_include_HEADERS = \
	network/channel.hpp \
	network/handshake.hpp \
	network/hosts.hpp \
	network/network.hpp \
	network/protocol.hpp \
	network/shared_const_buffer.hpp

bitcoin_util_includedir = $(includedir)/bitcoin/utility
bitcoin_util_include_HEADERS = \
	utility/assert.hpp \
	utility/async_parallel.hpp \
	utility/base58.hpp \
	utility/big_number.hpp \
	utility/script_number.hpp \
	utility/checksum.hpp \
	utility/ec_keys.hpp \
	utility/elliptic_curve_key.hpp \
	utility/hash.hpp \
	utility/logger.hpp \
	utility/mmfile.hpp \
	utility/serializer.hpp \
	utility/subscriber.hpp \
	utility/timed_section.hpp \
	utility/weak_bind.hpp

bitcoin_util_external_includedir = $(includedir)/bitcoin/utility/external/
bitcoin_util_external_include_HEADERS = \
    utility/external/hmac_sha512.h \
    utility/external/ripemd160.h \
    utility/external/sha1.h \
    utility/external/sha256.h \
    utility/external/sha512.h \
    utility/external/zeroize.h

bitcoin_impl_utility_includedir = $(includedir)/bitcoin/impl/utility
bitcoin_impl_utility_include_HEADERS = \
    impl/utility/serializer.ipp

