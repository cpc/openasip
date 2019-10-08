#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright (c) 2002-2010 Tampere University.
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
# @author 2006-2010 Pekka Jääskeläinen
#

import getopt, sys, os, glob, __builtin__, subprocess, time, signal, csv, tempfile, math

from os import *
from subprocess import *
from difflib import unified_diff
from pprint import pformat


# General settings.

# How long the scheduling can run without getting killed?
schedulingTimeoutSec = 100*60

# How long the simulation can run without getting killed?
simulationTimeoutSec = 120*60

def usage():
    print "Usage: scheduler_testbench.py [options]"
    print """
Options:
  -a <ADF> Override architecture with which to run tests
  -b <list of test case directories to include> The matching is done from the
     right of the test case pathname,
     for example, 'convolution,complex_multiply' matches both
     'DSPstone/fixed_point/{convolution,complex_multiply}' and
     'DSPstone/floating_point/{convolution,complex_multiply}'.
  -C Output in format suitable for copy-pasting to a spreadsheet (CSV).
  -d Leave the test directory dirty after a failed test (for inspecting
     what went wrong, etc.), that is, don't clean the produced files.
  -e The root directory from which to find the test cases.
  -g <optimization-switches> Checks if tpef or bc generated from source
     code is correct. Compiles generated_program.bc when used with -x
     generated_seq_program otherwise.
     e.g scheduler_tester.py -x -g \"-O3\"
  -h This help text.
  -i <comma separated list of stats>. Print more statistics for each run. 
     Stats available:
     c=cycle count, rr=register reads, rw=register writes, oc=operation count,
     opc=ops/cycle, and 'all' which includes all stats. Example: 'rr,rw'
     prints register read and write stats.
  -l Output as LaTeX table.
  -L Loosen the topresults. Set the worsened result as the new topresult to
     compare the future runs against but do not touch the results that
     improved.
  -o Delete the symbolic link 'data' pointing to the Operations directory
     containing the OSAL operations needed by the tests after the test
     cases have been executed.
  -p Do not delete the parallel programs from scheduling after simulation.
  -q Use compiled simulation (slow initialization, fast simulation, basic
     block simulation granularity).
  -r Regression test mode. Do not output anything unless there's an error, in
     which case output as normal.  
  -s Stop testing after the first FAILED test encountered.
  -t Update top results (only the improved cycle counts) if all tests passed.
  -T Update top results (even the worsened ones) if all tests passed, thus
     make this run the "baseline" to which future runs are compared against.
  -v Verbose output. Print error messages from scheduler, etc.
  -V Even more verbose. Print all commands executed.
  -w [limit] Consider worsened results to be an error (when -r is used) in
     case the average worsening (in percentages) is greater than the given limit.

  ----------------------------------------------------------------------------
  Configuration files in testpath:
  ----------------------------------------------------------------------------
  architectures.lst    An exclusive listing of ADF files to compile the test
                       case against. If this file is found in the test
                       directory, -a switch is ignored and only the ADFs
                       listed are used for benchmarking.
  description.txt      Description that is shown when test is runned -v
  disable.txt          It test is disabled for specific reason this file can
                       be created with reason message.
  program.bc           LLVM byte code file is test should be ran when
                       -x switch *is* set.		       
  sequential_program   Universal tpef binary, if wanted that test is ran
                       when -x switch *is not* set.
  simulate.ttasim      Lines to feed into ttasim to simulate and dump
                       verification output. If exists, no run command is given
    	               by the tester but only the commands from this file
                       are executed.
  extraCompileFlags    Adds contents of this file as extra parameters to tcecc.               
  ----------------------------------------------------------------------------
"""

# Location of the scheduler and simulator programs relative to the location
# of this script.

schedulerExe = "../../src/bintools/Scheduler/schedule"
tceccExe = "../../src/bintools/Compiler/tcecc"
simulatorExe = "../../src/codesign/ttasim/ttasim"

# Access the options globally from everywhere.
rootDir = os.path.dirname(os.path.abspath(sys.argv[0]))
ADFDir = os.path.normpath(rootDir + "/ADF")
operationDir = os.path.normpath(rootDir + "/Operations")
schedulerExe = os.path.normpath(rootDir + "/" + schedulerExe)
simulatorExe = os.path.normpath(rootDir + "/" + simulatorExe)
tceccExe = os.path.normpath(rootDir + "/" + tceccExe)
testRootDir = "."

backendCacheDir = None
def get_backend_cache_dir():    
    """Returns the location of the backend plugin directory.

    Creates a new temporary directory, if there's not one already."""
    global backendCacheDir
    if backendCacheDir is not None: return backendCacheDir

    if os.environ.get('TCE_ST_REUSE_OLD_BACKENDS', '0') == '1':
        # In case we know that there has not been changes to the
        # backend plugin generation routines, we can use the common
        # plugin cache dir to speed up compilation by setting the
        # env to 1.
        backendCacheDir = ''
    else:
        backendCacheDir = tempfile.mkdtemp(prefix="scheduler_tester-")
    return backendCacheDir

