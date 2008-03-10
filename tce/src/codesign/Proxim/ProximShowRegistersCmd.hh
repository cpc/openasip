/**
 * @file ProximShowRegistersCmd.hh
 *
 * Declaration of ProximShowRegistersCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_SHOW_REGISTERS_CMD
#define TTA_PROXIM_SHOW_REGISTERS_CMD

#include <wx/wx.h>
#include "ComponentCommand.hh"

class EditPart;

/**
 * Command for opening a register window displaying register state of
 * a registerfile.
 */
class ProximShowRegistersCmd : public ComponentCommand {
public:
    ProximShowRegistersCmd(EditPart* part);
    virtual ~ProximShowRegistersCmd();
    virtual bool Do();

private:
    EditPart* part_;
};

#endif
