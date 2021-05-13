###############################################################################
#  Copyright (c) 2014-2020 libbitcoin-system developers (see COPYING).
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
    libbitcoin-system.pc

# files => ${docdir}
#------------------------------------------------------------------------------
doc_DATA = \
    AUTHORS \
    COPYING \
    ChangeLog \
    INSTALL \
    NEWS \
    README

# src/libbitcoin-system.la => ${libdir}
#------------------------------------------------------------------------------
lib_LTLIBRARIES = src/libbitcoin-system.la
src_libbitcoin_system_la_CPPFLAGS = -I${srcdir}/include ${icu} ${boost_BUILD_CPPFLAGS} ${pthread_BUILD_CPPFLAGS} ${icu_i18n_BUILD_CPPFLAGS} ${secp256k1_BUILD_CPPFLAGS}
src_libbitcoin_system_la_LDFLAGS = ${boost_LDFLAGS}
src_libbitcoin_system_la_LIBADD = ${boost_chrono_LIBS} ${boost_date_time_LIBS} ${boost_filesystem_LIBS} ${boost_iostreams_LIBS} ${boost_locale_LIBS} ${boost_log_LIBS} ${boost_program_options_LIBS} ${boost_regex_LIBS} ${boost_system_LIBS} ${boost_thread_LIBS} ${pthread_LIBS} ${rt_LIBS} ${icu_i18n_LIBS} ${dl_LIBS} ${secp256k1_LIBS}
src_libbitcoin_system_la_SOURCES = \
    src/error.cpp \
    src/settings.cpp \
    src/chain/block.cpp \
    src/chain/block_filter.cpp \
    src/chain/chain_state.cpp \
    src/chain/compact.cpp \
    src/chain/header.cpp \
    src/chain/input.cpp \
    src/chain/output.cpp \
    src/chain/output_point.cpp \
    src/chain/payment_record.cpp \
    src/chain/point.cpp \
    src/chain/point_value.cpp \
    src/chain/points_value.cpp \
    src/chain/script.cpp \
    src/chain/stealth_record.cpp \
    src/chain/transaction.cpp \
    src/chain/witness.cpp \
    src/config/authority.cpp \
    src/config/base16.cpp \
    src/config/base2.cpp \
    src/config/base32.cpp \
    src/config/base58.cpp \
    src/config/base64.cpp \
    src/config/base85.cpp \
    src/config/block.cpp \
    src/config/checkpoint.cpp \
    src/config/compact_filter.cpp \
    src/config/directory.cpp \
    src/config/endpoint.cpp \
    src/config/hash160.cpp \
    src/config/hash256.cpp \
    src/config/header.cpp \
    src/config/input.cpp \
    src/config/output.cpp \
    src/config/parameter.cpp \
    src/config/parser.cpp \
    src/config/point.cpp \
    src/config/printer.cpp \
    src/config/script.cpp \
    src/config/transaction.cpp \
    src/formats/base_10.cpp \
    src/formats/base_16.cpp \
    src/formats/base_32.cpp \
    src/formats/base_58.cpp \
    src/formats/base_64.cpp \
    src/formats/base_85.cpp \
    src/log/file_collector.cpp \
    src/log/file_collector_repository.cpp \
    src/log/file_counter_formatter.cpp \
    src/log/sink.cpp \
    src/log/statsd_sink.cpp \
    src/log/udp_client_sink.cpp \
    src/machine/interpreter.cpp \
    src/machine/number.cpp \
    src/machine/opcode.cpp \
    src/machine/operation.cpp \
    src/machine/program.cpp \
    src/math/checksum.cpp \
    src/math/crypto.cpp \
    src/math/ec_point.cpp \
    src/math/ec_scalar.cpp \
    src/math/elliptic_curve.cpp \
    src/math/golomb_coded_sets.cpp \
    src/math/hash.cpp \
    src/math/ring_signature.cpp \
    src/math/secp256k1_initializer.cpp \
    src/math/secp256k1_initializer.hpp \
    src/math/siphash.cpp \
    src/math/stealth.cpp \
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
    src/message/block.cpp \
    src/message/block_transactions.cpp \
    src/message/compact_block.cpp \
    src/message/compact_filter.cpp \
    src/message/compact_filter_checkpoint.cpp \
    src/message/compact_filter_headers.cpp \
    src/message/fee_filter.cpp \
    src/message/filter_add.cpp \
    src/message/filter_clear.cpp \
    src/message/filter_load.cpp \
    src/message/get_address.cpp \
    src/message/get_block_transactions.cpp \
    src/message/get_blocks.cpp \
    src/message/get_compact_filter_checkpoint.cpp \
    src/message/get_compact_filter_headers.cpp \
    src/message/get_compact_filters.cpp \
    src/message/get_data.cpp \
    src/message/get_headers.cpp \
    src/message/header.cpp \
    src/message/headers.cpp \
    src/message/heading.cpp \
    src/message/inventory.cpp \
    src/message/inventory_vector.cpp \
    src/message/memory_pool.cpp \
    src/message/merkle_block.cpp \
    src/message/messages.cpp \
    src/message/network_address.cpp \
    src/message/not_found.cpp \
    src/message/ping.cpp \
    src/message/pong.cpp \
    src/message/prefilled_transaction.cpp \
    src/message/reject.cpp \
    src/message/send_compact.cpp \
    src/message/send_headers.cpp \
    src/message/transaction.cpp \
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
    src/utility/conditional_lock.cpp \
    src/utility/data.cpp \
    src/utility/data_slice.cpp \
    src/utility/deadline.cpp \
    src/utility/dispatcher.cpp \
    src/utility/flush_lock.cpp \
    src/utility/interprocess_lock.cpp \
    src/utility/istream_bit_reader.cpp \
    src/utility/istream_reader.cpp \
    src/utility/monitor.cpp \
    src/utility/neutrino_filter.cpp \
    src/utility/ostream_bit_writer.cpp \
    src/utility/ostream_writer.cpp \
    src/utility/prioritized_mutex.cpp \
    src/utility/property_tree.cpp \
    src/utility/pseudo_random.cpp \
    src/utility/scope_lock.cpp \
    src/utility/sequencer.cpp \
    src/utility/sequential_lock.cpp \
    src/utility/socket.cpp \
    src/utility/string.cpp \
    src/utility/thread.cpp \
    src/utility/threadpool.cpp \
    src/utility/tiff.cpp \
    src/utility/work.cpp \
    src/wallet/message.cpp \
    src/wallet/addresses/bitcoin_uri.cpp \
    src/wallet/addresses/payment_address.cpp \
    src/wallet/addresses/qr_code.cpp \
    src/wallet/addresses/stealth_address.cpp \
    src/wallet/addresses/stealth_receiver.cpp \
    src/wallet/addresses/stealth_sender.cpp \
    src/wallet/addresses/uri.cpp \
    src/wallet/addresses/witness_address.cpp \
    src/wallet/addresses/qrencode/bitstream.c \
    src/wallet/addresses/qrencode/bitstream.h \
    src/wallet/addresses/qrencode/mask.c \
    src/wallet/addresses/qrencode/mask.h \
    src/wallet/addresses/qrencode/mmask.c \
    src/wallet/addresses/qrencode/mmask.h \
    src/wallet/addresses/qrencode/mqrspec.c \
    src/wallet/addresses/qrencode/mqrspec.h \
    src/wallet/addresses/qrencode/qrencode.c \
    src/wallet/addresses/qrencode/qrencode.h \
    src/wallet/addresses/qrencode/qrencode_inner.h \
    src/wallet/addresses/qrencode/qrinput.c \
    src/wallet/addresses/qrencode/qrinput.h \
    src/wallet/addresses/qrencode/qrspec.c \
    src/wallet/addresses/qrencode/qrspec.h \
    src/wallet/addresses/qrencode/rsecc.c \
    src/wallet/addresses/qrencode/rsecc.h \
    src/wallet/addresses/qrencode/split.c \
    src/wallet/addresses/qrencode/split.h \
    src/wallet/keys/ec_private.cpp \
    src/wallet/keys/ec_public.cpp \
    src/wallet/keys/ek_private.cpp \
    src/wallet/keys/ek_public.cpp \
    src/wallet/keys/ek_token.cpp \
    src/wallet/keys/encrypted_keys.cpp \
    src/wallet/keys/hd_private.cpp \
    src/wallet/keys/hd_public.cpp \
    src/wallet/keys/mini_keys.cpp \
    src/wallet/keys/parse_encrypted_keys/parse_encrypted_key.hpp \
    src/wallet/keys/parse_encrypted_keys/parse_encrypted_key.ipp \
    src/wallet/keys/parse_encrypted_keys/parse_encrypted_prefix.hpp \
    src/wallet/keys/parse_encrypted_keys/parse_encrypted_prefix.ipp \
    src/wallet/keys/parse_encrypted_keys/parse_encrypted_private.cpp \
    src/wallet/keys/parse_encrypted_keys/parse_encrypted_private.hpp \
    src/wallet/keys/parse_encrypted_keys/parse_encrypted_public.cpp \
    src/wallet/keys/parse_encrypted_keys/parse_encrypted_public.hpp \
    src/wallet/keys/parse_encrypted_keys/parse_encrypted_token.cpp \
    src/wallet/keys/parse_encrypted_keys/parse_encrypted_token.hpp \
    src/wallet/mnemonics/electrum.cpp \
    src/wallet/mnemonics/electrum_v1.cpp \
    src/wallet/mnemonics/languages.cpp \
    src/wallet/mnemonics/mnemonic.cpp \
    src/wallet/mnemonics/dictionaries/electrum.cpp \
    src/wallet/mnemonics/dictionaries/electrum_v1.cpp \
    src/wallet/mnemonics/dictionaries/mnemonic.cpp

