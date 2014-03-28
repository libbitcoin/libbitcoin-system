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
import os
import sys
import subprocess

bpath = "/path/to/bitcoind/src/test/data/"
# Try valid or invalid json?
check_valid = False

def call(command):
    return subprocess.call(command, shell=True)

json_path = "script_valid.json" if check_valid else "script_invalid.json"

with open(bpath + json_path) as f:
    tests = json.loads(f.read())

prefix_path = os.path.dirname(sys.argv[0])
scritp = os.path.join(prefix_path, "scritp")

# valid tests len = 173
for counter, test in enumerate(tests):
    assert len(test) == 2 or len(test) == 3
    print "====", (counter + 1), "of", len(tests), "===="
    test = (test[0], test[1], test[2] if len(test) == 3 else "")
    arguments = '"%s" "%s" "%s"' % test
    retcode = call(scritp + " " + arguments)
    success = bool(retcode == 0)
    if success:
        print "Status: pass"
    else:
        print (counter + 1), "Status: fail", test
    if check_valid:
        assert success
    else:
        assert not success
    print

