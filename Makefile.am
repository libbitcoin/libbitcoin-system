###############################################################################
#  Copyright (c) 2014-2015 libbitcoin developers (see COPYING).
#
#         GENERATED SOURCE CODE, DO NOT EDIT EXCEPT EXPERIMENTALLY
#
###############################################################################

# Standard declarations.
#==============================================================================
# look for macros in the m4 subdirectory
ACLOCAL_AMFLAGS = -I m4


# Automake products.
#==============================================================================
# files => ${pkgconfigdir}
#------------------------------------------------------------------------------
pkgconfig_DATA = \
    libbitcoin.pc

# files => ${docdir}
#------------------------------------------------------------------------------
doc_DATA = \
    AUTHORS \
    COPYING \
    ChangeLog \
    INSTALL \
    NEWS \
    README

# src/libbitcoin.la => ${libdir}
#------------------------------------------------------------------------------
lib_LTLIBRARIES = src/libbitcoin.la
src_libbitcoin_la_CFLAGS = ${iconv_CFLAGS} ${pthread_CFLAGS} ${rt_CFLAGS}
src_libbitcoin_la_CPPFLAGS = -I${srcdir}/include ${testnet} ${boost_CPPFLAGS} ${iconv_CPPFLAGS} ${pthread_CPPFLAGS} ${rt_CPPFLAGS} ${secp256k1_CPPFLAGS}
src_libbitcoin_la_LDFLAGS = ${boost_LDFLAGS} ${iconv_LDFLAGS} ${pthread_LDFLAGS} ${rt_LDFLAGS}
src_libbitcoin_la_LIBADD = ${boost_chrono_LIBS} ${boost_date_time_LIBS} ${boost_filesystem_LIBS} ${boost_locale_LIBS} ${boost_program_options_LIBS} ${boost_regex_LIBS} ${boost_system_LIBS} ${iconv_LIBS} ${pthread_LIBS} ${rt_LIBS} ${secp256k1_LIBS}
src_libbitcoin_la_SOURCES = \
    src/block.cpp \
    src/compat.cpp \
    src/constants.cpp \
    src/error.cpp \
    src/satoshi_serialize.cpp \
    src/script.cpp \
    src/stealth.cpp \
    src/transaction.cpp \
    src/config/parameter.cpp \
    src/config/printer.cpp \
    src/formats/base10.cpp \
    src/formats/base16.cpp \
    src/formats/base58.cpp \
    src/formats/base64.cpp \
    src/formats/base85.cpp \
    src/math/checksum.cpp \
    src/math/ec_keys.cpp \
    src/math/hash.cpp \
    src/math/hash_number.cpp \
    src/math/script_number.cpp \
    src/math/external/hmac_sha256.c \
    src/math/external/hmac_sha512.c \
    src/math/external/pkcs5_pbkdf2.c \
    src/math/external/ripemd160.c \
    src/math/external/sha1.c \
    src/math/external/sha256.c \
    src/math/external/sha512.c \
    src/math/external/zeroize.c \
    src/network/channel.cpp \
    src/network/handshake.cpp \
    src/network/hosts.cpp \
    src/network/network.cpp \
    src/network/protocol.cpp \
    src/utility/binary.cpp \
    src/utility/general.cpp \
    src/utility/logger.cpp \
    src/utility/threadpool.cpp \
    src/utility/timed_section.cpp \
    src/wallet/address.cpp \
    src/wallet/dictionary.cpp \
    src/wallet/hd_keys.cpp \
    src/wallet/key_formats.cpp \
    src/wallet/message.cpp \
    src/wallet/mnemonic.cpp \
    src/wallet/stealth_address.cpp \
    src/wallet/uri.cpp

# local: test/libbitcoin_test
#------------------------------------------------------------------------------
if WITH_TESTS

# Execute 'test/libbitcoin_test' as a test.
TESTS = test/libbitcoin_test

