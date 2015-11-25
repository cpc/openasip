Information and guidelines for people contributing code back to TCE.

TCE coding style
================

There used to be a huge coding style manual all TCE developers slavishly and humbly followed :P Nowadays, as there is plenty of code, the easiest way is to just match the style of the existing code. 

If in doubt, look around. Good references are the oldest pieces of code (from the times when we had stricter code review process) inside src/base. Take a look at some of the files there to get the feeling.

The basic guidelines:

 * the indentations should be 4 spaces (no tabs)
 * maximum of 78 characters per line 

In case of emacs, you should use the stroustrup style.

Example .emacs:

```
(defun my-c-mode-common-hook ()
  (c-set-style "stroustrup")
  ;; other customizations can go here
  )
  (add-hook 'c-mode-common-hook 'my-c-mode-common-hook)

(setq default-tab-width 4)
(setq-default indent-tabs-mode nil)

(add-to-list 'auto-mode-alist '("\\.icc$" . c++-mode))
```

This setups the "stroustrup" style for C/C++ code and makes emacs detect the .icc files (inline C++ definitions in TCE) as C++ files.

TCE Test Suite
==============

The TCE test suite includes both fine grained unit tests and "systemtests" that test 
the command line interfaces of the tools using test scripts. In case you want to contribute code for TCE, you should ensure your modifications pass the test suite before submitting the patch/committing. In addition, if you add a new feature, you should include a system test (actually an integration test), some unit tests, or preferably both, with your merge request.

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

For the unit tests, the test directories are in ''tce/test''. chdir to the wanted test directory and execute ''make'' to run the tests recursively.
