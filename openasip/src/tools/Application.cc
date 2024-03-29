/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file Application.cc
 *
 * Implementation of Application class.
 *
 * Application is a class for generic services that are project-wide
 * applicable to standalone applications or modules. These services include
 * assertion, program exiting, debugging to a log file, catching unexpected
 * exceptions, "control-c" signal handling.
 *
 * @author Atte Oksman 2003 (oksman-no.spam-cs.tut.fi)
 * @author Pekka J��skel�inen 2005-2009 (pjaaskel-no.spam-cs.tut.fi)
 */

#include <string>
#include <iostream>
#include <fstream>
#include <cstddef>
#include <exception>
#include <cstdio>

// for backtrace printing:
#include <signal.h>

#include <sys/types.h>
// macros to evaluate exit status of pclose() (from autoconf manual)
#ifdef HAVE_SYS_WAIT_H
# include <sys/wait.h>
#endif
#ifndef WEXITSTATUS
# define WEXITSTATUS(stat_val) ((unsigned int) (stat_val) >> 8)
#endif
#ifndef WIFEXITED
# define WIFEXITED(stat_val) (((stat_val) & 255) == 0)
#endif

#include "Environment.hh"
#include "Application.hh"
#include "Exception.hh"
#include "tce_version_string.h" // automatically generated by make
#include "CmdLineOptions.hh"
#include "tce_config.h"
#include "FileSystem.hh"

using std::fgets; // cstdio
using std::exit;
using std::abort;
using std::atexit;
using std::string;
using std::cerr;
using std::cout;
using std::endl;
using std::ofstream;
using std::ios;
using std::set_unexpected;


// static member variable initializations
bool Application::initialized_ = false;
std::ostream* Application::logStream_ = NULL;
std::ostream* Application::errorStream_ = NULL;
std::ostream* Application::warningStream_ = NULL;
std::map<int, Application::UnixSignalHandler*> Application::signalHandlers_;

int Application::verboseLevel_ = Application::VERBOSE_LEVEL_DEFAULT;
CmdLineOptions* Application::cmdLineOptions_ = NULL;

int Application::argc_;
char** Application::argv_;
string Application::installationRoot_ = "";

/**
 * Initializes the state data needed by the application services.
 */
void
Application::initialize() {

    // ensure that initialization is done only once per application
    if (initialized_) {
        return;
    }

    // if this is a developer version, we can output debug messages to
    // cerr, in 'distributed version', we'll output the debug messages to
    // a file
    if (Environment::developerMode()) {
        logStream_ = &cerr;
    } else {
        ofstream* fileLog = new ofstream;
        fileLog->open(Environment::errorLogFilePath().c_str(), ios::app);
        if (!fileLog->is_open()) {
            logStream_ = &cerr;
        } else {
            logStream_ = fileLog;
        }
    }

    installationRoot_ = installationDir();

    errorStream_ = &cerr;
    warningStream_ = &cerr;

    // set the unexpected exception callback
    set_unexpected(Application::unexpectedExceptionHandler);

    // register finalization function to be called when exit() is called
    // so Application is finalized automatically on program exit
    if (atexit(Application::finalize) != 0) {
        writeToErrorLog(__FILE__, __LINE__, __FUNCTION__,
            "Application initialization failed.");
        abortProgram();
    }
 
    initialized_ = true;
}

void
Application::initialize(int argc, char* argv[]) {
    Application::initialize();
    argc_ = argc;
    argv_ = argv;
}

/**
 * Allows writing to the log stream with stream operators.
 *
 * Usage example: logStream() << "Debug output" << i << endl;
 *
 * @return A reference to the log stream.
 */
std::ostream&
Application::logStream() {
    initialize();
    return *logStream_;
}

/**
 * Stream where error messages that should be printed immediately to the user
 * should be written.
 *
 * Errors are fatal which means the action performed could not be finished.
 *
 * Usage example: errorStream() << "Compilation error: "...
 *
 * @return A reference to the error stream.
 */
std::ostream&
Application::errorStream() {
    initialize();
    return *errorStream_;
}

/**
 * Stream where warning messages that should be printed immediately to the user
 * should be written.
 *
 * Warnings are non-fatal notifications to the user. The action performed
 * might still be able to be finished.
 *
 * Usage example: warningStream() << "warning: uninitialized thing "...
 *
 * @return A reference to the warning stream.
 */
std::ostream&
Application::warningStream() {
    initialize();
    return *warningStream_;
}

/**
 * Cleans up the state data used by the application services.
 *
 * This method is called automatically when program is terminated.
 */
