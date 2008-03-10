/**
 * @file OSEdUserManualCmd.hh
 *
 * Declaration of OSEdUserManualCmd class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OSED_USER_MANUAL_CMD_HH
#define TTA_OSED_USER_MANUAL_CMD_HH

#include <string>

#include "GUICommand.hh"

/**
 * Command for opening user manual browser.
 */
class OSEdUserManualCmd : public GUICommand {
public:
    OSEdUserManualCmd();
    virtual ~OSEdUserManualCmd();
	
    virtual int id() const;
    virtual GUICommand* create() const;
    virtual bool Do();
    virtual bool isEnabled();
    virtual std::string icon() const;

private:
    /// Copying not allowed.
    OSEdUserManualCmd(const OSEdUserManualCmd&);
    /// Assignment not allowed.
    OSEdUserManualCmd& operator=(const OSEdUserManualCmd&);

    std::string userManual() const;
};

#endif
