#
# documentation
#
doc_DATA = README INSTALL COPYING

#doc_sphinxdir = ${docdir}/sphinx/
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
#doc_sphinx_imgdir = ${docdir}/sphinx/img/
#doc_sphinx_img_DATA = doc/sphinx/img/operations.png
#
#doc_sphinx_examplesdir = ${docdir}/sphinx/examples/
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

#
# pkg-config
#
pkgconfig_DATA = libbitcoin.pc

# look for macros in the m4 subdirectory
ACLOCAL_AMFLAGS = -I m4

#
# libraries
#
lib_LTLIBRARIES = src/libbitcoin.la

src_libbitcoin_la_CPPFLAGS = -I${srcdir}/include ${boost_CFLAGS} ${boost_date_time_CFLAGS} ${boost_filesystem_CFLAGS} ${boost_locale_CFLAGS} ${boost_program_options_CFLAGS} ${boost_regex_CFLAGS} ${boost_system_CFLAGS} ${pthread_CFLAGS} ${rt_CFLAGS} ${secp256k1_CFLAGS}

src_libbitcoin_la_LIBADD = ${boost_LIBS} ${boost_date_time_LIBS} ${boost_filesystem_LIBS} ${boost_locale_LIBS} ${boost_program_options_LIBS} ${boost_regex_LIBS} ${boost_system_LIBS} ${pthread_LIBS} ${rt_LIBS} ${secp256k1_LIBS}

bitcoin_includedir = ${includedir}/bitcoin
bitcoin_include_HEADERS = \
    include/bitcoin/bitcoin.hpp

bitcoin_bitcoin_includedir = ${includedir}/bitcoin/bitcoin
bitcoin_bitcoin_include_HEADERS = \
    include/bitcoin/bitcoin/block.hpp \
    include/bitcoin/bitcoin/compat.h \
    include/bitcoin/bitcoin/compat.hpp \
    include/bitcoin/bitcoin/constants.hpp \
    include/bitcoin/bitcoin/define.hpp \
    include/bitcoin/bitcoin/error.hpp \
    include/bitcoin/bitcoin/primitives.hpp \
    include/bitcoin/bitcoin/satoshi_serialize.hpp \
    include/bitcoin/bitcoin/script.hpp \
    include/bitcoin/bitcoin/stealth.hpp \
    include/bitcoin/bitcoin/transaction.hpp \
    include/bitcoin/bitcoin/version.hpp

bitcoin_bitcoin_impl_serialize_includedir = ${includedir}/bitcoin/bitcoin/impl/serialize
bitcoin_bitcoin_impl_serialize_include_HEADERS = \
    include/bitcoin/bitcoin/impl/serialize/block.ipp \
    include/bitcoin/bitcoin/impl/serialize/misc.ipp

bitcoin_bitcoin_config_includedir = ${includedir}/bitcoin/bitcoin/config
bitcoin_bitcoin_config_include_HEADERS = \
    include/bitcoin/bitcoin/config/parameter.hpp \
    include/bitcoin/bitcoin/config/printer.hpp

bitcoin_bitcoin_formats_includedir = ${includedir}/bitcoin/bitcoin/formats
bitcoin_bitcoin_formats_include_HEADERS = \
    include/bitcoin/bitcoin/formats/base10.hpp \
    include/bitcoin/bitcoin/formats/base16.hpp \
    include/bitcoin/bitcoin/formats/base58.hpp \
    include/bitcoin/bitcoin/formats/base64.hpp \
    include/bitcoin/bitcoin/formats/base85.hpp

bitcoin_bitcoin_impl_formats_includedir = ${includedir}/bitcoin/bitcoin/impl/formats
bitcoin_bitcoin_impl_formats_include_HEADERS = \
    include/bitcoin/bitcoin/impl/formats/base16.ipp

bitcoin_bitcoin_math_includedir = ${includedir}/bitcoin/bitcoin/math
bitcoin_bitcoin_math_include_HEADERS = \
    include/bitcoin/bitcoin/math/checksum.hpp \
    include/bitcoin/bitcoin/math/ec_keys.hpp \
    include/bitcoin/bitcoin/math/hash.hpp \
    include/bitcoin/bitcoin/math/hash_number.hpp \
    include/bitcoin/bitcoin/math/script_number.hpp

