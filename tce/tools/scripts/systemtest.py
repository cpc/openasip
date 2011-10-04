#!/usr/bin/env python
# -!- coding: utf-8 -!-
"""
    Copyright (c) 2011 Tampere University of Technology.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
"""
"""
Integration/systemtest script for the TCE tools.

To do:
* Support executing {run_,test_,run-,test-}.{py,sh} files directly. 
  * Read metadata (description, maybe expected output?) from the 
    script's comments. 
  * Setup PATH so one does not need relative paths to the src-tree 
    binaries, thus enable testing of build tree builds and even testing
    TCE installations.

@author 2011 Pekka Jääskeläinen 
"""
import os
import tempfile
import sys
import re
import glob
import time
import math
import signal
import StringIO

from difflib import unified_diff
from optparse import OptionParser
from subprocess import Popen, PIPE


def run_with_timeout(command, timeoutSecs, inputStream = "", combinedOutput=True):
    """
    Runs the given process until it exits or the given time out is reached.

    Returns a tuple: (bool:timeout, str:stdout, str:stderr, int:exitcode)
    """
    timePassed = 0.0
    increment = 0.01

    stderrFD, errFile = tempfile.mkstemp()
    if combinedOutput:
        stdoutFD, outFile = stderrFD, errFile
    else:
        stdoutFD, outFile = tempfile.mkstemp()

    process =  Popen(command, shell=True, stdin=PIPE, 
                     stdout=stdoutFD, stderr=stderrFD, close_fds=False)

    if process == None:
        print "Could not create process"
        sys.exit(1)
    try:
        if inputStream != "":
            for line in inputStream:
                process.stdin.write(line.strip() + '\n')
                process.stdin.flush()
            process.stdin.close()

        while True:
            status = process.poll()
            if status != None:
                # Process terminated succesfully.
                stdoutSize = os.lseek(stdoutFD, 0, 2)
                stderrSize = os.lseek(stderrFD, 0, 2)

                os.lseek(stdoutFD, 0, 0)
                os.lseek(stderrFD, 0, 0)

                stdoutContents = os.read(stdoutFD, stdoutSize)
                os.close(stdoutFD)
                os.remove(outFile)

                if not combinedOutput:
                    stderrContents = os.read(stderrFD, stderrSize)
                    os.close(stderrFD)
                    os.remove(errFile)
                else:
                    stderrContents = stdoutContents

                return (False, stdoutContents, stderrContents, process.returncode)

            if timePassed < timeoutSecs:
                time.sleep(increment)
                timePassed = timePassed + increment
            else:
                # time out, kill the process.
                stdoutSize = os.lseek(stdoutFD, 0, 2)
                stderrSize = os.lseek(stderrFD, 0, 2)

                os.lseek(stdoutFD, 0, 0)
                stdoutContents = os.read(stdoutFD, stdoutSize)
                os.close(stdoutFD)
                os.remove(outFile)

                if not combinedOutput:
                    os.lseek(stderrFD, 0, 0)
                    stderrContents = os.read(stderrFD, stderrSize)
                    os.close(stderrFD)
                    os.remove(errFile)
                else:
                    stderrContents = stdoutContents
                os.kill(process.pid, signal.SIGTSTP)
                return (True, stdoutContents, stderrContents, process.returncode)
    except Exception, e:
        # if something threw exception (e.g. ctrl-c)
        os.kill(process.pid, signal.SIGTSTP)
        try:
            # time out, kill the process.
            # time out, kill the process.
            stdoutSize = os.lseek(stdoutFD, 0, 2)
            stderrSize = os.lseek(stderrFD, 0, 2)

            os.lseek(stdoutFD, 0, 0)
            stdoutContents = os.read(stdoutFD, stdoutSize)
            os.close(stdoutFD)
            os.remove(outFile)

            if not combinedOutput:
                os.lseek(stderrFD, 0, 0)
                stderrContents = os.read(stderrFD, stderrSize)
            else:
                os.close(stderrFD)
                os.remove(errFile)
                stderrContents = stdoutContents

            os.kill(process.pid, signal.SIGTSTP)                
        except:
            pass

        return (False, stdoutContents, stderrContents, process.returncode)


def run_command(command, echoStdout=False, echoStderr=False, echoCmd=False, 
                stdoutFD=None, stderrFD=None, stdinFile=None):
    """Runs the given shell command and returns its exit code.

    If echoOutput is False, stdout and stderr are redirected to /dev/null."""
      
    if echoCmd:
        print command

    if not echoStdout:
        stdoutRedir = open('/dev/null', 'w')
        stdoutFD = stdoutRedir.fileno()

    if not echoStderr:
        stderrRedir = open('/dev/null', 'w')
        stderrFD = stderrRedir.fileno()

    process = \
       Popen(command, shell=True, stdin=PIPE, stdout=stdoutFD,
             stderr=stderrFD, close_fds=False)
    if stdinFile is not None:
        process.stdin.write(open(stdinFile).read())
        process.communicate()
        process.stdin.close()

    return process.wait()

