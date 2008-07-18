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
 * @author Atte Oksman 2003 (oksman@cs.tut.fi)
 * @author Pekka Jääskeläinen 2005 (pjaaskel@cs.tut.fi)
 */

#include <string>
#include <iostream>
#include <fstream>
#include <cstddef>
#include <exception>
#include <cstdio>

// for backtrace printing:
#include <signal.h>

// macros to evaluate exit status of pclose()
#include <sys/wait.h>

#include "Environment.hh"
#include "Application.hh"
#include "Exception.hh"
#include "config.h"

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
Application::UnixSignalHandler* Application::ctrlcHandler_(NULL);
Application::UnixSignalHandler* Application::fpeHandler_(NULL);

int Application::verboseLevel_ = Application::VERBOSE_LEVEL_DEFAULT;

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
    if (!DISTRIBUTED_VERSION) {
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
    std::size_t maxOutputLines) {

    char line[MAX_OUTPUT_LINE_LENGTH];

    // flush all streams to avoid: "...the output from a
    // command opened for writing may become intermingled with that of
    // the original process." (man popen)
    fflush(NULL);

    FILE* pipe = popen(command.c_str(), "r");

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
 * Sets handler for signal which is produced when user hits ctrl-c.
 *
 * The previous handler is saved.
 *
 * @param handler The handler instance to use.
 */
void
Application::setCtrlcHandler(UnixSignalHandler& handler) {
    ctrlcHandler_ = &handler;
    
    struct sigaction action;
    action.sa_flags = SA_SIGINFO; 
    action.sa_sigaction = ctrlcSignalRedirector;
 
    sigaction(SIGINT, &action, NULL);
}

/**
 * Restores the original ctrl-c handler.
 */
void
Application::restoreCtrlcHandler() {
    signal(SIGINT, SIG_DFL);
}

/**
 * Sets handler for signal SIGFPE
 *
 * The previous handler is saved.
 *
 * @param handler The handler instance to use.
 */
void
Application::setFpeHandler(UnixSignalHandler& handler) {
    fpeHandler_ = &handler;
    
    struct sigaction action;
    action.sa_flags = SA_SIGINFO; 
    action.sa_sigaction = fpeSignalRedirector;
 
    sigaction(SIGFPE, &action, NULL);
}

/**
 * Restores the original SIGFPE handler.
 */
void
Application::restoreFpeHandler() {
    signal(SIGFPE, SIG_DFL);
}

/**
 * Redirects the signal received when user hits ctrl-c to the user defined
 * wrapper.
 *
 * @param data Data from the signal.
 * @param info signal information struct
 * @param context signal context
 */
void
Application::ctrlcSignalRedirector(int data, siginfo_t *info, void *context) {
    assert(ctrlcHandler_ != NULL);
    if (context) {}
    ctrlcHandler_->execute(data, info);
}

/**
 * Redirects the received signal SIGFPE to the user defined wrapper.
 *
 * @param data Data from the signal.
 * @param info signal information struct
 * @param context signal context
 */
void
Application::fpeSignalRedirector(int data, siginfo_t *info, void *context) {
    assert(fpeHandler_ != NULL);
    if (context) {}
    fpeHandler_->execute(data, info);
}
