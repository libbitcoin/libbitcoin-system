###############################################################################
#  Copyright (c) 2014-2015 libbitcoin developers (see COPYING).
#
#         GENERATED SOURCE CODE, DO NOT EDIT EXCEPT EXPERIMENTALLY
#
###############################################################################

# Automake settings.
#==============================================================================
# Look for macros in the m4 subdirectory.
#------------------------------------------------------------------------------
ACLOCAL_AMFLAGS = -I m4


# Distribute, make and install products.
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
src_libbitcoin_la_CPPFLAGS = -I${srcdir}/include ${icu} ${png} ${qrencode} ${boost_CPPFLAGS} ${pthread_CPPFLAGS} ${png_CPPFLAGS} ${qrencode_CPPFLAGS} ${icu_i18n_CPPFLAGS} ${secp256k1_CPPFLAGS}
src_libbitcoin_la_LDFLAGS = ${boost_LDFLAGS}
src_libbitcoin_la_LIBADD = ${boost_chrono_LIBS} ${boost_date_time_LIBS} ${boost_filesystem_LIBS} ${boost_iostreams_LIBS} ${boost_locale_LIBS} ${boost_program_options_LIBS} ${boost_regex_LIBS} ${boost_system_LIBS} ${boost_thread_LIBS} ${pthread_LIBS} ${rt_LIBS} ${png_LIBS} ${qrencode_LIBS} ${icu_i18n_LIBS} ${dl_LIBS} ${secp256k1_LIBS}
src_libbitcoin_la_SOURCES = \
    src/constants.cpp \
    src/error.cpp \
    src/chain/block.cpp \
    src/chain/header.cpp \
    src/chain/input.cpp \
    src/chain/opcode.cpp \
    src/chain/operation.cpp \
    src/chain/output.cpp \
    src/chain/point.cpp \
    src/chain/script.cpp \
    src/chain/transaction.cpp \
    src/config/authority.cpp \
    src/config/btc256.cpp \
    src/config/checkpoint.cpp \
    src/config/directory.cpp \
    src/config/endpoint.cpp \
    src/config/parameter.cpp \
    src/config/parser.cpp \
    src/config/printer.cpp \
    src/formats/base10.cpp \
    src/formats/base16.cpp \
    src/formats/base58.cpp \
    src/formats/base64.cpp \
    src/formats/base85.cpp \
    src/math/checksum.cpp \
    src/math/crypto.cpp \
    src/math/elliptic_curve.cpp \
    src/math/hash.cpp \
    src/math/hash_number.cpp \
    src/math/script_number.cpp \
    src/math/secp256k1_initializer.cpp \
    src/math/secp256k1_initializer.hpp \
    src/math/stealth.cpp \
    src/math/uint256.cpp \
    src/math/external/aes256.c \
    src/math/external/aes256.h \
    src/math/external/crypto_scrypt.c \
    src/math/external/crypto_scrypt.h \
    src/math/external/hmac_sha256.c \
    src/math/external/hmac_sha256.h \
    src/math/external/hmac_sha512.c \
    src/math/external/hmac_sha512.h \
    src/math/external/lax_der_parsing.c \
    src/math/external/lax_der_parsing.h \
    src/math/external/pbkdf2_sha256.c \
    src/math/external/pbkdf2_sha256.h \
    src/math/external/pkcs5_pbkdf2.c \
    src/math/external/pkcs5_pbkdf2.h \
    src/math/external/ripemd160.c \
    src/math/external/ripemd160.h \
    src/math/external/sha1.c \
    src/math/external/sha1.h \
    src/math/external/sha256.c \
    src/math/external/sha256.h \
    src/math/external/sha512.c \
    src/math/external/sha512.h \
    src/math/external/zeroize.c \
    src/math/external/zeroize.h \
    src/message/address.cpp \
    src/message/alert.cpp \
    src/message/alert_payload.cpp \
    src/message/filter_add.cpp \
    src/message/filter_clear.cpp \
    src/message/filter_load.cpp \
    src/message/get_address.cpp \
    src/message/get_blocks.cpp \
    src/message/get_data.cpp \
    src/message/get_headers.cpp \
    src/message/headers.cpp \
    src/message/heading.cpp \
    src/message/inventory.cpp \
    src/message/inventory_type_id.cpp \
    src/message/inventory_vector.cpp \
    src/message/memory_pool.cpp \
    src/message/merkle_block.cpp \
    src/message/network_address.cpp \
    src/message/nonce.cpp \
    src/message/not_found.cpp \
    src/message/ping.cpp \
    src/message/pong.cpp \
    src/message/reject.cpp \
    src/message/verack.cpp \
    src/message/version.cpp \
    src/unicode/console_streambuf.cpp \
    src/unicode/ifstream.cpp \
    src/unicode/ofstream.cpp \
    src/unicode/unicode.cpp \
    src/unicode/unicode_istream.cpp \
    src/unicode/unicode_ostream.cpp \
    src/unicode/unicode_streambuf.cpp \
    src/utility/binary.cpp \
    src/utility/conditional_stack.cpp \
    src/utility/conditional_stack.hpp \
    src/utility/deadline.cpp \
    src/utility/dispatcher.cpp \
    src/utility/evaluation_context.cpp \
    src/utility/evaluation_context.hpp \
    src/utility/istream_reader.cpp \
    src/utility/log.cpp \
    src/utility/monitor.cpp \
    src/utility/ostream_writer.cpp \
    src/utility/png.cpp \
    src/utility/random.cpp \
    src/utility/string.cpp \
    src/utility/thread.cpp \
    src/utility/threadpool.cpp \
    src/utility/variable_uint_size.cpp \
    src/utility/work.cpp \
    src/wallet/bitcoin_uri.cpp \
    src/wallet/dictionary.cpp \
    src/wallet/ec_private.cpp \
    src/wallet/ec_public.cpp \
    src/wallet/ek_private.cpp \
    src/wallet/ek_public.cpp \
    src/wallet/ek_token.cpp \
    src/wallet/encrypted_keys.cpp \
    src/wallet/hd_private.cpp \
    src/wallet/hd_public.cpp \
    src/wallet/message.cpp \
    src/wallet/mini_keys.cpp \
    src/wallet/mnemonic.cpp \
    src/wallet/payment_address.cpp \
    src/wallet/qrcode.cpp \
    src/wallet/select_outputs.cpp \
    src/wallet/stealth_address.cpp \
    src/wallet/uri.cpp \
    src/wallet/parse_encrypted_keys/parse_encrypted_key.hpp \
    src/wallet/parse_encrypted_keys/parse_encrypted_key.ipp \
    src/wallet/parse_encrypted_keys/parse_encrypted_prefix.hpp \
    src/wallet/parse_encrypted_keys/parse_encrypted_prefix.ipp \
    src/wallet/parse_encrypted_keys/parse_encrypted_private.cpp \
    src/wallet/parse_encrypted_keys/parse_encrypted_private.hpp \
    src/wallet/parse_encrypted_keys/parse_encrypted_public.cpp \
    src/wallet/parse_encrypted_keys/parse_encrypted_public.hpp \
    src/wallet/parse_encrypted_keys/parse_encrypted_token.cpp \
    src/wallet/parse_encrypted_keys/parse_encrypted_token.hpp

