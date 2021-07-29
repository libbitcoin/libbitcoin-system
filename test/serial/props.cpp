/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#include "../test.hpp"

BOOST_AUTO_TEST_SUITE(props_tests)

// null

BOOST_AUTO_TEST_CASE(props__write__null_json__valid_expected)
{
    std::ostringstream stream;
    props instance;
    instance.write(stream, props::format::json);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE_EQUAL(stream.str(), "null");
}

BOOST_AUTO_TEST_CASE(props__write__null_info__valid_expected)
{
    std::ostringstream stream;
    props instance;
    instance.write(stream, props::format::info);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE_EQUAL(stream.str(), "null");
}

BOOST_AUTO_TEST_CASE(props__write__null_xml__valid_expected)
{
    std::ostringstream stream;
    props instance;
    instance.write(stream, props::format::xml);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE(stream.str().empty());
}

// true

BOOST_AUTO_TEST_CASE(props__write__true_json__valid_expected)
{
    std::ostringstream stream;
    props instance(props::truth::true_);
    instance.write(stream, props::format::json);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE_EQUAL(stream.str(), "true");
}

BOOST_AUTO_TEST_CASE(props__write__true_info__valid_expected)
{
    std::ostringstream stream;
    props instance(props::truth::true_);
    instance.write(stream, props::format::info);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE_EQUAL(stream.str(), "true");
}

BOOST_AUTO_TEST_CASE(props__write__true_xml__valid_expected)
{
    std::ostringstream stream;
    props instance(props::truth::true_);
    instance.write(stream, props::format::xml);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE_EQUAL(stream.str(), "true");
}

// false

BOOST_AUTO_TEST_CASE(props__write__false_json__valid_expected)
{
    std::ostringstream stream;
    props instance(props::truth::false_);
    instance.write(stream, props::format::json);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE_EQUAL(stream.str(), "false");
}

BOOST_AUTO_TEST_CASE(props__write__false_info__valid_expected)
{
    std::ostringstream stream;
    props instance(props::truth::false_);
    instance.write(stream, props::format::info);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE_EQUAL(stream.str(), "false");
}

BOOST_AUTO_TEST_CASE(props__write__false_xml__valid_expected)
{
    std::ostringstream stream;
    props instance(props::truth::false_);
    instance.write(stream, props::format::xml);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE_EQUAL(stream.str(), "false");
}

// string

BOOST_AUTO_TEST_CASE(props__write__string_json__valid_expected)
{
    std::ostringstream stream;
    props instance("string");
    instance.write(stream, props::format::json);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE_EQUAL(stream.str(), "\"string\"");
}

BOOST_AUTO_TEST_CASE(props__write__unspaced_string_info__valid_expected)
{
    std::ostringstream stream;
    props instance("string");
    instance.write(stream, props::format::info);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE_EQUAL(stream.str(), "string");
}

BOOST_AUTO_TEST_CASE(props__write__spaced_string_info__valid_expected)
{
    std::ostringstream stream;
    props instance("spaced string");
    instance.write(stream, props::format::json);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE_EQUAL(stream.str(), "\"spaced string\"");
}

BOOST_AUTO_TEST_CASE(props__write__string_xml__valid_expected)
{
    std::ostringstream stream;
    props instance("string");
    instance.write(stream, props::format::xml);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE_EQUAL(stream.str(), "string");
}

// number

BOOST_AUTO_TEST_CASE(props__write__positive_number_json__valid_expected)
{
    std::ostringstream stream;
    props instance(42);
    instance.write(stream, props::format::json);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE_EQUAL(stream.str(), "42");
}

BOOST_AUTO_TEST_CASE(props__write__negative_number_json__valid_expected)
{
    std::ostringstream stream;
    props instance(-42);
    instance.write(stream, props::format::json);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE_EQUAL(stream.str(), "-42");
}

BOOST_AUTO_TEST_CASE(props__write__minimum_number_json__invalid)
{
    std::ostringstream stream;
    props instance(min_int64);
    instance.write(stream, props::format::json);
    BOOST_REQUIRE(!stream);
}

