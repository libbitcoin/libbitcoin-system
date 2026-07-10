###############################################################################
#  Copyright (c) 2014-2026 libbitcoin-system developers (see COPYING).
#
#         GENERATED SOURCE CODE, DO NOT EDIT EXCEPT EXPERIMENTALLY
#
###############################################################################

# Automake settings.
#==============================================================================
# Look for macros in the 'm4' subdirectory.
#------------------------------------------------------------------------------
ACLOCAL_AMFLAGS = -I m4

# Distribute data.
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

# Libraries.
#==============================================================================
# Target library 'src/libbitcoin-system.la'
#------------------------------------------------------------------------------
lib_LTLIBRARIES = src/libbitcoin-system.la

src_libbitcoin_system_la_LIBS = src/libbitcoin-system.la

src_libbitcoin_system_la_CPPFLAGS = \
    -I${srcdir}/../../include \
    ${boost_BUILD_CPPFLAGS} \
    ${boost_container_BUILD_CPPFLAGS} \
    ${boost_iostreams_BUILD_CPPFLAGS} \
    ${boost_json_BUILD_CPPFLAGS} \
    ${boost_locale_BUILD_CPPFLAGS} \
    ${boost_program_options_BUILD_CPPFLAGS} \
    ${boost_thread_BUILD_CPPFLAGS} \
    ${boost_url_BUILD_CPPFLAGS} \
    ${secp256k1_BUILD_CPPFLAGS} \
    ${UltrafastSecp256k1_BUILD_CPPFLAGS}

src_libbitcoin_system_la_LDFLAGS = \
    ${boost_LDFLAGS} \
    ${boost_container_LDFLAGS} \
    ${boost_iostreams_LDFLAGS} \
    ${boost_json_LDFLAGS} \
    ${boost_locale_LDFLAGS} \
    ${boost_program_options_LDFLAGS} \
    ${boost_thread_LDFLAGS} \
    ${boost_url_LDFLAGS} \
    ${secp256k1_LDFLAGS} \
    ${UltrafastSecp256k1_LDFLAGS}

src_libbitcoin_system_la_LIBADD = \
    ${boost_LIBS} \
    ${boost_container_LIBS} \
    ${boost_iostreams_LIBS} \
    ${boost_json_LIBS} \
    ${boost_locale_LIBS} \
    ${boost_program_options_LIBS} \
    ${boost_thread_LIBS} \
    ${boost_url_LIBS} \
    ${secp256k1_LIBS} \
    ${UltrafastSecp256k1_LIBS}

src_libbitcoin_system_la_SOURCES = \
    ${srcdir}/../../src/arena.cpp \
    ${srcdir}/../../src/define.cpp \
    ${srcdir}/../../src/settings.cpp \
    ${srcdir}/../../src/chain/block.cpp \
    ${srcdir}/../../src/chain/chain_state.cpp \
    ${srcdir}/../../src/chain/checkpoint.cpp \
    ${srcdir}/../../src/chain/context.cpp \
    ${srcdir}/../../src/chain/header.cpp \
    ${srcdir}/../../src/chain/input.cpp \
    ${srcdir}/../../src/chain/operation.cpp \
    ${srcdir}/../../src/chain/outpoint.cpp \
    ${srcdir}/../../src/chain/output.cpp \
    ${srcdir}/../../src/chain/point.cpp \
    ${srcdir}/../../src/chain/script.cpp \
    ${srcdir}/../../src/chain/script_extract.cpp \
    ${srcdir}/../../src/chain/taproot.cpp \
    ${srcdir}/../../src/chain/transaction.cpp \
    ${srcdir}/../../src/chain/transaction_cache.cpp \
    ${srcdir}/../../src/chain/transaction_sighash.cpp \
    ${srcdir}/../../src/chain/transaction_sighash_v0.cpp \
    ${srcdir}/../../src/chain/transaction_sighash_v1.cpp \
    ${srcdir}/../../src/chain/witness.cpp \
    ${srcdir}/../../src/chain/witness_extract.cpp \
    ${srcdir}/../../src/chain/enums/opcode.cpp \
    ${srcdir}/../../src/chain/json/block.cpp \
    ${srcdir}/../../src/chain/json/checkpoint.cpp \
    ${srcdir}/../../src/chain/json/header.cpp \
    ${srcdir}/../../src/chain/json/input.cpp \
    ${srcdir}/../../src/chain/json/operation.cpp \
    ${srcdir}/../../src/chain/json/outpoint.cpp \
    ${srcdir}/../../src/chain/json/output.cpp \
    ${srcdir}/../../src/chain/json/point.cpp \
    ${srcdir}/../../src/chain/json/script.cpp \
    ${srcdir}/../../src/chain/json/transaction.cpp \
    ${srcdir}/../../src/chain/json/witness.cpp \
    ${srcdir}/../../src/chain/views/block_view.cpp \
    ${srcdir}/../../src/chain/views/transaction_view.cpp \
    ${srcdir}/../../src/config/authority.cpp \
    ${srcdir}/../../src/config/base16.cpp \
    ${srcdir}/../../src/config/base2.cpp \
    ${srcdir}/../../src/config/base32.cpp \
    ${srcdir}/../../src/config/base58.cpp \
    ${srcdir}/../../src/config/base64.cpp \
    ${srcdir}/../../src/config/base85.cpp \
    ${srcdir}/../../src/config/block.cpp \
    ${srcdir}/../../src/config/endpoint.cpp \
    ${srcdir}/../../src/config/hash160.cpp \
    ${srcdir}/../../src/config/hash256.cpp \
    ${srcdir}/../../src/config/header.cpp \
    ${srcdir}/../../src/config/input.cpp \
    ${srcdir}/../../src/config/output.cpp \
    ${srcdir}/../../src/config/parameter.cpp \
    ${srcdir}/../../src/config/parser.cpp \
    ${srcdir}/../../src/config/point.cpp \
    ${srcdir}/../../src/config/printer.cpp \
    ${srcdir}/../../src/config/script.cpp \
    ${srcdir}/../../src/config/transaction.cpp \
    ${srcdir}/../../src/config/url.cpp \
    ${srcdir}/../../src/config/utilities.cpp \
    ${srcdir}/../../src/config/version.cpp \
    ${srcdir}/../../src/crypto/aes256.cpp \
    ${srcdir}/../../src/crypto/der_parser.cpp \
    ${srcdir}/../../src/crypto/pseudo_random.cpp \
    ${srcdir}/../../src/crypto/ring_signature.cpp \
    ${srcdir}/../../src/crypto/secp256k1/batch.cpp \
    ${srcdir}/../../src/crypto/secp256k1/ecdsa.cpp \
    ${srcdir}/../../src/crypto/secp256k1/ec_context.cpp \
    ${srcdir}/../../src/crypto/secp256k1/keys.cpp \
    ${srcdir}/../../src/crypto/secp256k1/math.cpp \
    ${srcdir}/../../src/crypto/secp256k1/recover.cpp \
    ${srcdir}/../../src/crypto/secp256k1/schnorr.cpp \
    ${srcdir}/../../src/data/data_chunk.cpp \
    ${srcdir}/../../src/data/string.cpp \
    ${srcdir}/../../src/endian/endian.cpp \
    ${srcdir}/../../src/error/block_error_t.cpp \
    ${srcdir}/../../src/error/error_t.cpp \
    ${srcdir}/../../src/error/errorno_t.cpp \
    ${srcdir}/../../src/error/op_error_t.cpp \
    ${srcdir}/../../src/error/script_error_t.cpp \
    ${srcdir}/../../src/error/transaction_error_t.cpp \
    ${srcdir}/../../src/filter/golomb.cpp \
    ${srcdir}/../../src/hash/accumulator.cpp \
    ${srcdir}/../../src/hash/checksum.cpp \
    ${srcdir}/../../src/hash/siphash.cpp \
    ${srcdir}/../../src/math/math.cpp \
    ${srcdir}/../../src/radix/base_10.cpp \
    ${srcdir}/../../src/radix/base_2048.cpp \
    ${srcdir}/../../src/radix/base_32.cpp \
    ${srcdir}/../../src/radix/base_58.cpp \
    ${srcdir}/../../src/radix/base_64.cpp \
    ${srcdir}/../../src/radix/base_85.cpp \
    ${srcdir}/../../src/serial/props.cpp \
    ${srcdir}/../../src/stream/binary.cpp \
    ${srcdir}/../../src/unicode/ascii.cpp \
    ${srcdir}/../../src/unicode/code_points.cpp \
    ${srcdir}/../../src/unicode/conversion.cpp \
    ${srcdir}/../../src/unicode/normalization.cpp \
    ${srcdir}/../../src/unicode/unicode_tables.cpp \
    ${srcdir}/../../src/unicode/utf8_everywhere/console_streambuf.cpp \
    ${srcdir}/../../src/unicode/utf8_everywhere/environment.cpp \
    ${srcdir}/../../src/unicode/utf8_everywhere/ifstream.cpp \
    ${srcdir}/../../src/unicode/utf8_everywhere/ofstream.cpp \
    ${srcdir}/../../src/unicode/utf8_everywhere/paths.cpp \
    ${srcdir}/../../src/unicode/utf8_everywhere/stdio.cpp \
    ${srcdir}/../../src/unicode/utf8_everywhere/unicode_istream.cpp \
    ${srcdir}/../../src/unicode/utf8_everywhere/unicode_ostream.cpp \
    ${srcdir}/../../src/unicode/utf8_everywhere/unicode_streambuf.cpp \
    ${srcdir}/../../src/wallet/context.cpp \
    ${srcdir}/../../src/wallet/message.cpp \
    ${srcdir}/../../src/wallet/neutrino.cpp \
    ${srcdir}/../../src/wallet/point_value.cpp \
    ${srcdir}/../../src/wallet/points_value.cpp \
    ${srcdir}/../../src/wallet/addresses/bitcoin_uri.cpp \
    ${srcdir}/../../src/wallet/addresses/payment_address.cpp \
    ${srcdir}/../../src/wallet/addresses/qr_code.cpp \
    ${srcdir}/../../src/wallet/addresses/stealth_address.cpp \
    ${srcdir}/../../src/wallet/addresses/stealth_receiver.cpp \
    ${srcdir}/../../src/wallet/addresses/stealth_sender.cpp \
    ${srcdir}/../../src/wallet/addresses/tiff.cpp \
    ${srcdir}/../../src/wallet/addresses/uri.cpp \
    ${srcdir}/../../src/wallet/addresses/witness_address.cpp \
    ${srcdir}/../../src/wallet/addresses/qrencode/bitstream.c \
    ${srcdir}/../../src/wallet/addresses/qrencode/mask.c \
    ${srcdir}/../../src/wallet/addresses/qrencode/mmask.c \
    ${srcdir}/../../src/wallet/addresses/qrencode/mqrspec.c \
    ${srcdir}/../../src/wallet/addresses/qrencode/qrencode.c \
    ${srcdir}/../../src/wallet/addresses/qrencode/qrinput.c \
    ${srcdir}/../../src/wallet/addresses/qrencode/qrspec.c \
    ${srcdir}/../../src/wallet/addresses/qrencode/rsecc.c \
    ${srcdir}/../../src/wallet/addresses/qrencode/split.c \
    ${srcdir}/../../src/wallet/keys/ec_point.cpp \
    ${srcdir}/../../src/wallet/keys/ec_private.cpp \
    ${srcdir}/../../src/wallet/keys/ec_public.cpp \
    ${srcdir}/../../src/wallet/keys/ec_scalar.cpp \
    ${srcdir}/../../src/wallet/keys/ek_private.cpp \
    ${srcdir}/../../src/wallet/keys/ek_public.cpp \
    ${srcdir}/../../src/wallet/keys/ek_token.cpp \
    ${srcdir}/../../src/wallet/keys/encrypted_keys.cpp \
    ${srcdir}/../../src/wallet/keys/hd_private.cpp \
    ${srcdir}/../../src/wallet/keys/hd_public.cpp \
    ${srcdir}/../../src/wallet/keys/mini_keys.cpp \
    ${srcdir}/../../src/wallet/keys/stealth.cpp \
    ${srcdir}/../../src/wallet/keys/parse_encrypted_keys/parse_encrypted_private.cpp \
    ${srcdir}/../../src/wallet/keys/parse_encrypted_keys/parse_encrypted_public.cpp \
    ${srcdir}/../../src/wallet/keys/parse_encrypted_keys/parse_encrypted_token.cpp \
    ${srcdir}/../../src/wallet/mnemonics/electrum.cpp \
    ${srcdir}/../../src/wallet/mnemonics/electrum_v1.cpp \
    ${srcdir}/../../src/wallet/mnemonics/mnemonic.cpp \
    ${srcdir}/../../src/words/languages.cpp \
    ${srcdir}/../../src/words/catalogs/electrum.cpp \
    ${srcdir}/../../src/words/catalogs/electrum_v1.cpp \
    ${srcdir}/../../src/words/catalogs/mnemonic.cpp