# local: examples/libbitcoin_examples
#------------------------------------------------------------------------------
if WITH_EXAMPLES

noinst_PROGRAMS = examples/libbitcoin_examples
examples_libbitcoin_examples_CPPFLAGS = -I${srcdir}/include ${icu} ${png} ${qrencode} ${boost_CPPFLAGS} ${pthread_CPPFLAGS} ${png_CPPFLAGS} ${qrencode_CPPFLAGS} ${icu_i18n_CPPFLAGS} ${secp256k1_CPPFLAGS}
examples_libbitcoin_examples_LDFLAGS = ${boost_LDFLAGS}
examples_libbitcoin_examples_LDADD = src/libbitcoin.la ${boost_chrono_LIBS} ${boost_date_time_LIBS} ${boost_filesystem_LIBS} ${boost_iostreams_LIBS} ${boost_locale_LIBS} ${boost_program_options_LIBS} ${boost_regex_LIBS} ${boost_system_LIBS} ${boost_thread_LIBS} ${pthread_LIBS} ${rt_LIBS} ${png_LIBS} ${qrencode_LIBS} ${icu_i18n_LIBS} ${dl_LIBS} ${secp256k1_LIBS}
examples_libbitcoin_examples_SOURCES = \
    examples/main.cpp

endif WITH_EXAMPLES

