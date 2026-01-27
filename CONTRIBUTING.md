Information and guidelines for people contributing code back to OpenASIP.

Coding Style
================

The coding style is roughly defined by `openasip/.clang-format`. The all-important indentation style is 4 spaces, no tabs.

Note that large parts of the codebase were written before semi-formal style checking, so you may want to run the formatter on only your changes and leave the rest of the file alone to minimize unnecessary noise in your commits. For example, you could run the below from within `openasip/` before committing:

```shell
./tools/scripts/format-diff.sh
```

Alternatively, to format a whole branch at a time:

```shell
./tools/scripts/format-branch.sh
```

Test Suite
==============

The test suite includes both fine grained unit tests and "systemtests"
that test the command line interfaces of the tools using test scripts. In case you want to contribute code for OpenASIP, you should ensure your modifications pass the test suite before submitting the patch/committing. In addition, if you add a new feature, you should include a system test (actually an integration test), some unit tests, or preferably both, with your merge request.

The system tests are under 'testsuite' directory. '''Note:''' The test system currently works only with a source tree build.

We have a commit policy which does not allow commiting any code that does not pass the "quick" test suite that can be executed with ''compiletest.sh -q''. Preferably one should run the whole test suite (without the -q switch) which runs also the longer test benches.

'''Note:''' Use '-c' in case you do not want to 'make clean' first. That is, to use previously compiled files in your source tree.

Debugging a failed test
=======================

In case the compile test fails with system test errors (sys tests, long tests, longlong tests), the corresponding systemtest directory (''systemtest'', ''systemtest_long'' and ''systemtest_longlong'') contains a ''difference.txt'' file which shows the differences to  the expected output.

In case you want to execute a single systemtest, chdir to the wanted test directory and execute ''tools/scripts/systemtest.py -o -s'' this executes all system tests recursively in the current path and prints out the output differences in case of a failure to the file ''difference.txt''.

The ''systemtest.py'' test script uses '.testdesc' files to load the test information (name, binary to execute and its arguments) for each executed test. However, the .testdesc files often just call a shell script that performs the actual test steps. A shortcut for this type of test is to define a tcetest_* file which is a shell script that contains a certain magic words describing the test suite.

For example, ''systemtest/bintools/Compiler/tcetest_sp_init.sh'':

```
#!/bin/sh
### TCE TESTCASE
### title: Initialization of the stack pointer to a custom value
### xstdout: 1\n1
mach=data/minimal_with_stdout.adf
src=data/stack_test.c
program=$(mktemp tmpXXXXXX)
tcecc $src -llwpr -O0 -a $mach -o $program
# By default it should generate the SP initializer to 16777208
# for the minimal machine.
tcedisasm -s $mach $program | head -n10 | grep -c 16777208
# Force the initial SP to a lower value.
tcecc -llwpr $src -O0 -a $mach -o $program --init-sp 0x0000fff8
tcedisasm -s $mach $program | head -n10 | grep -c 65528

rm -f $program
```

The expected output (and input, in case it's  a console program reading from stdin) are stored in subdirs named after the .testdesc file (e.g. ''MyTest.testdesc'' has ''MyTest'' input/output subdir)
with files named ''1_mytest.txt'' and ''1_output.txt'' for the input and output, respectively.

For the unit tests, the test directories are in ''tce/test''.
cd to the wanted test directory and execute ''make'' to run the tests recursively.

Note: that some of the tests expect the TCE_DEVEL_MODE environment variable set
to 1 before executing in order to find the auxialiary files from the
build directory.
