#!/bin/bash
# rebuilds all opsets under tce root with OSALBuilder buildopset command

if [ ! -e "src/codesign/osal/OSALBuilder/buildopset" ]; then
    echo "Run this script in tce root directory."
    exit 1
fi

export BUILDOPSET="$(pwd)/src/codesign/osal/OSALBuilder/buildopset"

find ../../ -iname "*.opp" -execdir bash -c '$BUILDOPSET $(echo {} | cut -d"." -f1,2)' ';'