include_bitcoindir = \
    ${includedir}/bitcoin

include_bitcoin_HEADERS = \
    ${srcdir}/../../include/bitcoin/system.hpp

include_bitcoin_systemdir = \
    ${includedir}/bitcoin/system

include_bitcoin_system_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/allocator.hpp \
    ${srcdir}/../../include/bitcoin/system/arena.hpp \
    ${srcdir}/../../include/bitcoin/system/boost.hpp \
    ${srcdir}/../../include/bitcoin/system/constants.hpp \
    ${srcdir}/../../include/bitcoin/system/constraints.hpp \
    ${srcdir}/../../include/bitcoin/system/define.hpp \
    ${srcdir}/../../include/bitcoin/system/exceptions.hpp \
    ${srcdir}/../../include/bitcoin/system/execution.hpp \
    ${srcdir}/../../include/bitcoin/system/forks.hpp \
    ${srcdir}/../../include/bitcoin/system/funclets.hpp \
    ${srcdir}/../../include/bitcoin/system/have.hpp \
    ${srcdir}/../../include/bitcoin/system/literals.hpp \
    ${srcdir}/../../include/bitcoin/system/preprocessor.hpp \
    ${srcdir}/../../include/bitcoin/system/settings.hpp \
    ${srcdir}/../../include/bitcoin/system/typelets.hpp \
    ${srcdir}/../../include/bitcoin/system/types.hpp \
    ${srcdir}/../../include/bitcoin/system/version.hpp \
    ${srcdir}/../../include/bitcoin/system/warnings.hpp

include_bitcoin_system_chaindir = \
    ${includedir}/bitcoin/system/chain

include_bitcoin_system_chain_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/chain/annex.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/block.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/chain.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/chain_state.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/checkpoint.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/compact.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/context.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/header.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/input.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/operation.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/outpoint.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/output.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/point.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/prevout.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/script.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/stripper.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/taproot.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/tapscript.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/transaction.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/witness.hpp

include_bitcoin_system_chain_batchdir = \
    ${includedir}/bitcoin/system/chain/batch

include_bitcoin_system_chain_batch_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/chain/batch/batch.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/batch/cursor.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/batch/multisig.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/batch/signatures.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/batch/threshold.hpp

include_bitcoin_system_chain_enumsdir = \
    ${includedir}/bitcoin/system/chain/enums

include_bitcoin_system_chain_enums_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/chain/enums/coverage.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/enums/enums.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/enums/extension.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/enums/flags.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/enums/key_version.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/enums/magic_numbers.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/enums/numbers.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/enums/opcode.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/enums/policy.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/enums/script_pattern.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/enums/script_version.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/enums/selection.hpp