bitcoin_bitcoin_math_external_includedir = ${includedir}/bitcoin/bitcoin/math/external
bitcoin_bitcoin_math_external_include_HEADERS = \
    include/bitcoin/bitcoin/math/external/hmac_sha256.h \
    include/bitcoin/bitcoin/math/external/hmac_sha512.h \
    include/bitcoin/bitcoin/math/external/ripemd160.h \
    include/bitcoin/bitcoin/math/external/sha1.h \
    include/bitcoin/bitcoin/math/external/sha256.h \
    include/bitcoin/bitcoin/math/external/sha512.h \
    include/bitcoin/bitcoin/math/external/uint256.h \
    include/bitcoin/bitcoin/math/external/zeroize.h \
    include/bitcoin/bitcoin/math/external/pkcs5_pbkdf2.h

bitcoin_bitcoin_network_includedir = ${includedir}/bitcoin/bitcoin/network
bitcoin_bitcoin_network_include_HEADERS = \
    include/bitcoin/bitcoin/network/channel.hpp \
    include/bitcoin/bitcoin/network/handshake.hpp \
    include/bitcoin/bitcoin/network/hosts.hpp \
    include/bitcoin/bitcoin/network/network.hpp \
    include/bitcoin/bitcoin/network/protocol.hpp \
    include/bitcoin/bitcoin/network/shared_const_buffer.hpp

bitcoin_bitcoin_trie_includedir = ${includedir}/bitcoin/bitcoin/trie
bitcoin_bitcoin_trie_include_HEADERS = \
    include/bitcoin/bitcoin/trie/binary_trie.hpp \
    include/bitcoin/bitcoin/trie/trie_iterator.hpp \
    include/bitcoin/bitcoin/trie/trie_reverse_iterator.hpp \
    include/bitcoin/bitcoin/trie/trie_structure_node.hpp \
    include/bitcoin/bitcoin/trie/trie_value_node.hpp

bitcoin_bitcoin_impl_trie_includedir = ${includedir}/bitcoin/bitcoin/impl/trie
bitcoin_bitcoin_impl_trie_include_HEADERS = \
    include/bitcoin/bitcoin/impl/trie/binary_trie.ipp \
    include/bitcoin/bitcoin/impl/trie/trie_iterator.ipp \
    include/bitcoin/bitcoin/impl/trie/trie_reverse_iterator.ipp \
    include/bitcoin/bitcoin/impl/trie/trie_structure_node.ipp \
    include/bitcoin/bitcoin/impl/trie/trie_value_node.ipp

bitcoin_bitcoin_util_includedir = ${includedir}/bitcoin/bitcoin/utility
bitcoin_bitcoin_util_include_HEADERS = \
    include/bitcoin/bitcoin/utility/array_slice.hpp \
    include/bitcoin/bitcoin/utility/assert.hpp \
    include/bitcoin/bitcoin/utility/async_parallel.hpp \
    include/bitcoin/bitcoin/utility/binary.hpp \
    include/bitcoin/bitcoin/utility/data.hpp \
    include/bitcoin/bitcoin/utility/decorator.hpp \
    include/bitcoin/bitcoin/utility/endian.hpp \
    include/bitcoin/bitcoin/utility/general.hpp \
    include/bitcoin/bitcoin/utility/logger.hpp \
    include/bitcoin/bitcoin/utility/serializer.hpp \
    include/bitcoin/bitcoin/utility/subscriber.hpp \
    include/bitcoin/bitcoin/utility/threadpool.hpp \
    include/bitcoin/bitcoin/utility/timed_section.hpp \
    include/bitcoin/bitcoin/utility/weak_bind.hpp

bitcoin_bitcoin_impl_utility_includedir = ${includedir}/bitcoin/bitcoin/impl/utility
bitcoin_bitcoin_impl_utility_include_HEADERS = \
    include/bitcoin/bitcoin/impl/utility/array_slice.ipp \
    include/bitcoin/bitcoin/impl/utility/data.ipp \
    include/bitcoin/bitcoin/impl/utility/endian.ipp \
    include/bitcoin/bitcoin/impl/utility/general.ipp \
    include/bitcoin/bitcoin/impl/utility/serializer.ipp

