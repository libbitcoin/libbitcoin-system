doc_DATA = README.md INSTALL COPYING doc/README.tutorial

#doc_sphinxdir = $(docdir)/sphinx/
#doc_sphinx_DATA = \
#    doc/sphinx/Makefile \
#    doc/sphinx/overview.rst \
#    doc/sphinx/network.rst \
#    doc/sphinx/make.bat \
#    doc/sphinx/crypto.rst \
#    doc/sphinx/fullnode.rst \
#    doc/sphinx/offlinetx.rst \
#    doc/sphinx/blockchain.rst \
#    doc/sphinx/index.rst \
#    doc/sphinx/quickstart.rst \
#    doc/sphinx/conf.py \
#    doc/sphinx/introduction.rst
#
#doc_sphinx_imgdir = $(docdir)/sphinx/img/
#doc_sphinx_img_DATA = doc/sphinx/img/operations.png
#
#doc_sphinx_examplesdir = $(docdir)/sphinx/examples/
#doc_sphinx_examples_DATA = \
#    doc/sphinx/examples/connect.rst \
#    doc/sphinx/examples/initchain.rst \
#    doc/sphinx/examples/priv.rst \
#    doc/sphinx/examples/determ.rst \
#    doc/sphinx/examples/accept.rst \
#    doc/sphinx/examples/txrad.rst \
#    doc/sphinx/examples/satoshiwords.rst \
#    doc/sphinx/examples/fullnode.rst \
#    doc/sphinx/examples/display-last.rst \
#    doc/sphinx/examples/proto.rst

pkgconfig_DATA = libbitcoin.pc

ACLOCAL_AMFLAGS = -I m4

#
# headers
#
bitcoin_includedir = $(includedir)/bitcoin

bitcoin_include_HEADERS = \
    include/bitcoin/bitcoin.hpp
    
bitcoin_bitcoin_includedir = $(includedir)/bitcoin/bitcoin
bitcoin_bitcoin_include_HEADERS = \
    include/bitcoin/bitcoin/address.hpp \
    include/bitcoin/bitcoin/block.hpp \
    include/bitcoin/bitcoin/blockchain.hpp \
    include/bitcoin/bitcoin/compat.h \
    include/bitcoin/bitcoin/compat.hpp \
    include/bitcoin/bitcoin/constants.hpp \
    include/bitcoin/bitcoin/define.hpp \
    include/bitcoin/bitcoin/error.hpp \
    include/bitcoin/bitcoin/format.hpp \
    include/bitcoin/bitcoin/getx_responder.hpp \
    include/bitcoin/bitcoin/poller.hpp \
    include/bitcoin/bitcoin/primitives.hpp \
    include/bitcoin/bitcoin/satoshi_serialize.hpp \
    include/bitcoin/bitcoin/script.hpp \
    include/bitcoin/bitcoin/session.hpp \
    include/bitcoin/bitcoin/stealth.hpp \
    include/bitcoin/bitcoin/threadpool.hpp \
    include/bitcoin/bitcoin/transaction.hpp \
    include/bitcoin/bitcoin/transaction_indexer.hpp \
    include/bitcoin/bitcoin/transaction_pool.hpp \
    include/bitcoin/bitcoin/types.hpp \
    include/bitcoin/bitcoin/validate.hpp \
    include/bitcoin/bitcoin/version.hpp

bitcoin_bitcoin_impl_includedir = $(includedir)/bitcoin/bitcoin/impl
bitcoin_bitcoin_impl_include_HEADERS = \
    include/bitcoin/bitcoin/impl/format.ipp

bitcoin_bitcoin_impl_serialize_includedir = $(includedir)/bitcoin/bitcoin/impl/serialize
bitcoin_bitcoin_impl_serialize_include_HEADERS = \
    include/bitcoin/bitcoin/impl/serialize/block.ipp \
    include/bitcoin/bitcoin/impl/serialize/misc.ipp

bitcoin_bitcoin_network_includedir = $(includedir)/bitcoin/bitcoin/network
bitcoin_bitcoin_network_include_HEADERS = \
    include/bitcoin/bitcoin/network/channel.hpp \
    include/bitcoin/bitcoin/network/handshake.hpp \
    include/bitcoin/bitcoin/network/hosts.hpp \
    include/bitcoin/bitcoin/network/network.hpp \
    include/bitcoin/bitcoin/network/protocol.hpp \
    include/bitcoin/bitcoin/network/shared_const_buffer.hpp

bitcoin_bitcoin_util_includedir = $(includedir)/bitcoin/bitcoin/utility
bitcoin_bitcoin_util_include_HEADERS = \
    include/bitcoin/bitcoin/utility/assert.hpp \
    include/bitcoin/bitcoin/utility/async_parallel.hpp \
    include/bitcoin/bitcoin/utility/base58.hpp \
    include/bitcoin/bitcoin/utility/checksum.hpp \
    include/bitcoin/bitcoin/utility/decorator.hpp \
    include/bitcoin/bitcoin/utility/ec_keys.hpp \
    include/bitcoin/bitcoin/utility/hash.hpp \
    include/bitcoin/bitcoin/utility/hash_number.hpp \
    include/bitcoin/bitcoin/utility/logger.hpp \
    include/bitcoin/bitcoin/utility/mmfile.hpp \
    include/bitcoin/bitcoin/utility/script_number.hpp \
    include/bitcoin/bitcoin/utility/serializer.hpp \
    include/bitcoin/bitcoin/utility/subscriber.hpp \
    include/bitcoin/bitcoin/utility/timed_section.hpp \
    include/bitcoin/bitcoin/utility/weak_bind.hpp