# Find the ADF and Operations directory in the same directory
# the script is at.
fileListing = glob.glob(ADFDir + "/*.adf")
allArchitectures = map(os.path.basename, fileListing)

stopTestingAfterFailingTest = False
csvFormat = False
latexTable = False
normalOutput = True
moreStats = None
topStatsUpdates = False
baselineUpdate = False
verboseOutput = False
veryVerboseOutput = False
saveParallelPrograms = False
deleteOSALLink = False
outputOnlyIfFailure = False
leaveDirty = False
testCaseFilters = None
compiledSimulation = False
loosenResults = False

# How large can the average worsening be without it being
# an error, thus affect the result of -r
worsenedIsErrorLimit = None

#if needed to compile simulated program
extraCompileFlags = ""
recompile = False
makeCommand = "SCHEDULER_BENCHMARK_TEST_MAKEFILE_DEFS=" + \
              rootDir + "/compile_sources.make" + " make -s "

# List of architectures given in command line.
cmdLineArchitectures = []

failureFound = False

if len(allArchitectures) == 0:
    print "No architecture files found in " + ADFDir
    sys.exit(3)    

def ParseCommandLine():

    global stopTestingAfterFailingTest, csvFormat, \
           topStatsUpdates, baselineUpdate, verboseOutput, veryVerboseOutput, \
           saveParallelPrograms, \
           deleteOSALLink, outputOnlyIfFailure, architectures, \
           recompile, leaveDirty, latexTable, moreStats, \
           normalOutput, testCaseFilters, \
           extraCompileFlags, compiledSimulation, worsenedIsErrorLimit,\
           loosenResults, testRootDir

    try:
        args_start = 1
            
        opts, args = getopt.getopt(\
            sys.argv[args_start:], "g:a:b:shtTvVCopqrxw:dlLi:e:", ["help"])

    except getopt.GetoptError, e:
        # print help information and exit:
        print str(e)
        sys.exit(1)

    for o, a in opts:
        if o in ("-h", "--help"):
            usage()
            sys.exit()
        elif o == "-s":
            stopTestingAfterFailingTest = True
        elif o == "-t":
            topStatsUpdates = True
        elif o == "-T":
            baselineUpdate = True
        elif o == "-v":
            verboseOutput = True
        elif o == "-V":
            verboseOutput = True
            veryVerboseOutput = True            
        elif o == "-C":
            csvFormat = True
        elif o == "-p":
            saveParallelPrograms = True
        elif o == "-o":
            deleteOSALLink = True
        elif o == "-r":
            outputOnlyIfFailure = True
            # Write everything to a tmpfile and in case of an error only,
            # copy everything from that tmpfile to stdout at the end of
            # execution.
            sys.stdout = tmpfile()
        elif o == "-a":
            for arch in a.split(','):
                cmdLineArchitectures.append(arch)
        elif o == "-g":
            extraCompileFlags = a
            recompile = True
        elif o == "-d":
            leaveDirty = True
        elif o == '-l':
            latexTable = True
        elif o == '-L':
            loosenResults = True
        elif o == '-q':
            compiledSimulation = True
        elif o == '-i':
            if a == 'all':
                moreStats = 'c,rr,rw,oc,opc'.split(',')
            else:
                moreStats = a.split(',')
        elif o == '-b':
            testCaseFilters = a.split(',')
        elif o == '-w':
            worsenedIsErrorLimit = float(a)
        elif o == '-e':
            testRootDir = a
        else:
            usage()
            sys.exit(1)

    if args:
        usage()
        sys.exit(1)

    def file_readable(filename):
        return access(filename, R_OK)
               
    normalOutput = not latexTable and not csvFormat
        
