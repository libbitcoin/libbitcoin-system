#!/bin/bash

# Run these commands to start the fullnode after installing libbitcoin:
#
#   $ make
#   $ mkdir database
#   $ ./initchain database
#   $ screen -S fullnode
#   $ ./fullnode
#   ... press CTRL-A, D
#   $ ./blocks.sh
#
watch -n 0.5 "grep Block debug.log | tail -n 20"

