/**
 * @file PasteComponentCmd.hh
 *
 * Declaration of PasteComponentCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_PASTE_COMPONENT_CMD_HH
#define TTA_PASTE_COMPONENT_CMD_HH

#include <wx/wx.h>
#include <wx/cmdproc.h>

#include "EditorCommand.hh"
#include "Model.hh"

namespace TTAMachine {
    class Component;
    class Machine;
}

/**
 * Command for pasting components from the clipboard to the machine.
 */
class PasteComponentCmd: public EditorCommand {
public:
    PasteComponentCmd();
    virtual bool Do();
    virtual int id() const;
    virtual PasteComponentCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();

private:
    template <class ComponentNavigator>
    void paste(TTAMachine::Machine& machine,
               TTAMachine::Component* component,
               ComponentNavigator& navigator);

    bool setMachine(
        TTAMachine::Component* component,
        TTAMachine::Machine* machine);
};

#include "PasteComponentCmd.icc"

#endif