include_bitcoin_system_chain_jsondir = \
    ${includedir}/bitcoin/system/chain/json

include_bitcoin_system_chain_json_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/chain/json/block.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/json/checkpoint.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/json/header.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/json/input.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/json/json.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/json/macros.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/json/operation.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/json/outpoint.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/json/output.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/json/point.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/json/script.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/json/transaction.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/json/witness.hpp

include_bitcoin_system_chain_viewsdir = \
    ${includedir}/bitcoin/system/chain/views

include_bitcoin_system_chain_views_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/chain/views/block_view.hpp \
    ${srcdir}/../../include/bitcoin/system/chain/views/transaction_view.hpp

include_bitcoin_system_configdir = \
    ${includedir}/bitcoin/system/config

include_bitcoin_system_config_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/config/authority.hpp \
    ${srcdir}/../../include/bitcoin/system/config/base16.hpp \
    ${srcdir}/../../include/bitcoin/system/config/base2.hpp \
    ${srcdir}/../../include/bitcoin/system/config/base32.hpp \
    ${srcdir}/../../include/bitcoin/system/config/base58.hpp \
    ${srcdir}/../../include/bitcoin/system/config/base64.hpp \
    ${srcdir}/../../include/bitcoin/system/config/base85.hpp \
    ${srcdir}/../../include/bitcoin/system/config/block.hpp \
    ${srcdir}/../../include/bitcoin/system/config/config.hpp \
    ${srcdir}/../../include/bitcoin/system/config/endpoint.hpp \
    ${srcdir}/../../include/bitcoin/system/config/hash160.hpp \
    ${srcdir}/../../include/bitcoin/system/config/hash256.hpp \
    ${srcdir}/../../include/bitcoin/system/config/header.hpp \
    ${srcdir}/../../include/bitcoin/system/config/input.hpp \
    ${srcdir}/../../include/bitcoin/system/config/output.hpp \
    ${srcdir}/../../include/bitcoin/system/config/parameter.hpp \
    ${srcdir}/../../include/bitcoin/system/config/parser.hpp \
    ${srcdir}/../../include/bitcoin/system/config/point.hpp \
    ${srcdir}/../../include/bitcoin/system/config/printer.hpp \
    ${srcdir}/../../include/bitcoin/system/config/script.hpp \
    ${srcdir}/../../include/bitcoin/system/config/transaction.hpp \
    ${srcdir}/../../include/bitcoin/system/config/url.hpp \
    ${srcdir}/../../include/bitcoin/system/config/utilities.hpp \
    ${srcdir}/../../include/bitcoin/system/config/version.hpp

include_bitcoin_system_cryptodir = \
    ${includedir}/bitcoin/system/crypto

include_bitcoin_system_crypto_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/crypto/aes256.hpp \
    ${srcdir}/../../include/bitcoin/system/crypto/crypto.hpp \
    ${srcdir}/../../include/bitcoin/system/crypto/der_parser.hpp \
    ${srcdir}/../../include/bitcoin/system/crypto/pseudo_random.hpp \
    ${srcdir}/../../include/bitcoin/system/crypto/ring_signature.hpp \
    ${srcdir}/../../include/bitcoin/system/crypto/secp256k1.hpp \
    ${srcdir}/../../include/bitcoin/system/crypto/secp256k1_batch.hpp

include_bitcoin_system_datadir = \
    ${includedir}/bitcoin/system/data

include_bitcoin_system_data_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/data/array_cast.hpp \
    ${srcdir}/../../include/bitcoin/system/data/byte_cast.hpp \
    ${srcdir}/../../include/bitcoin/system/data/collection.hpp \
    ${srcdir}/../../include/bitcoin/system/data/data.hpp \
    ${srcdir}/../../include/bitcoin/system/data/data_array.hpp \
    ${srcdir}/../../include/bitcoin/system/data/data_chunk.hpp \
    ${srcdir}/../../include/bitcoin/system/data/data_reference.hpp \
    ${srcdir}/../../include/bitcoin/system/data/data_slab.hpp \
    ${srcdir}/../../include/bitcoin/system/data/data_slice.hpp \
    ${srcdir}/../../include/bitcoin/system/data/exclusive_slice.hpp \
    ${srcdir}/../../include/bitcoin/system/data/external_ptr.hpp \
    ${srcdir}/../../include/bitcoin/system/data/iterable.hpp \
    ${srcdir}/../../include/bitcoin/system/data/memory.hpp \
    ${srcdir}/../../include/bitcoin/system/data/no_fill_allocator.hpp \
    ${srcdir}/../../include/bitcoin/system/data/string.hpp

include_bitcoin_system_endiandir = \
    ${includedir}/bitcoin/system/endian

include_bitcoin_system_endian_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/endian/batch.hpp \
    ${srcdir}/../../include/bitcoin/system/endian/endian.hpp \
    ${srcdir}/../../include/bitcoin/system/endian/integers.hpp \
    ${srcdir}/../../include/bitcoin/system/endian/integrals.hpp \
    ${srcdir}/../../include/bitcoin/system/endian/minimal.hpp \
    ${srcdir}/../../include/bitcoin/system/endian/nominal.hpp \
    ${srcdir}/../../include/bitcoin/system/endian/stream.hpp \
    ${srcdir}/../../include/bitcoin/system/endian/swaps.hpp \
    ${srcdir}/../../include/bitcoin/system/endian/uintx_t.hpp \
    ${srcdir}/../../include/bitcoin/system/endian/unsafe.hpp

include_bitcoin_system_errordir = \
    ${includedir}/bitcoin/system/error

include_bitcoin_system_error_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/error/block_error_t.hpp \
    ${srcdir}/../../include/bitcoin/system/error/error.hpp \
    ${srcdir}/../../include/bitcoin/system/error/error_t.hpp \
    ${srcdir}/../../include/bitcoin/system/error/errorno_t.hpp \
    ${srcdir}/../../include/bitcoin/system/error/macros.hpp \
    ${srcdir}/../../include/bitcoin/system/error/op_error_t.hpp \
    ${srcdir}/../../include/bitcoin/system/error/script_error_t.hpp \
    ${srcdir}/../../include/bitcoin/system/error/transaction_error_t.hpp

include_bitcoin_system_filterdir = \
    ${includedir}/bitcoin/system/filter

include_bitcoin_system_filter_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/filter/bloom.hpp \
    ${srcdir}/../../include/bitcoin/system/filter/filter.hpp \
    ${srcdir}/../../include/bitcoin/system/filter/golomb.hpp \
    ${srcdir}/../../include/bitcoin/system/filter/sieve.hpp

include_bitcoin_system_hashdir = \
    ${includedir}/bitcoin/system/hash

include_bitcoin_system_hash_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/hash/accumulator.hpp \
    ${srcdir}/../../include/bitcoin/system/hash/algorithm.hpp \
    ${srcdir}/../../include/bitcoin/system/hash/algorithms.hpp \
    ${srcdir}/../../include/bitcoin/system/hash/checksum.hpp \
    ${srcdir}/../../include/bitcoin/system/hash/functions.hpp \
    ${srcdir}/../../include/bitcoin/system/hash/hash.hpp \
    ${srcdir}/../../include/bitcoin/system/hash/hmac.hpp \
    ${srcdir}/../../include/bitcoin/system/hash/pbkd.hpp \
    ${srcdir}/../../include/bitcoin/system/hash/scrypt.hpp \
    ${srcdir}/../../include/bitcoin/system/hash/siphash.hpp

include_bitcoin_system_hash_rmddir = \
    ${includedir}/bitcoin/system/hash/rmd

