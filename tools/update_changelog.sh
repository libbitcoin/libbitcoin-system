#!/bin/bash

FILE="$(dirname "$0")/../ChangeLog"

echo -e "Updated on `date`\n" > $FILE
echo -e "If working with git repository, use command 'git log --oneline --decorate' for latest change log\n" >> $FILE
git log --oneline --decorate >> $FILE