tempfiles = []
def create_temp_file(suffix=""):
    tf = tempfile.mkstemp(suffix=suffix)
    tempfiles.append(tf[1])
    return tf

def cleanup_and_exit(retval=0):
    for tf in tempfiles:
        os.unlink(tf)
    sys.exit(retval)

def parse_options():
    parser = OptionParser(usage="usage: %prog [options] tests_root_dir")
    parser.add_option("-s", "--print_successful", dest="print_successful", action="store_true",
                      help="Print successful tests also. " + \
                          "Default is to not print anything in case of an OK run.", 
                      default=False)
    parser.add_option("-o", "--output-differences", dest="output_diff", action="store_true", 
                      help="Output the found differences to 'differences.txt'.")
    parser.add_option("-d", "--dump-output", dest="dump_output", action="store_true",
                      help="Executes the test case(s) and dumps the stdout and stderr. " + \
                      "Useful for creating the initial test case verification files.", 
                      default=False)      
    parser.add_option("-w", "--watchdog-time", dest="timeout", type="int",
                      help="The number of seconds to wait before assuming a single test got " +\
                          "stuck and should be killed.",
                      default=2*60*60)
    parser.add_option("-t", "--test-case", dest="test_cases", action="append", type="string",
                      default=[],
                      help="Execute only the given test case files. " + \
                      "This option can be given multiple times.")   

    (options, args) = parser.parse_args()

    return (options, args)

output_diff_file = None
class IntegrationTestCase(object):
    """Represents a single integration/system test case in the TCE test suite.

    Also handles the loading of test case data from the test case metadata/script
    files."""
    def __init__(self, test_case_file):
        self._file_name = test_case_file
        self.test_dir = os.path.dirname(test_case_file)
        self.valid = False
        if test_case_file.endswith(".testdesc"): 
            self._load_legacy_testdesc()

        # list of tuples of test data:
        # (stdin, expected stdout)
        self._test_data = []
   

    def _parse_php_str_assignment(self, contents, variable):
        m = re.search(r"\$%s\s*=\s*(.*?)\s*\"\s*;" % variable, contents, re.DOTALL)
        if m is None: 
            return ""
        else:
            clean_str = ""
            for line in m.group(1).split('\n'):
                line = line.strip().replace("\\\"", "\"")
                if line.startswith("\""):
                    line = line[1:]
                if line.endswith("."):
                    line = line[:-1]
                if line.endswith("\""):
                    line = line[:-1]
                clean_str += line
            return clean_str.strip()

    def _load_verification_data(self):
        
        out_files = glob.glob(os.path.join(self.verification_data_dir, "*_output.txt"))
        
        for out_file in out_files:
            index = os.path.basename(out_file)[0:-len("output.txt")]
            in_files_pattern = \
                os.path.join(self.verification_data_dir, index + "*.txt")
            in_files = glob.glob(in_files_pattern)

            in_files.remove(out_file)

            if len(in_files) > 2:
                print >> sys.stderr, "More than 1 stdin files for %s." % out_file
                cleanup_and_exit(1)
            elif len(in_files) == 1:
                in_file = os.path.basename(in_files[0])
            else:
                in_file = None

            self._test_data.append((in_file, os.path.basename(out_file)))

        assert len(self._test_data) > 0

    def _load_legacy_testdesc(self):
        contents = open(self._file_name).read().strip()
        if not contents.startswith("<?php"):
            print >> sys.stderr, "Illegal test file", self._file_name
            self.valid = False
            return

        self.description = self._parse_php_str_assignment(contents, "test_description")
        self.bin = self._parse_php_str_assignment(contents, "test_bin")
        self.args = self._parse_php_str_assignment(contents, "bin_args")        
        self.type = "testdesc"
        self.verification_data_dir = os.path.basename(self._file_name).split(".testdesc")[0]

        if len(self.description) == 0 or len(self.bin) == 0:
            print >> sys.stderr, "Illegal test file", self._file_name
            self.valid = False
            return

        self.valid = True

    def __str__(self):
        return ("description: %s\n" + \
                "type:        %s\n" + \
                "bin:         %s\n" + \
                "args:        %s\n") % \
            (self.description, self.type, self.bin, self.args)

    def execute(self):
        """Assumes CWD is in the test directory when this is called."""

        if os.path.exists(os.path.basename(self._file_name) + ".disabled"):
            # The test case might have been disabled in ./initialize
            return True

        if options.print_successful:
            sys.stdout.write(self._file_name + ": " + self.description + "...")
            sys.stdout.flush()

        all_ok = True
        
        start_time = time.time()

        self._load_verification_data()

        for test_data in self._test_data:
            stdin_fn = test_data[0]
            if stdin_fn is not None:
                stdin_fn = os.path.join(self.verification_data_dir, stdin_fn)
                stdinStimulus = open(stdin_fn, 'r').readlines()
            else:
                stdinStimulus = ""
            stdout_fn = test_data[1]
            outputTemp = create_temp_file(".out")
            (timeout, stdoutStr, stderrStr, exitcode) = \
                run_with_timeout(self.bin + " " + self.args, options.timeout, inputStream=stdinStimulus)

            if timeout:
                if options.output_diff:
                    output_diff_file.write("FAIL (timeout %ss): " % options.timeout + \
                                           self._file_name + ": " + self.description + "\n")
                all_ok = False
                continue

            stdout = StringIO.StringIO(stdoutStr)
            gotOut = stdout.readlines()
            correctOut = open(os.path.join(self.verification_data_dir, stdout_fn)).readlines()

            stdoutDiff = list(unified_diff(correctOut, gotOut, 
                              fromfile="expected.stdout", tofile="produced.stdout"))
            if options.dump_output:
                sys.stdout.write(stdoutStr)
                continue           

            if len(stdoutDiff) > 0:
                if options.output_diff:
                    output_diff_file.write("FAIL: " + self._file_name + ": " + self.description + "\n")
                    for line in stdoutDiff:
                        output_diff_file.write(line)
                    output_diff_file.flush()
                all_ok = False

            if options.print_successful:
                sys.stdout.write(".")
                sys.stdout.flush()

        # Free some memory.
        self._test_data = []

        end_time = time.time()
        duration = end_time - start_time
        duration_str = "(%dm%.3fs)" % \
            (duration / 60, duration % 60)
        if not options.print_successful and not all_ok:
            sys.stdout.write(self._file_name + ": " + self.description + "...")
        if all_ok:
            if options.print_successful:
                sys.stdout.write("OK %s\n" % duration_str)
        else:
            sys.stdout.write("FAIL %s\n" % duration_str)

        return all_ok