void
Application::finalize() {

    if (!initialized_) {
        return;
    }

    if (logStream_ != &cerr && logStream_ != &cout && logStream_ != NULL) {
        logStream_->flush();
        delete logStream_;
        logStream_ = NULL;
    }
    delete cmdLineOptions_;
    cmdLineOptions_ = NULL;
    initialized_ = false;
}

/**
 * Records a message into the error log.
 *
 * @param fileName Source file of the code where the error occurred.
 * @param lineNumber Source line where the error occurred.
 * @param functionName Function where the error occurred.
 * @param message The error message.
 */
void
Application::writeToErrorLog(
    const string fileName,
    const int lineNumber,
    const string functionName,
    const string message,
    const int neededVerbosity) {

    if (neededVerbosity > verboseLevel_) {
        return;
    }

    if (!initialized_) {
        initialize();
    }

    *logStream_ << fileName << ":" << lineNumber << ": ";

    if (functionName != UNKNOWN_FUNCTION) {
        *logStream_ <<  "In function \'" << functionName << "\': ";
    }

    *logStream_ << message << endl;
}


/**
 * Exits the program in a normal situation.
 *
 * This method must be used when the program terminates due to nominal
 * conditions without returning from main().
 *
 * @param status Program's status of exit.
 */
void
Application::exitProgram(const int status) {
    exit(status);
}

/**
 * Exit the program in an abnormal situation.
 */
void
Application::abortProgram() {
    abort();
}

/**
 * Default callback for unexpected exceptions.
 */
void
Application::unexpectedExceptionHandler() {
    *logStream_
        << std::endl
        << "Program aborted because of leaked unexpected exception. "
        << std::endl << std::endl <<
        "Information of the last thrown TCE exception: " << std::endl
        << Exception::lastExceptionInfo() << std::endl;
    abortProgram();
}

/**
 * Returns true if all commands separated by space are found.
 *
 * Otherwise return false;
 */
bool
Application::shellCommandsExists(const std::string& commands) {
    return runShellCommandSilently(std::string("type ") + commands) == 0;
}

/**
 * Runs a shell command and redirects all output to /dev/null
 *
 * @param command Command to execute.
 * @return The return value of the program. -1 if some weird error occurred.
 */
int
Application::runShellCommandSilently(
    const std::string& command) {

    char line[MAX_OUTPUT_LINE_LENGTH];
    // flush all streams to avoid: "...the output from a
    // command opened for writing may become intermingled with that of
    // the original process." (man popen)
    fflush(NULL);

    std::string fullCommand = command + " 2>&1 ";
    FILE* pipe = popen(fullCommand.c_str(), "r");

    while (fgets(line, MAX_OUTPUT_LINE_LENGTH, pipe) == line) {
        // Drain stdout and stderr data.
    }

    int exitStatus = pclose(pipe);

    // see man wait4 for info about macros WIFEXITED and WEXITSTATUS
    if (WIFEXITED(exitStatus)) {
        return WEXITSTATUS(exitStatus);
    }

    return -1;
}

/**
 * Runs a shell command and captures its output (stdout) in the given vector.
 *
 * Assumes that the executed program does not block and wait for input.
 *
 * @param command Command to execute.
 * @param outputLines Vector to which put the output lines.
 * @param maxOutputLines Maximum lines to capture.
 * @return The return value of the program. -1 if some weird error occured.
 */
int
Application::runShellCommandAndGetOutput(
    const std::string& command,
    std::vector<std::string>& outputLines,
    std::size_t maxOutputLines,
    bool includeStdErr) {

    char line[MAX_OUTPUT_LINE_LENGTH];

    // flush all streams to avoid: "...the output from a
    // command opened for writing may become intermingled with that of
    // the original process." (man popen)
    fflush(NULL);

    string shellCommand = command;
    if (includeStdErr) {
        shellCommand += " 2>1";
    }
    FILE* pipe = popen(shellCommand.c_str(), "r");

    while (fgets(line, MAX_OUTPUT_LINE_LENGTH, pipe) == line) {

        if (outputLines.size() < maxOutputLines) {
            outputLines.push_back(string(line));
        }
    }

    int exitStatus = pclose(pipe);

    // see man wait4 for info about macros WIFEXITED and WEXITSTATUS
    if (WIFEXITED(exitStatus)) {
        return WEXITSTATUS(exitStatus);
    }

    return -1;
}

/**
 * Set the command line options instance for the application.
 *
 * The object becomes owned by Application class. It should be
 * fully built (parsed) before passed to Application.
 */
