/**
 * @file CommandThread.cc
 *
 * Definition of CommandThread class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#include "CommandThread.hh"
#include "FileSystem.hh"

using std::string;
using std::vector;

/**
 * Constructor.
 *
 * @param cmd Command to be runned by thread.
 */
CommandThread::CommandThread(const std::string& cmd) : wxThread(), cmd_(cmd) {
}

/**
 * Destructor.
 */
CommandThread::~CommandThread() {
}

/**
 * Entry point of the thread.
 *
 * @return Thread exit code.
 */
wxThread::ExitCode
CommandThread::Entry() {
    FileSystem::runShellCommand(cmd_);
    return 0;
}