def runWithTimeout(command, timeoutSecs, inputStream = ""):
    """
    Runs the given process until it exits or the given time out is reached.

    Returns a triplet of which first value tells whether exited without timeout,
    second gives the process' output from stdout as a string, third the stderr
    """
    global veryVerboseOutput
    
    timePassed = 0.0
    increment = 0.01
    
    stderrFD, errFile = tempfile.mkstemp()
    stdoutFD, outFile = tempfile.mkstemp()

    if veryVerboseOutput:
        print "running: %s input-stream: %s" % (command,inputStream)

    process =  Popen(command, shell=True, stdin=PIPE, stdout=stdoutFD, stderr=stderrFD, close_fds=False)

    if process == None:
        print "Could not create process"
        sys.exit(1)

    try:
        if inputStream != "":
            for line in inputStream:
                process.stdin.write(line)
                process.stdin.flush()

        while True:
            status = process.poll()
            if status != None:
                # Process terminated succesfully.            
                stdoutSize = os.lseek(stdoutFD, 0, 2)
                stderrSize = os.lseek(stderrFD, 0, 2)

                os.lseek(stdoutFD, 0, 0)
                os.lseek(stderrFD, 0, 0)

                stdoutContents = os.read(stdoutFD, stdoutSize)
                stderrContents = os.read(stderrFD, stderrSize)

                os.close(stdoutFD)
                os.remove(outFile)
                os.close(stderrFD)
                os.remove(errFile)

                return (True, stdoutContents, stderrContents)
        
            if timePassed < timeoutSecs:
                time.sleep(increment)
                timePassed = timePassed + increment

            else:
                # Simulation time out, kill the simulated process.
                stdoutSize = os.lseek(stdoutFD, 0, 2)
                stderrSize = os.lseek(stderrFD, 0, 2)

                os.lseek(stdoutFD, 0, 0)
                os.lseek(stderrFD, 0, 0)

                stdoutContents = os.read(stdoutFD, stdoutSize)
                stderrContents = os.read(stderrFD, stderrSize)

                os.close(stdoutFD)
                os.remove(outFile)
                os.close(stderrFD)
                os.remove(errFile)
            
                os.kill(process.pid, signal.SIGTSTP)
            
                return (False, stdoutContents, stderrContents)
    except KeyboardInterrupt:
        os.kill(process.pid, signal.SIGTSTP)
        raise
    except:
        # if something threw exception (e.g. ctrl-c)
        os.kill(process.pid, signal.SIGTSTP)

        try:
            # Simulation time out, kill the simulated process.
            stdoutSize = os.lseek(stdoutFD, 0, 2)
            stderrSize = os.lseek(stderrFD, 0, 2)

            os.lseek(stdoutFD, 0, 0)
            os.lseek(stderrFD, 0, 0)

            stdoutContents = os.read(stdoutFD, stdoutSize)
            stderrContents = os.read(stderrFD, stderrSize)

            os.close(stdoutFD)
            os.remove(outFile)
            os.close(stderrFD)
            os.remove(errFile)
        except:
            pass
        
        return (False, stdoutContents, stderrContents)

def callSilent(command):
    schedProc = Popen(command, shell=True, stdin=None, stdout=None, stderr=None, close_fds=True)
    schedProc.communicate()
    return schedProc.returncode
        
def tryRemove(filename):
    try:
        os.remove(filename)
    except:
        pass

class TestBenchException(RuntimeError):
    def __init__(self, msg):
        self.msg = msg
        
    def getMsg(self):
        return self.msg
    
class TestRunResult:
    """Stores data of a single test run.

    The test run is executed for a pair test case and a test architecture.
    """
    def __init__(self):
        self.cycleCount = -1
        self.verificationOK = False

class SimulationStats:
    def __init__(self):
        self.cycleCount = 0
        self.registerReads = 0
        self.registerWrites = 0
        self.operationExecutions = 0
    
    def decodeStatString(self, stat):
        """
        Interprets a statistics string given in a list of '-i' switch.
        
        Returns a tuple with first column a long name for statistic,
        second column a short name (can be equal to stat), and third
        column the value of the statistics.
        """
        if stat == 'c':
            return ('cycle count', 'cycles', '%.0f' % self.cycleCount)
        if stat == 'rr':
            return ('register reads', 'reg r', '%.0f' % self.registerReads)
        elif stat == 'rw':
            return ('register writes', 'reg w', '%.0f' % self.registerWrites)
        elif stat == 'oc':
            return ('operations executed', 'o', '%.0f' % self.operationExecutions)
        elif stat == 'opc':
            value = 0.0
            try:
                value = self.operationExecutions / self.cycleCount
            except:
                pass
            return ('operation per cycle', 'opc', '%.2f' % value)
        else:
            print 'unknown statistics type:',stat
            return (None, None, None)
    
    def toLatexRow(self, statColumns, colWidth=16):
        """
        Generates a row of results in LaTeX table format according to the
        columns listed.
        """
        latexRow = ""
        statsPrinted = 0
        for stat in statColumns:
            name, short, value = self.decodeStatString(stat)
            latexRow += ('%s' % value).rjust(colWidth)
            statsPrinted += 1
            if statsPrinted < len(statColumns):
                latexRow += ' &'
        return latexRow

