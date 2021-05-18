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
#include <bitcoin/system/unicode/code_points.hpp>

#include <string>
#include <bitcoin/system/unicode/conversion.hpp>
#include <bitcoin/system/utility/string.hpp>

namespace libbitcoin {
namespace system {

const char char_separators[]
{
    // ASCII separator characters.
    0x20  // space ' '
};

const std::string ascii_space
{
    char_separators[0]
};

const string_list ascii_separators
{
    { char_separators[0] }
};

const char char_whitespace[]
{
    // ASCII whitespace characters (C whitespace).
    0x09, // character tabulation '\t'
    0x0a, // line feed '\n'
    0x0b, // line tabulation '\v'
    0x0c, // form feed '\f'
    0x0d, // carriage return '\r'
    0x20  // space ' '
};

const string_list ascii_whitespace
{
    { char_whitespace[0] },
    { char_whitespace[1] },
    { char_whitespace[2] },
    { char_whitespace[3] },
    { char_whitespace[4] },
    { char_whitespace[5] }
};

const char32_t char32_separators[]
{
    // Non-ASCII Unicode separator-space characters.
    0x00000020, // space ' '
    0x000000a0, // no-break space
    0x00001680, // ogham space mark
    0x00002000, // en quad
    0x00002001, // em quad
    0x00002002, // en space
    0x00002003, // em space
    0x00002004, // three-per-em space
    0x00002005, // four-per-em space
    0x00002006, // six-per-em space
    0x00002007, // figure space
    0x00002008, // punctuation space
    0x00002009, // thin space
    0x0000200a, // hair space
    0x0000202f, // narrow no-break space
    0x0000205f, // medium mathematical space
    0x00003000  // ideographic space
};

const std::string ideographic_space
{
    to_utf8(char32_separators[16])
};

const string_list unicode_separators
{
    { to_utf8(char32_separators[0]) },
    { to_utf8(char32_separators[1]) },
    { to_utf8(char32_separators[2]) },
    { to_utf8(char32_separators[3]) },
    { to_utf8(char32_separators[4]) },
    { to_utf8(char32_separators[5]) },
    { to_utf8(char32_separators[6]) },
    { to_utf8(char32_separators[7]) },
    { to_utf8(char32_separators[8]) },
    { to_utf8(char32_separators[9]) },
    { to_utf8(char32_separators[10]) },
    { to_utf8(char32_separators[11]) },
    { to_utf8(char32_separators[12]) },
    { to_utf8(char32_separators[13]) },
    { to_utf8(char32_separators[14]) },
    { to_utf8(char32_separators[15]) },
    { to_utf8(char32_separators[16]) }
};

// unicode.org/Public/UNIDATA/PropList.txt [White_Space (25 code points)]
const char32_t char32_whitespace[]
{
    // ASCII/Unicode whitespace characters (C whitespace).
    0x00000009, // character tabulation '\t'
    0x0000000a, // line feed '\n'
    0x0000000b, // line tabulation '\v'
    0x0000000c, // form feed '\f'
    0x0000000d, // carriage return '\r'

    // ASCII/Unicode whitespace separator-space characters.
    0x00000020, // space ' '

    // Unicode separator-space characters.
    0x000000a0, // no-break space
    0x00001680, // ogham space mark
    0x00002000, // en quad
    0x00002001, // em quad
    0x00002002, // en space
    0x00002003, // em space
    0x00002004, // three-per-em space
    0x00002005, // four-per-em space
    0x00002006, // six-per-em space
    0x00002007, // figure space
    0x00002008, // punctuation space
    0x00002009, // thin space
    0x0000200a, // hair space
    0x0000202f, // narrow no-break space
    0x0000205f, // medium mathematical space
    0x00003000, // ideographic space

    // Unicode other whitespace characters.
    0x00000085, // next line
    0x00002028, // line separator
    0x00002029  // paragraph separator
};

const string_list unicode_whitespace
{
    { to_utf8(char32_whitespace[0]) },
    { to_utf8(char32_whitespace[1]) },
    { to_utf8(char32_whitespace[2]) },
    { to_utf8(char32_whitespace[3]) },
    { to_utf8(char32_whitespace[4]) },
    { to_utf8(char32_whitespace[5]) },
    { to_utf8(char32_whitespace[6]) },
    { to_utf8(char32_whitespace[7]) },
    { to_utf8(char32_whitespace[8]) },
    { to_utf8(char32_whitespace[9]) },
    { to_utf8(char32_whitespace[10]) },
    { to_utf8(char32_whitespace[11]) },
    { to_utf8(char32_whitespace[12]) },
    { to_utf8(char32_whitespace[13]) },
    { to_utf8(char32_whitespace[14]) },
    { to_utf8(char32_whitespace[15]) },
    { to_utf8(char32_whitespace[16]) },
    { to_utf8(char32_whitespace[17]) },
    { to_utf8(char32_whitespace[18]) },
    { to_utf8(char32_whitespace[19]) },
    { to_utf8(char32_whitespace[20]) },
    { to_utf8(char32_whitespace[21]) },
    { to_utf8(char32_whitespace[22]) },
    { to_utf8(char32_whitespace[23]) },
    { to_utf8(char32_whitespace[24]) }
};

// Based on Electrum source (not Unicode standard).
// github.com/spesmilo/electrum/blob/master/electrum/mnemonic.py#L41-L69
const char32_interval char32_chinese_japanese_korean[]
{
    { 0x00004e00, 0x00009fff }, // cjk unified ideographs
    { 0x00003400, 0x00004dbf }, // cjk unified ideographs extension a
    { 0x00020000, 0x0002a6df }, // cjk unified ideographs extension b
    { 0x0002a700, 0x0002b73f }, // cjk unified ideographs extension c
    { 0x0002b740, 0x0002b81f }, // cjk unified ideographs extension d
    { 0x0000f900, 0x0000faff }, // cjk compatibility ideographs
    { 0x0002f800, 0x0002fa1d }, // cjk compatibility ideographs supplement
    { 0x00003190, 0x0000319f }, // kanbun
    { 0x00002e80, 0x00002eff }, // cjk radicals supplement
    { 0x00002f00, 0x00002fdf }, // cjk radicals
    { 0x000031c0, 0x000031ef }, // cjk strokes
    { 0x00002ff0, 0x00002fff }, // ideographic description characters
    { 0x000e0100, 0x000e01ef }, // variation selectors supplement
    { 0x00003100, 0x0000312f }, // bopomofo
    { 0x000031a0, 0x000031bf }, // bopomofo extended
    { 0x0000ff00, 0x0000ffef }, // halfwidth and fullwidth forms
    { 0x00003040, 0x0000309f }, // hiragana
    { 0x000030a0, 0x000030ff }, // katakana
    { 0x000031f0, 0x000031ff }, // katakana phonetic extensions
    { 0x0001b000, 0x0001b0ff }, // kana supplement
    { 0x0000ac00, 0x0000d7af }, // hangul syllables
    { 0x00001100, 0x000011ff }, // hangul jamo
    { 0x0000a960, 0x0000a97f }, // hangul jamo extended a
    { 0x0000d7b0, 0x0000d7ff }, // hangul jamo extended b
    { 0x00003130, 0x0000318f }, // hangul compatibility jamo
    { 0x0000a4d0, 0x0000a4ff }, // lisu
    { 0x00016f00, 0x00016f9f }, // miao
    { 0x0000a000, 0x0000a48f }, // yi syllables
    { 0x0000a490, 0x0000a4cf }  // yi radicals
};

// unicode.org/Public/UNIDATA/PropList.txt [Diacritic (882 code points)]
// This is not used by Electrum, instead it uses Python's unicodedata.combining.
const char32_interval char32_diacritics[]
{
    { 0x0000005e, 0x0000005e }, // circumflex accent
    { 0x00000060, 0x00000060 }, // grave accent
    { 0x000000a8, 0x000000a8 }, // diaeresis
    { 0x000000af, 0x000000af }, // macron
    { 0x000000b4, 0x000000b4 }, // acute accent
    { 0x000000b7, 0x000000b7 }, // middle dot
    { 0x000000b8, 0x000000b8 }, // cedilla
    { 0x000002b0, 0x000002c1 }, // modifier letter small h..modifier letter reversed glottal stop
    { 0x000002c2, 0x000002c5 }, // modifier letter left arrowhead..modifier letter down arrowhead
    { 0x000002c6, 0x000002d1 }, // modifier letter circumflex accent..modifier letter half triangular colon
    { 0x000002d2, 0x000002df }, // modifier letter centred right half ring..modifier letter cross accent
    { 0x000002e0, 0x000002e4 }, // modifier letter small gamma..modifier letter small reversed glottal stop
    { 0x000002e5, 0x000002eb }, // modifier letter extra-high tone bar..modifier letter yang departing tone mark
    { 0x000002ec, 0x000002ec }, // modifier letter voicing
    { 0x000002ed, 0x000002ed }, // modifier letter unaspirated
    { 0x000002ee, 0x000002ee }, // modifier letter double apostrophe
    { 0x000002ef, 0x000002ff }, // modifier letter low down arrowhead..modifier letter low left arrow
    { 0x00000300, 0x0000034e }, // * see reference above
    { 0x00000350, 0x0000036f }, // * see reference above
    { 0x00000374, 0x00000374 }, // greek numeral sign
    { 0x00000375, 0x00000375 }, // greek lower numeral sign
    { 0x0000037a, 0x0000037a }, // greek ypogegrammeni
    { 0x00000384, 0x00000385 }, // greek tonos..greek dialytika tonos
    { 0x00000483, 0x00000487 }, // combining cyrillic titlo..combining cyrillic pokrytie
    { 0x00000559, 0x00000559 }, // armenian modifier letter left half ring
    { 0x00000591, 0x000005a1 }, // hebrew accent etnahta..hebrew accent pazer
    { 0x000005a3, 0x000005bd }, // hebrew accent munah..hebrew point meteg
    { 0x000005bf, 0x000005bf }, // hebrew point rafe
    { 0x000005c1, 0x000005c2 }, // hebrew point shin dot..hebrew point sin dot
    { 0x000005c4, 0x000005c4 }, // hebrew mark upper dot
    { 0x0000064b, 0x00000652 }, // arabic fathatan..arabic sukun
    { 0x00000657, 0x00000658 }, // arabic inverted damma..arabic mark noon ghunna
    { 0x000006df, 0x000006e0 }, // arabic small high rounded zero..arabic small high upright rectangular zero
    { 0x000006e5, 0x000006e6 }, // arabic small waw..arabic small yeh
    { 0x000006ea, 0x000006ec }, // arabic empty centre low stop..arabic rounded high stop with filled centre
    { 0x00000730, 0x0000074a }, // syriac pthaha above..syriac barrekh
    { 0x000007a6, 0x000007b0 }, // thaana abafili..thaana sukun
    { 0x000007eb, 0x000007f3 }, // nko combining short high tone..nko combining double dot above
    { 0x000007f4, 0x000007f5 }, // nko high tone apostrophe..nko low tone apostrophe
    { 0x00000818, 0x00000819 }, // samaritan mark occlusion..samaritan mark dagesh
    { 0x000008e3, 0x000008fe }, // arabic turned damma below..arabic damma with dot
    { 0x0000093c, 0x0000093c }, // devanagari sign nukta
    { 0x0000094d, 0x0000094d }, // devanagari sign virama
    { 0x00000951, 0x00000954 }, // devanagari stress sign udatta..devanagari acute accent
    { 0x00000971, 0x00000971 }, // devanagari sign high spacing dot
    { 0x000009bc, 0x000009bc }, // bengali sign nukta
    { 0x000009cd, 0x000009cd }, // bengali sign virama
    { 0x00000a3c, 0x00000a3c }, // gurmukhi sign nukta
    { 0x00000a4d, 0x00000a4d }, // gurmukhi sign virama
    { 0x00000abc, 0x00000abc }, // gujarati sign nukta
    { 0x00000acd, 0x00000acd }, // gujarati sign virama
    { 0x00000afd, 0x00000aff }, // gujarati sign three-dot nukta above..gujarati sign two-circle nukta above
    { 0x00000b3c, 0x00000b3c }, // oriya sign nukta
    { 0x00000b4d, 0x00000b4d }, // oriya sign virama
    { 0x00000bcd, 0x00000bcd }, // tamil sign virama
    { 0x00000c4d, 0x00000c4d }, // telugu sign virama
    { 0x00000cbc, 0x00000cbc }, // kannada sign nukta
    { 0x00000ccd, 0x00000ccd }, // kannada sign virama
    { 0x00000d3b, 0x00000d3c }, // malayalam sign vertical bar virama..malayalam sign circular virama
    { 0x00000d4d, 0x00000d4d }, // malayalam sign virama
    { 0x00000dca, 0x00000dca }, // sinhala sign al-lakuna
    { 0x00000e47, 0x00000e4c }, // thai character maitaikhu..thai character thanthakhat
    { 0x00000e4e, 0x00000e4e }, // thai character yamakkan
    { 0x00000ec8, 0x00000ecc }, // lao tone mai ek..lao cancellation mark
    { 0x00000f18, 0x00000f19 }, // tibetan astrological sign -khyud pa..tibetan astrological sign sdong tshugs
    { 0x00000f35, 0x00000f35 }, // tibetan mark ngas bzung nyi zla
    { 0x00000f37, 0x00000f37 }, // tibetan mark ngas bzung sgor rtags
    { 0x00000f39, 0x00000f39 }, // tibetan mark tsa -phru
    { 0x00000f3e, 0x00000f3f }, // tibetan sign yar tshes..tibetan sign mar tshes
    { 0x00000f82, 0x00000f84 }, // tibetan sign nyi zla naa da..tibetan mark halanta
    { 0x00000f86, 0x00000f87 }, // tibetan sign lci rtags..tibetan sign yang rtags
    { 0x00000fc6, 0x00000fc6 }, // tibetan symbol padma gdan
    { 0x00001037, 0x00001037 }, // myanmar sign dot below
    { 0x00001039, 0x0000103a }, // myanmar sign virama..myanmar sign asat
    { 0x00001087, 0x0000108c }, // myanmar sign shan tone-2..myanmar sign shan council tone-3
    { 0x0000108d, 0x0000108d }, // myanmar sign shan council emphatic tone
    { 0x0000108f, 0x0000108f }, // myanmar sign rumai palaung tone-5
    { 0x0000109a, 0x0000109b }, // myanmar sign khamti tone-1..myanmar sign khamti tone-3
    { 0x000017c9, 0x000017d3 }, // khmer sign muusikatoan..khmer sign bathamasat
    { 0x000017dd, 0x000017dd }, // khmer sign atthacan
    { 0x00001939, 0x0000193b }, // limbu sign mukphreng..limbu sign sa-i
    { 0x00001a75, 0x00001a7c }, // tai tham sign tone-1..tai tham sign khuen-lue karan
    { 0x00001a7f, 0x00001a7f }, // tai tham combining cryptogrammic dot
    { 0x00001ab0, 0x00001abd }, // combining doubled circumflex accent..combining parentheses below
    { 0x00001b34, 0x00001b34 }, // balinese sign rerekan
    { 0x00001b44, 0x00001b44 }, // balinese adeg adeg
    { 0x00001b6b, 0x00001b73 }, // balinese musical symbol combining tegeh..balinese musical symbol combining gong
    { 0x00001baa, 0x00001baa }, // sundanese sign pamaaeh
    { 0x00001bab, 0x00001bab }, // sundanese sign virama
    { 0x00001c36, 0x00001c37 }, // lepcha sign ran..lepcha sign nukta
    { 0x00001c78, 0x00001c7d }, // ol chiki mu ttuddag..ol chiki ahad
    { 0x00001cd0, 0x00001cd2 }, // vedic tone karshana..vedic tone prenkha
    { 0x00001cd3, 0x00001cd3 }, // vedic sign nihshvasa
    { 0x00001cd4, 0x00001ce0 }, // vedic sign yajurvedic midline svarita..vedic tone rigvedic kashmiri independent svarita
    { 0x00001ce1, 0x00001ce1 }, // vedic tone atharvavedic independent svarita
    { 0x00001ce2, 0x00001ce8 }, // vedic sign visarga svarita..vedic sign visarga anudatta with tail
    { 0x00001ced, 0x00001ced }, // vedic sign tiryak
    { 0x00001cf4, 0x00001cf4 }, // vedic tone candra above
    { 0x00001cf7, 0x00001cf7 }, // vedic sign atikrama
    { 0x00001cf8, 0x00001cf9 }, // vedic tone ring above..vedic tone double ring above
    { 0x00001d2c, 0x00001d6a }, // modifier letter capital a..greek subscript small letter chi
    { 0x00001dc4, 0x00001dcf }, // combining macron-acute..combining zigzag below
    { 0x00001df5, 0x00001df9 }, // combining up tack above..combining wide inverted bridge below
    { 0x00001dfd, 0x00001dff }, // combining almost equal to below..combining right arrowhead and down arrowhead below
    { 0x00001fbd, 0x00001fbd }, // greek koronis
    { 0x00001fbf, 0x00001fc1 }, // greek psili..greek dialytika and perispomeni
    { 0x00001fcd, 0x00001fcf }, // greek psili and varia..greek psili and perispomeni
    { 0x00001fdd, 0x00001fdf }, // greek dasia and varia..greek dasia and perispomeni
    { 0x00001fed, 0x00001fef }, // greek dialytika and varia..greek varia
    { 0x00001ffd, 0x00001ffe }, // greek oxia..greek dasia
    { 0x00002cef, 0x00002cf1 }, // coptic combining ni above..coptic combining spiritus lenis
    { 0x00002e2f, 0x00002e2f }, // vertical tilde
    { 0x0000302a, 0x0000302d }, // ideographic level tone mark..ideographic entering tone mark
    { 0x0000302e, 0x0000302f }, // hangul single dot tone mark..hangul double dot tone mark
    { 0x00003099, 0x0000309a }, // combining katakana-hiragana voiced sound mark..combining katakana-hiragana semi-voiced sound mark
    { 0x0000309b, 0x0000309c }, // katakana-hiragana voiced sound mark..katakana-hiragana semi-voiced sound mark
    { 0x000030fc, 0x000030fc }, // katakana-hiragana prolonged sound mark
    { 0x0000a66f, 0x0000a66f }, // combining cyrillic vzmet
    { 0x0000a67c, 0x0000a67d }, // combining cyrillic kavyka..combining cyrillic payerok
    { 0x0000a67f, 0x0000a67f }, // cyrillic payerok
    { 0x0000a69c, 0x0000a69d }, // modifier letter cyrillic hard sign..modifier letter cyrillic soft sign
    { 0x0000a6f0, 0x0000a6f1 }, // bamum combining mark koqndon..bamum combining mark tukwentis
    { 0x0000a717, 0x0000a71f }, // modifier letter dot vertical bar..modifier letter low inverted exclamation mark
    { 0x0000a720, 0x0000a721 }, // modifier letter stress and high tone..modifier letter stress and low tone
    { 0x0000a788, 0x0000a788 }, // modifier letter low circumflex accent
    { 0x0000a7f8, 0x0000a7f9 }, // modifier letter capital h with stroke..modifier letter small ligature oe
    { 0x0000a8c4, 0x0000a8c4 }, // saurashtra sign virama
    { 0x0000a8e0, 0x0000a8f1 }, // combining devanagari digit zero..combining devanagari sign avagraha
    { 0x0000a92b, 0x0000a92d }, // kayah li tone plophu..kayah li tone calya plophu
    { 0x0000a92e, 0x0000a92e }, // kayah li sign cwi
    { 0x0000a953, 0x0000a953 }, // rejang virama
    { 0x0000a9b3, 0x0000a9b3 }, // javanese sign cecak telu
    { 0x0000a9c0, 0x0000a9c0 }, // javanese pangkon
    { 0x0000a9e5, 0x0000a9e5 }, // myanmar sign shan saw
    { 0x0000aa7b, 0x0000aa7b }, // myanmar sign pao karen tone
    { 0x0000aa7c, 0x0000aa7c }, // myanmar sign tai laing tone-2
    { 0x0000aa7d, 0x0000aa7d }, // myanmar sign tai laing tone-5
    { 0x0000aabf, 0x0000aabf }, // tai viet tone mai ek
    { 0x0000aac0, 0x0000aac0 }, // tai viet tone mai nueng
    { 0x0000aac1, 0x0000aac1 }, // tai viet tone mai tho
    { 0x0000aac2, 0x0000aac2 }, // tai viet tone mai song
    { 0x0000aaf6, 0x0000aaf6 }, // meetei mayek virama
    { 0x0000ab5b, 0x0000ab5b }, // modifier breve with inverted breve
    { 0x0000ab5c, 0x0000ab5f }, // modifier letter small heng..modifier letter small u with left hook
    { 0x0000abec, 0x0000abec }, // meetei mayek lum iyek
    { 0x0000abed, 0x0000abed }, // meetei mayek apun iyek
    { 0x0000fb1e, 0x0000fb1e }, // hebrew point judeo-spanish varika
    { 0x0000fe20, 0x0000fe2f }, // combining ligature left half..combining cyrillic titlo right half
    { 0x0000ff3e, 0x0000ff3e }, // fullwidth circumflex accent
    { 0x0000ff40, 0x0000ff40 }, // fullwidth grave accent
    { 0x0000ff70, 0x0000ff70 }, // halfwidth katakana-hiragana prolonged sound mark
    { 0x0000ff9e, 0x0000ff9f }, // halfwidth katakana voiced sound mark..halfwidth katakana semi-voiced sound mark
    { 0x0000ffe3, 0x0000ffe3 }, // fullwidth macron
    { 0x000102e0, 0x000102e0 }, // coptic epact thousands mark
    { 0x00010ae5, 0x00010ae6 }, // manichaean abbreviation mark above..manichaean abbreviation mark below
    { 0x00010d22, 0x00010d23 }, // hanifi rohingya mark sakin..hanifi rohingya mark na khonna
    { 0x00010d24, 0x00010d27 }, // hanifi rohingya sign harbahay..hanifi rohingya sign tassi
    { 0x00010f46, 0x00010f50 }, // sogdian combining dot below..sogdian combining stroke below
    { 0x000110b9, 0x000110ba }, // kaithi sign virama..kaithi sign nukta
    { 0x00011133, 0x00011134 }, // chakma virama..chakma maayyaa
    { 0x00011173, 0x00011173 }, // mahajani sign nukta
    { 0x000111c0, 0x000111c0 }, // sharada sign virama
    { 0x000111ca, 0x000111cc }, // sharada sign nukta..sharada extra short vowel mark
    { 0x00011235, 0x00011235 }, // khojki sign virama
    { 0x00011236, 0x00011236 }, // khojki sign nukta
    { 0x000112e9, 0x000112ea }, // khudawadi sign nukta..khudawadi sign virama
    { 0x0001133c, 0x0001133c }, // grantha sign nukta
    { 0x0001134d, 0x0001134d }, // grantha sign virama
    { 0x00011366, 0x0001136c }, // combining grantha digit zero..combining grantha digit six
    { 0x00011370, 0x00011374 }, // combining grantha letter a..combining grantha letter pa
    { 0x00011442, 0x00011442 }, // newa sign virama
    { 0x00011446, 0x00011446 }, // newa sign nukta
    { 0x000114c2, 0x000114c3 }, // tirhuta sign virama..tirhuta sign nukta
    { 0x000115bf, 0x000115c0 }, // siddham sign virama..siddham sign nukta
    { 0x0001163f, 0x0001163f }, // modi sign virama
    { 0x000116b6, 0x000116b6 }, // takri sign virama
    { 0x000116b7, 0x000116b7 }, // takri sign nukta
    { 0x0001172b, 0x0001172b }, // ahom sign killer
    { 0x00011839, 0x0001183a }, // dogra sign virama..dogra sign nukta
    { 0x00011a34, 0x00011a34 }, // zanabazar square sign virama
    { 0x00011a47, 0x00011a47 }, // zanabazar square subjoiner
    { 0x00011a99, 0x00011a99 }, // soyombo subjoiner
    { 0x00011c3f, 0x00011c3f }, // bhaiksuki sign virama
    { 0x00011d42, 0x00011d42 }, // masaram gondi sign nukta
    { 0x00011d44, 0x00011d45 }, // masaram gondi sign halanta..masaram gondi virama
    { 0x00011d97, 0x00011d97 }, // gunjala gondi virama
    { 0x00016af0, 0x00016af4 }, // bassa vah combining high tone..bassa vah combining high-low tone
    { 0x00016f8f, 0x00016f92 }, // miao tone right..miao tone below
    { 0x00016f93, 0x00016f9f }, // miao letter tone-2..miao letter reformed tone-8
    { 0x0001d167, 0x0001d169 }, // musical symbol combining tremolo-1..musical symbol combining tremolo-3
    { 0x0001d16d, 0x0001d172 }, // musical symbol combining augmentation dot..musical symbol combining flag-5
    { 0x0001d17b, 0x0001d182 }, // musical symbol combining accent..musical symbol combining loure
    { 0x0001d185, 0x0001d18b }, // musical symbol combining doit..musical symbol combining triple tongue
    { 0x0001d1aa, 0x0001d1ad }, // musical symbol combining down bow..musical symbol combining snap pizzicato
    { 0x0001e8d0, 0x0001e8d6 }, // mende kikakui combining number teens..mende kikakui combining number millions
    { 0x0001e944, 0x0001e946 }, // adlam alif lengthener..adlam gemination mark
    { 0x0001e948, 0x0001e94a }  // adlam consonant modifier..adlam nukta
};

} // namespace system
} // namespace libbitcoin