include_bitcoin_system_hash_rmd_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/hash/rmd/algorithm.hpp \
    ${srcdir}/../../include/bitcoin/system/hash/rmd/rmd.hpp \
    ${srcdir}/../../include/bitcoin/system/hash/rmd/rmd128.hpp \
    ${srcdir}/../../include/bitcoin/system/hash/rmd/rmd160.hpp

include_bitcoin_system_hash_shadir = \
    ${includedir}/bitcoin/system/hash/sha

include_bitcoin_system_hash_sha_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/hash/sha/algorithm.hpp \
    ${srcdir}/../../include/bitcoin/system/hash/sha/sha.hpp \
    ${srcdir}/../../include/bitcoin/system/hash/sha/sha160.hpp \
    ${srcdir}/../../include/bitcoin/system/hash/sha/sha256.hpp \
    ${srcdir}/../../include/bitcoin/system/hash/sha/sha512.hpp

include_bitcoin_system_impl_chaindir = \
    ${includedir}/bitcoin/system/impl/chain

include_bitcoin_system_impl_chain_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/impl/chain/annex.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/chain/compact.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/chain/operation_patterns.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/chain/script_patterns.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/chain/tapscript.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/chain/transaction_patterns.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/chain/witness_patterns.ipp

include_bitcoin_system_impl_datadir = \
    ${includedir}/bitcoin/system/impl/data

include_bitcoin_system_impl_data_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/impl/data/array_cast.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/data/byte_cast.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/data/collection.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/data/data_array.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/data/data_reference.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/data/data_slab.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/data/data_slice.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/data/external_ptr.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/data/memory.ipp

include_bitcoin_system_impl_endiandir = \
    ${includedir}/bitcoin/system/impl/endian

include_bitcoin_system_impl_endian_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/impl/endian/batch.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/endian/integers.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/endian/integrals.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/endian/minimal.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/endian/nominal.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/endian/stream.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/endian/swaps.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/endian/uintx_t.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/endian/unsafe.ipp

include_bitcoin_system_impl_filterdir = \
    ${includedir}/bitcoin/system/impl/filter

include_bitcoin_system_impl_filter_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/impl/filter/bloom.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/filter/sieve.ipp

include_bitcoin_system_impl_hashdir = \
    ${includedir}/bitcoin/system/impl/hash

include_bitcoin_system_impl_hash_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/impl/hash/accumulator.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/hash/checksum.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/hash/functions.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/hash/hmac.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/hash/pbkd.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/hash/scrypt.ipp

include_bitcoin_system_impl_hash_rmddir = \
    ${includedir}/bitcoin/system/impl/hash/rmd

include_bitcoin_system_impl_hash_rmd_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/impl/hash/rmd/algorithm.ipp

include_bitcoin_system_impl_hash_shadir = \
    ${includedir}/bitcoin/system/impl/hash/sha

include_bitcoin_system_impl_hash_sha_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/impl/hash/sha/algorithm_compress.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/hash/sha/algorithm_double.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/hash/sha/algorithm_functions.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/hash/sha/algorithm_iterate.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/hash/sha/algorithm_konstant.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/hash/sha/algorithm_merkle.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/hash/sha/algorithm_midstate.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/hash/sha/algorithm_native.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/hash/sha/algorithm_padding.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/hash/sha/algorithm_parsing.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/hash/sha/algorithm_schedule.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/hash/sha/algorithm_sigma.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/hash/sha/algorithm_single.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/hash/sha/algorithm_stream.ipp

include_bitcoin_system_impl_machinedir = \
    ${includedir}/bitcoin/system/impl/machine

include_bitcoin_system_impl_machine_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/impl/machine/interpreter.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/machine/interpreter_connect.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/machine/interpreter_run.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/machine/number_boolean.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/machine/number_chunk.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/machine/number_integer.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/machine/program.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/machine/program_batch.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/machine/program_construct.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/machine/program_verify.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/machine/stack.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/machine/stack_variant.ipp

include_bitcoin_system_impl_mathdir = \
    ${includedir}/bitcoin/system/impl/math

include_bitcoin_system_impl_math_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/impl/math/addition.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/math/bits.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/math/bytes.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/math/byteswap.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/math/cast.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/math/division.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/math/limits.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/math/logarithm.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/math/multiplication.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/math/overflow.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/math/power.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/math/rotate.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/math/sign.ipp

include_bitcoin_system_impl_radixdir = \
    ${includedir}/bitcoin/system/impl/radix

include_bitcoin_system_impl_radix_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/impl/radix/base_16.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/radix/base_2n.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/radix/base_58.ipp

include_bitcoin_system_impl_serialdir = \
    ${includedir}/bitcoin/system/impl/serial

include_bitcoin_system_impl_serial_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/impl/serial/deserialize.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/serial/serialize.ipp

include_bitcoin_system_impl_streamdir = \
    ${includedir}/bitcoin/system/impl/stream

include_bitcoin_system_impl_stream_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/impl/stream/device.ipp

include_bitcoin_system_impl_stream_iostreamdir = \
    ${includedir}/bitcoin/system/impl/stream/iostream

include_bitcoin_system_impl_stream_iostream_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/impl/stream/iostream/iostream.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/stream/iostream/istream.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/stream/iostream/ostream.ipp

include_bitcoin_system_impl_stream_streamersdir = \
    ${includedir}/bitcoin/system/impl/stream/streamers

include_bitcoin_system_impl_stream_streamers_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/impl/stream/streamers/bit_reader.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/stream/streamers/bit_writer.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/stream/streamers/byte_reader.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/stream/streamers/byte_writer.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/stream/streamers/hex_reader.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/stream/streamers/hex_writer.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/stream/streamers/sha256_writer.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/stream/streamers/sha256t_writer.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/stream/streamers/sha256x2_writer.ipp

include_bitcoin_system_impl_unicodedir = \
    ${includedir}/bitcoin/system/impl/unicode

include_bitcoin_system_impl_unicode_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/impl/unicode/ascii.ipp

include_bitcoin_system_impl_wallet_addressesdir = \
    ${includedir}/bitcoin/system/impl/wallet/addresses

include_bitcoin_system_impl_wallet_addresses_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/impl/wallet/addresses/checked.ipp

include_bitcoin_system_impl_wordsdir = \
    ${includedir}/bitcoin/system/impl/words

include_bitcoin_system_impl_words_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/impl/words/dictionaries.ipp \
    ${srcdir}/../../include/bitcoin/system/impl/words/dictionary.ipp

include_bitcoin_system_intrinsicsdir = \
    ${includedir}/bitcoin/system/intrinsics

include_bitcoin_system_intrinsics_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/intrinsics/byte_swap.hpp \
    ${srcdir}/../../include/bitcoin/system/intrinsics/cpuid.hpp \
    ${srcdir}/../../include/bitcoin/system/intrinsics/detection.hpp \
    ${srcdir}/../../include/bitcoin/system/intrinsics/intrinsics.hpp \
    ${srcdir}/../../include/bitcoin/system/intrinsics/rotate.hpp \
    ${srcdir}/../../include/bitcoin/system/intrinsics/types.hpp

include_bitcoin_system_intrinsics_inteldir = \
    ${includedir}/bitcoin/system/intrinsics/intel

include_bitcoin_system_intrinsics_intel_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/intrinsics/intel/intel.hpp \
    ${srcdir}/../../include/bitcoin/system/intrinsics/intel/intel_128.hpp \
    ${srcdir}/../../include/bitcoin/system/intrinsics/intel/intel_256.hpp \
    ${srcdir}/../../include/bitcoin/system/intrinsics/intel/intel_512.hpp \
    ${srcdir}/../../include/bitcoin/system/intrinsics/intel/intel_sha.hpp

include_bitcoin_system_intrinsics_neondir = \
    ${includedir}/bitcoin/system/intrinsics/neon

