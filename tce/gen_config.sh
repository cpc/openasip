#!/bin/bash
#
# Generates the 'configure' script for TCE.
# 
# Uses kludges to get past issues between libtool versions etc.
#

rm -f acinclude.m4

autoreconf

# In case of libtool 2.x (at least in Ubuntu Intrepid), m4 dir
# should now contain libtool.m4. If that's the case, we do not
# need to include libtool.m4 in autoconf as it's found from there.
# Otherwise, rename the old working acinclude.m4 containing the
# libtool.m4 for libtool1.5x.
#
# TODO: find out why the new libtool script produces the linkage
# error with the older Ubuntu's (hidden 'stat' symbol or something).
if ! test -e m4/libtool.m4;
then

echo Assuming acinclude.m4 is needed. Using the old one.
ln -s acinclude.m4.lt15 acinclude.m4

else

echo Assuming acinclude.m4 is not needed.

fi