void
Application::setCmdLineOptions(CmdLineOptions* options) {

    // delete the possible old instance first
    delete cmdLineOptions_;
    cmdLineOptions_ = options;

    if (options->isVerboseSwitchDefined()) {
        setVerboseLevel(Application::VERBOSE_LEVEL_INCREASED);
    }

    if (options->isVerboseSpamSwitchDefined()) {
        setVerboseLevel(Application::VERBOSE_LEVEL_SPAM);
    }
}

CmdLineOptions*
Application::cmdLineOptions() {
    return cmdLineOptions_;
}

/**
 * Sets a new signal handler for the given signal
 *
 * Note in platforms which do not support signals (e.g. Windows),
 * this function does nothing.
 *
 * @param signalNum signal number
 * @param handler The handler to be set
 */
#ifdef __MINGW32__
void
Application::setSignalHandler(int, UnixSignalHandler&) {
    return;
}
#else
void
Application::setSignalHandler(int signalNum, UnixSignalHandler& handler) {
    signalHandlers_[signalNum] = &handler;
    
    struct sigaction action;
    action.sa_flags = SA_SIGINFO;
    action.sa_sigaction = signalRedirector;
    // valgrind complains if this is uninitialized
    sigemptyset(&action.sa_mask);
    sigaction(signalNum, &action, NULL);
}
#endif

/**
 * Returns a pointer to the signal's current handler
 *
 * In platforms which do not support signals this method always
 * returns NULL.
 *
 * @return a pointer to the signal's current handler
 * @exception InstanceNotFound if the signal has not been set a custom handler
 */
#ifdef __MINGW32__
Application::UnixSignalHandler*
Application::getSignalHandler(int) {
    return NULL;
}
#else
Application::UnixSignalHandler*
Application::getSignalHandler(int signalNum) {
    std::map<int, UnixSignalHandler*>::iterator it =
        signalHandlers_.find(signalNum);
    if (it != signalHandlers_.end()) {
        return it->second;
    } else {
        throw InstanceNotFound(__FILE__, __LINE__, __FUNCTION__);
    }
}
#endif

/**
 * Restores to the signal its original handler
 *
 * Note in platforms which do not support signals (e.g. Windows),
 * this function does nothing.
 *
 * @param signalNum signal number
 */
#ifdef __MINGW32__
void
Application::restoreSignalHandler(int) {
    return;
}
#else
void
Application::restoreSignalHandler(int signalNum) {
    signal(signalNum, SIG_DFL);
    signalHandlers_.erase(signalNum);
}
#endif

/**
 * Redirects the signal received to the current signal handler
 *
 * Note in platforms which do not support signals (e.g. Windows),
 * this function does nothing.
 *
 * @param data Data from the signal.
 * @param info signal information struct
 * @param context signal context
 */
#ifdef __MINGW32__
void
Application::signalRedirector(int, siginfo_t*, void*) {
    return;
}
#else
void
Application::signalRedirector(int data, siginfo_t* info, void* /*context*/) {
    UnixSignalHandler* handler = getSignalHandler(info->si_signo);
    assert(handler != NULL);
    handler->execute(data, info);
}
#endif

/**
 * Returns the version string of the TCE build.
 *
 * This string includes the version control revision (if available),
 * automatically generated during build. This cannot be in headers
 * as it would induce a (almost) complete build of TCE on every
 * revision change.
 */
std::string
Application::TCEVersionString() {
    return TCE_VERSION_STRING;
}

/**
 * Returns true if the application is running from an install path.
 *
 * Since there is no reliable way to detect this from the binary name
 * or such since TCE can be used as a library, we just assume we are
 * using an installed TCE in case the developer mode is not set.
 */
bool
Application::isInstalled() {
    return !Environment::developerMode();
}

/**
 * Returns the path to TCE installation root
 *
 * Installation root can be one created by 'make install' or .deb package
 * unpacked to homedir for example. Environment variable TCE_INSTALL_DIR
 * should be set in the second case pointing to that dir where .deb was
 * installed or tce installation manually copied/moved to.
 *
 * @return Path to TCE installation root
 */
string
Application::installationDir() {

    if (installationRoot_ != "") {
        return installationRoot_;
    }

    string userRoot = Environment::environmentVariable("TCE_INSTALL_DIR");

    // .deb check: check if envvar TCE_INSTALL_DIR is set
    // and that dir exists ("icons" dir in our case)
    if (userRoot != "" && FileSystem::fileExists(
        userRoot + "/share/openasip/data/icons")) {
        return userRoot;
    }
    return string(TCE_INSTALLATION_ROOT);
}