# local: examples/libbitcoin-system-examples
#------------------------------------------------------------------------------
if WITH_EXAMPLES

noinst_PROGRAMS = examples/libbitcoin-system-examples
examples_libbitcoin_system_examples_CPPFLAGS = -I${srcdir}/include ${icu} ${boost_BUILD_CPPFLAGS} ${pthread_BUILD_CPPFLAGS} ${icu_i18n_BUILD_CPPFLAGS} ${secp256k1_BUILD_CPPFLAGS}
examples_libbitcoin_system_examples_LDFLAGS = ${boost_LDFLAGS}
examples_libbitcoin_system_examples_LDADD = src/libbitcoin-system.la ${boost_chrono_LIBS} ${boost_date_time_LIBS} ${boost_filesystem_LIBS} ${boost_iostreams_LIBS} ${boost_locale_LIBS} ${boost_log_LIBS} ${boost_program_options_LIBS} ${boost_regex_LIBS} ${boost_system_LIBS} ${boost_thread_LIBS} ${pthread_LIBS} ${rt_LIBS} ${icu_i18n_LIBS} ${dl_LIBS} ${secp256k1_LIBS}
examples_libbitcoin_system_examples_SOURCES = \
    examples/main.cpp

endif WITH_EXAMPLES

# local: test/libbitcoin-system-test
#------------------------------------------------------------------------------
if WITH_TESTS