bitcoin_bitcoin_wallet_includedir = ${includedir}/bitcoin/bitcoin/wallet
bitcoin_bitcoin_wallet_include_HEADERS = \
    include/bitcoin/bitcoin/wallet/address.hpp \
    include/bitcoin/bitcoin/wallet/dictionary.hpp \
    include/bitcoin/bitcoin/wallet/hd_keys.hpp \
    include/bitcoin/bitcoin/wallet/key_formats.hpp \
    include/bitcoin/bitcoin/wallet/message.hpp \
    include/bitcoin/bitcoin/wallet/mnemonic.hpp \
    include/bitcoin/bitcoin/wallet/stealth_address.hpp \
    include/bitcoin/bitcoin/wallet/uri.hpp

src_libbitcoin_la_SOURCES = \
    src/block.cpp \
    src/compat.cpp \
    src/constants.cpp \
    src/error.cpp \
    src/config/parameter.cpp \
    src/config/printer.cpp \
    src/formats/base10.cpp \
    src/formats/base16.cpp \
    src/formats/base58.cpp \
    src/formats/base64.cpp \
    src/formats/base85.cpp \
    src/math/checksum.cpp \
    src/math/ec_keys.cpp \
    src/math/external/hmac_sha256.c \
    src/math/external/hmac_sha512.c \
    src/math/external/ripemd160.c \
    src/math/external/sha1.c \
    src/math/external/sha256.c \
    src/math/external/sha512.c \
    src/math/external/zeroize.c \
    src/math/external/pkcs5_pbkdf2.c \
    src/math/hash.cpp \
    src/math/hash_number.cpp \
    src/math/script_number.cpp \
    src/network/channel.cpp \
    src/network/handshake.cpp  \
    src/network/hosts.cpp \
    src/network/network.cpp  \
    src/network/protocol.cpp \
    src/satoshi_serialize.cpp \
    src/script.cpp \
    src/stealth.cpp \
    src/transaction.cpp \
    src/utility/binary.cpp \
    src/utility/general.cpp \
    src/utility/logger.cpp \
    src/utility/threadpool.cpp \
    src/utility/timed_section.cpp \
    src/wallet/address.cpp \ \
    src/wallet/dictionary.cpp \
    src/wallet/hd_keys.cpp \
    src/wallet/key_formats.cpp \
    src/wallet/message.cpp \
    src/wallet/mnemonic-wordlists.cpp \
    src/wallet/mnemonic.cpp \
    src/wallet/stealth_address.cpp \
    src/wallet/uri.cpp

#
# tests
#
if WITH_TESTS

TESTS = test/libbitcoin_test

check_PROGRAMS = test/libbitcoin_test

test_libbitcoin_test_CPPFLAGS = -I${srcdir}/include -I${srcdir}/test ${boost_CFLAGS} ${boost_date_time_CFLAGS} ${boost_filesystem_CFLAGS} ${boost_locale_CFLAGS} ${boost_program_options_CFLAGS} ${boost_regex_CFLAGS} ${boost_system_CFLAGS} ${boost_unit_test_framework_CFLAGS} ${pthread_CFLAGS} ${rt_CFLAGS} ${secp256k1_CFLAGS}

test_libbitcoin_test_LDADD = src/libbitcoin.la ${boost_LIBS} ${boost_date_time_LIBS} ${boost_filesystem_LIBS} ${boost_locale_LIBS} ${boost_program_options_LIBS} ${boost_regex_LIBS} ${boost_system_LIBS} ${boost_unit_test_framework_LIBS} ${pthread_LIBS} ${rt_LIBS} ${secp256k1_LIBS}

test_libbitcoin_test_SOURCES = \
    test/base10.cpp \
    test/base16.cpp \
    test/base58.cpp \
    test/base64.cpp \
    test/base85.cpp \
    test/big_number.cpp \
    test/binary.cpp \
    test/binary_trie.cpp \
    test/checksum.cpp \
    test/config.cpp \
    test/ec_keys.cpp \
    test/format.cpp \
    test/hash_number.cpp \
    test/hash.cpp \
    test/hd_keys.cpp \
    test/key_formats.cpp \
    test/main.cpp \
    test/message.cpp \
    test/mnemonic.cpp \
    test/parameter.cpp \
    test/printer.cpp \
    test/script_number.cpp \
    test/script.cpp \
    test/serialize.cpp \
    test/stealth_address.cpp \
    test/stealth.cpp \
    test/uri.cpp

endif