def get_fs_tree(root):
    """Returns all files found starting from the given root path.

    Does not include directories."""
    found_files = []
    for f in os.listdir(root):
        full_path = os.path.join(root, f)
        if not os.path.isdir(full_path):
            found_files.append(os.path.join(root, f))
        else:
            found_files += get_fs_tree(os.path.join(root, f))

    return found_files

def find_test_cases(root):
    fs_tree = get_fs_tree(root)
    test_cases = []
    for f in fs_tree:
        test_case = IntegrationTestCase(f)
        if test_case.valid: 
            test_cases.append(test_case)
    return test_cases

if __name__ == "__main__":
    options, args = parse_options()

    if options.output_diff:
        output_diff_file = open('difference.txt', 'w+')

    if options.test_cases == []:
        if len(args) == 0:
            root_dir = "."
        else:
            root_dir = args[0]
        all_test_cases = find_test_cases(root_dir)
    else:
        for fn in options.test_cases:
            if not os.access(fn, os.R_OK):
                print >> sys.stderr, "Cannot open %s." % fn
                cleanup_and_exit(1)
        all_test_cases = [IntegrationTestCase(x) for x in options.test_cases]
    test_dirs = dict()

    for test_case in all_test_cases:
        test_dirs[test_case.test_dir] = \
            test_dirs.get(test_case.test_dir, []) + [test_case]

    top_dir = os.getcwd()

    all_ok = True

    for test_dir in test_dirs.keys():
        if test_dir != '':
            os.chdir(test_dir)
        if os.access("initialize", os.X_OK):
            run_command("./initialize")

        for test_case in test_dirs[test_dir]:
            all_ok = test_case.execute() and all_ok

        if os.access("finalize", os.X_OK):
            run_command("./finalize")
        os.chdir(top_dir)

    if options.output_diff:
        output_diff_file.close()

    if not all_ok: 
        if options.output_diff:
            sys.stderr.write("Differences found against verification data are stored into difference.txt\n")
        cleanup_and_exit(1)
    else: 
        if options.output_diff and os.path.exists("difference.txt"):
            os.unlink("difference.txt")
        cleanup_and_exit(0)


    
