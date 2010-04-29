#!/bin/sh
#
# Builds a .tar.gz out of the current tree, suitable
# for a release.
autoreconf -i
# A hack to ensure newlib dir is fresh and clean because
# it's packaged as-is.
rm -r newlib-1.17.0 && bzr revert newlib-1.17.0
./configure
make dist