bitcoin_bitcoin_util_external_includedir = $(includedir)/bitcoin/bitcoin/utility/external
bitcoin_bitcoin_util_external_include_HEADERS = \
    include/bitcoin/bitcoin/utility/external/hmac_sha512.h \
    include/bitcoin/bitcoin/utility/external/ripemd160.h \
    include/bitcoin/bitcoin/utility/external/sha1.h \
    include/bitcoin/bitcoin/utility/external/sha256.h \
    include/bitcoin/bitcoin/utility/external/sha512.h \
    include/bitcoin/bitcoin/utility/external/zeroize.h \
    include/bitcoin/bitcoin/utility/external/uint256.h

bitcoin_bitcoin_impl_utility_includedir = $(includedir)/bitcoin/bitcoin/impl/utility
bitcoin_bitcoin_impl_utility_include_HEADERS = \
    include/bitcoin/bitcoin/impl/utility/serializer.ipp

bitcoin_bitcoin_wallet_includedir = $(includedir)/bitcoin/bitcoin/wallet
bitcoin_bitcoin_wallet_include_HEADERS = \
    include/bitcoin/bitcoin/wallet/amount.hpp \
    include/bitcoin/bitcoin/wallet/hd_keys.hpp \
    include/bitcoin/bitcoin/wallet/key_formats.hpp \
    include/bitcoin/bitcoin/wallet/mnemonic.hpp \
    include/bitcoin/bitcoin/wallet/stealth_address.hpp \
    include/bitcoin/bitcoin/wallet/uri.hpp

#
# binaries
#
lib_LTLIBRARIES = libbitcoin.la

libbitcoin_la_CPPFLAGS = \
    -I$(srcdir)/include \
    ${secp256k1_CFLAGS}

libbitcoin_la_SOURCES = \
    src/address.cpp \
    src/blockchain.cpp \
    src/block.cpp \
    src/compat.cpp \
    src/constants.cpp \
    src/error.cpp \
    src/format.cpp \
    src/getx_responder.cpp \
    src/network/channel.cpp \
    src/network/handshake.cpp  \
    src/network/hosts.cpp \
    src/network/network.cpp  \
    src/network/protocol.cpp \
    src/poller.cpp \
    src/satoshi_serialize.cpp \
    src/script.cpp \
    src/session.cpp \
    src/stealth.cpp \
    src/threadpool.cpp \
    src/transaction.cpp \
    src/transaction_indexer.cpp \
    src/transaction_pool.cpp \
    src/utility/base58.cpp \
    src/utility/checksum.cpp \
    src/utility/ec_keys.cpp \
    src/utility/hash.cpp \
    src/utility/hash_number.cpp \
    src/utility/logger.cpp \
    src/utility/mmfile.cpp \
    src/utility/script_number.cpp \
    src/utility/timed_section.cpp \
    src/utility/external/hmac_sha512.c \
    src/utility/external/ripemd160.c \
    src/utility/external/sha1.c \
    src/utility/external/sha256.c \
    src/utility/external/sha512.c \
    src/utility/external/zeroize.c \
    src/validate.cpp \
    src/wallet/amount.cpp \
    src/wallet/hd_keys.cpp \
    src/wallet/key_formats.cpp \
    src/wallet/mnemonic.cpp \
    src/wallet/stealth_address.cpp \
    src/wallet/uri.cpp

libbitcoin_la_LIBADD = \
    -lboost_system \
    -lboost_regex \
    -lboost_filesystem \
    -lpthread \
    ${secp256k1_LIBS}

#
# tests
#
TESTS = libbitcoin_test

check_PROGRAMS = libbitcoin_test

libbitcoin_test_CPPFLAGS = \
    -I$(srcdir)/include \
    -I$(srcdir)/test \
    ${crypto_CFLAGS} \
    ${secp256k1_CFLAGS} \
    -DBOOST_TEST_DYN_LINK

libbitcoin_test_SOURCES = \
    test/amount.cpp \
    test/base58.cpp \
    test/big_number.cpp \
    test/checksum.cpp \
    test/ec_keys.cpp \
    test/format.cpp \
    test/hash_number.cpp \
    test/hash.cpp \
    test/hd_keys.cpp \
    test/key_formats.cpp \
    test/main.cpp \
    test/script_number.cpp \
    test/script.cpp \
    test/serialize.cpp \
    test/stealth_address.cpp \
    test/stealth.cpp \
    test/uri.cpp

libbitcoin_test_LDADD = \
    libbitcoin.la \
    -lrt \
    -lboost_system \
    -lboost_unit_test_framework \
    ${crypto_LIBS} \
    ${secp256k1_LIBS}

