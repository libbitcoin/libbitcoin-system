%module wallet
%{
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/system_wallet.hpp>
%}

%import "boost/functional/hash_fwd.hpp"

%import "bitcoin/bitcoin/compat.h"
%import "bitcoin/bitcoin/compat.hpp"
%import "bitcoin/bitcoin/define.hpp"

%ignore getValue();

%import <bitcoin-ext.i>
%import "bitcoin/bitcoin/chain/point.hpp"
%import "bitcoin/bitcoin/chain/output_point.hpp"
%import "bitcoin/bitcoin/chain/witness.hpp"
%import "bitcoin/bitcoin/math/ec_scalar.hpp"

//%import <bitcoin-ext-typemap.i>
%import <bitcoin-chain-ext.i>
//%import <bitcoin-chain-ext-typemap.i>
%import <bitcoin-math-ext.i>
//%import <bitcoin-math-ext-typemap.i>
%import <bitcoin-utility-ext.i>
//%import <bitcoin-utility-ext-typemap.i>

%import "bitcoin/bitcoin/wallet/uri.hpp"
%ignore setValue(wallet::uri::query_map value);
%include "bitcoin/wallet_uri_query_map.hpp"
%include "bitcoin/wallet_uri.hpp"
%import "bitcoin/bitcoin/wallet/uri_reader.hpp"
%include "bitcoin/wallet_uri_reader.hpp"

//%rename(lt) libbitcoin::wallet::ec_public::operator<(const ec_public& other) const;
//%rename(eq) libbitcoin::wallet::ec_public::operator==(const ec_public& other) const;
//%rename(ne) libbitcoin::wallet::ec_public::operator!=(const ec_public& other) const;
//%rename(assign) libbitcoin::wallet::ec_public::operator=(const ec_public& other);
//%ignore operator>>(std::istream& in, ec_public& to);
//%ignore operator<<(std::ostream& out, const ec_public& of);
//%rename(toBoolean) libbitcoin::wallet::ec_public::operator bool() const;
//%rename(toEc_compressed) libbitcoin::wallet::ec_public::operator const ec_compressed&() const;

//%rename(lt) libbitcoin::wallet::ec_private::operator<(const ec_private& other) const;
//%rename(eq) libbitcoin::wallet::ec_private::operator==(const ec_private& other) const;
//%rename(ne) libbitcoin::wallet::ec_private::operator!=(const ec_private& other) const;
//%rename(assign) libbitcoin::wallet::ec_private::operator=(const ec_private& other);
//%ignore operator>>(std::istream& in, ec_private& to);
//%ignore operator<<(std::ostream& out, const ec_private& of);
//%rename(toBoolean) libbitcoin::wallet::ec_private::operator bool() const;
//%rename(toEc_secret) libbitcoin::wallet::ec_private::operator const ec_secret&() const;
%ignore setValue(wallet::wif_compressed value);
%include "bitcoin/wallet_wif_compressed.hpp"
%ignore setValue(wallet::wif_uncompressed value);
%include "bitcoin/wallet_wif_uncompressed.hpp"
%import "bitcoin/bitcoin/wallet/ec_private.hpp"
%import "bitcoin/math_ec_scalar.hpp"
%include "bitcoin/wallet_ec_private.hpp"

%import "bitcoin/bitcoin/math/ec_scalar.hpp"
%import "bitcoin/bitcoin/math/ec_point.hpp"
%import "bitcoin/bitcoin/wallet/ec_public.hpp"
%import "bitcoin/math_ec_point.hpp"
%import "bitcoin/math_ec_scalar.hpp"
%include "bitcoin/wallet_ec_public.hpp"

//%ignore libbitcoin::wallet::payment_address::payment_address(payment_address&& other);
//%ignore libbitcoin::wallet::payment_address::payment_address(short_hash&& hash, uint8_t version=mainnet_p2kh);
//%rename(lt) libbitcoin::wallet::payment_address::operator<(const payment_address& other) const;
//%rename(eq) libbitcoin::wallet::payment_address::operator==(const payment_address& other) const;
//%rename(ne) libbitcoin::wallet::payment_address::operator!=(const payment_address& other) const;
//%rename(assign) libbitcoin::wallet::payment_address::operator=(const payment_address& other);
//%ignore operator>>(std::istream& in, payment_address& to);
//%ignore operator<<(std::ostream& out, const payment_address& of);
//%rename(toBoolean) libbitcoin::wallet::payment_address::operator bool() const;
//%rename(toShort_hash) libbitcoin::wallet::payment_address::operator const short_hash&() const;

//%ignore std::hash<bc::wallet::payment_address>;
//%import <boost/functional.hpp>
%import "bitcoin/bitcoin/wallet/payment_address.hpp"
%include "bitcoin/wallet_payment_address.hpp"
//%template(PaymentAddressVector) std::vector<libbitcoin::wallet::payment_address>;

