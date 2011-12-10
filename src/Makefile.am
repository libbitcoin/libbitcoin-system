lib_LTLIBRARIES = libbitcoin.la
AM_CPPFLAGS = -I$(srcdir)/../include
AM_CXXFLAGS = ${my_CXXFLAGS}
libbitcoin_la_SOURCES = \
	network/network.cpp  \
	network/handshake.cpp  \
	network/channel.cpp \
	dialect.cpp  \
	util/serializer.cpp \
	util/logger.cpp \
	kernel.cpp \
	util/sha256.cpp \
	address.cpp \
	types.cpp \
	script.cpp \
	util/ripemd.cpp \
	block.cpp \
    util/elliptic_curve_key.cpp \
	transaction.cpp \
	error.cpp \
	util/threaded_service.cpp \
    validate.cpp \
	util/base58.cpp \
	util/big_number.cpp \
	util/clock.cpp \
	constants.cpp

if DO_BERKDB
libbitcoin_la_SOURCES += \
	blockchain/bdb/bdb_blockchain.cpp \
	blockchain/bdb/bitcoin.pb.cc
endif

if DO_POSTGRES
libbitcoin_la_SOURCES += \
	blockchain/postgresql/pq_blockchain.cpp \
	blockchain/postgresql/postgresql_blockchain.cpp
endif
