/**
 * @file OSEdQuitCmd.hh
 *
 * Declaration of OSEdQuitCmd class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OSED_QUIT_CMD_HH
#define TTA_OSED_QUIT_CMD_HH

#include <string>

#include "GUICommand.hh"

/**
 * Command that quits the execution of the application.
 */
class OSEdQuitCmd : public GUICommand {
public:
    OSEdQuitCmd();
    virtual ~OSEdQuitCmd();

    virtual int id() const;
    virtual GUICommand* create() const;
    virtual bool Do();
    virtual bool isEnabled();
    virtual std::string icon() const;

private:
    /// Copying not allowed.
    OSEdQuitCmd(const OSEdQuitCmd&);
    /// Assignment not allowed.
    OSEdQuitCmd& operator=(OSEdQuitCmd&);
};

#endif
