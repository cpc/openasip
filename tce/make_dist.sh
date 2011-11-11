#!/bin/sh
if ! bzr diff > /dev/null; 
then
    echo "Your TCE checkout must be clean to proceed."
    exit 1
fi

#
# Builds a .tar.gz out of the current tree, suitable
# for a release.
./autogen.sh
# A hack to ensure newlib dir is fresh and clean because
# it's packaged as-is.
rm -r newlib-1.17.0 && bzr revert newlib-1.17.0

# Similarly test. Its subidrs do not have the EXTRA_DIST 
# variables set so we'll just include the whole dir recursively 
# using the root Makefile.am's EXTRA_DIST.
rm -r test && bzr revert test
./configure
make dist
