
%import <p_u_long_long.i>
%apply unsigned long long *OUTVALUE { uint64_t& out };

%import <vector.i>

%rename(toBoolean) libbitcoin::config::authority::operator bool() const;
%rename(eq) libbitcoin::config::authority::operator==(const authority& other) const;
%rename(ne) libbitcoin::config::authority::operator!=(const authority& other) const;
%ignore operator>>(std::istream& input, authority& argument);
%ignore operator<<(std::ostream& output, const authority& argument);
%include "../include/bitcoin/bitcoin/config/authority.hpp"

%rename(eq) libbitcoin::config::checkpoint::operator==(const checkpoint& other) const;
%ignore operator>>(std::istream& input, checkpoint& argument);
%ignore operator<<(std::ostream& output, const checkpoint& argument);
%include "../include/bitcoin/bitcoin/config/checkpoint.hpp"

%rename(toHeader) libbitcoin::config::header::operator const chain::header&() const;
%ignore operator>>(std::istream& input, header& argument);
%ignore operator<<(std::ostream& output, const header& argument);
%include "../include/bitcoin/bitcoin/config/header.hpp"

%rename(toInput) libbitcoin::config::input::operator const chain::input&() const;
%ignore operator>>(std::istream& stream, input& argument);
%ignore operator<<(std::ostream& output, const input& argument);
%include "../include/bitcoin/bitcoin/config/input.hpp"

%rename(toTransaction) operator const chain::transaction&() const;
%ignore operator>>(std::istream& input, transaction& argument);
%ignore operator<<(std::ostream& output, const transaction& argument);
%include "../include/bitcoin/bitcoin/config/transaction.hpp"

%template(ConfigHeaderVector) std::vector<libbitcoin::config::header>;
%template(ConfigInputVector) std::vector<libbitcoin::config::input>;
%template(ConfigTransactionVector) std::vector<libbitcoin::config::transaction>;
