/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_WORDS_DICTIONARY_HPP
#define LIBBITCOIN_SYSTEM_WORDS_DICTIONARY_HPP

#include <array>
#include <string>
#include <vector>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/words/language.hpp>

namespace libbitcoin {
namespace system {
namespace words {

// Search container for a dictionary of lexically-sorted words.
// POD dictionary wrapper with O(n) search and O(1) index.
// Search order is guaranteed, always returns first match.
template<size_t Size>
class dictionary
{
public:
    typedef std::vector<size_t> search;
    typedef std::vector<int32_t> result;
    typedef struct { bool sorted; std::array<const char*, Size> word; } words;
    static_assert(Size <= possible_narrow_sign_cast<size_t>(max_int32));

    /// The number of words in the dictionary.
    static constexpr size_t size() NOEXCEPT { return Size; };

    /// Constructor.
    dictionary(language identifier, const words& words) NOEXCEPT;

    /// The language identifier of the dictionary.
    language identifier() const NOEXCEPT;

    /// The language name of the dictionary.
    std::string name() const NOEXCEPT;

    /// Search.

    /// Empty string if index > Size.
    std::string at(size_t index) const NOEXCEPT;

    /// Empty string for any index > Size.
    string_list at(const search& indexes) const NOEXCEPT;

    /// -1 if word is not found.
    int32_t index(const std::string& word) const NOEXCEPT;

    /// -1 for any word that is not found.
    result index(const string_list& words) const NOEXCEPT;

    /// True if the word is in the dictionary.
    bool contains(const std::string& word) const NOEXCEPT;

    /// True if all words are in the dictionary.
    bool contains(const string_list& words) const NOEXCEPT;

private:
    // This dictionary creates only this one word of state.
    const language identifier_;

    // Arrays of words are declared statically and held by reference here.
    // The array type is POD, so no words are copied into the array. Only
    // this wrapper dictionary object is created for each word list, for
    // each dictionaries object constructed by various mnemonic classes.
    const words& words_;
};

} // namespace words
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/words/dictionary.ipp>

#endif