TESTS = libbitcoin-system-test_runner.sh

check_PROGRAMS = test/libbitcoin-system-test
test_libbitcoin_system_test_CPPFLAGS = -I${srcdir}/include ${icu} ${boost_BUILD_CPPFLAGS} ${pthread_BUILD_CPPFLAGS} ${icu_i18n_BUILD_CPPFLAGS} ${secp256k1_BUILD_CPPFLAGS}
test_libbitcoin_system_test_LDFLAGS = ${boost_LDFLAGS}
test_libbitcoin_system_test_LDADD = src/libbitcoin-system.la ${boost_unit_test_framework_LIBS} ${boost_chrono_LIBS} ${boost_date_time_LIBS} ${boost_filesystem_LIBS} ${boost_iostreams_LIBS} ${boost_locale_LIBS} ${boost_log_LIBS} ${boost_program_options_LIBS} ${boost_regex_LIBS} ${boost_system_LIBS} ${boost_thread_LIBS} ${pthread_LIBS} ${rt_LIBS} ${icu_i18n_LIBS} ${dl_LIBS} ${secp256k1_LIBS}
test_libbitcoin_system_test_SOURCES = \
    test/main.cpp \
    test/settings.cpp \
    test/test.cpp \
    test/test.hpp \
    test/chain/block.cpp \
    test/chain/block_filter.cpp \
    test/chain/chain_state.cpp \
    test/chain/compact.cpp \
    test/chain/header.cpp \
    test/chain/input.cpp \
    test/chain/output.cpp \
    test/chain/output_point.cpp \
    test/chain/payment_record.cpp \
    test/chain/point.cpp \
    test/chain/point_value.cpp \
    test/chain/points_value.cpp \
    test/chain/satoshi_words.cpp \
    test/chain/script.cpp \
    test/chain/script.hpp \
    test/chain/stealth_record.cpp \
    test/chain/transaction.cpp \
    test/config/authority.cpp \
    test/config/base16.cpp \
    test/config/base2.cpp \
    test/config/base32.cpp \
    test/config/base58.cpp \
    test/config/base64.cpp \
    test/config/base85.cpp \
    test/config/block.cpp \
    test/config/checkpoint.cpp \
    test/config/compact_filter.cpp \
    test/config/endpoint.cpp \
    test/config/hash256.cpp \
    test/config/parameter.cpp \
    test/config/printer.cpp \
    test/formats/base_10.cpp \
    test/formats/base_16.cpp \
    test/formats/base_32.cpp \
    test/formats/base_58.cpp \
    test/formats/base_64.cpp \
    test/formats/base_85.cpp \
    test/machine/number.cpp \
    test/machine/number.hpp \
    test/machine/opcode.cpp \
    test/machine/operation.cpp \
    test/math/checksum.cpp \
    test/math/ec_point.cpp \
    test/math/ec_scalar.cpp \
    test/math/elliptic_curve.cpp \
    test/math/hash.cpp \
    test/math/hash.hpp \
    test/math/limits.cpp \
    test/math/math.cpp \
    test/math/ring_signature.cpp \
    test/math/siphash.cpp \
    test/math/siphash.hpp \
    test/math/stealth.cpp \
    test/math/uint256.cpp \
    test/message/address.cpp \
    test/message/alert.cpp \
    test/message/alert_payload.cpp \
    test/message/block.cpp \
    test/message/block_transactions.cpp \
    test/message/compact_block.cpp \
    test/message/compact_filter.cpp \
    test/message/compact_filter_checkpoint.cpp \
    test/message/compact_filter_headers.cpp \
    test/message/fee_filter.cpp \
    test/message/filter_add.cpp \
    test/message/filter_clear.cpp \
    test/message/filter_load.cpp \
    test/message/get_address.cpp \
    test/message/get_block_transactions.cpp \
    test/message/get_blocks.cpp \
    test/message/get_compact_filter_checkpoint.cpp \
    test/message/get_compact_filter_headers.cpp \
    test/message/get_compact_filters.cpp \
    test/message/get_data.cpp \
    test/message/get_headers.cpp \
    test/message/header.cpp \
    test/message/headers.cpp \
    test/message/heading.cpp \
    test/message/inventory.cpp \
    test/message/inventory_vector.cpp \
    test/message/memory_pool.cpp \
    test/message/merkle_block.cpp \
    test/message/messages.cpp \
    test/message/network_address.cpp \
    test/message/not_found.cpp \
    test/message/ping.cpp \
    test/message/pong.cpp \
    test/message/prefilled_transaction.cpp \
    test/message/reject.cpp \
    test/message/send_compact.cpp \
    test/message/send_headers.cpp \
    test/message/transaction.cpp \
    test/message/verack.cpp \
    test/message/version.cpp \
    test/unicode/unicode.cpp \
    test/unicode/unicode_istream.cpp \
    test/unicode/unicode_ostream.cpp \
    test/utility/binary.cpp \
    test/utility/bit_stream.cpp \
    test/utility/collection.cpp \
    test/utility/data.cpp \
    test/utility/data_slice.cpp \
    test/utility/deserialize.cpp \
    test/utility/endian.cpp \
    test/utility/neutrino_filter.cpp \
    test/utility/property_tree.cpp \
    test/utility/pseudo_random.cpp \
    test/utility/serialize.cpp \
    test/utility/serializer.cpp \
    test/utility/stream.cpp \
    test/utility/string.cpp \
    test/utility/thread.cpp \
    test/utility/tiff.cpp \
    test/wallet/message.cpp \
    test/wallet/addresses/bitcoin_uri.cpp \
    test/wallet/addresses/checked.cpp \
    test/wallet/addresses/payment_address.cpp \
    test/wallet/addresses/qr_code.cpp \
    test/wallet/addresses/stealth_address.cpp \
    test/wallet/addresses/stealth_receiver.cpp \
    test/wallet/addresses/stealth_sender.cpp \
    test/wallet/addresses/uri.cpp \
    test/wallet/addresses/uri_reader.cpp \
    test/wallet/addresses/witness_address.cpp \
    test/wallet/keys/ec_private.cpp \
    test/wallet/keys/ec_public.cpp \
    test/wallet/keys/encrypted_keys.cpp \
    test/wallet/keys/hd_private.cpp \
    test/wallet/keys/hd_public.cpp \
    test/wallet/mnemonics/dictionaries.cpp \
    test/wallet/mnemonics/dictionaries.hpp \
    test/wallet/mnemonics/dictionary.cpp \
    test/wallet/mnemonics/dictionary.hpp \
    test/wallet/mnemonics/electrum.cpp \
    test/wallet/mnemonics/electrum.hpp \
    test/wallet/mnemonics/electrum_v1.cpp \
    test/wallet/mnemonics/electrum_v1.hpp \
    test/wallet/mnemonics/languages.cpp \
    test/wallet/mnemonics/languages.hpp \
    test/wallet/mnemonics/mnemonic.cpp \
    test/wallet/mnemonics/mnemonic.hpp \
    test/wallet/mnemonics/dictionaries/electrum.cpp \
    test/wallet/mnemonics/dictionaries/electrum.hpp \
    test/wallet/mnemonics/dictionaries/electrum_v1.cpp \
    test/wallet/mnemonics/dictionaries/electrum_v1.hpp \
    test/wallet/mnemonics/dictionaries/mnemonic.cpp \
    test/wallet/mnemonics/dictionaries/mnemonic.hpp