include_bitcoin_system_intrinsics_neon_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/intrinsics/neon/neon.hpp \
    ${srcdir}/../../include/bitcoin/system/intrinsics/neon/neon_128.hpp \
    ${srcdir}/../../include/bitcoin/system/intrinsics/neon/neon_sha.hpp

include_bitcoin_system_intrinsics_nonedir = \
    ${includedir}/bitcoin/system/intrinsics/none

include_bitcoin_system_intrinsics_none_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/intrinsics/none/none_128.hpp \
    ${srcdir}/../../include/bitcoin/system/intrinsics/none/none_sha.hpp

include_bitcoin_system_intrinsics_svedir = \
    ${includedir}/bitcoin/system/intrinsics/sve

include_bitcoin_system_intrinsics_sve_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/intrinsics/sve/sve.hpp

include_bitcoin_system_machinedir = \
    ${includedir}/bitcoin/system/machine

include_bitcoin_system_machine_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/machine/interpreter.hpp \
    ${srcdir}/../../include/bitcoin/system/machine/machine.hpp \
    ${srcdir}/../../include/bitcoin/system/machine/number.hpp \
    ${srcdir}/../../include/bitcoin/system/machine/number_boolean.hpp \
    ${srcdir}/../../include/bitcoin/system/machine/number_chunk.hpp \
    ${srcdir}/../../include/bitcoin/system/machine/number_integer.hpp \
    ${srcdir}/../../include/bitcoin/system/machine/program.hpp \
    ${srcdir}/../../include/bitcoin/system/machine/stack.hpp

include_bitcoin_system_mathdir = \
    ${includedir}/bitcoin/system/math

include_bitcoin_system_math_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/math/addition.hpp \
    ${srcdir}/../../include/bitcoin/system/math/bits.hpp \
    ${srcdir}/../../include/bitcoin/system/math/bytes.hpp \
    ${srcdir}/../../include/bitcoin/system/math/byteswap.hpp \
    ${srcdir}/../../include/bitcoin/system/math/cast.hpp \
    ${srcdir}/../../include/bitcoin/system/math/division.hpp \
    ${srcdir}/../../include/bitcoin/system/math/functional.hpp \
    ${srcdir}/../../include/bitcoin/system/math/limits.hpp \
    ${srcdir}/../../include/bitcoin/system/math/logarithm.hpp \
    ${srcdir}/../../include/bitcoin/system/math/math.hpp \
    ${srcdir}/../../include/bitcoin/system/math/multiplication.hpp \
    ${srcdir}/../../include/bitcoin/system/math/overflow.hpp \
    ${srcdir}/../../include/bitcoin/system/math/power.hpp \
    ${srcdir}/../../include/bitcoin/system/math/rotate.hpp \
    ${srcdir}/../../include/bitcoin/system/math/sign.hpp

include_bitcoin_system_radixdir = \
    ${includedir}/bitcoin/system/radix

include_bitcoin_system_radix_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/radix/base_10.hpp \
    ${srcdir}/../../include/bitcoin/system/radix/base_16.hpp \
    ${srcdir}/../../include/bitcoin/system/radix/base_2048.hpp \
    ${srcdir}/../../include/bitcoin/system/radix/base_2n.hpp \
    ${srcdir}/../../include/bitcoin/system/radix/base_32.hpp \
    ${srcdir}/../../include/bitcoin/system/radix/base_58.hpp \
    ${srcdir}/../../include/bitcoin/system/radix/base_64.hpp \
    ${srcdir}/../../include/bitcoin/system/radix/base_85.hpp \
    ${srcdir}/../../include/bitcoin/system/radix/radix.hpp

include_bitcoin_system_serialdir = \
    ${includedir}/bitcoin/system/serial

include_bitcoin_system_serial_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/serial/deserialize.hpp \
    ${srcdir}/../../include/bitcoin/system/serial/props.hpp \
    ${srcdir}/../../include/bitcoin/system/serial/serial.hpp \
    ${srcdir}/../../include/bitcoin/system/serial/serialize.hpp

include_bitcoin_system_streamdir = \
    ${includedir}/bitcoin/system/stream

include_bitcoin_system_stream_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/stream/binary.hpp \
    ${srcdir}/../../include/bitcoin/system/stream/device.hpp \
    ${srcdir}/../../include/bitcoin/system/stream/make_stream.hpp \
    ${srcdir}/../../include/bitcoin/system/stream/make_streamer.hpp \
    ${srcdir}/../../include/bitcoin/system/stream/stream.hpp \
    ${srcdir}/../../include/bitcoin/system/stream/stream_result.hpp \
    ${srcdir}/../../include/bitcoin/system/stream/streamers.hpp \
    ${srcdir}/../../include/bitcoin/system/stream/streams.hpp

include_bitcoin_system_stream_devicesdir = \
    ${includedir}/bitcoin/system/stream/devices

include_bitcoin_system_stream_devices_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/stream/devices/copy_sink.hpp \
    ${srcdir}/../../include/bitcoin/system/stream/devices/copy_source.hpp \
    ${srcdir}/../../include/bitcoin/system/stream/devices/flip_sink.hpp \
    ${srcdir}/../../include/bitcoin/system/stream/devices/push_sink.hpp

include_bitcoin_system_stream_iostreamdir = \
    ${includedir}/bitcoin/system/stream/iostream

include_bitcoin_system_stream_iostream_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/stream/iostream/iostream.hpp \
    ${srcdir}/../../include/bitcoin/system/stream/iostream/istream.hpp \
    ${srcdir}/../../include/bitcoin/system/stream/iostream/ostream.hpp

include_bitcoin_system_stream_streamersdir = \
    ${includedir}/bitcoin/system/stream/streamers

include_bitcoin_system_stream_streamers_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/stream/streamers/bit_flipper.hpp \
    ${srcdir}/../../include/bitcoin/system/stream/streamers/bit_reader.hpp \
    ${srcdir}/../../include/bitcoin/system/stream/streamers/bit_writer.hpp \
    ${srcdir}/../../include/bitcoin/system/stream/streamers/byte_flipper.hpp \
    ${srcdir}/../../include/bitcoin/system/stream/streamers/byte_reader.hpp \
    ${srcdir}/../../include/bitcoin/system/stream/streamers/byte_writer.hpp \
    ${srcdir}/../../include/bitcoin/system/stream/streamers/hex_reader.hpp \
    ${srcdir}/../../include/bitcoin/system/stream/streamers/hex_writer.hpp \
    ${srcdir}/../../include/bitcoin/system/stream/streamers/sha256_writer.hpp \
    ${srcdir}/../../include/bitcoin/system/stream/streamers/sha256t_writer.hpp \
    ${srcdir}/../../include/bitcoin/system/stream/streamers/sha256x2_writer.hpp

include_bitcoin_system_stream_streamers_interfacesdir = \
    ${includedir}/bitcoin/system/stream/streamers/interfaces

include_bitcoin_system_stream_streamers_interfaces_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/stream/streamers/interfaces/bitflipper.hpp \
    ${srcdir}/../../include/bitcoin/system/stream/streamers/interfaces/bitreader.hpp \
    ${srcdir}/../../include/bitcoin/system/stream/streamers/interfaces/bitwriter.hpp \
    ${srcdir}/../../include/bitcoin/system/stream/streamers/interfaces/byteflipper.hpp \
    ${srcdir}/../../include/bitcoin/system/stream/streamers/interfaces/bytereader.hpp \
    ${srcdir}/../../include/bitcoin/system/stream/streamers/interfaces/bytewriter.hpp

include_bitcoin_system_unicodedir = \
    ${includedir}/bitcoin/system/unicode

