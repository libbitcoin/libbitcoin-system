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
%ignore operator=(const wallet_payment_address& other);
%include "bitcoin/wallet_payment_address.hpp"
//%template(PaymentAddressVector) std::vector<libbitcoin::wallet::payment_address>;
