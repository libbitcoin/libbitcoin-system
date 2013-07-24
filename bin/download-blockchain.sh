#!/bin/bash
# If you know a better download method, then please let me know.
# You can also use http too:
#   wget http://46.4.92.107/blockchain.tar.bz2
if [ $# -ne 1 ]; then
    echo "Usage: `basename $0` DOWNLOAD_DIRECTORY"
    exit $E_BADARGS
fi
echo "Type 'public' at the password prompt."
COMMAND="rsync -avz public@46.4.92.107:/var/www/blockchain/ $1/blockchain/"
echo "> $COMMAND"
$COMMAND