include_bitcoin_system_unicode_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/unicode/ascii.hpp \
    ${srcdir}/../../include/bitcoin/system/unicode/code_points.hpp \
    ${srcdir}/../../include/bitcoin/system/unicode/conversion.hpp \
    ${srcdir}/../../include/bitcoin/system/unicode/normalization.hpp \
    ${srcdir}/../../include/bitcoin/system/unicode/unicode.hpp \
    ${srcdir}/../../include/bitcoin/system/unicode/unicode_tables.hpp

include_bitcoin_system_unicode_utf8_everywheredir = \
    ${includedir}/bitcoin/system/unicode/utf8_everywhere

include_bitcoin_system_unicode_utf8_everywhere_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/unicode/utf8_everywhere/console_streambuf.hpp \
    ${srcdir}/../../include/bitcoin/system/unicode/utf8_everywhere/environment.hpp \
    ${srcdir}/../../include/bitcoin/system/unicode/utf8_everywhere/ifstream.hpp \
    ${srcdir}/../../include/bitcoin/system/unicode/utf8_everywhere/ofstream.hpp \
    ${srcdir}/../../include/bitcoin/system/unicode/utf8_everywhere/paths.hpp \
    ${srcdir}/../../include/bitcoin/system/unicode/utf8_everywhere/stdio.hpp \
    ${srcdir}/../../include/bitcoin/system/unicode/utf8_everywhere/unicode_istream.hpp \
    ${srcdir}/../../include/bitcoin/system/unicode/utf8_everywhere/unicode_ostream.hpp \
    ${srcdir}/../../include/bitcoin/system/unicode/utf8_everywhere/unicode_streambuf.hpp \
    ${srcdir}/../../include/bitcoin/system/unicode/utf8_everywhere/utf8_everywhere.hpp

include_bitcoin_system_walletdir = \
    ${includedir}/bitcoin/system/wallet

include_bitcoin_system_wallet_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/wallet/context.hpp \
    ${srcdir}/../../include/bitcoin/system/wallet/message.hpp \
    ${srcdir}/../../include/bitcoin/system/wallet/neutrino.hpp \
    ${srcdir}/../../include/bitcoin/system/wallet/point_value.hpp \
    ${srcdir}/../../include/bitcoin/system/wallet/points_value.hpp \
    ${srcdir}/../../include/bitcoin/system/wallet/wallet.hpp

include_bitcoin_system_wallet_addressesdir = \
    ${includedir}/bitcoin/system/wallet/addresses

include_bitcoin_system_wallet_addresses_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/wallet/addresses/bitcoin_uri.hpp \
    ${srcdir}/../../include/bitcoin/system/wallet/addresses/checked.hpp \
    ${srcdir}/../../include/bitcoin/system/wallet/addresses/payment_address.hpp \
    ${srcdir}/../../include/bitcoin/system/wallet/addresses/qr_code.hpp \
    ${srcdir}/../../include/bitcoin/system/wallet/addresses/stealth_address.hpp \
    ${srcdir}/../../include/bitcoin/system/wallet/addresses/stealth_receiver.hpp \
    ${srcdir}/../../include/bitcoin/system/wallet/addresses/stealth_sender.hpp \
    ${srcdir}/../../include/bitcoin/system/wallet/addresses/tiff.hpp \
    ${srcdir}/../../include/bitcoin/system/wallet/addresses/uri.hpp \
    ${srcdir}/../../include/bitcoin/system/wallet/addresses/uri_reader.hpp \
    ${srcdir}/../../include/bitcoin/system/wallet/addresses/witness_address.hpp

include_bitcoin_system_wallet_keysdir = \
    ${includedir}/bitcoin/system/wallet/keys

include_bitcoin_system_wallet_keys_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/wallet/keys/ec_point.hpp \
    ${srcdir}/../../include/bitcoin/system/wallet/keys/ec_private.hpp \
    ${srcdir}/../../include/bitcoin/system/wallet/keys/ec_public.hpp \
    ${srcdir}/../../include/bitcoin/system/wallet/keys/ec_scalar.hpp \
    ${srcdir}/../../include/bitcoin/system/wallet/keys/ek_private.hpp \
    ${srcdir}/../../include/bitcoin/system/wallet/keys/ek_public.hpp \
    ${srcdir}/../../include/bitcoin/system/wallet/keys/ek_token.hpp \
    ${srcdir}/../../include/bitcoin/system/wallet/keys/encrypted_keys.hpp \
    ${srcdir}/../../include/bitcoin/system/wallet/keys/hd_private.hpp \
    ${srcdir}/../../include/bitcoin/system/wallet/keys/hd_public.hpp \
    ${srcdir}/../../include/bitcoin/system/wallet/keys/mini_keys.hpp \
    ${srcdir}/../../include/bitcoin/system/wallet/keys/stealth.hpp

include_bitcoin_system_wallet_mnemonicsdir = \
    ${includedir}/bitcoin/system/wallet/mnemonics

include_bitcoin_system_wallet_mnemonics_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/wallet/mnemonics/electrum.hpp \
    ${srcdir}/../../include/bitcoin/system/wallet/mnemonics/electrum_v1.hpp \
    ${srcdir}/../../include/bitcoin/system/wallet/mnemonics/mnemonic.hpp

include_bitcoin_system_wordsdir = \
    ${includedir}/bitcoin/system/words

include_bitcoin_system_words_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/words/dictionaries.hpp \
    ${srcdir}/../../include/bitcoin/system/words/dictionary.hpp \
    ${srcdir}/../../include/bitcoin/system/words/language.hpp \
    ${srcdir}/../../include/bitcoin/system/words/languages.hpp \
    ${srcdir}/../../include/bitcoin/system/words/words.hpp

include_bitcoin_system_words_catalogsdir = \
    ${includedir}/bitcoin/system/words/catalogs

include_bitcoin_system_words_catalogs_HEADERS = \
    ${srcdir}/../../include/bitcoin/system/words/catalogs/electrum.hpp \
    ${srcdir}/../../include/bitcoin/system/words/catalogs/electrum_v1.hpp \
    ${srcdir}/../../include/bitcoin/system/words/catalogs/mnemonic.hpp

# Tests.
#==============================================================================
# Target test 'test/libbitcoin-system-test'
#------------------------------------------------------------------------------
if WITH_TESTS

check_PROGRAMS = test/libbitcoin-system-test

test_libbitcoin_system_test_CPPFLAGS = \
    ${boost_BUILD_CPPFLAGS} \
    ${boost_unit_test_framework_BUILD_CPPFLAGS} \
    ${src_libbitcoin_system_la_CPPFLAGS}

test_libbitcoin_system_test_LDFLAGS = \
    ${boost_LDFLAGS} \
    ${boost_unit_test_framework_LDFLAGS} \
    ${src_libbitcoin_system_la_LDFLAGS}

test_libbitcoin_system_test_LDADD = \
    ${boost_LIBS} \
    ${boost_unit_test_framework_LIBS} \
    ${src_libbitcoin_system_la_LIBS} \
    ${src_libbitcoin_system_la_LIBADD}

