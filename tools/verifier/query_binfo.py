import json
import sys
import time
import urllib2

def read_binfo(address, offset=0, limit=50):
    url = "https://blockchain.info/rawaddr/%s?offset=%s&limit=%s" % (
        address, offset, limit)
    f = urllib2.urlopen(url)
    return json.loads(f.read())

def open_all(address):
    limit = 50
    history = read_binfo(address, 0, limit)
    for i in range(1, 9999):
        history_ext = read_binfo(address, i * limit, limit)
        if not history_ext["txs"]:
            return history
        history["txs"].extend(history_ext["txs"])
        print "Done:", i
        time.sleep(0.5)
    raise Exception("Never reach here.")

def main(argv):
    if len(argv) < 3:
        print >> sys.stderr, "query_binfo ADDRESS FILENAME"
        return -1
    address = argv[1]
    output_filename = argv[2]
    history = open_all(address)
    with open(output_filename, "w") as f:
        f.write(json.dumps(history, indent=2))
    return 0

if __name__ == "__main__":
    sys.exit(main(sys.argv))

