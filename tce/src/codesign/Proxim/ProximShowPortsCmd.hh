/**
 * @file ProximShowPortsCmd.hh
 *
 * Declaration of ProximShowPortsCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_SHOW_PORTS_CMD
#define TTA_PROXIM_SHOW_PORTS_CMD

#include <wx/wx.h>
#include "ComponentCommand.hh"

class EditPart;

/**
 * Command for opening a function unit window displaying port state of
 * a function unit.
 */
class ProximShowPortsCmd : public ComponentCommand {
public:
    ProximShowPortsCmd(EditPart* part);
    virtual ~ProximShowPortsCmd();
    virtual bool Do();

private:
    EditPart* part_;
};

#endif
