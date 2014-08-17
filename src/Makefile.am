AUTOMAKE_OPTIONS = subdir-objects

lib_LTLIBRARIES = libbitcoin.la
AM_CPPFLAGS = -I$(srcdir)/../include ${libsecp256k1_CFLAGS}
libbitcoin_la_SOURCES = \
	address.cpp \
	blockchain/blockchain.cpp \
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
	utility/checksum.cpp \
	utility/ec_keys.cpp \
	utility/hash.cpp \
	utility/hash_number.cpp \
	utility/logger.cpp \
	utility/mmfile.cpp \
	utility/script_number.cpp \
	utility/timed_section.cpp \
	utility/external/hmac_sha512.c \
	utility/external/ripemd160.c \
	utility/external/sha1.c \
	utility/external/sha256.c \
	utility/external/sha512.c \
	utility/external/zeroize.c \
	validate.cpp

libbitcoin_la_LIBADD = \
    -lboost_thread -lboost_system -lboost_regex -lboost_filesystem -lpthread \
    -lsecp256k1 -lcrypto -ldl -lz

