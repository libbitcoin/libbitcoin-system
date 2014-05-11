AUTOMAKE_OPTIONS = subdir-objects

lib_LTLIBRARIES = libbitcoin.la
AM_CPPFLAGS = -I$(srcdir)/../include
libbitcoin_la_SOURCES = \
	address.cpp \
	blockchain/blockchain.cpp \
	blockchain/database/stealth_database.cpp \
	blockchain/organizer.cpp \
	block.cpp \
	constants.cpp \
	error.cpp \
	format.cpp \
	getx_responder.cpp \
	network/channel.cpp \
	network/handshake.cpp  \
	network/hosts.cpp \
	network/network.cpp  \
	network/protocol.cpp \
	poller.cpp \
	satoshi_serialize.cpp \
	script.cpp \
	session.cpp \
	stealth.cpp \
	threadpool.cpp \
	transaction.cpp \
	transaction_indexer.cpp \
	transaction_pool.cpp \
	utility/base58.cpp \
	utility/big_number.cpp \
	utility/script_number.cpp \
	utility/checksum.cpp \
	utility/ec_keys.cpp \
	utility/elliptic_curve_key.cpp \
	utility/external/hmac_sha512.c \
	utility/external/ripemd160.c \
	utility/external/sha1.c \
	utility/external/sha256.c \
	utility/external/sha512.c \
	utility/external/zeroize.c \
	utility/hash.cpp \
	utility/logger.cpp \
	utility/mmfile.cpp \
	utility/timed_section.cpp \
	validate.cpp

if DO_LEVELDB
libbitcoin_la_SOURCES += \
	blockchain/leveldb/leveldb_blockchain.cpp \
	blockchain/leveldb/leveldb_chain_keeper.cpp \
	blockchain/leveldb/leveldb_common.cpp \
	blockchain/leveldb/leveldb_organizer.cpp \
	blockchain/leveldb/leveldb_validate_block.cpp
endif

libbitcoin_la_LIBADD = \
    -lboost_thread -lboost_system -lboost_regex -lboost_filesystem -lpthread \
    -lcurl -lsecp256k1 @LDFLAG_LEVELDB@ -lcrypto -ldl -lz

