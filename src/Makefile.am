AUTOMAKE_OPTIONS = subdir-objects

lib_LTLIBRARIES = libbitcoin.la
AM_CPPFLAGS = -I$(srcdir)/../include
libbitcoin_la_SOURCES = \
    satoshi_serialize.cpp \
	getx_responder.cpp \
	threadpool.cpp \
	network/network.cpp  \
	network/handshake.cpp  \
	network/channel.cpp \
	network/hosts.cpp \
	network/protocol.cpp \
	poller.cpp \
	utility/logger.cpp \
	address.cpp \
	format.cpp \
	script.cpp \
	block.cpp \
    utility/elliptic_curve_key.cpp \
	transaction.cpp \
	stealth.cpp \
	error.cpp \
    validate.cpp \
	session.cpp \
	utility/base58.cpp \
	utility/big_number.cpp \
	utility/mmfile.cpp \
	utility/timed_section.cpp \
	constants.cpp \
	blockchain/organizer.cpp \
	blockchain/blockchain.cpp \
    blockchain/database/stealth_database.cpp \
	transaction_pool.cpp \
	transaction_indexer.cpp \
	utility/hash.cpp \
    utility/external/hmac_sha512.c \
    utility/external/ripemd160.c \
    utility/external/sha1.c \
    utility/external/sha256.c \
    utility/external/sha512.c \
    utility/external/zeroize.c

if DO_LEVELDB
libbitcoin_la_SOURCES += \
	blockchain/leveldb/leveldb_blockchain.cpp \
	blockchain/leveldb/leveldb_chain_keeper.cpp \
	blockchain/leveldb/leveldb_organizer.cpp \
	blockchain/leveldb/leveldb_validate_block.cpp \
	blockchain/leveldb/leveldb_common.cpp
endif

libbitcoin_la_LIBADD = \
    -lboost_thread -lboost_system -lboost_regex -lboost_filesystem -lpthread \
    -lcurl @LDFLAG_LEVELDB@ -lcrypto -ldl -lz