BOOST_AUTO_TEST_CASE(props__write__maximum_number_json__invalid)
{
    std::ostringstream stream;
    props instance(max_int64);
    instance.write(stream, props::format::json);
    BOOST_REQUIRE(!stream);
}

BOOST_AUTO_TEST_CASE(props__write__positive_number_info__valid_expected)
{
    std::ostringstream stream;
    props instance(42);
    instance.write(stream, props::format::info);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE_EQUAL(stream.str(), "42");
}

BOOST_AUTO_TEST_CASE(props__write__positive_number_xml__valid_expected)
{
    std::ostringstream stream;
    props instance(42);
    instance.write(stream, props::format::xml);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE_EQUAL(stream.str(), "42");
}

// array

BOOST_AUTO_TEST_CASE(props__write__array_json_empty__valid_expected)
{
    std::ostringstream stream;
    array_props instance("child");
    instance.write(stream, props::format::json);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE_EQUAL(stream.str(), "[]");
}

BOOST_AUTO_TEST_CASE(props__write__array_json__valid_expected)
{
    std::ostringstream stream;
    array_props instance("child", { 0, 1 });
    instance.add({ 2 });
    instance.add({ 3 });
    instance.write(stream, props::format::json);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE_EQUAL(stream.str(), "[0,1,2,3]");
}

BOOST_AUTO_TEST_CASE(props__write__array_info__valid_expected)
{
    std::ostringstream stream;
    array_props instance("child", { 0, 1 });
    instance.add({ 2 });
    instance.add({ 3 });
    instance.write(stream, props::format::info);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE_EQUAL(stream.str(), "{0 1 2 3}");
}

BOOST_AUTO_TEST_CASE(props__write__array_xml__valid_expected)
{
    std::ostringstream stream;
    array_props instance("child", { 0, 1 });
    instance.add({ 2 });
    instance.add({ 3 });
    instance.write(stream, props::format::xml);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE_EQUAL(stream.str(), "<child>0</child><child>1</child><child>2</child><child>3</child>");
}

// object

BOOST_AUTO_TEST_CASE(props__write__object_json_empty__valid_expected)
{
    std::ostringstream stream;
    object_props instance{};
    instance.write(stream, props::format::json);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE_EQUAL(stream.str(), "{}");
}

BOOST_AUTO_TEST_CASE(props__write__object_json__valid_expected)
{
    std::ostringstream stream;
    object_props instance
    ({
        { "child0", { 0 } },
        { "child1", { 1 } } 
    });
    instance.add
    ({
        { "child2", { 2 } },
        { "child3", { 3 } }
    });
    instance.write(stream, props::format::json);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE_EQUAL(stream.str(), "{\"child0\":0,\"child1\":1,\"child2\":2,\"child3\":3}");
}

BOOST_AUTO_TEST_CASE(props__write__object_info__valid_expected)
{
    std::ostringstream stream;
    object_props instance
    ({
        { "child0", { 0 } },
        { "child1", { 1 } } 
    });
    instance.add
    ({
        { "child2", { 2 } },
        { "child3", { 3 } }
    });
    instance.write(stream, props::format::info);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE_EQUAL(stream.str(), "{child0 0,child1 1,child2 2,child3 3}");
}
 
BOOST_AUTO_TEST_CASE(props__write__object_xml__valid_expected)
{
    std::ostringstream stream;
    object_props instance
    ({
        { "child0", { 0 } },
        { "child1", { 1 } } 
    });
    instance.add
    ({
        { "child2", { 2 } },
        { "child3", { 3 } }
    });
    instance.write(stream, props::format::xml);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE_EQUAL(stream.str(), "<child0>0</child0><child1>1</child1><child2>2</child2><child3>3</child3>");
}

// complex object

