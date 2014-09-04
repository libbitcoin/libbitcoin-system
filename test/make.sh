#!/bin/bash
#
# Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
#
# This file is part of libbitcoin.
#
# libbitcoin is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License with
# additional permissions to the one published by the Free Software
# Foundation, either version 3 of the License, or (at your option) 
# any later version. For more information see LICENSE.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#
TESTBINARY=test_libbitcoin
CXX=g++
CXXFLAGS=$(pkg-config --cflags libbitcoin libcrypto)
LDFLAGS="$(pkg-config --libs libbitcoin libcrypto) -lboost_unit_test_framework"
mkdir -p obj/
for i in $(ls *.cpp); do
    OBJ_FILE=obj/${i%.cpp}.o
    if test $i -nt $OBJ_FILE; then
        echo "Compiling $i..."
        $CXX $CXXFLAGS -c $i -o $OBJ_FILE -DBOOST_TEST_DYN_LINK
    fi
done
echo "Linking..."
$CXX -o $TESTBINARY obj/*.o $LDFLAGS
#./$TESTBINARY

