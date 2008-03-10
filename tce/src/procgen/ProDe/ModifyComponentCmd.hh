/**
 * @file ModifyComponentCmd.hh
 *
 * Declaration of ModifyComponentCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_MODIFY_COMPONENT_CMD_HH
#define TTA_MODIFY_COMPONENT_CMD_HH

#include <wx/wx.h>
#include <wx/cmdproc.h>
#include "EditorCommand.hh"


/**
 * wxCommand for modifying components in the Machine.
 */
class ModifyComponentCmd: public EditorCommand {
public:
    ModifyComponentCmd();
    virtual bool Do();
    virtual int id() const;
    virtual ModifyComponentCmd* create() const;
    virtual std::string shortName() const;
    virtual std::string icon() const;
    virtual bool isEnabled();
};

#endif