endif WITH_TESTS

# files => ${includedir}/bitcoin
#------------------------------------------------------------------------------
include_bitcoindir = ${includedir}/bitcoin
include_bitcoin_HEADERS = \
    include/bitcoin/system.hpp

include_bitcoin_systemdir = ${includedir}/bitcoin/system
include_bitcoin_system_HEADERS = \
    include/bitcoin/system/assert.hpp \
    include/bitcoin/system/constants.hpp \
    include/bitcoin/system/define.hpp \
    include/bitcoin/system/error.hpp \
    include/bitcoin/system/settings.hpp \
    include/bitcoin/system/version.hpp

include_bitcoin_system_chaindir = ${includedir}/bitcoin/system/chain
include_bitcoin_system_chain_HEADERS = \
    include/bitcoin/system/chain/block.hpp \
    include/bitcoin/system/chain/block_filter.hpp \
    include/bitcoin/system/chain/chain_state.hpp \
    include/bitcoin/system/chain/compact.hpp \
    include/bitcoin/system/chain/header.hpp \
    include/bitcoin/system/chain/input.hpp \
    include/bitcoin/system/chain/input_point.hpp \
    include/bitcoin/system/chain/output.hpp \
    include/bitcoin/system/chain/output_point.hpp \
    include/bitcoin/system/chain/payment_record.hpp \
    include/bitcoin/system/chain/point.hpp \
    include/bitcoin/system/chain/point_value.hpp \
    include/bitcoin/system/chain/points_value.hpp \
    include/bitcoin/system/chain/script.hpp \
    include/bitcoin/system/chain/stealth_record.hpp \
    include/bitcoin/system/chain/transaction.hpp \
    include/bitcoin/system/chain/witness.hpp

