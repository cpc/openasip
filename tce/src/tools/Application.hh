/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file Application.hh
 *
 * Declaration of Application class and services of standalone applications.
 *
 * Application is a class for generic services that are project-wide
 * applicable to standalone applications or modules. These services include
 * assertion, program exiting, debugging to a log file, catching unexpected
 * exceptions, "control-c" signal handling.
 *
 * @author Atte Oksman 2003 (oksman@cs.tut.fi)
 * @author Pekka Jääskeläinen 2005 (pjaaskel@cs.tut.fi)
 */

#ifndef TTA_APPLICATION_HH
#define TTA_APPLICATION_HH

#include <cstdlib>
#include <string>
#include <iostream>
#include <vector>
#include <signal.h>

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
#define assert(condition) \
    if (!(condition)) { \
        abortWithError("Assertion failed: " #condition); \
    }

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
class Application {
public:
    static void initialize();
    static void finalize();

    static void writeToErrorLog(
        const std::string fileName,
        const int lineNumber,
        const std::string functionName,
        const std::string message,
        const int neededVerbosity = 0);

    static void exitProgram(const int status = EXIT_SUCCESS);
    static void abortProgram();

    static void unexpectedExceptionHandler();

    static int runShellCommandAndGetOutput(
	const std::string& command,
	std::vector<std::string>& outputLines,
	std::size_t maxOutputLines  = DEFAULT_MAX_OUTPUT_LINES);

    static std::ostream& logStream();

    static int verboseLevel() {return verboseLevel_;}
    static void setVerboseLevel(const int level = VERBOSE_LEVEL_DEFAULT)
        {verboseLevel_ = level;}

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

    static void setCtrlcHandler(UnixSignalHandler& handler);
    static void restoreCtrlcHandler();
    static void setFpeHandler(UnixSignalHandler& handler);
    static void restoreFpeHandler();


    /// Default verbose level - do not print anything unnecessary
    static const int VERBOSE_LEVEL_DEFAULT = 0;
    /// Increased verbose level - print information about modules etc
    static const int VERBOSE_LEVEL_INCREASED = 1;

private:
    static void ctrlcSignalRedirector(int data, siginfo_t *info, void *context);
    static void fpeSignalRedirector(int data, siginfo_t *info, void *context);
    /// True when initialize() is called. Ensures that initialization is
    /// done only once.
    static bool initialized_;

    /// The stream for error logging.
    static std::ostream* logStream_;

    ///* @note if more signals need to be handled, refactor the following into a
    /// std::map<int, UnixSignalHandler*> or similar.
    
    /// The handler for signal produced by user pressing ctrl-c.
    static UnixSignalHandler* ctrlcHandler_;

    /// The handler for signal SIGFPE.
    static UnixSignalHandler* fpeHandler_;

    /// Verbose level directs how much output will be printed to console
    static int verboseLevel_;

};

#endif
