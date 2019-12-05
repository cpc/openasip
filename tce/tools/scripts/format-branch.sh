#!/bin/bash

if [ ! -d .git ]; then
  if [ -d src ]; then
      cd ..
      if [ ! -d .git ]; then
          echo "must be run in TCE repository root or the source root"
	  exit 1
      fi
  else
      echo "must be run in TCE repository root or the source root"
      exit 1
  fi
fi

REF_BRANCH=${1:-origin/master}
SCRIPTDIR=tce/tools/scripts

echo "Diffing against ${REF_BRANCH}..."

PATCHY=/tmp/p.patch

rm -f $PATCHY
git diff master -U0 --no-color >$PATCHY

$SCRIPTDIR/clang-format-diff.py \
 -regex '(.*(\.hh$|\.cc$|\.c$|\.h$))' \
 -i -p1 -style \
 "{BasedOnStyle: Google, IndentWidth: 4, \
 AlwaysBreakAfterReturnType: AllDefinitions, AccessModifierOffset: -4, \
 AlignAfterOpenBracket: AlwaysBreak, ColumnLimit: 78}" \
 -v <$PATCHY