include_bitcoin_system_configdir = ${includedir}/bitcoin/system/config
include_bitcoin_system_config_HEADERS = \
    include/bitcoin/system/config/authority.hpp \
    include/bitcoin/system/config/base16.hpp \
    include/bitcoin/system/config/base2.hpp \
    include/bitcoin/system/config/base32.hpp \
    include/bitcoin/system/config/base58.hpp \
    include/bitcoin/system/config/base64.hpp \
    include/bitcoin/system/config/base85.hpp \
    include/bitcoin/system/config/block.hpp \
    include/bitcoin/system/config/checkpoint.hpp \
    include/bitcoin/system/config/compact_filter.hpp \
    include/bitcoin/system/config/directory.hpp \
    include/bitcoin/system/config/endpoint.hpp \
    include/bitcoin/system/config/hash160.hpp \
    include/bitcoin/system/config/hash256.hpp \
    include/bitcoin/system/config/header.hpp \
    include/bitcoin/system/config/input.hpp \
    include/bitcoin/system/config/output.hpp \
    include/bitcoin/system/config/parameter.hpp \
    include/bitcoin/system/config/parser.hpp \
    include/bitcoin/system/config/point.hpp \
    include/bitcoin/system/config/printer.hpp \
    include/bitcoin/system/config/script.hpp \
    include/bitcoin/system/config/settings.hpp \
    include/bitcoin/system/config/transaction.hpp

include_bitcoin_system_formatsdir = ${includedir}/bitcoin/system/formats
include_bitcoin_system_formats_HEADERS = \
    include/bitcoin/system/formats/base_10.hpp \
    include/bitcoin/system/formats/base_16.hpp \
    include/bitcoin/system/formats/base_32.hpp \
    include/bitcoin/system/formats/base_58.hpp \
    include/bitcoin/system/formats/base_64.hpp \
    include/bitcoin/system/formats/base_85.hpp

include_bitcoin_system_impl_formatsdir = ${includedir}/bitcoin/system/impl/formats
include_bitcoin_system_impl_formats_HEADERS = \
    include/bitcoin/system/impl/formats/base_16.ipp \
    include/bitcoin/system/impl/formats/base_58.ipp

include_bitcoin_system_impl_log_featuresdir = ${includedir}/bitcoin/system/impl/log/features
include_bitcoin_system_impl_log_features_HEADERS = \
    include/bitcoin/system/impl/log/features/counter.ipp \
    include/bitcoin/system/impl/log/features/gauge.ipp \
    include/bitcoin/system/impl/log/features/metric.ipp \
    include/bitcoin/system/impl/log/features/rate.ipp \
    include/bitcoin/system/impl/log/features/timer.ipp

include_bitcoin_system_impl_machinedir = ${includedir}/bitcoin/system/impl/machine
include_bitcoin_system_impl_machine_HEADERS = \
    include/bitcoin/system/impl/machine/interpreter.ipp \
    include/bitcoin/system/impl/machine/number.ipp \
    include/bitcoin/system/impl/machine/operation.ipp \
    include/bitcoin/system/impl/machine/program.ipp