# local: test/libbitcoin_test
#------------------------------------------------------------------------------
if WITH_TESTS

TESTS = libbitcoin_test_runner.sh

check_PROGRAMS = test/libbitcoin_test
test_libbitcoin_test_CPPFLAGS = -I${srcdir}/include ${icu} ${png} ${qrencode} ${boost_CPPFLAGS} ${pthread_CPPFLAGS} ${png_CPPFLAGS} ${qrencode_CPPFLAGS} ${icu_i18n_CPPFLAGS} ${secp256k1_CPPFLAGS}
test_libbitcoin_test_LDFLAGS = ${boost_LDFLAGS}
test_libbitcoin_test_LDADD = src/libbitcoin.la ${boost_unit_test_framework_LIBS} ${boost_chrono_LIBS} ${boost_date_time_LIBS} ${boost_filesystem_LIBS} ${boost_iostreams_LIBS} ${boost_locale_LIBS} ${boost_program_options_LIBS} ${boost_regex_LIBS} ${boost_system_LIBS} ${boost_thread_LIBS} ${pthread_LIBS} ${rt_LIBS} ${png_LIBS} ${qrencode_LIBS} ${icu_i18n_LIBS} ${dl_LIBS} ${secp256k1_LIBS}
test_libbitcoin_test_SOURCES = \
    test/main.cpp \
    test/chain/block.cpp \
    test/chain/header.cpp \
    test/chain/input.cpp \
    test/chain/output.cpp \
    test/chain/point.cpp \
    test/chain/satoshi_words.cpp \
    test/chain/script.cpp \
    test/chain/script.hpp \
    test/chain/transaction.cpp \
    test/config/authority.cpp \
    test/config/btc256.cpp \
    test/config/checkpoint.cpp \
    test/config/endpoint.cpp \
    test/config/parameter.cpp \
    test/config/printer.cpp \
    test/formats/base10.cpp \
    test/formats/base16.cpp \
    test/formats/base58.cpp \
    test/formats/base64.cpp \
    test/formats/base85.cpp \
    test/math/big_number.cpp \
    test/math/big_number.hpp \
    test/math/checksum.cpp \
    test/math/elliptic_curve.cpp \
    test/math/hash.cpp \
    test/math/hash.hpp \
    test/math/hash_number.cpp \
    test/math/script_number.cpp \
    test/math/script_number.hpp \
    test/math/stealth.cpp \
    test/message/address.cpp \
    test/message/alert.cpp \
    test/message/alert_payload.cpp \
    test/message/filter_add.cpp \
    test/message/filter_clear.cpp \
    test/message/filter_load.cpp \
    test/message/get_address.cpp \
    test/message/get_blocks.cpp \
    test/message/get_data.cpp \
    test/message/get_headers.cpp \
    test/message/headers.cpp \
    test/message/heading.cpp \
    test/message/inventory.cpp \
    test/message/inventory_type_id.cpp \
    test/message/inventory_vector.cpp \
    test/message/memory_pool.cpp \
    test/message/merkle_block.cpp \
    test/message/network_address.cpp \
    test/message/not_found.cpp \
    test/message/ping.cpp \
    test/message/pong.cpp \
    test/message/reject.cpp \
    test/message/verack.cpp \
    test/message/version.cpp \
    test/unicode/unicode.cpp \
    test/unicode/unicode_istream.cpp \
    test/unicode/unicode_ostream.cpp \
    test/utility/binary.cpp \
    test/utility/collection.cpp \
    test/utility/data.cpp \
    test/utility/endian.cpp \
    test/utility/png.cpp \
    test/utility/random.cpp \
    test/utility/serializer.cpp \
    test/utility/stream.cpp \
    test/utility/thread.cpp \
    test/utility/variable_uint_size.cpp \
    test/wallet/bitcoin_uri.cpp \
    test/wallet/ec_private.cpp \
    test/wallet/ec_public.cpp \
    test/wallet/encrypted_keys.cpp \
    test/wallet/hd_private.cpp \
    test/wallet/hd_public.cpp \
    test/wallet/message.cpp \
    test/wallet/mnemonic.cpp \
    test/wallet/mnemonic.hpp \
    test/wallet/payment_address.cpp \
    test/wallet/qrcode.cpp \
    test/wallet/stealth_address.cpp \
    test/wallet/uri.cpp \
    test/wallet/uri_reader.cpp

