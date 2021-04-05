/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_SYSTEM_WALLET_MNEMONIC_HPP
#define LIBBITCOIN_SYSTEM_WALLET_MNEMONIC_HPP

#include <cstddef>
#include <string>
#include <bitcoin/system/compat.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/hash.hpp>
#include <bitcoin/system/utility/data.hpp>
#include <bitcoin/system/utility/string.hpp>
#include <bitcoin/system/wallet/dictionary.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

/// A wallet mnemonic, as defined by BIP39.
class BC_API mnemonic
{
public:
    static const size_t word_multiple;
    static const size_t word_minimum;
    static const size_t word_maximum;

    static const size_t entropy_multiple;
    static const size_t entropy_minumum;
    static const size_t entropy_maximum;

    static const dictionary& to_dictionary(reference lexicon);
    static reference to_reference(const string_list& words);

#ifdef WITH_ICU
    static long_hash to_seed(const string_list& words,
        const std::string& passphrase="");
#endif

    /// Constructors.
    mnemonic();
    mnemonic(const mnemonic& other);
    mnemonic(const std::string& sentence);
    mnemonic(const data_slice& entropy, reference lexicon=reference::en);
    mnemonic(const string_list& words, reference lexicon=reference::none);

    /// Operators.
    bool operator<(const mnemonic& other) const;
    bool operator==(const mnemonic& other) const;
    bool operator!=(const mnemonic& other) const;
    mnemonic& operator=(const mnemonic& other);
    friend std::istream& operator>>(std::istream& in, mnemonic& to);
    friend std::ostream& operator<<(std::ostream& out, const mnemonic& of);

    /// Cast operators.
    operator bool() const;
    operator reference() const;
    operator const data_chunk&() const;

    /// Serializer.
    std::string sentence() const;

    /// Accessors.
    const data_chunk& entropy() const;
    const string_list& words() const;
    reference lexicon() const;

    /// Methods.
#ifdef WITH_ICU
    long_hash to_seed(const std::string& passphrase="") const;
#endif

private:
    static mnemonic from_entropy(const data_slice& entropy,
        reference lexicon);
    static mnemonic from_words(const string_list& words,
        reference language);

    mnemonic(const data_chunk& entropy, const string_list& words,
        reference lexicon);

    /// These should be const, apart from the need to implement assignment.
    data_chunk entropy_;
    string_list words_;
    reference lexicon_;
};

} // namespace wallet
} // namespace system
} // namespace libbitcoin

#endif
