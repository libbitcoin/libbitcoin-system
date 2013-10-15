AUTOMAKE_OPTIONS = subdir-objects

lib_LTLIBRARIES = libbitcoin.la
AM_CPPFLAGS = -I$(srcdir)/../include
AM_CXXFLAGS = ${my_CXXFLAGS}
libbitcoin_la_SOURCES = \
    satoshi_serialize.cpp \
    deterministic_wallet.cpp \
	getx_responder.cpp \
	threadpool.cpp \
	network/network.cpp  \
	network/handshake.cpp  \
	network/channel.cpp \
	network/hosts.cpp \
	network/protocol.cpp \
	poller.cpp \
	utility/logger.cpp \
	utility/sha256.cpp \
	address.cpp \
	format.cpp \
	script.cpp \
	utility/ripemd.cpp \
	block.cpp \
    utility/elliptic_curve_key.cpp \
	transaction.cpp \
	error.cpp \
    validate.cpp \
	session.cpp \
	utility/base58.cpp \
	utility/big_number.cpp \
	utility/key_formats.cpp \
	utility/timed_section.cpp \
	utility/mnemonic.cpp \
	constants.cpp \
	blockchain/organizer.cpp \
	blockchain/blockchain.cpp \
	transaction_pool.cpp \
	transaction_indexer.cpp

if DO_LEVELDB
libbitcoin_la_SOURCES += \
	blockchain/leveldb/leveldb_blockchain.cpp \
	blockchain/leveldb/leveldb_chain_keeper.cpp \
	blockchain/leveldb/leveldb_organizer.cpp \
	blockchain/leveldb/leveldb_validate_block.cpp \
	blockchain/leveldb/leveldb_common.cpp
endif