//%rename(lt) libbitcoin::wallet::stealth_address::operator<(const stealth_address& other) const;
//%rename(eq) libbitcoin::wallet::stealth_address::operator==(const stealth_address& other) const;
//%rename(ne) libbitcoin::wallet::stealth_address::operator!=(const stealth_address& other) const;
//%rename(assign) libbitcoin::wallet::stealth_address::operator=(const stealth_address& other);
//%ignore operator>>(std::istream& in, stealth_address& to);
//%ignore operator<<(std::ostream& out, const stealth_address& of);

//%rename(toBoolean) libbitcoin::wallet::stealth_address::operator bool() const;
//%rename(toData_chunk) libbitcoin::wallet::stealth_address::operator const data_chunk() const;

%import "bitcoin/bitcoin/wallet/stealth_address.hpp"
%include "bitcoin/wallet_stealth_address.hpp"

//%rename(lt) libbitcoin::wallet::bitcoin_uri::operator<(const bitcoin_uri& other) const;
//%rename(eq) libbitcoin::wallet::bitcoin_uri::operator==(const bitcoin_uri& other) const;
//%rename(ne) libbitcoin::wallet::bitcoin_uri::operator!=(const bitcoin_uri& other) const;
//%rename(assign) libbitcoin::wallet::bitcoin_uri::operator=(const bitcoin_uri& other);
//%ignore operator>>(std::istream& in, bitcoin_uri& to);
//%ignore operator<<(std::ostream& out, const bitcoin_uri& from);
//%rename(toBoolean) libbitcoin::wallet::bitcoin_uri::operator bool() const;
%import "bitcoin/bitcoin/wallet/bitcoin_uri.hpp"
%ignore getValue();
%ignore setValue(wallet::bitcoin_uri value);

%include "bitcoin/wallet_bitcoin_uri.hpp"
%import "bitcoin/bitcoin/wallet/dictionary.hpp"
%ignore setValue(wallet::dictionary value);
%include "bitcoin/wallet_dictionary.hpp"

%import "bitcoin/bitcoin/wallet/encrypted_keys.hpp"
%ignore setValue(wallet::ek_seed value);
%include "bitcoin/wallet_ek_seed.hpp"
%ignore setValue(wallet::encrypted_private value);
%include "bitcoin/wallet_encrypted_private.hpp"
%ignore setValue(wallet::encrypted_public value);
%include "bitcoin/wallet_encrypted_public.hpp"
%ignore setValue(wallet::encrypted_token value);
%include "bitcoin/wallet_encrypted_token.hpp"
%include "bitcoin/wallet_encrypted_keys.hpp"

//%rename(lt) libbitcoin::wallet::ek_private::operator<(const ek_private& other) const;
//%rename(eq) libbitcoin::wallet::ek_private::operator==(const ek_private& other) const;
//%rename(ne) libbitcoin::wallet::ek_private::operator!=(const ek_private& other) const;
//%rename(assign) libbitcoin::wallet::ek_private::operator=(const ek_private& other);
//%ignore operator>>(std::istream& in, ek_private& to);
//%ignore operator<<(std::ostream& out, const ek_private& of);
//%rename(toBoolean) libbitcoin::wallet::ek_private::operator bool() const;
//%rename(toEncrypted_private) libbitcoin::wallet::ek_private::operator const encrypted_private&() const;

%import "bitcoin/bitcoin/wallet/ek_private.hpp"

%include "bitcoin/wallet_ek_private.hpp"

//%rename(lt) libbitcoin::wallet::ek_public::operator<(const ek_public& other) const;
//%rename(eq) libbitcoin::wallet::ek_public::operator==(const ek_public& other) const;
//%rename(ne) libbitcoin::wallet::ek_public::operator!=(const ek_public& other) const;
//%rename(assign) libbitcoin::wallet::ek_public::operator=(const ek_public& other);
//%ignore operator>>(std::istream& in, ek_public& to);
//%ignore operator<<(std::ostream& out, const ek_public& of);
//%rename(toBoolean) libbitcoin::wallet::ek_public::operator bool() const;
//%rename(toEncrypted_public) libbitcoin::wallet::ek_public::operator const encrypted_public&() const;

%import "bitcoin/bitcoin/wallet/ek_public.hpp"
%include "bitcoin/wallet_ek_public.hpp"

