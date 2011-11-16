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

bitcoin_storage_includedir = $(includedir)/bitcoin/storage
bitcoin_storage_include_HEADERS = \
	storage/storage.hpp

if DO_BERKDB
bitcoin_storage_include_HEADERS += \
	storage/bdb_detail.hpp \
	storage/bdb_storage.hpp
endif

if DO_POSTGRES
bitcoin_storage_include_HEADERS += \
	storage/postgresql_storage.hpp
endif

bitcoin_network_includedir = $(includedir)/bitcoin/network
bitcoin_network_include_HEADERS = \
	network/network.hpp \
	network/types.hpp

bitcoin_util_includedir = $(includedir)/bitcoin/util
bitcoin_util_include_HEADERS = \
	util/elliptic_curve_key.hpp \
	util/sha256.hpp \
	util/postbind.hpp \
	util/threaded_service.hpp \
	util/serializer.hpp \
	util/assert.hpp \
	util/big_number.hpp \
	util/clock.hpp \
	util/base58.hpp \
	util/ripemd.hpp \
	util/logger.hpp
