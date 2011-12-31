lib_LTLIBRARIES = libbitcoin.la
AM_CPPFLAGS = -I$(srcdir)/../include
AM_CXXFLAGS = ${my_CXXFLAGS}
libbitcoin_la_SOURCES = \
	network/network.cpp  \
	network/handshake.cpp  \
	network/channel.cpp \
	exporter.cpp  \
	utility/serializer.cpp \
	utility/logger.cpp \
	kernel.cpp \
	utility/sha256.cpp \
	address.cpp \
	data_helpers.cpp \
	script.cpp \
	utility/ripemd.cpp \
	block.cpp \
    utility/elliptic_curve_key.cpp \
	transaction.cpp \
	error.cpp \
	utility/threaded_service.cpp \
    validate.cpp \
	utility/base58.cpp \
	utility/big_number.cpp \
	utility/clock.cpp \
	constants.cpp \
	blockchain/organizer.cpp

if DO_BDB
libbitcoin_la_SOURCES += \
	blockchain/bdb/data_type.cpp \
	blockchain/bdb/txn_guard.cpp \
	blockchain/bdb/bdb_blockchain.cpp \
	blockchain/bdb/bitcoin.pb.cc \
	blockchain/bdb/bdb_chain_keeper.cpp \
	blockchain/bdb/bdb_organizer.cpp \
	blockchain/bdb/bdb_validate_block.cpp \
	blockchain/bdb/bdb_common.cpp \
	blockchain/bdb/protobuf_wrapper.cpp
endif

if DO_POSTGRES
libbitcoin_la_SOURCES += \
	blockchain/postgresql/pq_blockchain.cpp \
	blockchain/postgresql/postgresql_blockchain.cpp
endif