endif WITH_TESTS

# files => ${includedir}/bitcoin
#------------------------------------------------------------------------------
include_bitcoindir = ${includedir}/bitcoin
include_bitcoin_HEADERS = \
    include/bitcoin/bitcoin.hpp

include_bitcoin_bitcoindir = ${includedir}/bitcoin/bitcoin
include_bitcoin_bitcoin_HEADERS = \
    include/bitcoin/bitcoin/compat.h \
    include/bitcoin/bitcoin/compat.hpp \
    include/bitcoin/bitcoin/constants.hpp \
    include/bitcoin/bitcoin/define.hpp \
    include/bitcoin/bitcoin/error.hpp \
    include/bitcoin/bitcoin/handlers.hpp \
    include/bitcoin/bitcoin/messages.hpp \
    include/bitcoin/bitcoin/version.hpp

include_bitcoin_bitcoin_chaindir = ${includedir}/bitcoin/bitcoin/chain
include_bitcoin_bitcoin_chain_HEADERS = \
    include/bitcoin/bitcoin/chain/block.hpp \
    include/bitcoin/bitcoin/chain/header.hpp \
    include/bitcoin/bitcoin/chain/history_row.hpp \
    include/bitcoin/bitcoin/chain/input.hpp \
    include/bitcoin/bitcoin/chain/opcode.hpp \
    include/bitcoin/bitcoin/chain/operation.hpp \
    include/bitcoin/bitcoin/chain/output.hpp \
    include/bitcoin/bitcoin/chain/point.hpp \
    include/bitcoin/bitcoin/chain/script.hpp \
    include/bitcoin/bitcoin/chain/spend.hpp \
    include/bitcoin/bitcoin/chain/stealth_row.hpp \
    include/bitcoin/bitcoin/chain/transaction.hpp

include_bitcoin_bitcoin_configdir = ${includedir}/bitcoin/bitcoin/config
include_bitcoin_bitcoin_config_HEADERS = \
    include/bitcoin/bitcoin/config/authority.hpp \
    include/bitcoin/bitcoin/config/btc256.hpp \
    include/bitcoin/bitcoin/config/checkpoint.hpp \
    include/bitcoin/bitcoin/config/directory.hpp \
    include/bitcoin/bitcoin/config/endpoint.hpp \
    include/bitcoin/bitcoin/config/parameter.hpp \
    include/bitcoin/bitcoin/config/parser.hpp \
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
    include/bitcoin/bitcoin/impl/formats/base16.ipp \
    include/bitcoin/bitcoin/impl/formats/base58.ipp

include_bitcoin_bitcoin_impl_mathdir = ${includedir}/bitcoin/bitcoin/impl/math
include_bitcoin_bitcoin_impl_math_HEADERS = \
    include/bitcoin/bitcoin/impl/math/checksum.ipp \
    include/bitcoin/bitcoin/impl/math/hash.ipp

include_bitcoin_bitcoin_impl_utilitydir = ${includedir}/bitcoin/bitcoin/impl/utility
include_bitcoin_bitcoin_impl_utility_HEADERS = \
    include/bitcoin/bitcoin/impl/utility/array_slice.ipp \
    include/bitcoin/bitcoin/impl/utility/collection.ipp \
    include/bitcoin/bitcoin/impl/utility/data.ipp \
    include/bitcoin/bitcoin/impl/utility/deserializer.ipp \
    include/bitcoin/bitcoin/impl/utility/endian.ipp \
    include/bitcoin/bitcoin/impl/utility/istream_reader.ipp \
    include/bitcoin/bitcoin/impl/utility/ostream_writer.ipp \
    include/bitcoin/bitcoin/impl/utility/resubscriber.ipp \
    include/bitcoin/bitcoin/impl/utility/serializer.ipp \
    include/bitcoin/bitcoin/impl/utility/subscriber.ipp \
    include/bitcoin/bitcoin/impl/utility/track.ipp

