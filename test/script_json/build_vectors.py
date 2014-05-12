#!/usr/bin/env python
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
import json
import sys

def display_scripts(json_path):
    with open(json_path) as f:
        tests = json.loads(f.read())
    is_first = True
    for test in tests:
        if not is_first:
            print ","
        is_first = False
        input = test[0]
        output = test[1]
        description = ""
        if len(test) == 3:
            description = test[2]
        sys.stdout.write('    {"%s", "%s", "%s"}' % (
            input, output, description))
    print

def main():
    print "#include <string>"
    print "#include <vector>"
    print
    print "struct script_test"
    print "{"
    print "    std::string input, output, description;"
    print "};"
    print
    print "typedef std::vector<script_test> script_test_list;"
    print
    print "script_test_list valid_scripts{{"
    display_scripts("script_valid.json")
    print "}};"
    print
    print "script_test_list invalid_scripts{{"
    display_scripts("script_invalid.json")
    print "}};"

if __name__ == "__main__":
    main()

