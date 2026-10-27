/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#ifndef LIBBITCOIN_SYSTEM_TEST_SLIP39_HPP
#define LIBBITCOIN_SYSTEM_TEST_SLIP39_HPP

#include <bitcoin/system.hpp>

 // Avoid using namespace in shared headers, but okay here.
using namespace bc::system;
using namespace bc::system::wallet;

namespace test {
namespace mnemonics_slip39 {

// All published vectors use this passphrase.
constexpr auto passphrase = "TREZOR";

struct slip39_vector
{
    std::string description;
    string_list mnemonics;
    std::string secret_;
    std::string hd_key_;

    // An invalid share set has no master secret.
    bool valid() const
    {
        return !secret_.empty();
    }

    data_chunk secret() const
    {
        data_chunk out;
        decode_base16(out, secret_);
        return out;
    }

    slip39::list shares() const
    {
        slip39::list out;

        for (const auto& mnemonic: mnemonics)
            out.push_back(slip39{ mnemonic });

        return out;
    }

    hd_private hd_key() const
    {
        return { hd_key_ };
    }
};

typedef std_vector<slip39_vector> slip39_vectors;

// github.com/trezor/python-shamir-mnemonic/blob/master/vectors.json
const slip39_vectors vectors
{
    slip39_vector
    {
        "1. Valid mnemonic without sharing (128 bits)",
        {
            "duckling enlarge academic academic agency result length solution fridge kidney coal piece deal husband erode duke ajar critical decision keyboard"
        },
        "bb54aac4b89dc868ba37d9cc21b2cece",
        "xprv9s21ZrQH143K4QViKpwKCpS2zVbz8GrZgpEchMDg6KME9HZtjfL7iThE9w5muQA4YPHKN1u5VM1w8D4pvnjxa2BmpGMfXr7hnRrRHZ93awZ"
    },
    slip39_vector
    {
        "2. Mnemonic with invalid checksum (128 bits)",
        {
            "duckling enlarge academic academic agency result length solution fridge kidney coal piece deal husband erode duke ajar critical decision kidney"
        },
        "",
        ""
    },
    slip39_vector
    {
        "3. Mnemonic with invalid padding (128 bits)",
        {
            "duckling enlarge academic academic email result length solution fridge kidney coal piece deal husband erode duke ajar music cargo fitness"
        },
        "",
        ""
    },
    slip39_vector
    {
        "4. Basic sharing 2-of-3 (128 bits)",
        {
            "shadow pistol academic always adequate wildlife fancy gross oasis cylinder mustang wrist rescue view short owner flip making coding armed",
            "shadow pistol academic acid actress prayer class unknown daughter sweater depict flip twice unkind craft early superior advocate guest smoking"
        },
        "b43ceb7e57a0ea8766221624d01b0864",
        "xprv9s21ZrQH143K2nNuAbfWPHBtfiSCS14XQgb3otW4pX655q58EEZeC8zmjEUwucBu9dPnxdpbZLCn57yx45RBkwJHnwHFjZK4XPJ8SyeYjYg"
    },
    slip39_vector
    {
        "5. Basic sharing 2-of-3 (128 bits)",
        {
            "shadow pistol academic always adequate wildlife fancy gross oasis cylinder mustang wrist rescue view short owner flip making coding armed"
        },
        "",
        ""
    },
    slip39_vector
    {
        "6. Mnemonics with different identifiers (128 bits)",
        {
            "adequate smoking academic acid debut wine petition glen cluster slow rhyme slow simple epidemic rumor junk tracks treat olympic tolerate",
            "adequate stay academic agency agency formal party ting frequent learn upstairs remember smear leaf damage anatomy ladle market hush corner"
        },
        "",
        ""
    },
    slip39_vector
    {
        "7. Mnemonics with different iteration exponents (128 bits)",
        {
            "peasant leaves academic acid desert exact olympic math alive axle trial tackle drug deny decent smear dominant desert bucket remind",
            "peasant leader academic agency cultural blessing percent network envelope medal junk primary human pumps jacket fragment payroll ticket evoke voice"
        },
        "",
        ""
    },
    slip39_vector
    {
        "8. Mnemonics with mismatching group thresholds (128 bits)",
        {
            "liberty category beard echo animal fawn temple briefing math username various wolf aviation fancy visual holy thunder yelp helpful payment",
            "liberty category beard email beyond should fancy romp founder easel pink holy hairy romp loyalty material victim owner toxic custody",
            "liberty category academic easy being hazard crush diminish oral lizard reaction cluster force dilemma deploy force club veteran expect photo"
        },
        "",
        ""
    },
    slip39_vector
    {
        "9. Mnemonics with mismatching group counts (128 bits)",
        {
            "average senior academic leaf broken teacher expect surface hour capture obesity desire negative dynamic dominant pistol mineral mailman iris aide",
            "average senior academic agency curious pants blimp spew clothes slice script dress wrap firm shaft regular slavery negative theater roster"
        },
        "",
        ""
    },
    slip39_vector
    {
        "10. Mnemonics with greater group threshold than group counts (128 bits)",
        {
            "music husband acrobat acid artist finance center either graduate swimming object bike medical clothes station aspect spider maiden bulb welcome",
            "music husband acrobat agency advance hunting bike corner density careful material civil evil tactics remind hawk discuss hobo voice rainbow",
            "music husband beard academic black tricycle clock mayor estimate level photo episode exclude ecology papa source amazing salt verify divorce"
        },
        "",
        ""
    },
    slip39_vector
    {
        "11. Mnemonics with duplicate member indices (128 bits)",
        {
            "device stay academic always dive coal antenna adult black exceed stadium herald advance soldier busy dryer daughter evaluate minister laser",
            "device stay academic always dwarf afraid robin gravity crunch adjust soul branch walnut coastal dream costume scholar mortgage mountain pumps"
        },
        "",
        ""
    },
    slip39_vector
    {
        "12. Mnemonics with mismatching member thresholds (128 bits)",
        {
            "hour painting academic academic device formal evoke guitar random modern justice filter withdraw trouble identify mailman insect general cover oven",
            "hour painting academic agency artist again daisy capital beaver fiber much enjoy suitable symbolic identify photo editor romp float echo"
        },
        "",
        ""
    },
    slip39_vector
    {
        "13. Mnemonics giving an invalid digest (128 bits)",
        {
            "guilt walnut academic acid deliver remove equip listen vampire tactics nylon rhythm failure husband fatigue alive blind enemy teaspoon rebound",
            "guilt walnut academic agency brave hamster hobo declare herd taste alpha slim criminal mild arcade formal romp branch pink ambition"
        },
        "",
        ""
    },
    slip39_vector
    {
        "14. Insufficient number of groups (128 bits, case 1)",
        {
            "eraser senior beard romp adorn nuclear spill corner cradle style ancient family general leader ambition exchange unusual garlic promise voice"
        },
        "",
        ""
    },
    slip39_vector
    {
        "15. Insufficient number of groups (128 bits, case 2)",
        {
            "eraser senior decision scared cargo theory device idea deliver modify curly include pancake both news skin realize vitamins away join",
            "eraser senior decision roster beard treat identify grumpy salt index fake aviation theater cubic bike cause research dragon emphasis counter"
        },
        "",
        ""
    },
    slip39_vector
    {
        "16. Threshold number of groups, but insufficient number of members in one group (128 bits)",
        {
            "eraser senior decision shadow artist work morning estate greatest pipeline plan ting petition forget hormone flexible general goat admit surface",
            "eraser senior beard romp adorn nuclear spill corner cradle style ancient family general leader ambition exchange unusual garlic promise voice"
        },
        "",
        ""
    },
    slip39_vector
    {
        "17. Threshold number of groups and members in each group (128 bits, case 1)",
        {
            "eraser senior decision roster beard treat identify grumpy salt index fake aviation theater cubic bike cause research dragon emphasis counter",
            "eraser senior ceramic snake clay various huge numb argue hesitate auction category timber browser greatest hanger petition script leaf pickup",
            "eraser senior ceramic shaft dynamic become junior wrist silver peasant force math alto coal amazing segment yelp velvet image paces",
            "eraser senior ceramic round column hawk trust auction smug shame alive greatest sheriff living perfect corner chest sled fumes adequate",
            "eraser senior decision smug corner ruin rescue cubic angel tackle skin skunk program roster trash rumor slush angel flea amazing"
        },
        "7c3397a292a5941682d7a4ae2d898d11",
        "xprv9s21ZrQH143K3dzDLfeY3cMp23u5vDeFYftu5RPYZPucKc99mNEddU4w99GxdgUGcSfMpVDxhnR1XpJzZNXRN1m6xNgnzFS5MwMP6QyBRKV"
    },
    slip39_vector
    {
        "18. Threshold number of groups and members in each group (128 bits, case 2)",
        {
            "eraser senior decision smug corner ruin rescue cubic angel tackle skin skunk program roster trash rumor slush angel flea amazing",
            "eraser senior beard romp adorn nuclear spill corner cradle style ancient family general leader ambition exchange unusual garlic promise voice",
            "eraser senior decision scared cargo theory device idea deliver modify curly include pancake both news skin realize vitamins away join"
        },
        "7c3397a292a5941682d7a4ae2d898d11",
        "xprv9s21ZrQH143K3dzDLfeY3cMp23u5vDeFYftu5RPYZPucKc99mNEddU4w99GxdgUGcSfMpVDxhnR1XpJzZNXRN1m6xNgnzFS5MwMP6QyBRKV"
    },
    slip39_vector
    {
        "19. Threshold number of groups and members in each group (128 bits, case 3)",
        {
            "eraser senior beard romp adorn nuclear spill corner cradle style ancient family general leader ambition exchange unusual garlic promise voice",
            "eraser senior acrobat romp bishop medical gesture pumps secret alive ultimate quarter priest subject class dictate spew material endless market"
        },
        "7c3397a292a5941682d7a4ae2d898d11",
        "xprv9s21ZrQH143K3dzDLfeY3cMp23u5vDeFYftu5RPYZPucKc99mNEddU4w99GxdgUGcSfMpVDxhnR1XpJzZNXRN1m6xNgnzFS5MwMP6QyBRKV"
    },
    slip39_vector
    {
        "20. Valid mnemonic without sharing (256 bits)",
        {
            "theory painting academic academic armed sweater year military elder discuss acne wildlife boring employer fused large satoshi bundle carbon diagnose anatomy hamster leaves tracks paces beyond phantom capital marvel lips brave detect luck"
        },
        "989baf9dcaad5b10ca33dfd8cc75e42477025dce88ae83e75a230086a0e00e92",
        "xprv9s21ZrQH143K41mrxxMT2FpiheQ9MFNmWVK4tvX2s28KLZAhuXWskJCKVRQprq9TnjzzzEYePpt764csiCxTt22xwGPiRmUjYUUdjaut8RM"
    },
    slip39_vector
    {
        "21. Mnemonic with invalid checksum (256 bits)",
        {
            "theory painting academic academic armed sweater year military elder discuss acne wildlife boring employer fused large satoshi bundle carbon diagnose anatomy hamster leaves tracks paces beyond phantom capital marvel lips brave detect lunar"
        },
        "",
        ""
    },
    slip39_vector
    {
        "22. Mnemonic with invalid padding (256 bits)",
        {
            "theory painting academic academic campus sweater year military elder discuss acne wildlife boring employer fused large satoshi bundle carbon diagnose anatomy hamster leaves tracks paces beyond phantom capital marvel lips facility obtain sister"
        },
        "",
        ""
    },
    slip39_vector
    {
        "23. Basic sharing 2-of-3 (256 bits)",
        {
            "humidity disease academic always aluminum jewelry energy woman receiver strategy amuse duckling lying evidence network walnut tactics forget hairy rebound impulse brother survive clothes stadium mailman rival ocean reward venture always armed unwrap",
            "humidity disease academic agency actress jacket gross physics cylinder solution fake mortgage benefit public busy prepare sharp friar change work slow purchase ruler again tricycle involve viral wireless mixture anatomy desert cargo upgrade"
        },
        "c938b319067687e990e05e0da0ecce1278f75ff58d9853f19dcaeed5de104aae",
        "xprv9s21ZrQH143K3a4GRMgK8WnawupkwkP6gyHxRsXnMsYPTPH21fWwNcAytijtfyftqNfiaY8LgQVdBQvHZ9FBvtwdjC7LCYxjYruJFuLzyMQ"
    },
    slip39_vector
    {
        "24. Basic sharing 2-of-3 (256 bits)",
        {
            "humidity disease academic always aluminum jewelry energy woman receiver strategy amuse duckling lying evidence network walnut tactics forget hairy rebound impulse brother survive clothes stadium mailman rival ocean reward venture always armed unwrap"
        },
        "",
        ""
    },
    slip39_vector
    {
        "25. Mnemonics with different identifiers (256 bits)",
        {
            "smear husband academic acid deadline scene venture distance dive overall parking bracelet elevator justice echo burning oven chest duke nylon",
            "smear isolate academic agency alpha mandate decorate burden recover guard exercise fatal force syndrome fumes thank guest drift dramatic mule"
        },
        "",
        ""
    },
    slip39_vector
    {
        "26. Mnemonics with different iteration exponents (256 bits)",
        {
            "finger trash academic acid average priority dish revenue academic hospital spirit western ocean fact calcium syndrome greatest plan losing dictate",
            "finger traffic academic agency building lilac deny paces subject threaten diploma eclipse window unknown health slim piece dragon focus smirk"
        },
        "",
        ""
    },
    slip39_vector
    {
        "27. Mnemonics with mismatching group thresholds (256 bits)",
        {
            "flavor pink beard echo depart forbid retreat become frost helpful juice unwrap reunion credit math burning spine black capital lair",
            "flavor pink beard email diet teaspoon freshman identify document rebound cricket prune headset loyalty smell emission skin often square rebound",
            "flavor pink academic easy credit cage raisin crazy closet lobe mobile become drink human tactics valuable hand capture sympathy finger"
        },
        "",
        ""
    },
    slip39_vector
    {
        "28. Mnemonics with mismatching group counts (256 bits)",
        {
            "column flea academic leaf debut extra surface slow timber husky lawsuit game behavior husky swimming already paper episode tricycle scroll",
            "column flea academic agency blessing garbage party software stadium verify silent umbrella therapy decorate chemical erode dramatic eclipse replace apart"
        },
        "",
        ""
    },
    slip39_vector
    {
        "29. Mnemonics with greater group threshold than group counts (256 bits)",
        {
            "smirk pink acrobat acid auction wireless impulse spine sprinkle fortune clogs elbow guest hush loyalty crush dictate tracks airport talent",
            "smirk pink acrobat agency dwarf emperor ajar organize legs slice harvest plastic dynamic style mobile float bulb health coding credit",
            "smirk pink beard academic alto strategy carve shame language rapids ruin smart location spray training acquire eraser endorse submit peaceful"
        },
        "",
        ""
    },
    slip39_vector
    {
        "30. Mnemonics with duplicate member indices (256 bits)",
        {
            "fishing recover academic always device craft trend snapshot gums skin downtown watch device sniff hour clock public maximum garlic born",
            "fishing recover academic always aircraft view software cradle fangs amazing package plastic evaluate intend penalty epidemic anatomy quarter cage apart"
        },
        "",
        ""
    },
    slip39_vector
    {
        "31. Mnemonics with mismatching member thresholds (256 bits)",
        {
            "evoke garden academic academic answer wolf scandal modern warmth station devote emerald market physics surface formal amazing aquatic gesture medical",
            "evoke garden academic agency deal revenue knit reunion decrease magazine flexible company goat repair alarm military facility clogs aide mandate"
        },
        "",
        ""
    },
    slip39_vector
    {
        "32. Mnemonics giving an invalid digest (256 bits)",
        {
            "river deal academic acid average forbid pistol peanut custody bike class aunt hairy merit valid flexible learn ajar very easel",
            "river deal academic agency camera amuse lungs numb isolate display smear piece traffic worthy year patrol crush fact fancy emission"
        },
        "",
        ""
    },
    slip39_vector
    {
        "33. Insufficient number of groups (256 bits, case 1)",
        {
            "wildlife deal beard romp alcohol space mild usual clothes union nuclear testify course research heat listen task location thank hospital slice smell failure fawn helpful priest ambition average recover lecture process dough stadium"
        },
        "",
        ""
    },
    slip39_vector
    {
        "34. Insufficient number of groups (256 bits, case 2)",
        {
            "wildlife deal decision scared acne fatal snake paces obtain election dryer dominant romp tactics railroad marvel trust helpful flip peanut theory theater photo luck install entrance taxi step oven network dictate intimate listen",
            "wildlife deal decision smug ancestor genuine move huge cubic strategy smell game costume extend swimming false desire fake traffic vegan senior twice timber submit leader payroll fraction apart exact forward pulse tidy install"
        },
        "",
        ""
    },
    slip39_vector
    {
        "35. Threshold number of groups, but insufficient number of members in one group (256 bits)",
        {
            "wildlife deal decision shadow analysis adjust bulb skunk muscle mandate obesity total guitar coal gravity carve slim jacket ruin rebuild ancestor numerous hour mortgage require herd maiden public ceiling pecan pickup shadow club",
            "wildlife deal beard romp alcohol space mild usual clothes union nuclear testify course research heat listen task location thank hospital slice smell failure fawn helpful priest ambition average recover lecture process dough stadium"
        },
        "",
        ""
    },
    slip39_vector
    {
        "36. Threshold number of groups and members in each group (256 bits, case 1)",
        {
            "wildlife deal ceramic round aluminum pitch goat racism employer miracle percent math decision episode dramatic editor lily prospect program scene rebuild display sympathy have single mustang junction relate often chemical society wits estate",
            "wildlife deal decision scared acne fatal snake paces obtain election dryer dominant romp tactics railroad marvel trust helpful flip peanut theory theater photo luck install entrance taxi step oven network dictate intimate listen",
            "wildlife deal ceramic scatter argue equip vampire together ruin reject literary rival distance aquatic agency teammate rebound false argue miracle stay again blessing peaceful unknown cover beard acid island language debris industry idle",
            "wildlife deal ceramic snake agree voter main lecture axis kitchen physics arcade velvet spine idea scroll promise platform firm sharp patrol divorce ancestor fantasy forbid goat ajar believe swimming cowboy symbolic plastic spelling",
            "wildlife deal decision shadow analysis adjust bulb skunk muscle mandate obesity total guitar coal gravity carve slim jacket ruin rebuild ancestor numerous hour mortgage require herd maiden public ceiling pecan pickup shadow club"
        },
        "5385577c8cfc6c1a8aa0f7f10ecde0a3318493262591e78b8c14c6686167123b",
        "xprv9s21ZrQH143K2UspC9FRPfQC9NcDB4HPkx1XG9UEtuceYtpcCZ6ypNZWdgfxQ9dAFVeD1F4Zg4roY7nZm2LB7THPD6kaCege3M7EuS8v85c"
    },
    slip39_vector
    {
        "37. Threshold number of groups and members in each group (256 bits, case 2)",
        {
            "wildlife deal decision scared acne fatal snake paces obtain election dryer dominant romp tactics railroad marvel trust helpful flip peanut theory theater photo luck install entrance taxi step oven network dictate intimate listen",
            "wildlife deal beard romp alcohol space mild usual clothes union nuclear testify course research heat listen task location thank hospital slice smell failure fawn helpful priest ambition average recover lecture process dough stadium",
            "wildlife deal decision smug ancestor genuine move huge cubic strategy smell game costume extend swimming false desire fake traffic vegan senior twice timber submit leader payroll fraction apart exact forward pulse tidy install"
        },
        "5385577c8cfc6c1a8aa0f7f10ecde0a3318493262591e78b8c14c6686167123b",
        "xprv9s21ZrQH143K2UspC9FRPfQC9NcDB4HPkx1XG9UEtuceYtpcCZ6ypNZWdgfxQ9dAFVeD1F4Zg4roY7nZm2LB7THPD6kaCege3M7EuS8v85c"
    },
    slip39_vector
    {
        "38. Threshold number of groups and members in each group (256 bits, case 3)",
        {
            "wildlife deal beard romp alcohol space mild usual clothes union nuclear testify course research heat listen task location thank hospital slice smell failure fawn helpful priest ambition average recover lecture process dough stadium",
            "wildlife deal acrobat romp anxiety axis starting require metric flexible geology game drove editor edge screw helpful have huge holy making pitch unknown carve holiday numb glasses survive already tenant adapt goat fangs"
        },
        "5385577c8cfc6c1a8aa0f7f10ecde0a3318493262591e78b8c14c6686167123b",
        "xprv9s21ZrQH143K2UspC9FRPfQC9NcDB4HPkx1XG9UEtuceYtpcCZ6ypNZWdgfxQ9dAFVeD1F4Zg4roY7nZm2LB7THPD6kaCege3M7EuS8v85c"
    },
    slip39_vector
    {
        "39. Mnemonic with insufficient length",
        {
            "junk necklace academic academic acne isolate join hesitate lunar roster dough calcium chemical ladybug amount mobile glasses verify cylinder"
        },
        "",
        ""
    },
    slip39_vector
    {
        "40. Mnemonic with invalid master secret length",
        {
            "fraction necklace academic academic award teammate mouse regular testify coding building member verdict purchase blind camera duration email prepare spirit quarter"
        },
        "",
        ""
    },
    slip39_vector
    {
        "41. Valid mnemonics which can detect some errors in modular arithmetic",
        {
            "herald flea academic cage avoid space trend estate dryer hairy evoke eyebrow improve airline artwork garlic premium duration prevent oven",
            "herald flea academic client blue skunk class goat luxury deny presence impulse graduate clay join blanket bulge survive dish necklace",
            "herald flea academic acne advance fused brother frozen broken game ranked ajar already believe check install theory angry exercise adult"
        },
        "ad6f2ad8b59bbbaa01369b9006208d9a",
        "xprv9s21ZrQH143K2R4HJxcG1eUsudvHM753BZ9vaGkpYCoeEhCQx147C5qEcupPHxcXYfdYMwJmsKXrHDhtEwutxTTvFzdDCZVQwHneeQH8ioH"
    },
    slip39_vector
    {
        "42. Valid extendable mnemonic without sharing (128 bits)",
        {
            "testify swimming academic academic column loyalty smear include exotic bedroom exotic wrist lobe cover grief golden smart junior estimate learn"
        },
        "1679b4516e0ee5954351d288a838f45e",
        "xprv9s21ZrQH143K2w6eTpQnB73CU8Qrhg6gN3D66Jr16n5uorwoV7CwxQ5DofRPyok5DyRg4Q3BfHfCgJFk3boNRPPt1vEW1ENj2QckzVLQFXu"
    },
    slip39_vector
    {
        "43. Extendable basic sharing 2-of-3 (128 bits)",
        {
            "enemy favorite academic acid cowboy phrase havoc level response walnut budget painting inside trash adjust froth kitchen learn tidy punish",
            "enemy favorite academic always academic sniff script carpet romp kind promise scatter center unfair training emphasis evening belong fake enforce"
        },
        "48b1a4b80b8c209ad42c33672bdaa428",
        "xprv9s21ZrQH143K4FS1qQdXYAFVAHiSAnjj21YAKGh2CqUPJ2yQhMmYGT4e5a2tyGLiVsRgTEvajXkxhg92zJ8zmWZas9LguQWz7WZShfJg6RS"
    },
    slip39_vector
    {
        "44. Valid extendable mnemonic without sharing (256 bits)",
        {
            "impulse calcium academic academic alcohol sugar lyrics pajamas column facility finance tension extend space birthday rainbow swimming purple syndrome facility trial warn duration snapshot shadow hormone rhyme public spine counter easy hawk album"
        },
        "8340611602fe91af634a5f4608377b5235fa2d757c51d720c0c7656249a3035f",
        "xprv9s21ZrQH143K2yJ7S8bXMiGqp1fySH8RLeFQKQmqfmmLTRwWmAYkpUcWz6M42oGoFMJRENmvsGQmunWTdizsi8v8fku8gpbVvYSiCYJTF1Y"
    },
    slip39_vector
    {
        "45. Extendable basic sharing 2-of-3 (256 bits)",
        {
            "western apart academic always artist resident briefing sugar woman oven coding club ajar merit pecan answer prisoner artist fraction amount desktop mild false necklace muscle photo wealthy alpha category unwrap spew losing making",
            "western apart academic acid answer ancient auction flip image penalty oasis beaver multiple thunder problem switch alive heat inherit superior teaspoon explain blanket pencil numb lend punish endless aunt garlic humidity kidney observe"
        },
        "8dc652d6d6cd370d8c963141f6d79ba440300f25c467302c1d966bff8f62300d",
        "xprv9s21ZrQH143K2eFW2zmu3aayWWd6MJZBG7RebW35fiKcoCZ6jFi6U5gzffB9McDdiKTecUtRqJH9GzueCXiQK1LaQXdgthS8DgWfC8Uu3z7"
    }
};

} // namespace mnemonics_slip39
} // namespace test

#endif