test_libbitcoin_system_test_SOURCES = \
    ${srcdir}/../../test/allocator.cpp \
    ${srcdir}/../../test/arena.cpp \
    ${srcdir}/../../test/constants.cpp \
    ${srcdir}/../../test/constraints.cpp \
    ${srcdir}/../../test/define.cpp \
    ${srcdir}/../../test/funclets.cpp \
    ${srcdir}/../../test/hacks.cpp \
    ${srcdir}/../../test/literals.cpp \
    ${srcdir}/../../test/main.cpp \
    ${srcdir}/../../test/settings.cpp \
    ${srcdir}/../../test/test.cpp \
    ${srcdir}/../../test/typelets.cpp \
    ${srcdir}/../../test/types.cpp \
    ${srcdir}/../../test/chain/annex.cpp \
    ${srcdir}/../../test/chain/block.cpp \
    ${srcdir}/../../test/chain/block_malleable.cpp \
    ${srcdir}/../../test/chain/chain_state.cpp \
    ${srcdir}/../../test/chain/checkpoint.cpp \
    ${srcdir}/../../test/chain/compact.cpp \
    ${srcdir}/../../test/chain/context.cpp \
    ${srcdir}/../../test/chain/header.cpp \
    ${srcdir}/../../test/chain/input.cpp \
    ${srcdir}/../../test/chain/operation.cpp \
    ${srcdir}/../../test/chain/outpoint.cpp \
    ${srcdir}/../../test/chain/output.cpp \
    ${srcdir}/../../test/chain/point.cpp \
    ${srcdir}/../../test/chain/satoshi_words.cpp \
    ${srcdir}/../../test/chain/script.cpp \
    ${srcdir}/../../test/chain/script_connect.cpp \
    ${srcdir}/../../test/chain/script_extract.cpp \
    ${srcdir}/../../test/chain/script_patterns.cpp \
    ${srcdir}/../../test/chain/script_serialize.cpp \
    ${srcdir}/../../test/chain/stripper.cpp \
    ${srcdir}/../../test/chain/taproot.cpp \
    ${srcdir}/../../test/chain/tapscript.cpp \
    ${srcdir}/../../test/chain/transaction.cpp \
    ${srcdir}/../../test/chain/witness.cpp \
    ${srcdir}/../../test/chain/batch/threshold.cpp \
    ${srcdir}/../../test/chain/enums/opcode.cpp \
    ${srcdir}/../../test/chain/json/block.cpp \
    ${srcdir}/../../test/chain/json/checkpoint.cpp \
    ${srcdir}/../../test/chain/json/header.cpp \
    ${srcdir}/../../test/chain/json/input.cpp \
    ${srcdir}/../../test/chain/json/operation.cpp \
    ${srcdir}/../../test/chain/json/outpoint.cpp \
    ${srcdir}/../../test/chain/json/output.cpp \
    ${srcdir}/../../test/chain/json/point.cpp \
    ${srcdir}/../../test/chain/json/script.cpp \
    ${srcdir}/../../test/chain/json/transaction.cpp \
    ${srcdir}/../../test/chain/json/witness.cpp \
    ${srcdir}/../../test/chain/views/block_view.cpp \
    ${srcdir}/../../test/chain/views/transaction_view.cpp \
    ${srcdir}/../../test/config/authority.cpp \
    ${srcdir}/../../test/config/base16.cpp \
    ${srcdir}/../../test/config/base2.cpp \
    ${srcdir}/../../test/config/base32.cpp \
    ${srcdir}/../../test/config/base58.cpp \
    ${srcdir}/../../test/config/base64.cpp \
    ${srcdir}/../../test/config/base85.cpp \
    ${srcdir}/../../test/config/block.cpp \
    ${srcdir}/../../test/config/endpoint.cpp \
    ${srcdir}/../../test/config/hash256.cpp \
    ${srcdir}/../../test/config/parameter.cpp \
    ${srcdir}/../../test/config/printer.cpp \
    ${srcdir}/../../test/config/url.cpp \
    ${srcdir}/../../test/config/utilities.cpp \
    ${srcdir}/../../test/config/version.cpp \
    ${srcdir}/../../test/crypto/aes256.cpp \
    ${srcdir}/../../test/crypto/pseudo_random.cpp \
    ${srcdir}/../../test/crypto/ring_signature.cpp \
    ${srcdir}/../../test/crypto/secp256k1/batch.cpp \
    ${srcdir}/../../test/crypto/secp256k1/ecdsa.cpp \
    ${srcdir}/../../test/crypto/secp256k1/keys.cpp \
    ${srcdir}/../../test/crypto/secp256k1/math.cpp \
    ${srcdir}/../../test/crypto/secp256k1/schnorr.cpp \
    ${srcdir}/../../test/data/array_cast.cpp \
    ${srcdir}/../../test/data/byte_cast.cpp \
    ${srcdir}/../../test/data/collection.cpp \
    ${srcdir}/../../test/data/data_array.cpp \
    ${srcdir}/../../test/data/data_chunk.cpp \
    ${srcdir}/../../test/data/data_reference.cpp \
    ${srcdir}/../../test/data/data_slab.cpp \
    ${srcdir}/../../test/data/data_slice.cpp \
    ${srcdir}/../../test/data/exclusive_slice.cpp \
    ${srcdir}/../../test/data/external_ptr.cpp \
    ${srcdir}/../../test/data/integer.cpp \
    ${srcdir}/../../test/data/iterable.cpp \
    ${srcdir}/../../test/data/memory.cpp \
    ${srcdir}/../../test/data/no_fill_allocator.cpp \
    ${srcdir}/../../test/data/string.cpp \
    ${srcdir}/../../test/endian/batch.cpp \
    ${srcdir}/../../test/endian/integers.cpp \
    ${srcdir}/../../test/endian/integrals.cpp \
    ${srcdir}/../../test/endian/minimal.cpp \
    ${srcdir}/../../test/endian/nominal.cpp \
    ${srcdir}/../../test/endian/stream.cpp \
    ${srcdir}/../../test/endian/swaps.cpp \
    ${srcdir}/../../test/endian/uintx_t.cpp \
    ${srcdir}/../../test/endian/unsafe.cpp \
    ${srcdir}/../../test/error/block_error_t.cpp \
    ${srcdir}/../../test/error/error.cpp \
    ${srcdir}/../../test/error/error_t.cpp \
    ${srcdir}/../../test/error/errorno_t.cpp \
    ${srcdir}/../../test/error/op_error_t.cpp \
    ${srcdir}/../../test/error/script_error_t.cpp \
    ${srcdir}/../../test/error/transaction_error_t.cpp \
    ${srcdir}/../../test/filter/bloom.cpp \
    ${srcdir}/../../test/filter/golomb.cpp \
    ${srcdir}/../../test/filter/sieve.cpp \
    ${srcdir}/../../test/hash/accumulator.cpp \
    ${srcdir}/../../test/hash/checksum.cpp \
    ${srcdir}/../../test/hash/functions.cpp \
    ${srcdir}/../../test/hash/hmac.cpp \
    ${srcdir}/../../test/hash/pbkd.cpp \
    ${srcdir}/../../test/hash/scrypt.cpp \
    ${srcdir}/../../test/hash/siphash.cpp \
    ${srcdir}/../../test/hash/performance/performance.cpp \
    ${srcdir}/../../test/hash/performance/baseline/rmd160.cpp \
    ${srcdir}/../../test/hash/performance/baseline/sha256.cpp \
    ${srcdir}/../../test/hash/rmd/algorithm.cpp \
    ${srcdir}/../../test/hash/rmd/analysis.cpp \
    ${srcdir}/../../test/hash/sha/algorithm.cpp \
    ${srcdir}/../../test/hash/sha/analysis.cpp \
    ${srcdir}/../../test/hash/sha/sha160.cpp \
    ${srcdir}/../../test/hash/sha/sha256.cpp \
    ${srcdir}/../../test/hash/sha/sha512.cpp \
    ${srcdir}/../../test/hash/sha/vector.cpp \
    ${srcdir}/../../test/intrinsics/byte_swap.cpp \
    ${srcdir}/../../test/intrinsics/cpuid.cpp \
    ${srcdir}/../../test/intrinsics/detection.cpp \
    ${srcdir}/../../test/intrinsics/intel_128.cpp \
    ${srcdir}/../../test/intrinsics/intel_256.cpp \
    ${srcdir}/../../test/intrinsics/intel_512.cpp \
    ${srcdir}/../../test/intrinsics/rotate.cpp \
    ${srcdir}/../../test/intrinsics/types.cpp \
    ${srcdir}/../../test/intrinsics/platforms/intel.cpp \
    ${srcdir}/../../test/intrinsics/platforms/neon.cpp \
    ${srcdir}/../../test/intrinsics/platforms/sve.cpp \
    ${srcdir}/../../test/machine/interpreter.cpp \
    ${srcdir}/../../test/machine/number.cpp \
    ${srcdir}/../../test/machine/program.cpp \
    ${srcdir}/../../test/machine/sizing.cpp \
    ${srcdir}/../../test/machine/stack.cpp \
    ${srcdir}/../../test/math/addition.cpp \
    ${srcdir}/../../test/math/bits.cpp \
    ${srcdir}/../../test/math/bytes.cpp \
    ${srcdir}/../../test/math/byteswap.cpp \
    ${srcdir}/../../test/math/cast.cpp \
    ${srcdir}/../../test/math/division.cpp \
    ${srcdir}/../../test/math/expectations.cpp \
    ${srcdir}/../../test/math/limits.cpp \
    ${srcdir}/../../test/math/logarithm.cpp \
    ${srcdir}/../../test/math/multiplication.cpp \
    ${srcdir}/../../test/math/overflow.cpp \
    ${srcdir}/../../test/math/power.cpp \
    ${srcdir}/../../test/math/rotate.cpp \
    ${srcdir}/../../test/math/sign.cpp \
    ${srcdir}/../../test/mocks/blocks.cpp \
    ${srcdir}/../../test/radix/base_10.cpp \
    ${srcdir}/../../test/radix/base_16.cpp \
    ${srcdir}/../../test/radix/base_2048.cpp \
    ${srcdir}/../../test/radix/base_2n.cpp \
    ${srcdir}/../../test/radix/base_32.cpp \
    ${srcdir}/../../test/radix/base_58.cpp \
    ${srcdir}/../../test/radix/base_64.cpp \
    ${srcdir}/../../test/radix/base_85.cpp \
    ${srcdir}/../../test/serial/deserialize.cpp \
    ${srcdir}/../../test/serial/props.cpp \
    ${srcdir}/../../test/serial/serialize.cpp \
    ${srcdir}/../../test/stream/binary.cpp \
    ${srcdir}/../../test/stream/device.cpp \
    ${srcdir}/../../test/stream/stream.cpp \
    ${srcdir}/../../test/stream/streamers.cpp \
    ${srcdir}/../../test/stream/devices/copy_sink.cpp \
    ${srcdir}/../../test/stream/devices/copy_source.cpp \
    ${srcdir}/../../test/stream/devices/flip_sink.cpp \
    ${srcdir}/../../test/stream/devices/push_sink.cpp \
    ${srcdir}/../../test/stream/iostream/iostream.cpp \
    ${srcdir}/../../test/stream/iostream/istream.cpp \
    ${srcdir}/../../test/stream/iostream/ostream.cpp \
    ${srcdir}/../../test/stream/streamers/bit_flipper.cpp \
    ${srcdir}/../../test/stream/streamers/bit_reader.cpp \
    ${srcdir}/../../test/stream/streamers/bit_writer.cpp \
    ${srcdir}/../../test/stream/streamers/byte_flipper.cpp \
    ${srcdir}/../../test/stream/streamers/byte_reader.cpp \
    ${srcdir}/../../test/stream/streamers/byte_writer.cpp \
    ${srcdir}/../../test/stream/streamers/hex_reader.cpp \
    ${srcdir}/../../test/stream/streamers/hex_writer.cpp \
    ${srcdir}/../../test/stream/streamers/sha256_writer.cpp \
    ${srcdir}/../../test/stream/streamers/sha256t_writer.cpp \
    ${srcdir}/../../test/stream/streamers/sha256x2_writer.cpp \
    ${srcdir}/../../test/unicode/ascii.cpp \
    ${srcdir}/../../test/unicode/code_points.cpp \
    ${srcdir}/../../test/unicode/conversion.cpp \
    ${srcdir}/../../test/unicode/normalization.cpp \
    ${srcdir}/../../test/unicode/utf8_everywhere/environment.cpp \
    ${srcdir}/../../test/unicode/utf8_everywhere/ifstream.cpp \
    ${srcdir}/../../test/unicode/utf8_everywhere/ofstream.cpp \
    ${srcdir}/../../test/unicode/utf8_everywhere/paths.cpp \
    ${srcdir}/../../test/unicode/utf8_everywhere/unicode_istream.cpp \
    ${srcdir}/../../test/unicode/utf8_everywhere/unicode_ostream.cpp \
    ${srcdir}/../../test/wallet/context.cpp \
    ${srcdir}/../../test/wallet/message.cpp \
    ${srcdir}/../../test/wallet/neutrino.cpp \
    ${srcdir}/../../test/wallet/point_value.cpp \
    ${srcdir}/../../test/wallet/points_value.cpp \
    ${srcdir}/../../test/wallet/addresses/bitcoin_uri.cpp \
    ${srcdir}/../../test/wallet/addresses/checked.cpp \
    ${srcdir}/../../test/wallet/addresses/payment_address.cpp \
    ${srcdir}/../../test/wallet/addresses/qr_code.cpp \
    ${srcdir}/../../test/wallet/addresses/stealth_address.cpp \
    ${srcdir}/../../test/wallet/addresses/stealth_receiver.cpp \
    ${srcdir}/../../test/wallet/addresses/stealth_sender.cpp \
    ${srcdir}/../../test/wallet/addresses/tiff.cpp \
    ${srcdir}/../../test/wallet/addresses/uri.cpp \
    ${srcdir}/../../test/wallet/addresses/uri_reader.cpp \
    ${srcdir}/../../test/wallet/addresses/witness_address.cpp \
    ${srcdir}/../../test/wallet/keys/ec_point.cpp \
    ${srcdir}/../../test/wallet/keys/ec_private.cpp \
    ${srcdir}/../../test/wallet/keys/ec_public.cpp \
    ${srcdir}/../../test/wallet/keys/ec_scalar.cpp \
    ${srcdir}/../../test/wallet/keys/encrypted_keys.cpp \
    ${srcdir}/../../test/wallet/keys/hd_private.cpp \
    ${srcdir}/../../test/wallet/keys/hd_public.cpp \
    ${srcdir}/../../test/wallet/keys/mini_keys.cpp \
    ${srcdir}/../../test/wallet/keys/stealth.cpp \
    ${srcdir}/../../test/wallet/mnemonics/electrum.cpp \
    ${srcdir}/../../test/wallet/mnemonics/electrum_v1.cpp \
    ${srcdir}/../../test/wallet/mnemonics/mnemonic.cpp \
    ${srcdir}/../../test/words/dictionaries.cpp \
    ${srcdir}/../../test/words/dictionary.cpp \
    ${srcdir}/../../test/words/languages.cpp \
    ${srcdir}/../../test/words/catalogs/electrum.cpp \
    ${srcdir}/../../test/words/catalogs/electrum_v1.cpp \
    ${srcdir}/../../test/words/catalogs/mnemonic.cpp

TESTS = test_runner.sh

endif WITH_TESTS

# Binaries.
#==============================================================================
# Target binary 'examples/libbitcoin-system-examples'
#------------------------------------------------------------------------------
if WITH_EXAMPLES

noinst_PROGRAMS = examples/libbitcoin-system-examples

examples_libbitcoin_system_examples_CPPFLAGS = \
    ${src_libbitcoin_system_la_CPPFLAGS}

examples_libbitcoin_system_examples_LDFLAGS = \
    ${src_libbitcoin_system_la_LDFLAGS}

examples_libbitcoin_system_examples_LDADD = \
    ${src_libbitcoin_system_la_LIBS} \
    ${src_libbitcoin_system_la_LIBADD}

examples_libbitcoin_system_examples_SOURCES = \
    ${srcdir}/../../examples/main.cpp

target_examples = examples/libbitcoin-system-examples

examples: ${target_examples}

endif WITH_EXAMPLES
