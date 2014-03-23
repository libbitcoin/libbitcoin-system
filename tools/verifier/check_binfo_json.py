import json
import sys

def check(history):
    assert len(history["txs"]) == history["n_tx"]
    print "OK"

def main(argv):
    if len(argv) < 2:
        print >> sys.stderr, "check_binfo_json FILENAME"
        return -1
    filename = argv[1]
    with open(filename) as f:
        history = json.loads(f.read())
    check(history)

if __name__ == "__main__":
    sys.exit(main(sys.argv))