include_bitcoin_bitcoin_mathdir = ${includedir}/bitcoin/bitcoin/math
include_bitcoin_bitcoin_math_HEADERS = \
    include/bitcoin/bitcoin/math/checksum.hpp \
    include/bitcoin/bitcoin/math/crypto.hpp \
    include/bitcoin/bitcoin/math/elliptic_curve.hpp \
    include/bitcoin/bitcoin/math/hash.hpp \
    include/bitcoin/bitcoin/math/hash_number.hpp \
    include/bitcoin/bitcoin/math/script_number.hpp \
    include/bitcoin/bitcoin/math/stealth.hpp \
    include/bitcoin/bitcoin/math/uint256.hpp

include_bitcoin_bitcoin_messagedir = ${includedir}/bitcoin/bitcoin/message
include_bitcoin_bitcoin_message_HEADERS = \
    include/bitcoin/bitcoin/message/address.hpp \
    include/bitcoin/bitcoin/message/alert.hpp \
    include/bitcoin/bitcoin/message/alert_payload.hpp \
    include/bitcoin/bitcoin/message/filter_add.hpp \
    include/bitcoin/bitcoin/message/filter_clear.hpp \
    include/bitcoin/bitcoin/message/filter_load.hpp \
    include/bitcoin/bitcoin/message/get_address.hpp \
    include/bitcoin/bitcoin/message/get_blocks.hpp \
    include/bitcoin/bitcoin/message/get_data.hpp \
    include/bitcoin/bitcoin/message/get_headers.hpp \
    include/bitcoin/bitcoin/message/headers.hpp \
    include/bitcoin/bitcoin/message/heading.hpp \
    include/bitcoin/bitcoin/message/inventory.hpp \
    include/bitcoin/bitcoin/message/inventory_type_id.hpp \
    include/bitcoin/bitcoin/message/inventory_vector.hpp \
    include/bitcoin/bitcoin/message/memory_pool.hpp \
    include/bitcoin/bitcoin/message/merkle_block.hpp \
    include/bitcoin/bitcoin/message/network_address.hpp \
    include/bitcoin/bitcoin/message/nonce.hpp \
    include/bitcoin/bitcoin/message/not_found.hpp \
    include/bitcoin/bitcoin/message/ping.hpp \
    include/bitcoin/bitcoin/message/pong.hpp \
    include/bitcoin/bitcoin/message/reject.hpp \
    include/bitcoin/bitcoin/message/verack.hpp \
    include/bitcoin/bitcoin/message/version.hpp

include_bitcoin_bitcoin_unicodedir = ${includedir}/bitcoin/bitcoin/unicode
include_bitcoin_bitcoin_unicode_HEADERS = \
    include/bitcoin/bitcoin/unicode/console_streambuf.hpp \
    include/bitcoin/bitcoin/unicode/ifstream.hpp \
    include/bitcoin/bitcoin/unicode/ofstream.hpp \
    include/bitcoin/bitcoin/unicode/unicode.hpp \
    include/bitcoin/bitcoin/unicode/unicode_istream.hpp \
    include/bitcoin/bitcoin/unicode/unicode_ostream.hpp \
    include/bitcoin/bitcoin/unicode/unicode_streambuf.hpp

