/*
    Copyright (c) 2002-2012 Tampere University.

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
 */
/**
 * @file Application.hh
 *
 * Declaration of Application class and services of standalone applications.
 *
 * Application is a class for generic services that are project-wide
 * applicable to standalone applications or modules. These services include
 * assertion, program exiting, debugging to a log file, catching unexpected
 * exceptions and "control-c", SIGFPE and SIGSEGV signal handling.
 *
 * @author Atte Oksman 2003 (oksman-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2005-2012 (pjaaskel-no.spam-cs.tut.fi)
 */

#ifndef TTA_APPLICATION_HH
#define TTA_APPLICATION_HH

#include <cstdlib>
#include <string>
#include <iostream>
#include <vector>
#ifndef __MINGW32__
# include <signal.h>
#else
// provide a dummy type to allow dummy signal functionality
typedef int siginfo_t;

#endif
#include <map>

#ifdef assert
#undef assert
#endif

#define UNKNOWN_FUNCTION "[?]"

// taken from
// http://gcc.gnu.org/onlinedocs/gcc-3.4.1/gcc/Function-Names.html#
// Function%20Names
#if __STDC_VERSION__ < 199901L
# if __GNUC__ >= 2
#  define __func__ __PRETTY_FUNCTION__
# else
#  define __func__ UNKNOWN_FUNCTION
# endif
#endif

// abort the program with an error message
#define abortWithError(message) \
    Application::writeToErrorLog(__FILE__, __LINE__, __func__, message); \
    Application::abortProgram();

// provide a TCE version of assert macro for error logging
#ifdef NDEBUG
#define assert(condition)
#else
#define assert(condition) \
    if (!(condition)) { \
        abortWithError("Assertion failed: " #condition); \
    }
#endif

// provide an easy way to "debug print"
#define debugLog(text) \
    Application::writeToErrorLog(__FILE__, __LINE__, __func__, \
        std::string("DEBUG: ") + std::string(text))

// provide an easy way to print out exception data
#define CATCH_ANY(XXX__) \
    try { XXX__; } \
    catch (const Exception& e) { \
   debugLog(e.errorMessage() + " in " + \
	    e.fileName() + ":" + \
	    e.procedureName() + ":" + \
	    Conversion::toString(e.lineNum())); } \
    catch ( ... ) { debugLog("Unknown exception"); }

// easy way to do conditional verbose logging
#define verboseLogC(text, neededVerbosity) \
    if (Application::verboseLevel() >= neededVerbosity) { \
       Application::logStream() << text << std::endl; }

// provide an easy way to verbose log printing
#define verboseLog(text) verboseLogC(text, 1)

// provide an easy way to print out the contents of a variable
#define PRINT_VAR(VARIABLE__) \
    Application::logStream() << #VARIABLE__ << " == " \
    << VARIABLE__ << std::endl

/// default value of maximum amount of output lines saved from popen() output
/// in runShellCommandAndGetOutput()
const int DEFAULT_MAX_OUTPUT_LINES = 200;

/// maximum length of an output line saved from popen() output in
/// runShellCommandAndGetOutput()
const int MAX_OUTPUT_LINE_LENGTH = 512;

/**
 * A project-wide class for basic application services.
 *
 * Provides application services such as program exit and abort. It also
 * provides generic debugging aids like error logging and handling of
 * unexpected exceptions.
 *
 * Initialization of this toolkit happens when any function is called the
 * first time.
 *
 * Should be used by every standalone application of the toolset.
 */
class CmdLineOptions;

class Application {
public:
	static void initialize(int argc, char* argv[]);
    static void initialize();
    static void finalize();

    static void writeToErrorLog(
        const std::string fileName,
        const int lineNumber,
        const std::string functionName,
        const std::string message,
        const int neededVerbosity = 0);

    static void exitProgram(const int status = EXIT_SUCCESS);
    static void abortProgram() __attribute__((noreturn));

    static void unexpectedExceptionHandler();

    static int runShellCommandAndGetOutput(
        const std::string& command,
        std::vector<std::string>& outputLines,
        std::size_t maxOutputLines  = DEFAULT_MAX_OUTPUT_LINES);

    static std::ostream& logStream();
    static std::ostream& warningStream();
    static std::ostream& errorStream();

    static int verboseLevel() {return verboseLevel_;}
    static void setVerboseLevel(const int level = VERBOSE_LEVEL_DEFAULT)
        {verboseLevel_ = level;}

    static void setCmdLineOptions(CmdLineOptions* options_);
    static CmdLineOptions* cmdLineOptions();
    static int argc() { return argc_; }
    static char** argv() { return argv_; }
    static bool isInstalled();
    static std::string installationDir();

    /**
     * An interface for classes that can receive notification when a Unix
     * signal occurs.
     */
    class UnixSignalHandler {
    public:
        /**
         * Executed when the Unix signal occurs.
         *
         * @param data Data of the signal.
         */
        virtual void execute(int data, siginfo_t *info) = 0;
        virtual ~UnixSignalHandler() {}
    };
    
    // Unix signal handler set/reset functions
    static void setSignalHandler(int signalNum, UnixSignalHandler& handler);
    static UnixSignalHandler* getSignalHandler(int signalNum);
    static void restoreSignalHandler(int signalNum);

    static std::string TCEVersionString();

    /// Default verbose level - do not print anything unnecessary
    static const int VERBOSE_LEVEL_DEFAULT = 0;
    /// Increased verbose level - print information about modules etc
    static const int VERBOSE_LEVEL_INCREASED = 1;

private:
    static void signalRedirector(int data, siginfo_t *info, void *context);

    /// True when initialize() is called. Ensures that initialization is
    /// done only once.
    static bool initialized_;

    /// The stream for debug logging.
    static std::ostream* logStream_;

    /// The stream for user error notifications.
    static std::ostream* errorStream_;

    /// The stream for user error notifications.
    static std::ostream* warningStream_;

    /// Signal handlers in a map associated by their signal numbers
    static std::map<int, UnixSignalHandler*> signalHandlers_;

    /// Verbose level directs how much output will be printed to console
    static int verboseLevel_;

    /// Holds command line options passed to program
    static CmdLineOptions* cmdLineOptions_;

    /// The original argc and argv given to the main program, if applicable.
    static int argc_;
    static char** argv_;

    /// Path to the TCE installation root
    static std::string installationRoot_;
};

#endif
