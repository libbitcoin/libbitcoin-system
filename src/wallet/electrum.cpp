/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/wallet/electrum.hpp>

#include <algorithm>
#include <cstdint>
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/locale.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/unicode/unicode.hpp>
#include <bitcoin/system/unicode/unicode_ostream.hpp>
#include <bitcoin/system/utility/assert.hpp>
#include <bitcoin/system/utility/binary.hpp>
#include <bitcoin/system/utility/collection.hpp>
#include <bitcoin/system/utility/string.hpp>
#include <bitcoin/system/utility/container_sink.hpp>
#include <bitcoin/system/utility/ostream_writer.hpp>
#include <bitcoin/system/wallet/electrum_dictionary.hpp>
#include "../math/external/pkcs5_pbkdf2.h"

namespace libbitcoin {
namespace system {
namespace wallet {
namespace electrum {

using namespace std::placeholders;

// Electrum mnemonic private constants.
static constexpr size_t hmac_iterations = 2048;
static const std::string passphrase_prefix = "electrum";
static const std::string seed_version = "Seed version";
static const auto hmac_data = to_chunk(seed_version);

// Valid seed prefixes.
static const std::string seed_prefix_empty{};
static const std::string seed_prefix_standard{ "01" };
static const std::string seed_prefix_witness{ "100" };
static const std::string seed_prefix_two_factor_authentication{ "101" };

#ifdef WITH_ICU

typedef std::pair<char32_t, char32_t> unicode_interval;
typedef std::vector<unicode_interval> unicode_interval_list;

// Based on Electrum source, which references:
// http://www.asahi-net.or.jp/~ax2s-kmtn/ref/unicode/e_asia.html
static const unicode_interval_list cjk
{
    { 0x4e00, 0x9fff }, // cjk unified ideographs
    { 0x3400, 0x4dbf }, // cjk unified ideographs extension a
    { 0x20000, 0x2a6df }, // cjk unified ideographs extension b
    { 0x2a700, 0x2b73f }, // cjk unified ideographs extension c
    { 0x2b740, 0x2b81f }, // cjk unified ideographs extension d
    { 0xf900, 0xfaff }, // cjk compatibility ideographs
    { 0x2f800, 0x2fa1d }, // cjk compatibility ideographs supplement
    { 0x3190, 0x319f }, // kanbun
    { 0x2e80, 0x2eff }, // cjk radicals supplement
    { 0x2f00, 0x2fdf }, // cjk radicals
    { 0x31c0, 0x31ef }, // cjk strokes
    { 0x2ff0, 0x2fff }, // ideographic description characters
    { 0xe0100, 0xe01ef }, // variation selectors supplement
    { 0x3100, 0x312f }, // bopomofo
    { 0x31a0, 0x31bf }, // bopomofo extended
    { 0xff00, 0xffef }, // halfwidth and fullwidth forms
    { 0x3040, 0x309f }, // hiragana
    { 0x30a0, 0x30ff }, // katakana
    { 0x31f0, 0x31ff }, // katakana phonetic extensions
    { 0x1b000, 0x1b0ff }, // kana supplement
    { 0xac00, 0xd7af }, // hangul syllables
    { 0x1100, 0x11ff }, // hangul jamo
    { 0xa960, 0xa97f }, // hangul jamo extended a
    { 0xd7b0, 0xd7ff }, // hangul jamo extended b
    { 0x3130, 0x318f }, // hangul compatibility jamo
    { 0xa4d0, 0xa4ff }, // lisu
    { 0x16f00, 0x16f9f }, // miao
    { 0xa000, 0xa48f }, // yi syllables
    { 0xa490, 0xa4cf }, // yi radicals
};

// Combining Diacritics list taken from:
// https://www.unicode.org/Public/UNIDATA/PropList.txt
// * https://www.unicode.org/Public/11.0.0/charts/CodeCharts.pdf
//   Section "Combining Diacritical Marks"
static const unicode_interval_list combining
{
    { 0x005e, 0x005e }, // circumflex accent
    { 0x0060, 0x0060 }, // grave accent
    { 0x00a8, 0x00a8 }, // diaeresis
    { 0x00af, 0x00af }, // macron
    { 0x00b4, 0x00b4 }, // acute accent
    { 0x00b7, 0x00b7 }, // middle dot
    { 0x00b8, 0x00b8 }, // cedilla
    { 0x02b0, 0x02c1 }, // modifier letter small h..modifier letter reversed glottal stop
    { 0x02c2, 0x02c5 }, // modifier letter left arrowhead..modifier letter down arrowhead
    { 0x02c6, 0x02d1 }, // modifier letter circumflex accent..modifier letter half triangular colon
    { 0x02d2, 0x02df }, // modifier letter centred right half ring..modifier letter cross accent
    { 0x02e0, 0x02e4 }, // modifier letter small gamma..modifier letter small reversed glottal stop
    { 0x02e5, 0x02eb }, // modifier letter extra-high tone bar..modifier letter yang departing tone mark
    { 0x02ec, 0x02ec }, // modifier letter voicing
    { 0x02ed, 0x02ed }, // modifier letter unaspirated
    { 0x02ee, 0x02ee }, // modifier letter double apostrophe
    { 0x02ef, 0x02ff }, // modifier letter low down arrowhead..modifier letter low left arrow
    { 0x0300, 0x034e }, // * see reference above
    { 0x0350, 0x036f }, // * see reference above
    { 0x0374, 0x0374 }, // greek numeral sign
    { 0x0375, 0x0375 }, // greek lower numeral sign
    { 0x037a, 0x037a }, // greek ypogegrammeni
    { 0x0384, 0x0385 }, // greek tonos..greek dialytika tonos
    { 0x0483, 0x0487 }, // combining cyrillic titlo..combining cyrillic pokrytie
    { 0x0559, 0x0559 }, // armenian modifier letter left half ring
    { 0x0591, 0x05a1 }, // hebrew accent etnahta..hebrew accent pazer
    { 0x05a3, 0x05bd }, // hebrew accent munah..hebrew point meteg
    { 0x05bf, 0x05bf }, // hebrew point rafe
    { 0x05c1, 0x05c2 }, // hebrew point shin dot..hebrew point sin dot
    { 0x05c4, 0x05c4 }, // hebrew mark upper dot
    { 0x064b, 0x0652 }, // arabic fathatan..arabic sukun
    { 0x0657, 0x0658 }, // arabic inverted damma..arabic mark noon ghunna
    { 0x06df, 0x06e0 }, // arabic small high rounded zero..arabic small high upright rectangular zero
    { 0x06e5, 0x06e6 }, // arabic small waw..arabic small yeh
    { 0x06ea, 0x06ec }, // arabic empty centre low stop..arabic rounded high stop with filled centre
    { 0x0730, 0x074a }, // syriac pthaha above..syriac barrekh
    { 0x07a6, 0x07b0 }, // thaana abafili..thaana sukun
    { 0x07eb, 0x07f3 }, // nko combining short high tone..nko combining double dot above
    { 0x07f4, 0x07f5 }, // nko high tone apostrophe..nko low tone apostrophe
    { 0x0818, 0x0819 }, // samaritan mark occlusion..samaritan mark dagesh
    { 0x08e3, 0x08fe }, // arabic turned damma below..arabic damma with dot
    { 0x093c, 0x093c }, // devanagari sign nukta
    { 0x094d, 0x094d }, // devanagari sign virama
    { 0x0951, 0x0954 }, // devanagari stress sign udatta..devanagari acute accent
    { 0x0971, 0x0971 }, // devanagari sign high spacing dot
    { 0x09bc, 0x09bc }, // bengali sign nukta
    { 0x09cd, 0x09cd }, // bengali sign virama
    { 0x0a3c, 0x0a3c }, // gurmukhi sign nukta
    { 0x0a4d, 0x0a4d }, // gurmukhi sign virama
    { 0x0abc, 0x0abc }, // gujarati sign nukta
    { 0x0acd, 0x0acd }, // gujarati sign virama
    { 0x0afd, 0x0aff }, // gujarati sign three-dot nukta above..gujarati sign two-circle nukta above
    { 0x0b3c, 0x0b3c }, // oriya sign nukta
    { 0x0b4d, 0x0b4d }, // oriya sign virama
    { 0x0bcd, 0x0bcd }, // tamil sign virama
    { 0x0c4d, 0x0c4d }, // telugu sign virama
    { 0x0cbc, 0x0cbc }, // kannada sign nukta
    { 0x0ccd, 0x0ccd }, // kannada sign virama
    { 0x0d3b, 0x0d3c }, // malayalam sign vertical bar virama..malayalam sign circular virama
    { 0x0d4d, 0x0d4d }, // malayalam sign virama
    { 0x0dca, 0x0dca }, // sinhala sign al-lakuna
    { 0x0e47, 0x0e4c }, // thai character maitaikhu..thai character thanthakhat
    { 0x0e4e, 0x0e4e }, // thai character yamakkan
    { 0x0ec8, 0x0ecc }, // lao tone mai ek..lao cancellation mark
    { 0x0f18, 0x0f19 }, // tibetan astrological sign -khyud pa..tibetan astrological sign sdong tshugs
    { 0x0f35, 0x0f35 }, // tibetan mark ngas bzung nyi zla
    { 0x0f37, 0x0f37 }, // tibetan mark ngas bzung sgor rtags
    { 0x0f39, 0x0f39 }, // tibetan mark tsa -phru
    { 0x0f3e, 0x0f3f }, // tibetan sign yar tshes..tibetan sign mar tshes
    { 0x0f82, 0x0f84 }, // tibetan sign nyi zla naa da..tibetan mark halanta
    { 0x0f86, 0x0f87 }, // tibetan sign lci rtags..tibetan sign yang rtags
    { 0x0fc6, 0x0fc6 }, // tibetan symbol padma gdan
    { 0x1037, 0x1037 }, // myanmar sign dot below
    { 0x1039, 0x103a }, // myanmar sign virama..myanmar sign asat
    { 0x1087, 0x108c }, // myanmar sign shan tone-2..myanmar sign shan council tone-3
    { 0x108d, 0x108d }, // myanmar sign shan council emphatic tone
    { 0x108f, 0x108f }, // myanmar sign rumai palaung tone-5
    { 0x109a, 0x109b }, // myanmar sign khamti tone-1..myanmar sign khamti tone-3
    { 0x17c9, 0x17d3 }, // khmer sign muusikatoan..khmer sign bathamasat
    { 0x17dd, 0x17dd }, // khmer sign atthacan
    { 0x1939, 0x193b }, // limbu sign mukphreng..limbu sign sa-i
    { 0x1a75, 0x1a7c }, // tai tham sign tone-1..tai tham sign khuen-lue karan
    { 0x1a7f, 0x1a7f }, // tai tham combining cryptogrammic dot
    { 0x1ab0, 0x1abd }, // combining doubled circumflex accent..combining parentheses below
    { 0x1b34, 0x1b34 }, // balinese sign rerekan
    { 0x1b44, 0x1b44 }, // balinese adeg adeg
    { 0x1b6b, 0x1b73 }, // balinese musical symbol combining tegeh..balinese musical symbol combining gong
    { 0x1baa, 0x1baa }, // sundanese sign pamaaeh
    { 0x1bab, 0x1bab }, // sundanese sign virama
    { 0x1c36, 0x1c37 }, // lepcha sign ran..lepcha sign nukta
    { 0x1c78, 0x1c7d }, // ol chiki mu ttuddag..ol chiki ahad
    { 0x1cd0, 0x1cd2 }, // vedic tone karshana..vedic tone prenkha
    { 0x1cd3, 0x1cd3 }, // vedic sign nihshvasa
    { 0x1cd4, 0x1ce0 }, // vedic sign yajurvedic midline svarita..vedic tone rigvedic kashmiri independent svarita
    { 0x1ce1, 0x1ce1 }, // vedic tone atharvavedic independent svarita
    { 0x1ce2, 0x1ce8 }, // vedic sign visarga svarita..vedic sign visarga anudatta with tail
    { 0x1ced, 0x1ced }, // vedic sign tiryak
    { 0x1cf4, 0x1cf4 }, // vedic tone candra above
    { 0x1cf7, 0x1cf7 }, // vedic sign atikrama
    { 0x1cf8, 0x1cf9 }, // vedic tone ring above..vedic tone double ring above
    { 0x1d2c, 0x1d6a }, // modifier letter capital a..greek subscript small letter chi
    { 0x1dc4, 0x1dcf }, // combining macron-acute..combining zigzag below
    { 0x1df5, 0x1df9 }, // combining up tack above..combining wide inverted bridge below
    { 0x1dfd, 0x1dff }, // combining almost equal to below..combining right arrowhead and down arrowhead below
    { 0x1fbd, 0x1fbd }, // greek koronis
    { 0x1fbf, 0x1fc1 }, // greek psili..greek dialytika and perispomeni
    { 0x1fcd, 0x1fcf }, // greek psili and varia..greek psili and perispomeni
    { 0x1fdd, 0x1fdf }, // greek dasia and varia..greek dasia and perispomeni
    { 0x1fed, 0x1fef }, // greek dialytika and varia..greek varia
    { 0x1ffd, 0x1ffe }, // greek oxia..greek dasia
    { 0x2cef, 0x2cf1 }, // coptic combining ni above..coptic combining spiritus lenis
    { 0x2e2f, 0x2e2f }, // vertical tilde
    { 0x302a, 0x302d }, // ideographic level tone mark..ideographic entering tone mark
    { 0x302e, 0x302f }, // hangul single dot tone mark..hangul double dot tone mark
    { 0x3099, 0x309a }, // combining katakana-hiragana voiced sound mark..combining katakana-hiragana semi-voiced sound mark
    { 0x309b, 0x309c }, // katakana-hiragana voiced sound mark..katakana-hiragana semi-voiced sound mark
    { 0x30fc, 0x30fc }, // katakana-hiragana prolonged sound mark
    { 0xa66f, 0xa66f }, // combining cyrillic vzmet
    { 0xa67c, 0xa67d }, // combining cyrillic kavyka..combining cyrillic payerok
    { 0xa67f, 0xa67f }, // cyrillic payerok
    { 0xa69c, 0xa69d }, // modifier letter cyrillic hard sign..modifier letter cyrillic soft sign
    { 0xa6f0, 0xa6f1 }, // bamum combining mark koqndon..bamum combining mark tukwentis
    { 0xa717, 0xa71f }, // modifier letter dot vertical bar..modifier letter low inverted exclamation mark
    { 0xa720, 0xa721 }, // modifier letter stress and high tone..modifier letter stress and low tone
    { 0xa788, 0xa788 }, // modifier letter low circumflex accent
    { 0xa7f8, 0xa7f9 }, // modifier letter capital h with stroke..modifier letter small ligature oe
    { 0xa8c4, 0xa8c4 }, // saurashtra sign virama
    { 0xa8e0, 0xa8f1 }, // combining devanagari digit zero..combining devanagari sign avagraha
    { 0xa92b, 0xa92d }, // kayah li tone plophu..kayah li tone calya plophu
    { 0xa92e, 0xa92e }, // kayah li sign cwi
    { 0xa953, 0xa953 }, // rejang virama
    { 0xa9b3, 0xa9b3 }, // javanese sign cecak telu
    { 0xa9c0, 0xa9c0 }, // javanese pangkon
    { 0xa9e5, 0xa9e5 }, // myanmar sign shan saw
    { 0xaa7b, 0xaa7b }, // myanmar sign pao karen tone
    { 0xaa7c, 0xaa7c }, // myanmar sign tai laing tone-2
    { 0xaa7d, 0xaa7d }, // myanmar sign tai laing tone-5
    { 0xaabf, 0xaabf }, // tai viet tone mai ek
    { 0xaac0, 0xaac0 }, // tai viet tone mai nueng
    { 0xaac1, 0xaac1 }, // tai viet tone mai tho
    { 0xaac2, 0xaac2 }, // tai viet tone mai song
    { 0xaaf6, 0xaaf6 }, // meetei mayek virama
    { 0xab5b, 0xab5b }, // modifier breve with inverted breve
    { 0xab5c, 0xab5f }, // modifier letter small heng..modifier letter small u with left hook
    { 0xabec, 0xabec }, // meetei mayek lum iyek
    { 0xabed, 0xabed }, // meetei mayek apun iyek
    { 0xfb1e, 0xfb1e }, // hebrew point judeo-spanish varika
    { 0xfe20, 0xfe2f }, // combining ligature left half..combining cyrillic titlo right half
    { 0xff3e, 0xff3e }, // fullwidth circumflex accent
    { 0xff40, 0xff40 }, // fullwidth grave accent
    { 0xff70, 0xff70 }, // halfwidth katakana-hiragana prolonged sound mark
    { 0xff9e, 0xff9f }, // halfwidth katakana voiced sound mark..halfwidth katakana semi-voiced sound mark
    { 0xffe3, 0xffe3 }, // fullwidth macron
    { 0x102e0, 0x102e0 }, // coptic epact thousands mark
    { 0x10ae5, 0x10ae6 }, // manichaean abbreviation mark above..manichaean abbreviation mark below
    { 0x10d22, 0x10d23 }, // hanifi rohingya mark sakin..hanifi rohingya mark na khonna
    { 0x10d24, 0x10d27 }, // hanifi rohingya sign harbahay..hanifi rohingya sign tassi
    { 0x10f46, 0x10f50 }, // sogdian combining dot below..sogdian combining stroke below
    { 0x110b9, 0x110ba }, // kaithi sign virama..kaithi sign nukta
    { 0x11133, 0x11134 }, // chakma virama..chakma maayyaa
    { 0x11173, 0x11173 }, // mahajani sign nukta
    { 0x111c0, 0x111c0 }, // sharada sign virama
    { 0x111ca, 0x111cc }, // sharada sign nukta..sharada extra short vowel mark
    { 0x11235, 0x11235 }, // khojki sign virama
    { 0x11236, 0x11236 }, // khojki sign nukta
    { 0x112e9, 0x112ea }, // khudawadi sign nukta..khudawadi sign virama
    { 0x1133c, 0x1133c }, // grantha sign nukta
    { 0x1134d, 0x1134d }, // grantha sign virama
    { 0x11366, 0x1136c }, // combining grantha digit zero..combining grantha digit six
    { 0x11370, 0x11374 }, // combining grantha letter a..combining grantha letter pa
    { 0x11442, 0x11442 }, // newa sign virama
    { 0x11446, 0x11446 }, // newa sign nukta
    { 0x114c2, 0x114c3 }, // tirhuta sign virama..tirhuta sign nukta
    { 0x115bf, 0x115c0 }, // siddham sign virama..siddham sign nukta
    { 0x1163f, 0x1163f }, // modi sign virama
    { 0x116b6, 0x116b6 }, // takri sign virama
    { 0x116b7, 0x116b7 }, // takri sign nukta
    { 0x1172b, 0x1172b }, // ahom sign killer
    { 0x11839, 0x1183a }, // dogra sign virama..dogra sign nukta
    { 0x11a34, 0x11a34 }, // zanabazar square sign virama
    { 0x11a47, 0x11a47 }, // zanabazar square subjoiner
    { 0x11a99, 0x11a99 }, // soyombo subjoiner
    { 0x11c3f, 0x11c3f }, // bhaiksuki sign virama
    { 0x11d42, 0x11d42 }, // masaram gondi sign nukta
    { 0x11d44, 0x11d45 }, // masaram gondi sign halanta..masaram gondi virama
    { 0x11d97, 0x11d97 }, // gunjala gondi virama
    { 0x16af0, 0x16af4 }, // bassa vah combining high tone..bassa vah combining high-low tone
    { 0x16f8f, 0x16f92 }, // miao tone right..miao tone below
    { 0x16f93, 0x16f9f }, // miao letter tone-2..miao letter reformed tone-8
    { 0x1d167, 0x1d169 }, // musical symbol combining tremolo-1..musical symbol combining tremolo-3
    { 0x1d16d, 0x1d172 }, // musical symbol combining augmentation dot..musical symbol combining flag-5
    { 0x1d17b, 0x1d182 }, // musical symbol combining accent..musical symbol combining loure
    { 0x1d185, 0x1d18b }, // musical symbol combining doit..musical symbol combining triple tongue
    { 0x1d1aa, 0x1d1ad }, // musical symbol combining down bow..musical symbol combining snap pizzicato
    { 0x1e8d0, 0x1e8d6 }, // mende kikakui combining number teens..mende kikakui combining number millions
    { 0x1e944, 0x1e946 }, // adlam alif lengthener..adlam gemination mark
    { 0x1e948, 0x1e94a }, // adlam consonant modifier..adlam nukta
};

static bool in_range(const unicode_interval_list& intervals, char32_t ch)
{
    for (const auto& interval: intervals)
        if (ch >= interval.first && ch <= interval.second)
            return true;

    return false;
}

static auto is_cjk = std::bind(in_range, cjk, _1);
static auto is_combining = std::bind(in_range, combining, _1);

// This check can use the standard C locale because whitespace is
// restricted to ascii white-space in this context.
static inline bool is_ascii_space(char32_t current)
{
#ifdef WIN32
    static std::locale standard_locale{"C"};
    return current <= 0x7f && std::isspace(current, standard_locale);
#else
    return current <= 0x7f && std::isspace(current);
#endif
}

static size_t special_modulo(int32_t index_distance, size_t dictionary_size)
{
    return index_distance < 0 ? 
        dictionary_size - (-index_distance % dictionary_size) :
        index_distance % dictionary_size;
}

static data_chunk old_mnemonic_decode(const word_list& mnemonic)
{
    static constexpr size_t mnemonic_word_multiple = 3;

    if ((mnemonic.size() < mnemonic_word_multiple) ||
        (mnemonic.size() % mnemonic_word_multiple) != 0)
        return {};

    const auto& lexicon = language::electrum::en_v1;
    const auto seed_size = sizeof(uint32_t) * mnemonic.size() / 3;
    const auto size = lexicon.size();

    data_chunk seed;
    seed.reserve(seed_size);
    data_sink ostream(seed);
    ostream_writer sink(ostream);

    for (size_t i = 0; i < mnemonic.size() / 3; i += 3)
    {
        const auto first = find_position(lexicon, mnemonic[i]);
        const auto second = find_position(lexicon, mnemonic[i+1]);
        const auto third = find_position(lexicon, mnemonic[i+2]);

        if ((first == -1) || (second == -1) || (third == -1))
            return {};

        // TODO: prove there is no overflow risk.
        const auto value = first +
            size * special_modulo(second - first, size) +
            size * size * special_modulo(third - second, size);

        BITCOIN_ASSERT(value < max_uint32);
        sink.write_variable_little_endian(static_cast<uint32_t>(value));
    }

    return seed;
}

static bool is_old_seed(const word_list& mnemonic, const dictionary& lexicon)
{
    // Cannot be an old seed if it's not the en dictionary.
    if (lexicon != language::electrum::en)
        return false;

    // valid electrum 1.x seed lengths
    static constexpr size_t seed_size_v1_1 = 16;
    static constexpr size_t seed_size_v1_2 = 32;

    // valid electrum 1.x mnemonic lengths
    static constexpr size_t mnemonic_size_v1_1 = 12;
    static constexpr size_t mnemonic_size_v1_2 = 24;

    const auto seed = old_mnemonic_decode(mnemonic);

    return !seed.empty() && (
        seed.size() == seed_size_v1_1 ||
        seed.size() == seed_size_v1_2 ||
        mnemonic.size() == mnemonic_size_v1_1 ||
        mnemonic.size() == mnemonic_size_v1_2);
}

/**
 * Normalize a new electrum mnenomic (list of words).  This processes
 * the mnemonic for nfkd normalization, lower cases, normalizes
 * whitespaces, removes combining diacritics/accents, then removes
 * whitespaces between CJK.
 *
 * This is modeled after Electrum's "normalize_text" routine:
 * https://github.com/spesmilo/electrum/blob/master/electrum/mnemonic.py#L77
 */
static std::string normalize_text(const std::string& text)
{
    // Create a normalized and lower-cased word list.
    auto normal_list = split(to_lower(to_normal_nfkd_form(text)));

    // Whitespace removal.
    for (auto& word : normal_list)
        boost::algorithm::trim(word);

    // Normalize whitespaces and convert the UTF-8 string to a UTF-32 string.
    auto expanded = boost::locale::conv::utf_to_utf<char32_t, uint8_t>(
        reinterpret_cast<const uint8_t*>(join(normal_list).c_str()),
            boost::locale::conv::stop);

    std::u32string normal{};

    // Remove unicode combining characters (accents).
    for (const auto ch : expanded)
        if (!is_combining(ch))
            normal += ch;

    if (normal.empty())
        return {};

    if (normal.size() < 3)
        return boost::locale::conv::utf_to_utf<char, char32_t>(
            reinterpret_cast<const char32_t*>(normal.c_str()),
                boost::locale::conv::stop);

    // Remove whitespaces between CJK by building an output with them.
    std::u32string output{ normal.begin(), normal.begin() + 1 };

    for (size_t i = 1; i < normal.size() - 1; i++)
    {
        if (!(is_ascii_space(normal[i]) &&
              is_cjk(normal[i-1]) && is_cjk(normal[i+1])))
            output += normal[i];
    }

    output += normal[normal.size() -1];

    // Return the converted UTF-32 string to UTF-8.
    return boost::locale::conv::utf_to_utf<char, char32_t>(
        reinterpret_cast<const char32_t*>(output.c_str()),
            boost::locale::conv::stop);
}

static bool is_new_seed(const word_list& mnemonic, const std::string& prefix)
{
    const auto normal = to_chunk(normalize_text(join(mnemonic)));
    const auto seed = encode_base16(hmac_sha512_hash(normal, hmac_data));
    return boost::starts_with(seed, prefix);
}

static word_list mnemonic_encode(cpp_int entropy, const dictionary& lexicon)
{
    word_list mnemonic;
    const auto dictionary_size = lexicon.size();

    while (entropy != 0)
    {
        const cpp_int index = entropy % dictionary_size;
        mnemonic.push_back(lexicon[static_cast<size_t>(index)]);
        entropy /= dictionary_size;
    }

    return mnemonic;
}

static cpp_int mnemonic_decode(const word_list& mnemonic,
    const dictionary& lexicon)
{
    cpp_int entropy = 0;
    const auto dictionary_size = lexicon.size();

    for (const auto& word: boost::adaptors::reverse(mnemonic))
    {
        const auto position = find_position(lexicon, word);
        if (position == -1)
            return { 1 };

        entropy *= dictionary_size + position;
    }

    return entropy;
}

static std::string get_seed_prefix(seed prefix)
{
    switch (prefix)
    {
        case seed::standard:
            return seed_prefix_standard;
        case seed::witness:
            return seed_prefix_witness;
        case seed::two_factor_authentication:
            return seed_prefix_two_factor_authentication;
    }

    return seed_prefix_empty;
}

word_list create_mnemonic(const data_chunk& entropy, const dictionary& lexicon,
    seed prefix)
{
    word_list mnemonic;
    const auto electrum_prefix = get_seed_prefix(prefix);

    // cpp_int requires hex string for arbitrary precision int construction.
    auto numeric_entropy = cpp_int("0x" + encode_base16(entropy));

    do
    {
        mnemonic = mnemonic_encode(numeric_entropy++, lexicon);
        BITCOIN_ASSERT(mnemonic_decode(mnemonic, lexicon) == 0);
    } while (is_old_seed(mnemonic, lexicon) ||
        !is_new_seed(mnemonic, electrum_prefix));

    return mnemonic;
}

long_hash decode_mnemonic(const word_list& mnemonic,
    const std::string& passphrase)
{
    const auto sentence = normalize_text(join(mnemonic));
    const auto salt = passphrase_prefix + normalize_text(passphrase);
    return pkcs5_pbkdf2_hmac_sha512(to_chunk(sentence), to_chunk(salt),
        hmac_iterations);
}

bool validate_mnemonic(const word_list& mnemonic, const dictionary& lexicon,
    seed prefix)
{
    return is_new_seed(mnemonic, get_seed_prefix(prefix)) &&
        (mnemonic_decode(mnemonic, lexicon) == 0);
}

bool validate_mnemonic(const word_list& mnemonic,
    const dictionary_list& lexicons, const seed prefix)
{
    for (const auto& lexicon: lexicons)
        if (validate_mnemonic(mnemonic, *lexicon, prefix))
            return true;

    return false;
}

long_hash decode_mnemonic(const word_list& mnemonic)
{
    const auto sentence = normalize_text(join(mnemonic));
    const auto& salt = passphrase_prefix;
    return pkcs5_pbkdf2_hmac_sha512(to_chunk(sentence), to_chunk(salt),
        hmac_iterations);
}

#endif

} // namespace electrum
} // namespace wallet
} // namespace system
} // namespace libbitcoin
