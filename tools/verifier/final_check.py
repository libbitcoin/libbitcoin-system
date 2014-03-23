import json

filename = "history.proc.json"

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

check()

