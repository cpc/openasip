/**
 * @file OSEdModifyBehaviorCmd.hh
 *
 * Declaration of OSEdModifyBehaviorCmd class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OSED_MODIFY_BEHAVIOR_CMD_HH
#define TTA_OSED_MODIFY_BEHAVIOR_CMD_HH

#include <string>

#include "GUICommand.hh"

/**
 * Command for modifying operation behavior.
 */
class OSEdModifyBehaviorCmd : public GUICommand {
public:
    OSEdModifyBehaviorCmd();
    virtual ~OSEdModifyBehaviorCmd();

    virtual int id() const;
    virtual GUICommand* create() const;
    virtual bool Do();
    virtual bool isEnabled();
    virtual std::string icon() const;

private:
    /// Copying not allowed.
    OSEdModifyBehaviorCmd(const OSEdModifyBehaviorCmd&);
    /// Assignment not allowed.
    OSEdModifyBehaviorCmd& operator=(const OSEdModifyBehaviorCmd&);
};

#endif