check_PROGRAMS = test/libbitcoin_test
test_libbitcoin_test_CFLAGS = ${iconv_CFLAGS} ${pthread_CFLAGS} ${rt_CFLAGS}
test_libbitcoin_test_CPPFLAGS = -I${srcdir}/include ${testnet} ${boost_CPPFLAGS} ${iconv_CPPFLAGS} ${pthread_CPPFLAGS} ${rt_CPPFLAGS} ${secp256k1_CPPFLAGS}
test_libbitcoin_test_LDFLAGS = ${boost_LDFLAGS} ${iconv_LDFLAGS} ${pthread_LDFLAGS} ${rt_LDFLAGS}
test_libbitcoin_test_LDADD = src/libbitcoin.la ${boost_chrono_LIBS} ${boost_date_time_LIBS} ${boost_filesystem_LIBS} ${boost_locale_LIBS} ${boost_program_options_LIBS} ${boost_regex_LIBS} ${boost_system_LIBS} ${iconv_LIBS} ${pthread_LIBS} ${rt_LIBS} ${secp256k1_LIBS} ${boost_unit_test_framework_LIBS}
test_libbitcoin_test_SOURCES = \
    test/base10.cpp \
    test/base16.cpp \
    test/base58.cpp \
    test/base64.cpp \
    test/base85.cpp \
    test/big_number.cpp \
    test/big_number.hpp \
    test/binary.cpp \
    test/binary_trie.cpp \
    test/checksum.cpp \
    test/config.cpp \
    test/ec_keys.cpp \
    test/format.cpp \
    test/hash.cpp \
    test/hash.hpp \
    test/hash_number.cpp \
    test/hd_keys.cpp \
    test/key_formats.cpp \
    test/main.cpp \
    test/message.cpp \
    test/mnemonic.cpp \
    test/mnemonic.hpp \
    test/parameter.cpp \
    test/printer.cpp \
    test/satoshi_words.cpp \
    test/script.cpp \
    test/script.hpp \
    test/script_number.cpp \
    test/script_number.hpp \
    test/serialize.cpp \
    test/stealth.cpp \
    test/stealth_address.cpp \
    test/uri.cpp

endif WITH_TESTS

# files => ${includedir}/bitcoin
#------------------------------------------------------------------------------
include_bitcoindir = ${includedir}/bitcoin
include_bitcoin_HEADERS = \
    include/bitcoin/bitcoin.hpp

include_bitcoin_bitcoindir = ${includedir}/bitcoin/bitcoin
include_bitcoin_bitcoin_HEADERS = \
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

include_bitcoin_bitcoin_configdir = ${includedir}/bitcoin/bitcoin/config
include_bitcoin_bitcoin_config_HEADERS = \
    include/bitcoin/bitcoin/config/parameter.hpp \
    include/bitcoin/bitcoin/config/printer.hpp

include_bitcoin_bitcoin_formatsdir = ${includedir}/bitcoin/bitcoin/formats
include_bitcoin_bitcoin_formats_HEADERS = \
    include/bitcoin/bitcoin/formats/base10.hpp \
    include/bitcoin/bitcoin/formats/base16.hpp \
    include/bitcoin/bitcoin/formats/base58.hpp \
    include/bitcoin/bitcoin/formats/base64.hpp \
    include/bitcoin/bitcoin/formats/base85.hpp

include_bitcoin_bitcoin_impl_formatsdir = ${includedir}/bitcoin/bitcoin/impl/formats
include_bitcoin_bitcoin_impl_formats_HEADERS = \
    include/bitcoin/bitcoin/impl/formats/base16.ipp

include_bitcoin_bitcoin_impl_serializedir = ${includedir}/bitcoin/bitcoin/impl/serialize
include_bitcoin_bitcoin_impl_serialize_HEADERS = \
    include/bitcoin/bitcoin/impl/serialize/block.ipp \
    include/bitcoin/bitcoin/impl/serialize/misc.ipp

include_bitcoin_bitcoin_impl_triedir = ${includedir}/bitcoin/bitcoin/impl/trie
include_bitcoin_bitcoin_impl_trie_HEADERS = \
    include/bitcoin/bitcoin/impl/trie/binary_trie.ipp \
    include/bitcoin/bitcoin/impl/trie/trie_iterator.ipp \
    include/bitcoin/bitcoin/impl/trie/trie_reverse_iterator.ipp \
    include/bitcoin/bitcoin/impl/trie/trie_structure_node.ipp \
    include/bitcoin/bitcoin/impl/trie/trie_value_node.ipp