include_bitcoin_system_impl_mathdir = ${includedir}/bitcoin/system/impl/math
include_bitcoin_system_impl_math_HEADERS = \
    include/bitcoin/system/impl/math/checksum.ipp \
    include/bitcoin/system/impl/math/hash.ipp \
    include/bitcoin/system/impl/math/math.ipp

include_bitcoin_system_impl_utilitydir = ${includedir}/bitcoin/system/impl/utility
include_bitcoin_system_impl_utility_HEADERS = \
    include/bitcoin/system/impl/utility/collection.ipp \
    include/bitcoin/system/impl/utility/data.ipp \
    include/bitcoin/system/impl/utility/data_slice.ipp \
    include/bitcoin/system/impl/utility/deserialize.ipp \
    include/bitcoin/system/impl/utility/deserializer.ipp \
    include/bitcoin/system/impl/utility/endian.ipp \
    include/bitcoin/system/impl/utility/istream_bit_reader.ipp \
    include/bitcoin/system/impl/utility/istream_reader.ipp \
    include/bitcoin/system/impl/utility/ostream_bit_writer.ipp \
    include/bitcoin/system/impl/utility/ostream_writer.ipp \
    include/bitcoin/system/impl/utility/pending.ipp \
    include/bitcoin/system/impl/utility/property_tree.ipp \
    include/bitcoin/system/impl/utility/resubscriber.ipp \
    include/bitcoin/system/impl/utility/serialize.ipp \
    include/bitcoin/system/impl/utility/serializer.ipp \
    include/bitcoin/system/impl/utility/subscriber.ipp \
    include/bitcoin/system/impl/utility/track.ipp

include_bitcoin_system_impl_wallet_addressesdir = ${includedir}/bitcoin/system/impl/wallet/addresses
include_bitcoin_system_impl_wallet_addresses_HEADERS = \
    include/bitcoin/system/impl/wallet/addresses/checked.ipp

include_bitcoin_system_impl_wallet_mnemonicsdir = ${includedir}/bitcoin/system/impl/wallet/mnemonics
include_bitcoin_system_impl_wallet_mnemonics_HEADERS = \
    include/bitcoin/system/impl/wallet/mnemonics/dictionaries.ipp \
    include/bitcoin/system/impl/wallet/mnemonics/dictionary.ipp

include_bitcoin_system_logdir = ${includedir}/bitcoin/system/log
include_bitcoin_system_log_HEADERS = \
    include/bitcoin/system/log/attributes.hpp \
    include/bitcoin/system/log/file_char_traits.hpp \
    include/bitcoin/system/log/file_collector.hpp \
    include/bitcoin/system/log/file_collector_repository.hpp \
    include/bitcoin/system/log/file_counter_formatter.hpp \
    include/bitcoin/system/log/rotable_file.hpp \
    include/bitcoin/system/log/severity.hpp \
    include/bitcoin/system/log/sink.hpp \
    include/bitcoin/system/log/source.hpp \
    include/bitcoin/system/log/statsd_sink.hpp \
    include/bitcoin/system/log/statsd_source.hpp \
    include/bitcoin/system/log/udp_client_sink.hpp

include_bitcoin_system_log_featuresdir = ${includedir}/bitcoin/system/log/features
include_bitcoin_system_log_features_HEADERS = \
    include/bitcoin/system/log/features/counter.hpp \
    include/bitcoin/system/log/features/gauge.hpp \
    include/bitcoin/system/log/features/metric.hpp \
    include/bitcoin/system/log/features/rate.hpp \
    include/bitcoin/system/log/features/timer.hpp

include_bitcoin_system_machinedir = ${includedir}/bitcoin/system/machine
include_bitcoin_system_machine_HEADERS = \
    include/bitcoin/system/machine/interpreter.hpp \
    include/bitcoin/system/machine/number.hpp \
    include/bitcoin/system/machine/opcode.hpp \
    include/bitcoin/system/machine/operation.hpp \
    include/bitcoin/system/machine/program.hpp \
    include/bitcoin/system/machine/rule_fork.hpp \
    include/bitcoin/system/machine/script_pattern.hpp \
    include/bitcoin/system/machine/script_version.hpp \
    include/bitcoin/system/machine/sighash_algorithm.hpp

include_bitcoin_system_mathdir = ${includedir}/bitcoin/system/math
include_bitcoin_system_math_HEADERS = \
    include/bitcoin/system/math/checksum.hpp \
    include/bitcoin/system/math/crypto.hpp \
    include/bitcoin/system/math/ec_point.hpp \
    include/bitcoin/system/math/ec_scalar.hpp \
    include/bitcoin/system/math/elliptic_curve.hpp \
    include/bitcoin/system/math/golomb_coded_sets.hpp \
    include/bitcoin/system/math/hash.hpp \
    include/bitcoin/system/math/limits.hpp \
    include/bitcoin/system/math/math.hpp \
    include/bitcoin/system/math/ring_signature.hpp \
    include/bitcoin/system/math/siphash.hpp \
    include/bitcoin/system/math/stealth.hpp