include_bitcoin_bitcoin_utilitydir = ${includedir}/bitcoin/bitcoin/utility
include_bitcoin_bitcoin_utility_HEADERS = \
    include/bitcoin/bitcoin/utility/array_slice.hpp \
    include/bitcoin/bitcoin/utility/asio.hpp \
    include/bitcoin/bitcoin/utility/assert.hpp \
    include/bitcoin/bitcoin/utility/atomic.hpp \
    include/bitcoin/bitcoin/utility/binary.hpp \
    include/bitcoin/bitcoin/utility/collection.hpp \
    include/bitcoin/bitcoin/utility/color.hpp \
    include/bitcoin/bitcoin/utility/container_sink.hpp \
    include/bitcoin/bitcoin/utility/container_source.hpp \
    include/bitcoin/bitcoin/utility/data.hpp \
    include/bitcoin/bitcoin/utility/deadline.hpp \
    include/bitcoin/bitcoin/utility/decorator.hpp \
    include/bitcoin/bitcoin/utility/delegates.hpp \
    include/bitcoin/bitcoin/utility/deserializer.hpp \
    include/bitcoin/bitcoin/utility/dispatcher.hpp \
    include/bitcoin/bitcoin/utility/enable_shared_from_base.hpp \
    include/bitcoin/bitcoin/utility/endian.hpp \
    include/bitcoin/bitcoin/utility/exceptions.hpp \
    include/bitcoin/bitcoin/utility/istream_reader.hpp \
    include/bitcoin/bitcoin/utility/log.hpp \
    include/bitcoin/bitcoin/utility/monitor.hpp \
    include/bitcoin/bitcoin/utility/ostream_writer.hpp \
    include/bitcoin/bitcoin/utility/png.hpp \
    include/bitcoin/bitcoin/utility/random.hpp \
    include/bitcoin/bitcoin/utility/reader.hpp \
    include/bitcoin/bitcoin/utility/resubscriber.hpp \
    include/bitcoin/bitcoin/utility/serializer.hpp \
    include/bitcoin/bitcoin/utility/string.hpp \
    include/bitcoin/bitcoin/utility/subscriber.hpp \
    include/bitcoin/bitcoin/utility/synchronizer.hpp \
    include/bitcoin/bitcoin/utility/thread.hpp \
    include/bitcoin/bitcoin/utility/threadpool.hpp \
    include/bitcoin/bitcoin/utility/timer.hpp \
    include/bitcoin/bitcoin/utility/track.hpp \
    include/bitcoin/bitcoin/utility/variable_uint_size.hpp \
    include/bitcoin/bitcoin/utility/work.hpp \
    include/bitcoin/bitcoin/utility/writer.hpp

include_bitcoin_bitcoin_walletdir = ${includedir}/bitcoin/bitcoin/wallet
include_bitcoin_bitcoin_wallet_HEADERS = \
    include/bitcoin/bitcoin/wallet/bitcoin_uri.hpp \
    include/bitcoin/bitcoin/wallet/dictionary.hpp \
    include/bitcoin/bitcoin/wallet/ec_private.hpp \
    include/bitcoin/bitcoin/wallet/ec_public.hpp \
    include/bitcoin/bitcoin/wallet/ek_private.hpp \
    include/bitcoin/bitcoin/wallet/ek_public.hpp \
    include/bitcoin/bitcoin/wallet/ek_token.hpp \
    include/bitcoin/bitcoin/wallet/encrypted_keys.hpp \
    include/bitcoin/bitcoin/wallet/hd_private.hpp \
    include/bitcoin/bitcoin/wallet/hd_public.hpp \
    include/bitcoin/bitcoin/wallet/message.hpp \
    include/bitcoin/bitcoin/wallet/mini_keys.hpp \
    include/bitcoin/bitcoin/wallet/mnemonic.hpp \
    include/bitcoin/bitcoin/wallet/payment_address.hpp \
    include/bitcoin/bitcoin/wallet/qrcode.hpp \
    include/bitcoin/bitcoin/wallet/select_outputs.hpp \
    include/bitcoin/bitcoin/wallet/settings.hpp \
    include/bitcoin/bitcoin/wallet/stealth_address.hpp \
    include/bitcoin/bitcoin/wallet/uri.hpp \
    include/bitcoin/bitcoin/wallet/uri_reader.hpp


# Custom make targets.
#==============================================================================
# make target: examples
#------------------------------------------------------------------------------
target_examples = \
    examples/libbitcoin_examples

examples: ${target_examples}