include_bitcoin_bitcoin_impl_utilitydir = ${includedir}/bitcoin/bitcoin/impl/utility
include_bitcoin_bitcoin_impl_utility_HEADERS = \
    include/bitcoin/bitcoin/impl/utility/array_slice.ipp \
    include/bitcoin/bitcoin/impl/utility/data.ipp \
    include/bitcoin/bitcoin/impl/utility/endian.ipp \
    include/bitcoin/bitcoin/impl/utility/general.ipp \
    include/bitcoin/bitcoin/impl/utility/serializer.ipp

include_bitcoin_bitcoin_mathdir = ${includedir}/bitcoin/bitcoin/math
include_bitcoin_bitcoin_math_HEADERS = \
    include/bitcoin/bitcoin/math/checksum.hpp \
    include/bitcoin/bitcoin/math/ec_keys.hpp \
    include/bitcoin/bitcoin/math/hash.hpp \
    include/bitcoin/bitcoin/math/hash_number.hpp \
    include/bitcoin/bitcoin/math/script_number.hpp

include_bitcoin_bitcoin_math_externaldir = ${includedir}/bitcoin/bitcoin/math/external
include_bitcoin_bitcoin_math_external_HEADERS = \
    include/bitcoin/bitcoin/math/external/hmac_sha256.h \
    include/bitcoin/bitcoin/math/external/hmac_sha512.h \
    include/bitcoin/bitcoin/math/external/pkcs5_pbkdf2.h \
    include/bitcoin/bitcoin/math/external/ripemd160.h \
    include/bitcoin/bitcoin/math/external/sha1.h \
    include/bitcoin/bitcoin/math/external/sha256.h \
    include/bitcoin/bitcoin/math/external/sha512.h \
    include/bitcoin/bitcoin/math/external/uint256.h \
    include/bitcoin/bitcoin/math/external/zeroize.h

include_bitcoin_bitcoin_networkdir = ${includedir}/bitcoin/bitcoin/network
include_bitcoin_bitcoin_network_HEADERS = \
    include/bitcoin/bitcoin/network/channel.hpp \
    include/bitcoin/bitcoin/network/handshake.hpp \
    include/bitcoin/bitcoin/network/hosts.hpp \
    include/bitcoin/bitcoin/network/network.hpp \
    include/bitcoin/bitcoin/network/protocol.hpp \
    include/bitcoin/bitcoin/network/shared_const_buffer.hpp

include_bitcoin_bitcoin_triedir = ${includedir}/bitcoin/bitcoin/trie
include_bitcoin_bitcoin_trie_HEADERS = \
    include/bitcoin/bitcoin/trie/binary_trie.hpp \
    include/bitcoin/bitcoin/trie/trie_iterator.hpp \
    include/bitcoin/bitcoin/trie/trie_reverse_iterator.hpp \
    include/bitcoin/bitcoin/trie/trie_structure_node.hpp \
    include/bitcoin/bitcoin/trie/trie_value_node.hpp

include_bitcoin_bitcoin_utilitydir = ${includedir}/bitcoin/bitcoin/utility
include_bitcoin_bitcoin_utility_HEADERS = \
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

include_bitcoin_bitcoin_walletdir = ${includedir}/bitcoin/bitcoin/wallet
include_bitcoin_bitcoin_wallet_HEADERS = \
    include/bitcoin/bitcoin/wallet/address.hpp \
    include/bitcoin/bitcoin/wallet/dictionary.hpp \
    include/bitcoin/bitcoin/wallet/hd_keys.hpp \
    include/bitcoin/bitcoin/wallet/key_formats.hpp \
    include/bitcoin/bitcoin/wallet/message.hpp \
    include/bitcoin/bitcoin/wallet/mnemonic.hpp \
    include/bitcoin/bitcoin/wallet/stealth_address.hpp \
    include/bitcoin/bitcoin/wallet/uri.hpp

