import json
import os
import sys

bpath = "/home/genjix/src/bitcoin/src/test/data/"

with open(bpath + "script_valid.json") as f:
    tests = json.loads(f.read())

prefix_path = os.path.dirname(sys.argv[0])
scritp = os.path.join(prefix_path, "scritp")

for counter, test in enumerate(tests[:34]):
    assert len(test) == 2 or len(test) == 3
    print "====", (counter + 1), "of", len(tests), "===="
    test = (test[0], test[1], test[2] if len(test) == 3 else "")
    arguments = '"%s" "%s" "%s"' % test
    retcode = os.system(scritp + " " + arguments)
    assert retcode == 0 or retcode == 256
    if retcode == 0:
        print "Status: pass"
    elif retcode == 256:
        print "Status: fail"
    print

