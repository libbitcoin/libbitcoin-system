import json
import os
import sys

bpath = "/home/genjix/src/bitcoin/src/test/data/"

with open(bpath + "script_valid.json") as f:
    tests = json.loads(f.read())

prefix_path = os.path.dirname(sys.argv[0])
scritp = os.path.join(prefix_path, "scritp")

# valid tests len = 173
for counter, test in enumerate(tests):
    assert len(test) == 2 or len(test) == 3
    print "====", (counter + 1), "of", len(tests), "===="
    test = (test[0], test[1], test[2] if len(test) == 3 else "")
    arguments = '"%s" "%s" "%s"' % test
    retcode = os.system(scritp + " " + arguments)
    success = bool(open("/tmp/script_status").read())
    if success:
        print "Status: pass"
    else:
        print (counter + 1), "Status: fail", test
    assert success
    print