class TestCase:
    """Represents a single test case.

    The data is loaded from a single test case directory. The test case
    includes the architectures the test program is compiled and simulated
    against. Is able to run the tests and store results.
    """

    def loadOldResults(self):
        if not access(self.directory + "/topresults.csv", R_OK):
            self.oldResults = None
            return
        reader = csv.reader(__builtin__.open(self.directory + "/topresults.csv", "rb"))
        self.oldResults = {}
        for row in reader:
            self.oldResults[row[0]] = row[1:]
        
    def __init__(self, directory):

        """Loads a new test case stored in the given directory."""        

        global cmdLineArchitectures
        global extraCompileFlags

        self.testExtraCompileFlags = extraCompileFlags
        self.description = ""
        self.architectures = []
        self.directory = directory
        if directory.startswith('./'):
            self.title = directory[2:]
        else:
            self.title = directory

        descriptionFile = __builtin__.open(directory + '/description.txt', 'r')
        self.description = descriptionFile.read().strip()
        descriptionFile.close()

        if os.access(directory + '/architectures.lst', R_OK):
            archFile = __builtin__.open(directory + '/architectures.lst')
            for line in archFile.read().splitlines():
                arch = line.strip()
                if arch != '':
                    self.architectures.append(arch)
            archFile.close()
        elif len(cmdLineArchitectures) > 0:
            self.architectures = cmdLineArchitectures            
        else:
            self.architectures = allArchitectures

        if os.access(directory + '/extraCompileFlags', R_OK):
            flagsFile = __builtin__.open(directory + '/extraCompileFlags')
            for line in flagsFile.read().splitlines():
                param = line.strip()
                if param != '':
                    self.testExtraCompileFlags += ' ';
                    self.testExtraCompileFlags += param;
                    self.testExtraCompileFlags += ' ';

        self.improvedRuns = False
        # Simulation results for each architecture (the verification data and the 
        # cycle count).
        self.results = {}      
        # Simulation stats for each architecture.
        self.stats = {}  
        self.loadOldResults()
        self.setupExecuted = False
        self.parallelPrograms = []
        
        # If requested. stores the results for each architecture in LaTeX format
        self.latexResults = {}
        self.latexColumnCount = 0

        self.seqCycleCount = -1

    def setupTestDirectory(self):
        if os.path.lexists("data") and os.readlink("data") != operationDir:
            tryRemove("data")
        if os.path.lexists("src/data") and os.readlink("src/data") != operationDir:
            tryRemove("src/data")

        try:
            os.symlink(operationDir, "data")
        except:
            pass

        try:
            os.symlink(operationDir, "src/data")
        except:
            pass
        
        tryRemove("cyclecount")
        tryRemove("generated_seq_program")
        tryRemove("src/generated_seq_program")
        tryRemove("generated_program.be.bc")
        tryRemove("generated_program.le.bc")
        tryRemove("src/generated_program.le.bc")
        tryRemove("src/generated_program.be.bc")


    def cleanupTestDirectory(self):
        global saveParallelPrograms, deleteOSALLink, leaveDirty

        if deleteOSALLink:
            tryRemove("data")
            
        tryRemove("cyclecount")
        tryRemove("ttasim.out")
        tryRemove("generated_seq_program")
        tryRemove("src/generated_seq_program")
        tryRemove("generated_program.be.bc")
        tryRemove("generated_program.le.bc")
        tryRemove("src/generated_program.be.bc")
        tryRemove("src/generated_program.le.bc")
        tryRemove("operations_executed")
        tryRemove("registers_read")
        tryRemove("registers_written")

        if not saveParallelPrograms:
            for tpef in self.parallelPrograms:
                tryRemove(tpef)
                
        if access("./cleanup.sh", X_OK):
            callSilent("./cleanup.sh")

        if access("src/Makefile", R_OK):
            callSilent("cd src;" + makeCommand + " clean")

    def testFailed(self, reason, verbose=""):
        global failureFound, leaveDirty
        if csvFormat:
            sys.stdout.write("FAILED,,,\n")
        else:
            sys.stdout.write("FAILED! " + reason)
            sys.stdout.write(" (!)\n")
        
            if verboseOutput and len(verbose) > 0:
                sys.stdout.write("[" + verbose + "]\n")
            
        sys.stdout.flush()
        failureFound = True

    def verifySimulation(self):
        if access("correct_simulation_output", R_OK):
            correctOutputFile = __builtin__.open("correct_simulation_output", "r")
            correctOutput = correctOutputFile.read().strip().split("\n")
            self.simStdOut = self.simStdOut.strip().split("\n")    
	
            if correctOutput != self.simStdOut:
                # The outputs differ.
                result = list(unified_diff(correctOutput, self.simStdOut))
                catted = ""
                for line in result:
                    catted = catted + line.strip() + "\n"
                self.testFailed("verification with 'correct_simulation_output' failed",
                                "differences:\n" + catted)

                return False

        if access("./verify.sh", X_OK):
            retcode = callSilent("./verify.sh")
            if retcode != 0:
                self.testFailed("verification with 'verify.sh' failed (retcode %d)" % retcode)
                return False
                     
        return True

    def schedule(self, archFilename, seqProgFileName, dstProgFileName):

        schedulingCommand = tceccExe + ' ' + self.testExtraCompileFlags + ' '
        if get_backend_cache_dir() != '':
            schedulingCommand += "--plugin-cache-dir=" + get_backend_cache_dir() + " "

        if (leaveDirty):
            schedulingCommand += '-d '

        if (veryVerboseOutput):
            schedulingCommand += ' -v '

        schedulingCommand += ' ' + self.testExtraCompileFlags;
        schedulingCommand += " -o " + dstProgFileName + \
                             " -a " + archFilename + \
                             " " + seqProgFileName
                
                
        exitOk, stdoutContents, stderrContents = runWithTimeout(schedulingCommand, schedulingTimeoutSec)

        if not exitOk:
            self.testFailed("scheduling timeout")
            return False
        
        errmsg = stderrContents
        status = not exitOk

        if not WIFEXITED(status) or WEXITSTATUS(status) != 0 or \
               not access(dstProgFileName, R_OK):
            self.testFailed("compilation error", errmsg)
            return False
        

        if veryVerboseOutput and len(errmsg) > 0:
            sys.stdout.write("[" + errmsg + "]\n")

        return True

    def simulate(self, archFilename, progFilename):

        global compiledSimulation
        
        # Create the simulation script.
        simulationScript = ""       

        if not archFilename == "":
            simulationScript += "mach " + archFilename + "\n"

        simulationScript = simulationScript + "prog " + progFilename + "\n"

        if access("simulate.ttasim", R_OK):
            script = __builtin__.open("simulate.ttasim", "r")
            simulationScript = simulationScript + script.read()
        else:
            simulationScript = simulationScript + "until 0\n"

        tryRemove("cyclecount")
        tryRemove('operations_executed')
        tryRemove('registers_written')
        tryRemove('registers_read')
        # Create a script that creates the stat files. Do not overwrite
	    # possible existing files to allow the custom simulation 
	    # script to create a stats of its own (for example, in case of
	    # wanting to include only a part of the simulated program in the stats)

        simulationScript = simulationScript + '''
if ![file exists cyclecount] {
set cycle_file [open cyclecount w] ; puts $cycle_file "[info proc cycles]"
flush $cycle_file
close $cycle_file
}
'''
	if not compiledSimulation:
	        simulationScript += '''
		if ![file exists operations_executed] {
		set f [open operations_executed w] ; puts $f "[info stats executed_operations]"
		flush $f
		close $f
		}
		
		if ![file exists registers_written] {
		set f [open registers_written w] ; puts $f "[info stats register_writes]"
		flush $f
		close $f
		}
		
		if ![file exists registers_read] {
		set f [open registers_read w] ; puts $f "[info stats register_reads]"
		flush $f
		close $f
		}
		'''
        simulationScript = simulationScript + "quit\n"

        simulationCommand = simulatorExe

        if compiledSimulation:
            simulationCommand += " -q"

        exitOk, stdoutContents, stderrContents = runWithTimeout(simulationCommand,
                                                                simulationTimeoutSec,
                                                                simulationScript)

        if not exitOk:
            self.testFailed("simulation timeout")
            return False

        self.simStdOut = stdoutContents
        self.simStdErr = stderrContents

        verbose = ""
        if len(self.simStdOut) > 0:
            verbose = verbose + "stdout: "
            verbose = verbose + self.simStdOut

        if len(self.simStdErr) > 0:
            verbose = verbose + "stderr: "
            verbose = verbose + self.simStdErr
            self.testFailed("simulation error, stderr: " + self.simStdErr );
            return False
        
        def getStat(fileName):
            if access(fileName, R_OK):
                f = __builtin__.open(fileName, "r")
                try:
                    return float(f.read().strip())
                except:
                    pass
            else:
                return None

        self.lastStats = SimulationStats()
        self.lastStats.cycleCount = getStat('cyclecount')

        if self.lastStats.cycleCount is None:
            self.testFailed("simulation", "failed to get cycle count " + verbose)            
            return False

        if not exitOk:

            gotOutput = len(self.simStdOut) > 0 or len(self.simStdErr) > 0

            self.testFailed("simulation", verbose)            
            return False
                                                
        self.lastStats.operationExecutions = getStat('operations_executed')
        self.lastStats.registerReads = getStat('registers_read')
        self.lastStats.registerWrites = getStat('registers_written')            
        
        self.stats[archFilename] = self.lastStats
        return True

    def runWithArchitecture(self, architecture, seqProgFileBase):
        """Runs the test case with given architecture definition file.

        Returns true in case test passed.
        """

        global normalOutput
        
        archFilename = architecture
        if not access(archFilename, R_OK):
            archFilename = ADFDir + "/" + architecture

        if not access(archFilename, R_OK):
            print "Cannot find ",archFilename
            sys.exit(2)

        machineLittleEndian = "<little-endian/>" in __builtin__.open(archFilename).read()
        if machineLittleEndian:
            seqProgFile = seqProgFileBase + ".le.bc";
            if verboseOutput:
                print "Machine is little endian. using LE bc file."
        else:
            seqProgFile = seqProgFileBase + ".be.bc";
            if verboseOutput:
                print "Machine is big endian. using BE bc file."

        if csvFormat:
            sys.stdout.write(self.title + "," + architecture + ",")
            sys.stdout.flush()
        elif normalOutput:
            sys.stdout.write(architecture + ": ")
            sys.stdout.flush()

        success = True

        progFileName = ""
        if architecture.lower().endswith(".adf"):
            progFileName = architecture[:-4]
        else:
            progFileName = architecture

        progFileName = progFileName + ".tpef"

        # always write the TPEF to the current directory
        progFileName = os.getcwd() + "/" + os.path.basename(progFileName)
        
        self.parallelPrograms.append(progFileName)

        # let's try to remove file if exists..
        tryRemove(progFileName)
        
        if not self.schedule(archFilename, seqProgFile, progFileName):
            return False

        if not self.simulate(archFilename, progFileName):
            return False

        if not self.verifySimulation():
            return False

        if self.oldResults is None or not architecture in self.oldResults:
            percentage = None
            difference = None
            if normalOutput:
                sys.stdout.write("OK. cycles: %.0f (first result) (+)" % self.lastStats.cycleCount)
            elif csvFormat:
                sys.stdout.write("OK,%.0f,-,first\n"
                             % (self.lastStats.cycleCount))
                
            self.improvedRuns = True
        else:
            oldResult = int(self.oldResults[architecture][-1])
            difference = self.lastStats.cycleCount - oldResult
            percentage = float(int((difference / oldResult)*1000))/10

            if difference > 0:
                sign = "-"
            elif difference < 0:
                sign = "+"
                self.improvedRuns = True
            else:
                sign = "="

            if normalOutput:
                sys.stdout.write("OK. cycles: %.0f difference: %.0f (%.1f%%) (%s)"
                                 % (self.lastStats.cycleCount, difference, percentage, sign))
            elif csvFormat:
                sys.stdout.write("OK,%.0f,%.0f,%.1f%%\n"
                             % (self.lastStats.cycleCount, difference, percentage))

                
        if normalOutput:
            if moreStats is not None:
                for stat in moreStats:
                    name, short, value = self.lastStats.decodeStatString(stat)
                    sys.stdout.write(' %s: %s' % (short, value))
            sys.stdout.write('\n')

        self.results[architecture] = (self.lastStats.cycleCount, difference, percentage)

        sys.stdout.flush()

        return success

    def verifyCompiler(self, extraFlags):
        """Checks if test contain src directory and generates sequential program to verify.
        """

        global makeCommand, schedulingTimeoutSec, verboseOutput

        # don't run generate if there is not Makefile for test
        if access("src/Makefile", R_OK):

            seqProgramNameBe = "generated_program.be.bc"
            seqProgramNameLe = "generated_program.le.bc"
            compileRule = "llvm"
            
            command = ("cd src;" +
                       makeCommand + ' clean;' +
                       'SCHEDULER_TESTER_FLAGS="'  + extraFlags + '" ' +                       
                       makeCommand + " GCCLLVM=" + tceccExe + " " + compileRule)

            command += ";cp " + seqProgramNameBe + " .."
            command += ";cp " + seqProgramNameLe + " .."

            # copy generated_program.bc to program.bc to be able to run the
            # tests with the latest compiled binary
            # Hmm? Should not do this ever. program.bc are for precompiled
            # bitcodes!
            #command += ";cp " + seqProgramName + " ../program.bc"

            if verboseOutput:
                print "Compiling from sources:"
                print command;

            exitOk, stdoutContents, stderrContents = runWithTimeout(command, schedulingTimeoutSec)

            if not exitOk:
                self.testFailed("compiling timeout")
                return False

            errorMessage = stdoutContents + stderrContents 

            if not access(seqProgramNameBe, R_OK):
                print "Error while compiling be program\n" + errorMessage
                return False

            if not access(seqProgramNameLe, R_OK):
                print "Error while compiling le program\n" + errorMessage
                return False

        else:
            print "src dir does not contain Makefile. Cannot build from src."
            return False
        return True
        
    def run(self):
        """Runs the test case with all architectures.

        Returns true only if all tests passed.
        """
        global latexTable, recompile, normalOutput

        self.oldDir = os.getcwd()
        os.chdir(self.directory)

        self.setupTestDirectory()        

        if not self.setupExecuted and access("./setup.sh", X_OK):
            callSilent("./setup.sh")
            self.setupExecuted = True

        allPassed = True

        # LLVM bytecode from LLVM/TCE
        seqProgFileBase = "program"

        # Recompile and set names for test programs.
        if recompile:
            srcCompileOk = self.verifyCompiler(self.testExtraCompileFlags)
            allPassed = allPassed and srcCompileOk

            if srcCompileOk == False:
                self.testFailed("Source compilation failed.")
                os.chdir(self.oldDir)
                return False
            else:
                seqProgFileBase = "generated_program"
        