static const object_props root
{
    {
        "month",
        object_props
        {
            { "year", 2021 },
            { "sundays", array_props{ "sunday", { 4, 11, 18, 25 } } },
            { "mondays", array_props{ "monday", { 5, 12, 19, 26 } } },
            { "tuesdays", array_props{ "tuesday", { 6, 13, 20, 27 } } },
            { "wednesdays", array_props{ "wednesday", { 7, 14, 21, 28 } } },
            { "thursdays", array_props{ "thursday", { 1, 8, 15, 22, 29 } } },
            { "fridays", array_props{ "friday", { 2, 9, 16, 23, 30 } } },
            { "saturdays", array_props{ "saturday", { 3, 10, 17, 24, 31 } } }
        }
    }
};

BOOST_AUTO_TEST_CASE(props__write__complex_object_json_flat__valid_expected)
{
    std::ostringstream stream;
    root.write(stream, props::format::json);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE_EQUAL(stream.str(), "{\"month\":{\"year\":2021,\"sundays\":[4,11,18,25],\"mondays\":[5,12,19,26],\"tuesdays\":[6,13,20,27],\"wednesdays\":[7,14,21,28],\"thursdays\":[1,8,15,22,29],\"fridays\":[2,9,16,23,30],\"saturdays\":[3,10,17,24,31]}}");
}

BOOST_AUTO_TEST_CASE(props__write__complex_object_json_unflat__valid_expected)
{
    std::ostringstream stream;
    root.write(stream, props::format::json, false);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE_EQUAL(stream.str(), 
        "{\n"
        "    \"month\":\n"
        "    {\n"
        "        \"year\":2021,\n"
        "        \"sundays\":\n"
        "        [\n"
        "            4,\n"
        "            11,\n"
        "            18,\n"
        "            25\n"
        "        ],\n"
        "        \"mondays\":\n"
        "        [\n"
        "            5,\n"
        "            12,\n"
        "            19,\n"
        "            26\n"
        "        ],\n"
        "        \"tuesdays\":\n"
        "        [\n"
        "            6,\n"
        "            13,\n"
        "            20,\n"
        "            27\n"
        "        ],\n"
        "        \"wednesdays\":\n"
        "        [\n"
        "            7,\n"
        "            14,\n"
        "            21,\n"
        "            28\n"
        "        ],\n"
        "        \"thursdays\":\n"
        "        [\n"
        "            1,\n"
        "            8,\n"
        "            15,\n"
        "            22,\n"
        "            29\n"
        "        ],\n"
        "        \"fridays\":\n"
        "        [\n"
        "            2,\n"
        "            9,\n"
        "            16,\n"
        "            23,\n"
        "            30\n"
        "        ],\n"
        "        \"saturdays\":\n"
        "        [\n"
        "            3,\n"
        "            10,\n"
        "            17,\n"
        "            24,\n"
        "            31\n"
        "        ]\n"
        "    }\n"
        "}"
    );
}

BOOST_AUTO_TEST_CASE(props__write__complex_object_info_flat__valid_expected)
{
    std::ostringstream stream;
    root.write(stream, props::format::info);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE_EQUAL(stream.str(), "{month {year 2021,sundays {4 11 18 25},mondays {5 12 19 26},tuesdays {6 13 20 27},wednesdays {7 14 21 28},thursdays {1 8 15 22 29},fridays {2 9 16 23 30},saturdays {3 10 17 24 31}}}");
}

// TODO: clean up spaces.
BOOST_AUTO_TEST_CASE(props__write__complex_object_info_unflat__valid_expected)
{
    std::ostringstream stream;
    root.write(stream, props::format::info, false);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE_EQUAL(stream.str(), 
        "{\n"
        "    month \n"
        "    {\n"
        "        year 2021\n"
        "        sundays \n"
        "        {\n"
        "            4\n"
        "            11\n"
        "            18\n"
        "            25\n"
        "        }\n"
        "        mondays \n"
        "        {\n"
        "            5\n"
        "            12\n"
        "            19\n"
        "            26\n"
        "        }\n"
        "        tuesdays \n"
        "        {\n"
        "            6\n"
        "            13\n"
        "            20\n"
        "            27\n"
        "        }\n"
        "        wednesdays \n"
        "        {\n"
        "            7\n"
        "            14\n"
        "            21\n"
        "            28\n"
        "        }\n"
        "        thursdays \n"
        "        {\n"
        "            1\n"
        "            8\n"
        "            15\n"
        "            22\n"
        "            29\n"
        "        }\n"
        "        fridays \n"
        "        {\n"
        "            2\n"
        "            9\n"
        "            16\n"
        "            23\n"
        "            30\n"
        "        }\n"
        "        saturdays \n"
        "        {\n"
        "            3\n"
        "            10\n"
        "            17\n"
        "            24\n"
        "            31\n"
        "        }\n"
        "    }\n"
        "}"
    );
}