//%rename(lt) libbitcoin::wallet::ek_token::operator<(const ek_token& other) const;
//%rename(eq) libbitcoin::wallet::ek_token::operator==(const ek_token& other) const;
//%rename(ne) libbitcoin::wallet::ek_token::operator!=(const ek_token& other) const;
//%rename(assign) libbitcoin::wallet::ek_token::operator=(const ek_token& other);
//%ignore operator>>(std::istream& in, ek_token& to);
//%ignore operator<<(std::ostream& out, const ek_token& of);
//%rename(toBoolean) libbitcoin::wallet::ek_token::operator bool() const;
//%rename(toEncrypted_token) libbitcoin::wallet::ek_token::operator const encrypted_token&() const;

%import "bitcoin/bitcoin/wallet/ek_token.hpp"
%include "bitcoin/wallet_ek_token.hpp"

//%rename(electrum_decode_mnemonic) libbitcoin::wallet::electrum::decode_mnemonic(const word_list& mnemonic);

%import "bitcoin/bitcoin/wallet/electrum_dictionary.hpp"
%ignore setValue(wallet::electrum::word_list value);
%include "bitcoin/wallet_electrum_word_list.hpp"
%ignore setValue(wallet::dictionary_v1 value);
%include "bitcoin/wallet_electrum_dictionary.hpp"
%ignore setValue(wallet::dictionary_list value);
%include "bitcoin/wallet_dictionary_list.hpp"
%import "bitcoin/bitcoin/wallet/electrum.hpp"
%import "bitcoin/math_long_hash.hpp"
%include "bitcoin/wallet_electrum.hpp"

//%rename(eq) libbitcoin::wallet::hd_lineage::operator==(const hd_lineage& other) const;
//%rename(ne) libbitcoin::wallet::hd_lineage::operator!=(const hd_lineage& other) const;
//%rename(lt) libbitcoin::wallet::hd_public::operator<(const hd_public& other) const;
//%rename(eq) libbitcoin::wallet::hd_public::operator==(const hd_public& other) const;
//%rename(ne) libbitcoin::wallet::hd_public::operator!=(const hd_public& other) const;
//%rename(assign) libbitcoin::wallet::hd_public::operator=(const hd_public& other);
//%ignore operator>>(std::istream& in, hd_public& to);
//%ignore operator<<(std::ostream& out, const hd_public& of);
//%rename(toBoolean) operator bool() const;
//%rename(toEc_compressed) operator const ec_compressed&() const;

%import "bitcoin/bitcoin/wallet/hd_public.hpp"
%ignore setValue(wallet::hd_chain_code value);
%include "bitcoin/wallet_hd_chain_code.hpp"
%ignore setValue(wallet::hd_key value);
%include "bitcoin/wallet_hd_key.hpp"
%include "bitcoin/wallet_hd_public.hpp"

//%rename(lt) libbitcoin::wallet::hd_private::operator<(const hd_private& other) const;
//%rename(eq) libbitcoin::wallet::hd_private::operator==(const hd_private& other) const;
//%rename(ne) libbitcoin::wallet::hd_private::operator!=(const hd_private& other) const;
//%rename(assign) libbitcoin::wallet::hd_private::operator=(hd_private other);
//%ignore operator>>(std::istream& in, hd_private& to);
//%ignore operator<<(std::ostream& out, const hd_private& of);
//%rename(swap) swap(hd_private& left, hd_private& right);
//%rename(toEc_secret) operator const ec_secret&() const;

%import "bitcoin/bitcoin/wallet/hd_private.hpp"
%include "bitcoin/wallet_hd_private.hpp"

//%import <stdint.i>
//%import <carrays.i>
//%apply bool * {bool& out_compressed}
//%apply uint8_t * {uint8_t& out_recovery_id}
%import "bitcoin/bitcoin/wallet/message.hpp"
%ignore setValue(wallet::message_signature value);
%include "bitcoin/wallet_message_signature.hpp"
%include "bitcoin/wallet_message.hpp"

%ignore setValue(wallet::word_list value);
%include "bitcoin/wallet_word_list.hpp"

%import "bitcoin/bitcoin/wallet/mini_keys.hpp"
%include "bitcoin/wallet_mini_keys.hpp"
%import "bitcoin/bitcoin/wallet/mnemonic.hpp"
%include "bitcoin/wallet_mnemonic.hpp"

%import "bitcoin/bitcoin/wallet/qrcode.hpp"
%include "bitcoin/wallet_qrcode.hpp"
%import "bitcoin/bitcoin/wallet/select_outputs.hpp"
%include "bitcoin/wallet_select_outputs.hpp"

//%rename(toBoolean) libbitcoin::wallet::stealth_receiver::operator bool() const;

%import "bitcoin/bitcoin/wallet/stealth_receiver.hpp"
%include "bitcoin/wallet_stealth_receiver.hpp"

//%rename(toBoolean) libbitcoin::wallet::stealth_sender::operator bool() const;

%import "bitcoin/bitcoin/wallet/stealth_sender.hpp"
%include "bitcoin/wallet_stealth_sender.hpp"