include_bitcoin_system_messagedir = ${includedir}/bitcoin/system/message
include_bitcoin_system_message_HEADERS = \
    include/bitcoin/system/message/address.hpp \
    include/bitcoin/system/message/alert.hpp \
    include/bitcoin/system/message/alert_payload.hpp \
    include/bitcoin/system/message/block.hpp \
    include/bitcoin/system/message/block_transactions.hpp \
    include/bitcoin/system/message/compact_block.hpp \
    include/bitcoin/system/message/compact_filter.hpp \
    include/bitcoin/system/message/compact_filter_checkpoint.hpp \
    include/bitcoin/system/message/compact_filter_headers.hpp \
    include/bitcoin/system/message/fee_filter.hpp \
    include/bitcoin/system/message/filter_add.hpp \
    include/bitcoin/system/message/filter_clear.hpp \
    include/bitcoin/system/message/filter_load.hpp \
    include/bitcoin/system/message/get_address.hpp \
    include/bitcoin/system/message/get_block_transactions.hpp \
    include/bitcoin/system/message/get_blocks.hpp \
    include/bitcoin/system/message/get_compact_filter_checkpoint.hpp \
    include/bitcoin/system/message/get_compact_filter_headers.hpp \
    include/bitcoin/system/message/get_compact_filters.hpp \
    include/bitcoin/system/message/get_data.hpp \
    include/bitcoin/system/message/get_headers.hpp \
    include/bitcoin/system/message/header.hpp \
    include/bitcoin/system/message/headers.hpp \
    include/bitcoin/system/message/heading.hpp \
    include/bitcoin/system/message/inventory.hpp \
    include/bitcoin/system/message/inventory_vector.hpp \
    include/bitcoin/system/message/memory_pool.hpp \
    include/bitcoin/system/message/merkle_block.hpp \
    include/bitcoin/system/message/messages.hpp \
    include/bitcoin/system/message/network_address.hpp \
    include/bitcoin/system/message/not_found.hpp \
    include/bitcoin/system/message/ping.hpp \
    include/bitcoin/system/message/pong.hpp \
    include/bitcoin/system/message/prefilled_transaction.hpp \
    include/bitcoin/system/message/reject.hpp \
    include/bitcoin/system/message/send_compact.hpp \
    include/bitcoin/system/message/send_headers.hpp \
    include/bitcoin/system/message/transaction.hpp \
    include/bitcoin/system/message/verack.hpp \
    include/bitcoin/system/message/version.hpp

include_bitcoin_system_unicodedir = ${includedir}/bitcoin/system/unicode
include_bitcoin_system_unicode_HEADERS = \
    include/bitcoin/system/unicode/console_streambuf.hpp \
    include/bitcoin/system/unicode/file_lock.hpp \
    include/bitcoin/system/unicode/ifstream.hpp \
    include/bitcoin/system/unicode/ofstream.hpp \
    include/bitcoin/system/unicode/unicode.hpp \
    include/bitcoin/system/unicode/unicode_istream.hpp \
    include/bitcoin/system/unicode/unicode_ostream.hpp \
    include/bitcoin/system/unicode/unicode_streambuf.hpp

