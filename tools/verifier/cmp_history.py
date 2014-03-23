import sys
import json
import time
import urllib2

address = "1EXoDusjGwvnjZUyKkxZ4UHEf77z6A5S4P"
binfo_filename = "history.binfo.json"
libbtc_filename = "history.libbtc.json"
limit = 292127;
output_filename = "history.proc.json"

def json_load(filename):
    with open(filename) as f:
        return json.loads(f.read())
    raise Exception("Problem loading JSON file.")

def mark_output(history, outpoint):
    found = False
    for row in history["history"]:
        if (row["output"][0] == outpoint[0] and
            row["output"][1] == outpoint[1]):
            assert row["output_found"] == False
            row["output_found"] = True
            found = True
    if not found:
        print json.dumps(bi_tx, indent=2)
    assert found

def mark_input(history, inpoint):
    found = False
    for row in history["history"]:
        if row["spend"] is None:
            continue
        if (row["spend"][0] == inpoint[0] and
            row["spend"][1] == inpoint[1]):
            assert row["spend_found"] == False
            row["spend_found"] = True
            found = True
    if not found:
        print json.dumps(bi_tx, indent=2)
    assert found

def process(tx, history):
    for output in tx["out"]:
        if output["addr"] != address:
            continue
        outpoint = (tx["hash"], output["n"])
        mark_output(history, outpoint)
    for idx, input in enumerate(tx["inputs"]):
        try:
            prev = input["prev_out"]
        except KeyError:
            continue
        if prev["addr"] != address:
            continue
        inpoint = (tx["hash"], idx)
        mark_input(history, inpoint)

def main():
    bi_history = json_load(binfo_filename)
    lb_history = json_load(libbtc_filename)

    for tx in bi_history["txs"]:
        if not "block_height" in tx or tx["block_height"] > limit:
            continue
        process(tx, lb_history)

    with open(output_filename, "w") as f:
        f.write(json.dumps(lb_history, indent=2))
    return 0

if __name__ == "__main__":
    sys.exit(main())

