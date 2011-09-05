#!/usr/bin/env python
#
# Tests the tcecc-spu by compiling code from sources and running them
# in the PS3 via ssh. The ip of the PS3 must be set to PS3_IP_ADDR and
# the ssh connection must be authenticated without a password query to make 
# the binary execution work. In addition, requires spu-gcc cross compilation
# toolchain to be found in the PATH.

import sys
import os
import glob
import commands
import tempfile

from optparse import OptionParser
from difflib import unified_diff

compiler = "../../../../tce/src/bintools/Compiler/tcecc-spu"
tempfiles = set()
options = None
args = None

def run_command(command, echoStdout=False, echoStderr=False, echoCmd=False, stdoutFD=None, stderrFD=None):
    """Runs the given shell command and returns its exit code.

    If echoOutput is False, stdout and stderr are redirected to /dev/null."""
    
    from subprocess import Popen, PIPE
   
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
    return process.wait()

def create_temp_file(suffix=""):
    tf = tempfile.mkstemp(suffix=suffix)
    tempfiles.add(tf[1])
    return tf

def cleanup_and_exit(retval=0):
    for tf in tempfiles:
        os.unlink(tf)
    sys.exit(retval)

class TestCase(object):
    def __init__(self, root_dir):
        self.root_dir = root_dir
        correct_stdout_fn = os.path.join(self.root_dir, "correct.stdout")
        if os.path.exists(correct_stdout_fn):
            self.correct_stdout = open(correct_stdout_fn).readlines()
        else:
            self.correct_stdout = []

        correct_stderr_fn = os.path.join(self.root_dir, "correct.stderr")
        if os.path.exists(correct_stderr_fn):
            self.correct_stderr = open(correct_stderr_fn).readlines()            
        else:
            self.correct_stderr = []

        self.program_bin = "tcecc-spu-testprog"

    def execute(self):
        if self.compile():
            return self.verify()
        else:
            return False

    def compile(self):
        wild_card = os.path.join(self.root_dir, "src", "*.c")
        c_files = glob.glob(wild_card)
        if len(c_files) == 0: 
            print >> sys.stderr, "Illegal test directory. No .c files in", self.root_dir + "/src."
            return False

        if options.verbose: 
            verbose_switch = "-v"
        else:
            verbose_switch = ""

        exitCode = run_command(compiler + " " + verbose_switch + " " + " ".join(c_files) + " -o " + \
                                   self.program_bin, echoStdout=True, echoCmd=options.verbose)
        if exitCode != 0:
            print >> sys.stderr, "!!! %s: compilation failed" % self.root_dir
            return False
        else:
            tempfiles.add(self.program_bin)
            return True
        

    def verify(self):
        ps3_ip = os.environ['PS3_IP_ADDR']
        stdoutTemp = create_temp_file(".stdout")
        stderrTemp = create_temp_file(".stderr")
        exitcode = run_command("scp -q %s %s:~/ ; ssh %s ./%s" %
                               (self.program_bin, ps3_ip, ps3_ip, self.program_bin),
                               echoStdout=True, echoStderr=True, echoCmd=options.verbose,
                               stdoutFD=stdoutTemp[0], stderrFD=stderrTemp[0])

        if options.dump_output:
            sys.stderr.write(open(stderrTemp[1]).read())
            sys.stdout.write(open(stdoutTemp[1]).read())
            return True

        gotStdout = open(stdoutTemp[1]).readlines()
        gotStderr = open(stderrTemp[1]).readlines()


        stdoutDiff = list(unified_diff(gotStdout, self.correct_stdout,  
                                       fromfile="produced.stdout", tofile="expected.stdout"))

        all_ok = True

        if len(stdoutDiff) > 0:
            print >> sys.stderr, "!!! %s: verification failed. stdout diff:" % self.root_dir
            for line in stdoutDiff:
                sys.stderr.write(line)
            all_ok = False

        stderrDiff = list(unified_diff(gotStderr, self.correct_stderr,  
                                       fromfile="produced.stderr", tofile="expected.stderr"))

        if len(stderrDiff) > 0:
            print >> sys.stderr, "!!! %s: verification failed. stderr diff:" % self.root_dir
            for line in stdoutDiff:
                sys.stderr.write(line)
            all_ok = False

        if not all_ok:
            return False
        else:
            if options.print_success:
                print >> sys.stdout, "### %s: OK." % self.root_dir
            return True


def get_subdirectories(root):
    "Walk does not follow symbolic links. So here's replacement."
    found_subdirs = [root]
    for subdir in os.listdir(root):
        full_path = os.path.join(root, subdir)
        if os.path.isdir(full_path):
            found_subdirs += get_subdirectories(os.path.join(root, subdir))
            # TODO: check that same directory is not found many times in list
            #       (loop detection)
            
    return found_subdirs    

def find_test_cases(root):
    subdirs = get_subdirectories(root)
    test_cases = []
    for subdir in subdirs:
        if os.path.exists(os.path.join(subdir, 'src')):
            test_cases.append(TestCase(subdir))
    return test_cases

def parse_options():
    parser = OptionParser(usage="usage: %prog [options] tests_root_dir")
    parser.add_option("-s", "--print-successful", dest="print_success", action="store_true",
                      help="Print successful tests also. Default is to not print anything in case of an OK run.", 
                      default=False)
    parser.add_option("-d", "--dump-output", dest="dump_output", action="store_true",
                      help="Compiles the test case(s), runs it and dumps the stdout and stderr of "
                      "the verification run and exists. Useful for creating " + \
                          "the initial test case verification files.", default=False)                     

    parser.add_option("-v", "--verbose", dest="verbose", action="store_true",
                      help="Verbose output. Output the commands executed etc.", default=False)
    (options, args) = parser.parse_args()
    if len(args) == 0:
        parser.print_help()
        sys.exit(0)

    return (options, args)

if __name__ == "__main__":
    options, args = parse_options()
    test_cases = find_test_cases(args[0])
    all_ok = True
    for test_case in test_cases:
        all_ok = test_case.execute() and all_ok
    if not all_ok: 
        cleanup_and_exit(1)
    else: 
        cleanup_and_exit(0)


    