include_bitcoin_system_utilitydir = ${includedir}/bitcoin/system/utility
include_bitcoin_system_utility_HEADERS = \
    include/bitcoin/system/utility/asio.hpp \
    include/bitcoin/system/utility/atomic.hpp \
    include/bitcoin/system/utility/binary.hpp \
    include/bitcoin/system/utility/bit_reader.hpp \
    include/bitcoin/system/utility/bit_writer.hpp \
    include/bitcoin/system/utility/collection.hpp \
    include/bitcoin/system/utility/conditional_lock.hpp \
    include/bitcoin/system/utility/container_sink.hpp \
    include/bitcoin/system/utility/container_source.hpp \
    include/bitcoin/system/utility/data.hpp \
    include/bitcoin/system/utility/data_slice.hpp \
    include/bitcoin/system/utility/deadline.hpp \
    include/bitcoin/system/utility/decorator.hpp \
    include/bitcoin/system/utility/delegates.hpp \
    include/bitcoin/system/utility/deserialize.hpp \
    include/bitcoin/system/utility/deserializer.hpp \
    include/bitcoin/system/utility/dispatcher.hpp \
    include/bitcoin/system/utility/enable_shared_from_base.hpp \
    include/bitcoin/system/utility/endian.hpp \
    include/bitcoin/system/utility/exceptions.hpp \
    include/bitcoin/system/utility/flush_lock.hpp \
    include/bitcoin/system/utility/handlers.hpp \
    include/bitcoin/system/utility/interprocess_lock.hpp \
    include/bitcoin/system/utility/istream_bit_reader.hpp \
    include/bitcoin/system/utility/istream_reader.hpp \
    include/bitcoin/system/utility/monitor.hpp \
    include/bitcoin/system/utility/neutrino_filter.hpp \
    include/bitcoin/system/utility/noncopyable.hpp \
    include/bitcoin/system/utility/ostream_bit_writer.hpp \
    include/bitcoin/system/utility/ostream_writer.hpp \
    include/bitcoin/system/utility/pending.hpp \
    include/bitcoin/system/utility/prioritized_mutex.hpp \
    include/bitcoin/system/utility/property_tree.hpp \
    include/bitcoin/system/utility/pseudo_random.hpp \
    include/bitcoin/system/utility/reader.hpp \
    include/bitcoin/system/utility/resubscriber.hpp \
    include/bitcoin/system/utility/scope_lock.hpp \
    include/bitcoin/system/utility/sequencer.hpp \
    include/bitcoin/system/utility/sequential_lock.hpp \
    include/bitcoin/system/utility/serialize.hpp \
    include/bitcoin/system/utility/serializer.hpp \
    include/bitcoin/system/utility/socket.hpp \
    include/bitcoin/system/utility/string.hpp \
    include/bitcoin/system/utility/subscriber.hpp \
    include/bitcoin/system/utility/synchronizer.hpp \
    include/bitcoin/system/utility/thread.hpp \
    include/bitcoin/system/utility/threadpool.hpp \
    include/bitcoin/system/utility/tiff.hpp \
    include/bitcoin/system/utility/timer.hpp \
    include/bitcoin/system/utility/track.hpp \
    include/bitcoin/system/utility/work.hpp \
    include/bitcoin/system/utility/writer.hpp

include_bitcoin_system_walletdir = ${includedir}/bitcoin/system/wallet
include_bitcoin_system_wallet_HEADERS = \
    include/bitcoin/system/wallet/message.hpp

include_bitcoin_system_wallet_addressesdir = ${includedir}/bitcoin/system/wallet/addresses
include_bitcoin_system_wallet_addresses_HEADERS = \
    include/bitcoin/system/wallet/addresses/bitcoin_uri.hpp \
    include/bitcoin/system/wallet/addresses/checked.hpp \
    include/bitcoin/system/wallet/addresses/payment_address.hpp \
    include/bitcoin/system/wallet/addresses/qr_code.hpp \
    include/bitcoin/system/wallet/addresses/stealth_address.hpp \
    include/bitcoin/system/wallet/addresses/stealth_receiver.hpp \
    include/bitcoin/system/wallet/addresses/stealth_sender.hpp

include_bitcoin_system_wallet_keysdir = ${includedir}/bitcoin/system/wallet/keys
include_bitcoin_system_wallet_keys_HEADERS = \
    include/bitcoin/system/wallet/keys/ec_private.hpp \
    include/bitcoin/system/wallet/keys/ec_public.hpp \
    include/bitcoin/system/wallet/keys/ek_private.hpp \
    include/bitcoin/system/wallet/keys/ek_public.hpp \
    include/bitcoin/system/wallet/keys/ek_token.hpp \
    include/bitcoin/system/wallet/keys/encrypted_keys.hpp \
    include/bitcoin/system/wallet/keys/hd_private.hpp \
    include/bitcoin/system/wallet/keys/hd_public.hpp \
    include/bitcoin/system/wallet/keys/mini_keys.hpp \
    include/bitcoin/system/wallet/keys/uri.hpp \
    include/bitcoin/system/wallet/keys/uri_reader.hpp \
    include/bitcoin/system/wallet/keys/witness_address.hpp

include_bitcoin_system_wallet_mnemonicsdir = ${includedir}/bitcoin/system/wallet/mnemonics
include_bitcoin_system_wallet_mnemonics_HEADERS = \
    include/bitcoin/system/wallet/mnemonics/dictionaries.hpp \
    include/bitcoin/system/wallet/mnemonics/dictionary.hpp \
    include/bitcoin/system/wallet/mnemonics/electrum.hpp \
    include/bitcoin/system/wallet/mnemonics/electrum_v1.hpp \
    include/bitcoin/system/wallet/mnemonics/language.hpp \
    include/bitcoin/system/wallet/mnemonics/languages.hpp \
    include/bitcoin/system/wallet/mnemonics/mnemonic.hpp


# Custom make targets.
#==============================================================================
# make target: examples
#------------------------------------------------------------------------------
target_examples = \
    examples/libbitcoin-system-examples

examples: ${target_examples}

