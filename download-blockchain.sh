#!/bin/bash -x
# If you know a better download method, then please let me know.
#wget http://46.4.92.107/blockchain.tar.bz2
echo "Type 'public' at the password prompt."
rsync -avz public@46.4.92.107:/var/www/blockchain/ blockchain/

