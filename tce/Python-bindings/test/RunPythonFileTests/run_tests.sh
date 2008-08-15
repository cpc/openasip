#!/bin/sh
# Copyright 2002-2008 Tampere University of Technology.  All Rights Reserved.
#
# This file is part of TTA-Based Codesign Environment (TCE).
#
# TCE is free software; you can redistribute it and/or modify it under the
# terms of the GNU General Public License version 2 as published by the Free
# Software Foundation.
#
# TCE is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin St,
# Fifth Floor, Boston, MA  02110-1301  USA
#
# As a special exception, you may use this file as part of a free software
# library without restriction.  Specifically, if other files instantiate
# templates or use macros or inline functions from this file, or you compile
# this file and link it with other files to produce an executable, this file
# does not by itself cause the resulting executable to be covered by the GNU
# General Public License.  This exception does not however invalidate any
# other reasons why the executable file might be covered by the GNU General
# Public License.
export PYTHONPATH=.:${PYTHONPATH}
export LD_LIBRARY_PATH=../../../src/base/.libs:../../../src/tools/.libs:../../../src/applibs/.libs:../../../src/.libs
./run_python_file hello.py > hello.output
if diff hello.output hello.expected
then
  echo "Embedded hello test passed"
else
  echo "Embedded hello test failed"
fi
python hello.py > hello.output
if diff hello.output hello.expected
then
  echo "Standalone hello test passed"
else
  echo "Standalone hello test failed"
fi
./run_python_file disass_test.py > disass.output
if diff disass.output disass.expected
then
  echo "Embedded disass test passed"
else
  echo "Embedded disass test failed"
fi
python disass_test.py > disass.output
if diff disass.output disass.expected
then
  echo "Standalone disass test passed"
else
  echo "Standalone disass test failed"
fi