BOOST_AUTO_TEST_CASE(props__write__complex_object_xml_flat__valid_expected)
{
    std::ostringstream stream;
    root.write(stream, props::format::xml);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE_EQUAL(stream.str(), "<month><year>2021</year><sundays><sunday>4</sunday><sunday>11</sunday><sunday>18</sunday><sunday>25</sunday></sundays><mondays><monday>5</monday><monday>12</monday><monday>19</monday><monday>26</monday></mondays><tuesdays><tuesday>6</tuesday><tuesday>13</tuesday><tuesday>20</tuesday><tuesday>27</tuesday></tuesdays><wednesdays><wednesday>7</wednesday><wednesday>14</wednesday><wednesday>21</wednesday><wednesday>28</wednesday></wednesdays><thursdays><thursday>1</thursday><thursday>8</thursday><thursday>15</thursday><thursday>22</thursday><thursday>29</thursday></thursdays><fridays><friday>2</friday><friday>9</friday><friday>16</friday><friday>23</friday><friday>30</friday></fridays><saturdays><saturday>3</saturday><saturday>10</saturday><saturday>17</saturday><saturday>24</saturday><saturday>31</saturday></saturdays></month>");
}

BOOST_AUTO_TEST_CASE(props__write__complex_object_xml_unflat__valid_expected)
{
    std::ostringstream stream;
    root.write(stream, props::format::xml, false);
    BOOST_REQUIRE(stream);

    BOOST_REQUIRE_EQUAL(stream.str(),
        "<month>\n"
        "    <year>2021</year>\n"
        "    <sundays>\n"
        "        <sunday>4</sunday>\n"
        "        <sunday>11</sunday>\n"
        "        <sunday>18</sunday>\n"
        "        <sunday>25</sunday>\n"
        "    </sundays>\n"
        "    <mondays>\n"
        "        <monday>5</monday>\n"
        "        <monday>12</monday>\n"
        "        <monday>19</monday>\n"
        "        <monday>26</monday>\n"
        "    </mondays>\n"
        "    <tuesdays>\n"
        "        <tuesday>6</tuesday>\n"
        "        <tuesday>13</tuesday>\n"
        "        <tuesday>20</tuesday>\n"
        "        <tuesday>27</tuesday>\n"
        "    </tuesdays>\n"
        "    <wednesdays>\n"
        "        <wednesday>7</wednesday>\n"
        "        <wednesday>14</wednesday>\n"
        "        <wednesday>21</wednesday>\n"
        "        <wednesday>28</wednesday>\n"
        "    </wednesdays>\n"
        "    <thursdays>\n"
        "        <thursday>1</thursday>\n"
        "        <thursday>8</thursday>\n"
        "        <thursday>15</thursday>\n"
        "        <thursday>22</thursday>\n"
        "        <thursday>29</thursday>\n"
        "    </thursdays>\n"
        "    <fridays>\n"
        "        <friday>2</friday>\n"
        "        <friday>9</friday>\n"
        "        <friday>16</friday>\n"
        "        <friday>23</friday>\n"
        "        <friday>30</friday>\n"
        "    </fridays>\n"
        "    <saturdays>\n"
        "        <saturday>3</saturday>\n"
        "        <saturday>10</saturday>\n"
        "        <saturday>17</saturday>\n"
        "        <saturday>24</saturday>\n"
        "        <saturday>31</saturday>\n"
        "    </saturdays>\n"
        "</month>"
    );
}

BOOST_AUTO_TEST_SUITE_END()
