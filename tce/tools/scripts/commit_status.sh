#!/bin/sh
# Copyright (c) 2002-2009 Tampere University of Technology.
#
# This file is part of TTA-Based Codesign Environment (TCE).
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.
#
# Script that shows commit status of source codes of local copy.
# This should help noticing uncommitted source files.
#
# (c) 2005 pekka.jaaskelainen@tut.fi
bzr status | egrep -v \
"^[^MC].*("\
".*\.cls$|.*\.sty$|.*\.bib$|.*\.log$|.*\.toc$|.*\.blg$|.*\.pdf$|.*\.ps$|"\
".*\.dvi$|.*\.aux$|.*\.bbl$|.*/fig|.*/eps|.*\.so|.*\.ind|.*\.idx|.*Makefile$|"\
".*Makefile.in|.*\.ilg|.*\.bb|.*\.libs|.*\.da|.*\.gcov|.*\.deps|.*/prode|"\
".*/tceasm|.*temp.tceasm|.*dumptpef|.*/MachineWindow|.*\.ttasim-history|"\
"config-status|stamp-h*|online-manual|lessthan.txt|autom.*cache|"\
"libtool|a.out|configure|config.h|config.status|aclocal.m4|tce-config|"\
".*/includedir|.*/libdir|.*/written.conf|.*/new.db|.*/example.opb|"\
".*_bem\.bem|.*BEMSerializerTest/data|*._output\.tpef|.*test/base/osal.*|"\
".*test/base/tpef.*|.*test/base/mach.*|.*PIG/generatebits|.*BEMGenerator/"\
"createbem|.*Scheduler/schedule|.*Estimator/estimate|.*Proxim/proxim|"\
".*buildopset|.*testosal|.*osed|.*HDB/createhdb|opset/base/base\.opb|"\
".*man/OSALGUI/OSALGUI|.*man/ProDe/ProDe|.*core.*[0-9]*|.*svn-commit.*|"\
".*\.backup|.*/runner|.*/runner.cpp|.*HDBManagerTest/data/newHDB.hdb|"\
".*/new.idf|.*Estimator/data/test.hdb|.*Estimator/data/worm.tpef.trace|"\
".*ProGe/generateprocessor|test/.*/GUIOptionsSerializerTest/gui.config|"\
".*Estimator/estimator-static|.*null_classes.lst|.*\.proximHistory|"\
".*/difference.txt$|.*data/.*\.opb$|.*data/hello.trace$|.*ttasim/ttasim$|"\
".*ttasim/ttasim-static$|.*BEMViewer/viewbem|"\
".*systemtest/bintools/BEMGenerator/data/complex.bem|"\
".*HDBManagerTest/data/newHDB..hdb|.*ttasim.out|"\
".*ProgramWriterTest/data/.*.tpf|.*DictionaryTool/dictionary_tool|"\
".*HDBEditor/hdbeditor|.*/tcedisasm|.*applibs/ProgramRepresentation|"\
".*ProGe/proge-output|.*/tremor/ttasim.out|.*fft_compr/imem_init|"\
".*systemtest_long/procgen/ProGe/testbench|"\
".*systemtest_long/procgen/ProGe/dpram_trace|"\
".*systemtest_long/procgen/ProGe/work|"\
".*systemtest_long/procgen/ProGe/bus.dump|"\
".*systemtest.*/lastresults.csv|"\
".*systemtest.*/bintools/Scheduler/tests/.*/data|*.\.opb|"\
".*systemtest_long/procgen/ProGe/data/decompressor.vhdl|"\
".*ExecutionTraceTest/data|.*DSDBManagerTest/dsdb..ddb|"\
".*\.dot|.*Explorer/explore|.*connectivity.tpef|"\
".*Python-bindings/src/setup.py|.*\.pyc|.*/registers_read|.*/registers_written|"\
".*/operations_executed|.*\.vhdbak|systemtest_long/procgen/ProGe/.*\.sh|"\
".*tce/cscope.*|llvm-frontend/build/|.*\@|"\
".*tce/tags|.*doc/man/TCE/TCE/|.*/llvm-tce/llvmtce|.*/tceopgen/tceopgen|"\
".*\.l[oa]$|.*/TableGen/tblgen|tce/systemtest/bintools/Scheduler/.*\.tpef|"\
".*\.bak|doc/man/html|.*/conflicting_fsa.txt|.*/dot.txt|systemtest/bintools/PIG/.*\.sh|.*/results.tex|"\
"tce/src/bintools/Compiler/llvm-tce/llvm-tce|"\
".*/program.bc"\
")"