#        if configFileDefined:
        for arch in self.architectures:
            allPassed = self.runWithArchitecture(arch, seqProgFileBase) and allPassed
            if stopTestingAfterFailingTest and not allPassed:
                os.chdir(self.oldDir)
                return False

        if not leaveDirty:            
            self.cleanupTestDirectory()

        os.chdir(self.oldDir)        
        return allPassed

    def updateStatisticsFiles(self):
        """Updates the top execution statistics file and the last execution statistics file.

        This should be called only after running all test cases sucessfully, as it makes
        no sense to update the statistics if the algorithm fails for other test cases.
        """
        timeStamp = time.strftime("%d.%m.%y %H:%M")        
        lastRunWriter = csv.writer(__builtin__.open(self.directory + "/lastresults.csv", "w"))
        for arch in self.architectures:
            lastRunWriter.writerow([arch, timeStamp, "%.0f" %
                                    self.results[arch][0]])       

        if (topStatsUpdates and self.improvedRuns) or baselineUpdate or loosenResults:
            topStatsWriter = csv.writer(__builtin__.open(self.directory + "/topresults.csv", "w"))
            
            for arch in set(self.architectures + (self.oldResults and self.oldResults.keys() or [])):
                oldResult = None
                if self.oldResults is not None and arch in self.oldResults:
                    oldResult = self.oldResults[arch]

                # Do not discard the rows for architectures we did not test.
                if arch in self.results:
                    cycles = int(self.results[arch][0])
                    if loosenResults:
                        if oldResult is None: continue
                        # "loosen results" mode only makes some topresults worse to
                        # give headroom for worsening (useful e.g. when supporting two LLVM 
                        # versions)
                        if int(oldResult[-1]) < cycles:
                            topStatsWriter.writerow([arch, timeStamp, "%.0f" % cycles])
                        else:
                            topStatsWriter.writerow([arch] + oldResult)
                    elif oldResult is None or int(oldResult[-1]) > cycles or baselineUpdate:
                        topStatsWriter.writerow([arch, timeStamp, "%.0f" % cycles])
                    else:
                        topStatsWriter.writerow([arch] + oldResult)
                else:
                    topStatsWriter.writerow([arch] + oldResult)


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
        
