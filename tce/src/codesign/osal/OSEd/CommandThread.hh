/**
 * @file CommandThread.hh
 *
 * Declaration of CommandThread class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_COMMAND_THREAD_HH
#define TTA_COMMAND_THREAD_HH

#include <wx/thread.h>
#include <string>
#include <vector>

/**
 * Class for running command in its own thread.
 */
class CommandThread : public wxThread {
public:
    explicit CommandThread(const std::string& cmd);
    virtual ~CommandThread();

    virtual ExitCode Entry();
  
private:
    /// Copying not allowed.
    CommandThread(const CommandThread&);
    /// Assignment not allowed.
    CommandThread& operator=(const CommandThread&);
    
    /// Command to be executed.
    std::string cmd_;
};

#endif
