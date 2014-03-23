import sys
import json
import time
import urllib2

#address = "1Fufjpf9RM2aQsGedhSpbSCGRHrmLMJ7yY"
address = "1EXoDusjGwvnjZUyKkxZ4UHEf77z6A5S4P"
filename = "history.proc.json"
limit = 292127;

def read_binfo(address, offset=0, limit=50):
    url = "https://blockchain.info/rawaddr/%s?offset=%s&limit=%s" % (
        address, offset, limit)
    f = urllib2.urlopen(url)
    return f.read()

def open_all(address, i):
    with open(filename) as f:
        history = json.loads(f.read())

    binfo_history = json.loads(read_binfo(address, i * 50, 50))
    if not binfo_history["txs"]:
        print "Finished."
        sys.exit(0)

    return history, binfo_history

def write_all(history):
    with open(filename, "w") as f:
        f.write(json.dumps(history, indent=2))

def do_outpart(address, i):
    history, binfo_history = open_all(address, i)

    for bi_tx in binfo_history["txs"]:
        if not "block_height" in bi_tx or bi_tx["block_height"] > limit:
            continue
        for bi_out in bi_tx["out"]:
            if bi_out["addr"] != address:
                continue
            found = False
            for row in history["history"]:
                if (row["output"][0] == bi_tx["hash"] and
                    row["output"][1] == bi_out["n"]):
                    assert row["output_found"] == False
                    row["output_found"] = True
                    found = True
            if not found:
                print json.dumps(bi_tx, indent=2)
            assert found

    write_all(history)
    print "Done:", i

def do_inpart(address, i):
    history, binfo_history = open_all(address, i)

    for bi_tx in binfo_history["txs"]:
        if not "block_height" in bi_tx or bi_tx["block_height"] > limit:
            continue
        for bi_idx, bi_inp in enumerate(bi_tx["inputs"]):
            try:
                prev = bi_inp["prev_out"]
            except KeyError:
                continue
            if prev["addr"] != address:
                continue
            found = False
            for row in history["history"]:
                if row["spend"] is None:
                    continue
                if (row["spend"][0] == bi_tx["hash"] and
                    row["spend"][1] == bi_idx):
                    assert row["spend_found"] == False
                    row["spend_found"] = True
                    found = True
            if not found:
                print json.dumps(bi_tx, indent=2)
            assert found

    write_all(history)
    print "Done:", i

def check():
    with open(filename) as f:
        history = json.loads(f.read())
    uniq_txs = set()
    for row in history["history"]:
        if row["output_found"] == False:
            print "Not found:", row
        else:
            uniq_txs.add(row["output"][0])
        if row["spend_found"] == False:
            print "Not found:", row
        elif row["spend_found"] == True:
            uniq_txs.add(row["spend"][0])
    print "Txs:", len(uniq_txs)

#for i in range(999):
##    do_outpart(address, i)
#    do_inpart(address, i)
#    time.sleep(0.5)
check()