class Tester:
    """
    Test suite - responsible for running all test cases and printing out
    results.
    """

    def __init__(self):
        self.testcases = []
        self.loadTestCases()

    def loadTestCases(self):
        global testCaseFilters, recompile
        
        self.testCases = []
        
        for root in get_subdirectories(testRootDir):
            
            if recompile:
                inputProg = 'src'
            else:
                inputProg = 'program.bc'
            
            if os.access(root + "/description.txt", R_OK):

                disableText = None
                if os.access(root + "/disabled.txt", R_OK):
                    disableFile = __builtin__.open(root + "/disabled.txt", "r")
                    disableText =  "Reason: " + disableFile.read()
                    disableFile.close()
                    
                    
                if disableText is None and os.access(root + "/" + inputProg, R_OK):
                    
                    if testCaseFilters is not None:
                        found = False
                        for filter in testCaseFilters:
                            if root.endswith(filter):
                                found = True
                                break
                        if not found:
                            continue
                    newCase = TestCase(root)
                
                    # Push the new test case to our test case sequence
                    self.testCases.append(newCase)

                #info about disabled tests (to not to forget fix them...)
                elif normalOutput:                    
                    if  not os.access(root + "/" + inputProg, R_OK):
                        disableText = inputProg + " not found"
                        
                    if disableText:
                        print ("Test in " + root +
                               "/description.txt is disabled with selected frontend: " + disableText)
                    else:
                        print ("Test in " + root +
                               "/description.txt is disabled with selected frontend")


                    
    def printSummary(self):
        global recompile, worsenedIsErrorLimit, failureFound
                
        improved = 0
        worsened = 0
        equal = 0
        newResults = 0
        broken = 0        
        totalCombinations = 0
        improvementSum = 0
        worseningSum = 0    
        
        for testCase in self.testCases:
            
            for arch in testCase.architectures:
                
                if not arch in testCase.results:
                    broken = broken + 1
                else:
                    (cycles, difference, percentage) = testCase.results[arch]
                    if difference == None or percentage == None:
                        newResults = newResults + 1
                    else:
                        if difference == 0:
                            equal = equal + 1
                        else:
                            if percentage < 0:
                                improved = improved + 1
                                improvementSum = improvementSum + abs(percentage)
                            else:
                                worsened = worsened + 1
                                worseningSum = worseningSum + percentage
                        
                totalCombinations = totalCombinations + 1

        sys.stdout.write("SUMMARY\n")

        if improved > 0:
            sys.stdout.write(
                "Improved schedule for %d/%d case(s). Average improvement %.1f%%.\n"
                % (improved, totalCombinations, (improvementSum / improved)))

        if equal > 0:
            sys.stdout.write(
                "Equal schedule for %d/%d case(s).\n"
                % (equal, totalCombinations))

        if worsened > 0:
            percentage = (worseningSum / worsened)
            sys.stdout.write(                
                "Worsened schedule for %d/%d case(s). Average worsening %.1f%%.\n"
                % (worsened, totalCombinations, percentage))
            if worsenedIsErrorLimit is not None and percentage > worsenedIsErrorLimit:
                # Compiler seems to have some indetermism, thus the
                # results might randomly be different than  in
                # the previous run, try to filter these out by
                # watching the average worsening only.
                failureFound = True
            

        if newResults > 0:
            sys.stdout.write(
                "Got first results for %d/%d case(s).\n" % (newResults, totalCombinations))

        if broken > 0:
            sys.stdout.write(
                "Broken schedule for %d/%d case(s). Top results not updated.\n"
                % (broken, totalCombinations))
            
    def updateStatisticsFiles(self):
        for testCase in self.testCases:
            testCase.updateStatisticsFiles()

    def initOperations(self):
        curdir = os.getcwd()
        os.chdir(operationDir)
        callSilent("make -s > /dev/null")
        os.chdir(curdir)

    def runTests(self):
        global normalOutput
        
        self.initOperations()
        
        testsDone = 0
        allSuccessful = True
        self.archs = []

        for testCase in self.testCases:

            if normalOutput:
                if len(testCase.title) > 0:
                    print testCase.title + ":"
                if len(testCase.description) > 0:
                    print testCase.description

            allSuccessful = testCase.run() and allSuccessful
            
            if stopTestingAfterFailingTest and not allSuccessful:
                return

            if normalOutput:
                print

            if len(self.archs) == 0:
                for arch in testCase.architectures:
                    self.archs.append(arch)

            if testsDone == 0:
                if latexTable:
                    self.printLatexHeader()
                
            if latexTable:
                self.printLatexRow(testCase)
                               
            testsDone += 1

        if normalOutput:
            self.printSummary()
            
        if latexTable:
            self.printLatexFooter()
        
        if allSuccessful:
            self.updateStatisticsFiles()
    
    def printLatexHeader(self, firstColumnWidth=30, valueColumnWidth=16):
        """
        Prints the LaTeX table header.
        """
        global moreStats
        cols = ''
        for i in range(0, len(moreStats)*len(self.archs)):
            cols += 'l|'
            
        sys.stdout.write('\\begin{tabular}{|l|%s} \hline\n' % cols)
        sys.stdout.write(''.ljust(firstColumnWidth))
        sys.stdout.write(' & ')
        archsPrinted = 0
        for arch in self.archs:
            archName = os.path.basename(arch)[0:-4]
            if len(moreStats) > 1:
                sys.stdout.write('\\multicolumn{%d}{l}{\\textbf{%s}}' % (len(moreStats), archName))
            else:
                sys.stdout.write('\\textbf{%s}' % archName)
            archsPrinted += 1
            if archsPrinted < len(self.archs):
                sys.stdout.write(' & ')
        
        if len(moreStats) > 1:
            # Print the column headers only if there are more than one
            # column per architecture.
            sys.stdout.write('\\\\\n')
            sys.stdout.write(''.ljust(firstColumnWidth) + ' & ')
            archsPrinted = 0
            for arch in self.archs:
                st = SimulationStats()
                statsPrinted = 0
                for stat in moreStats:
                    name, shortName, value = st.decodeStatString(stat)
                    sys.stdout.write(name.rjust(valueColumnWidth))
                    statsPrinted += 1
                    if statsPrinted < len(moreStats):
                        sys.stdout.write(' &')
                                                                
                archsPrinted += 1
                if archsPrinted < len(self.archs):
                    sys.stdout.write(' &')                       
        
        sys.stdout.write('\\\\ \hline\n')
    
    def printLatexRow(self, testCase, firstColumnWidth=30):
        """
        Prints a single row of the LaTeX table.
        """        
        global moreStats
        sys.stdout.write(os.path.basename(testCase.directory).replace('_', '\_').ljust(firstColumnWidth))
        sys.stdout.write(' & ')

        archsPrinted = 0
        for arch in self.archs:
            result = testCase.stats[arch]
            sys.stdout.write(result.toLatexRow(moreStats))
            archsPrinted += 1
            
            if archsPrinted < len(self.archs):
                # More architectures going to be appended.
                sys.stdout.write(' &')
                
        sys.stdout.write('\\\\\n')
           
    def printLatexFooter(self, testCase=None):
        """
        Prints the LaTeX table footer.
        """
        sys.stdout.write('\\hline \\end{tabular}\n')        


def cleanup_and_exit(retval):
    if backendCacheDir is not None:
        os.system("rm -fr " + backendCacheDir)
    sys.exit(retval)

def main():
    global failureFound, outputOnlyIfFailure
    ParseCommandLine()
    try:    
        testCases = Tester()
    except TestBenchException, e:
        print "Error while initializing test system: " + e.getMsg()
        cleanup_and_exit(3)

    try:
        testCases.runTests()
        if failureFound and outputOnlyIfFailure:
            sys.__stdout__.write("There were failing tests!\n")
            sys.stdout.seek(0)
            sys.__stdout__.write(sys.stdout.read())
            sys.stdout = sys.__stdout__
            cleanup_and_exit(1)
            
    except TestBenchException, e:
        print "Error while running tests: " + e.getMsg()
        cleanup_and_exit(1)
    
if __name__ == '__main__':
    main()
    cleanup_and_exit(0)

