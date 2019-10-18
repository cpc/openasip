#!/usr/bin/env python
# -!- coding: utf-8 -!-
"""
    Copyright (c) 2011-2015 Pekka Jääskeläinen / Tampere University.

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
Integration/systemtest script for the TCE command line tools.
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

try:
    import multiprocessing
    from multiprocessing import Pool
    mp_supported = True
except:
    mp_supported = False

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
                      default=4*60*60)
    parser.add_option("-t", "--test-case", dest="test_cases", action="append", type="string",
                      default=[],
                      help="Execute only the given test case files. " + \
                      "This option can be given multiple times.")
    if mp_supported:
        parser.add_option("-p", "--parallel-processes", dest="par_process_count", type="int", 
                          default=multiprocessing.cpu_count(),
                          help="The number of parallel processes to use for running the test dirs. " + \
                              "Use 1 to disable parallel execution.")
        parser.add_option("-a", "--all-parallel", dest="all_parallel", action="store_true",
                          help="Assume all tests can be ran in parallel (after running all initialize files first). ")

    (options, args) = parser.parse_args()

    return (options, args)

output_diff_file = None
class IntegrationTestCase(object):
    """Represents a single integration/system test case in the TCE test suite.

    Also handles the loading of test case data from the test case metadata/script
    files."""
    def __init__(self, test_case_file):
        self._file_name = test_case_file
        self.test_dir = os.path.dirname(test_case_file) or "."

        # In case the test tests only one stdin case, this is set to the
        # expected stdout/stderr content for verification.
        self.xstdout = None
        # In case the tcetest does not contain an xstdout line, it means
        # that stdout should be ignored and the test can fail only with
        # a nonzero exit code. On the other hand, if xstdout is empty, 
        # the tests expectes there's no stdout printouts.
        self.ignore_stdout = False
        self.xstderr = None
        self.stdin = ""
        self.valid = False
        if test_case_file.endswith(".testdesc"): 
            self._load_legacy_testdesc()
        elif os.path.basename(test_case_file).startswith("tcetest_") and \
                os.path.basename(test_case_file).endswith(".sh"):
            self._load_sh_testdesc()        

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
                # Support also files without running index number in front.
                # E.g. ld32_st32_output.txt and ld32_st32.txt (input).
                no_index_file = os.path.join(self.verification_data_dir, index[0:-1] + ".txt")
                if os.path.exists(no_index_file):
                    in_file = index[0:-1] + ".txt"
                else:                                 
                    in_file = None

            self._test_data.append((in_file, os.path.basename(out_file)))
            self.ignore_stdout = False;

        if len(self._test_data) == 0:
            # Assume the test case should be executed only once, does not
            # read stdin and is assumed to output empty stdout and stderr
            if self.stdin is None:
                self.stdin = [""]
            if self.xstdout is None:
                self.xstdout = [""]
            if self.xstderr is None:
                self.xstderr = [""]
            # A None test data file denotes this here: the test input/outputs are
            # read from the variables in that case.
            self._test_data.append((None, None))

    def _load_legacy_testdesc(self):
        contents = open(self._file_name).read().strip()
        if not contents.startswith("<?"):
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

    def _load_sh_testdesc(self):
        contents = open(self._file_name).read().strip()
        self.valid = False
        if not "### TCE TESTCASE" in contents or \
                not "### title: " in contents:
            print >> sys.stderr, (str(self._file_name)
                                  + ": Invalid test description.")
            sys.exit(2)
            return

        m = re.search(r"###\stitle:\s(.*)", contents)
        if not m:
            return

        self.description = m.group(1).strip()
        self.bin = os.path.join(".", os.path.basename(self._file_name))

        m = re.search(r"###\sxstdout:\s(.*)", contents)
        if not m:
            self.xstdout = None
            self.ignore_stdout = True
        else:
            # The verifier assumes the lines are terminated with the new line 
            # character.
            self.xstdout = [x + "\n" for x in m.group(1).strip().split("\\n")]
            self.ignore_stdout = False

        self.args = ""
        self.type = "sh"
        # The default verification data dir is the file name with tcetest_ prefix and .sh suffix
        # stripped.
        self.verification_data_dir = os.path.basename(self._file_name)[8:-3]

        if len(self.description) == 0 or len(self.bin) == 0:
            print >> sys.stderr, "Illegal test file", self._file_name
            self.valid = False
            return

        self.valid = True

    def __str__(self):
        return ("description: %s\n" + \
                "type:        %s\n" + \
                "bin:         %s\n" + \
                "args:        %s\n" + \
                "data dir:    %s\n") % \
            (self.description, self.type, self.bin, self.args, self.verification_data_dir)

    def execute(self, stdout_stream=sys.stdout):
        """Assumes CWD is in the test directory when this is called."""

        if os.path.exists(os.path.basename(self._file_name) + ".disabled"):
            # The test case might have been disabled in ./initialize
            return True

        if options.print_successful:
            stdout_stream.write(self._file_name + ": " + self.description + "...")
            stdout_stream.flush()

        all_ok = True
        
        start_time = time.time()

        self._load_verification_data()

        for test_data in self._test_data:
            stdin_fn = test_data[0]
            if stdin_fn is not None:
                stdin_fn = os.path.join(self.verification_data_dir, stdin_fn)
                stdinStimulus = open(stdin_fn, 'r').readlines()
            else:
                stdinStimulus = self.stdin

            outputTemp = create_temp_file(".out")
            (timeout, stdoutStr, stderrStr, exitcode) = \
                run_with_timeout(self.bin + " " + self.args, options.timeout, inputStream=stdinStimulus)

            if timeout:
                if options.output_diff:
                    output_diff_file.write("FAIL (timeout %ss): " % options.timeout + \
                                           self._file_name + ": " + self.description + " (%s) " % stdin_fn + "\n")
                all_ok = False
                continue

            stdout = StringIO.StringIO(stdoutStr)
            gotOut = stdout.readlines()

            stdout_fn = test_data[1]
            if stdout_fn is not None:
                correctOut = open(os.path.join(self.verification_data_dir, stdout_fn)).readlines()
            else:
                correctOut = self.xstdout


            # Allow checking only against the script exit code. In this case the
            # xstdout can be completely empty. It is marked a list with an empty 
            # string.
            if correctOut is None or correctOut == []:
                correctOut = [""]

            if gotOut is None or gotOut == []:
                gotOut = [""]

            if not self.ignore_stdout:
                stdoutDiff = list(unified_diff(correctOut, gotOut, 
                                               fromfile="expected.stdout", tofile="produced.stdout"))
            if options.dump_output:
                stdout_stream.write(stdoutStr)

            if not self.ignore_stdout and len(stdoutDiff) > 0:
                if options.output_diff:
                    stdin_fn_out = ""
                    if stdin_fn is not None: stdin_fn_out = ' (%s)' % stdin_fn
                    output_diff_file.write("FAIL: " + self._file_name + ": " + self.description + \
                                               "%s" % stdin_fn_out + "\n")
                    for line in stdoutDiff:
                        output_diff_file.write(line)
                    output_diff_file.flush()
                all_ok = False

            if exitcode != 0:
                if options.output_diff:
                    output_diff_file.write("FAIL: " + self._file_name + ": " + self.description + \
                                               " [nonzero (%d) exit code]\n" % exitcode)
                    if self.ignore_stdout and gotOut:
                        # In case we ignore stdout in the verification, it might still
                        # contain useful information for debugging the failed error code,
                        # thus let's dump it to the log here.
                        for line in gotOut:
                            output_diff_file.write(line)
                    output_diff_file.flush()
                all_ok = False

            if options.print_successful:
                stdout_stream.write(".")
                stdout_stream.flush()

        # Free some memory.
        self._test_data = []

        end_time = time.time()
        duration = end_time - start_time
        duration_str = "(%dm%.3fs)" % \
            (duration / 60, duration % 60)
        if not options.print_successful and not all_ok:
            stdout_stream.write(self._file_name + ": " + self.description + "...")
        if all_ok:
            if options.print_successful:
                stdout_stream.write("OK %s\n" % duration_str)
        else:
            stdout_stream.write("FAIL %s\n" % duration_str)

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

def get_fs_tree_dirs(root, max_depth=10, ignoredirs=[".deps", ".libs"]):
    """Returns all dirs found starting from the given root path.

    Does not include files."""
    found_dirs = []

    for f in os.listdir(root):
        full_path = os.path.join(root, f)
        if os.path.split(full_path)[-1] in ignoredirs: continue

        if os.path.isdir(full_path):
            found_dirs += [full_path]
            if max_depth > 0:
                found_dirs += get_fs_tree_dirs(os.path.join(root, f), max_depth=max_depth-1)

    return found_dirs

def find_test_cases(root):
    fs_tree = get_fs_tree(root)
    test_cases = []
    for f in fs_tree:
        test_case = IntegrationTestCase(f)
        if test_case.valid: 
            test_cases.append(test_case)
    return test_cases

def init_test_dir(test_dir):
    top_dir = os.getcwd()
    if test_dir != '':
        os.chdir(test_dir)
    if os.access("initialize", os.X_OK):
#        print("(%s) initialize %s" % (os.getpid(), test_dir))
        run_command("./initialize")
    os.chdir(top_dir)
    return True

def finalize_test_dir(test_dir):
    top_dir = os.getcwd()
    if test_dir != '':
        os.chdir(test_dir)
    if os.access("finalize", os.X_OK):
#        print("(%s) finalize %s" % (os.getpid(), test_dir))
        run_command("./finalize")
    os.chdir(top_dir)
    return True

# Run a single test case assuming parallel execution of multiple
# test cases.
def run_test_case_par(test_case):

    top_dir = os.getcwd()   
    os.chdir(test_case.test_dir)

    stdout_stream = StringIO.StringIO()

    ok = test_case.execute(stdout_stream) 

    stdout_str = stdout_stream.getvalue()
    if len(stdout_str):
        # todo: should use a lock here.
        # seems not so easy to get a lock to the processes in a Pool
        sys.stdout.write("[%s] " % os.getpid())
        sys.stdout.write(stdout_str)
        sys.stdout.flush()

    os.chdir(top_dir)

    return ok

# Run a single test directory sequentially, assuming parallel execution
# of multiple test dirs.
def run_test_dir_par(test_dir, test_cases):

    top_dir = os.getcwd()   

    init_test_dir(test_dir)
    os.chdir(test_dir)

    all_ok = True
    for test_case in test_cases:
        stdout_stream = StringIO.StringIO()
        all_ok = test_case.execute(stdout_stream) and all_ok
        stdout_str = stdout_stream.getvalue()
        if len(stdout_str):
            # todo: should use a lock here.
            # seems not so easy to get a lock to the processes in a Pool
            for line in stdout_str.splitlines():
                sys.stdout.write("[%s] %s\n" % (os.getpid(), line))
            sys.stdout.flush()

    os.chdir(top_dir)

    finalize_test_dir(test_dir)

    return all_ok


def process_test_dir_seq(test_dir, test_cases):

    top_dir = os.getcwd()
    
    init_test_dir(test_dir)
    os.chdir(test_dir)

    all_ok = True
    for test_case in test_cases:
        all_ok = test_case.execute() and all_ok

    os.chdir(top_dir)

    finalize_test_dir(test_dir)

    return all_ok

def run_test_dirs_in_parallel(test_dirs):

    all_ok = True

    exec_pool = Pool(options.par_process_count)
    exec_results = []
    for test_dir in test_dirs.keys():
        exec_results.append(exec_pool.apply_async(run_test_dir_par, (test_dir, test_dirs[test_dir])))
    exec_pool.close()
    all_ok = all([x.get(options.timeout) for x in exec_results])
    exec_pool.join()    

    return all_ok

def run_initializers_in_parallel(test_dirs):
    initializer_pool = Pool(options.par_process_count)
    initializer_results = []

    for test_dir in test_dirs.keys():
        initializer_results.append(initializer_pool.apply_async(init_test_dir, (test_dir, )))

    initializer_pool.close()
    [x.get(options.timeout) for x in initializer_results]
    initializer_pool.join()

def run_finalizers_in_parallel(test_dirs):
    finalizer_pool = Pool(options.par_process_count)
    finalizer_results = []

    for test_dir in test_dirs.keys():
        finalizer_results.append(finalizer_pool.apply_async(finalize_test_dir, (test_dir, )))

    finalizer_pool.close()
    [x.get(options.timeout) for x in finalizer_results]
    finalizer_pool.join()

def run_all_tests_in_parallel(test_dirs):    
    # Initalize all test dirs first so all the test cases can be ran in any 
    # order. Then, distribute the test cases to processes, and finally, 
    # finalize all test dirs. This does not work with the scheduler_tester.py
    # tests because multiple testdescs reuse the same test dirs and
    # scheduler_tester.py uses constant name files for functioning.
    run_initializers_in_parallel(test_dirs)

    exec_pool = Pool(options.par_process_count)
    exec_results = []
    for test_dir in test_dirs.keys():
        for test_case in test_dirs[test_dir]:
            exec_results.append(exec_pool.apply_async(run_test_case_par, (test_case, )))
    exec_pool.close()
    all_ok = all([x.get(options.timeout) for x in exec_results])
    exec_pool.join()    

    run_finalizers_in_parrallel(test_dirs)


def setup_exec_env():
    """Sets up the execution environment variables."""
    # Setup the PATH to point to the TCE binaries to test.
    # Find the tce/src from the dir parents.    
    bld_root = os.getcwd()
    while bld_root != "":
        if os.path.exists(bld_root + "/tce/src"):
            break
        bld_root = "/".join(os.path.split(bld_root)[0:-1])
        if bld_root.endswith('/'): bld_root = bld_root[0:-1]

    if bld_root == "": 
        sys.stderr.write("Cannot find TCE build root to set up the test environment PATH.\n")
        cleanup_and_exit(2)

    bld_root += "/tce"
#    print bld_root

    subtreeroots = ["scripts", "src/bintools", "src/codesign", "src/procgen"]
    # If any of the dirs inside the subtreeroots contains at least one
    # of these tested binaries, put the dir to the PATH.
    wanted_binaries = ["buildcompressor", "buildestimatorplugin",
                       "buildexplorerplugin", "buildicdecoderplugin", "buildopset",
                       "c2vhdl", "createbem", "createhdb", "dictionary_tool",
                       "dump_instruction_execution_trace", "dumptpef",
                       "estimate", "explore", "generatebits", "generate_cachegrind",
                       "generateprocessor", "hdbeditor", "llvm-tce", "machine_instruction_info",
                       "mc-stats", "minimize-ic", "osed", "pareto-vis", "prode",
                       "proxim", "tceasm", "tcecc", "tce-config", "tcedisasm",
                       "tceoclextgen", "tceopgen",
                       "testhdb", "testosal", "ttasim", "ttasim-tandem",
                       "ttaunittester", "viewbem", "rtlstats", "generatetests"]
    tce_path_env = ""
    max_depth = 2
    for root in subtreeroots:
        new_dirs = get_fs_tree_dirs(os.path.join(bld_root, root), max_depth=max_depth)
        for new_dir in new_dirs:
            for wanted in wanted_binaries:
                if os.path.exists(os.path.join(new_dir, wanted)):
                    #print "Found", wanted, "in", new_dir
                    tce_path_env += new_dir + ":"
                    wanted_binaries.remove(wanted)
                    break

    # Let's add these on demand. For now dirs required by pocl build.
    includedirs = ['src/tools', 'src/applibs/Simulator', 'src/base/mach',
                   'src/base/memory', 'src/base/program',
                   'src/applibs/LLVMBackend', '']
    cpp_flags = ""
    for include in includedirs:
        cpp_flags += "-I" + os.path.join(bld_root, include) + " "

    lib_tce_dir = os.path.join(bld_root, 'src')

    os.environ['TCE_LDFLAGS'] = "-L" + lib_tce_dir
    os.environ['TCE_CPPFLAGS'] = cpp_flags
    os.environ['TCE_LD_LIBRARY_PATH'] = lib_tce_dir
    os.environ['TCE_BUILD_ROOT'] = bld_root
    os.environ['ORIGINALPATH'] = os.environ['PATH']
    os.environ['PATH'] = tce_path_env + os.environ['PATH']
    os.environ['minimal_with_stdout'] = \
        os.path.join(bld_root, 'data', 'mach', 'minimal_with_stdout.adf')

if __name__ == "__main__":
    options, args = parse_options()
    setup_exec_env()

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

    all_ok = True

    if mp_supported and options.par_process_count > 1: 
        if options.all_parallel:
            all_ok = run_all_tests_in_parallel(test_dirs)
        else:
            all_ok = run_test_dirs_in_parallel(test_dirs)
    else:
        for test_dir in test_dirs.keys():
            all_ok = process_test_dir_seq(test_dir, test_dirs[test_dir]) and all_ok

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


    
