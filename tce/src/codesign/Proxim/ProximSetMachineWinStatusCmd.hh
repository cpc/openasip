/**
 * @file ProximShowRegistersCmd.hh
 *
 * Declaration of ProximSetMachineWinStatusCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_SET_MACHINE_WIN_STATUS_CMD
#define TTA_PROXIM_SET_MACHINE_WIN_STATUS_CMD

#include <wx/wx.h>
#include "ComponentCommand.hh"

class EditPart;

/**
 * Command for setting the machine window status bar text.
 */
class ProximSetMachineWinStatusCmd : public ComponentCommand {
public:
    ProximSetMachineWinStatusCmd(EditPart* part);
    virtual ~ProximSetMachineWinStatusCmd();
    virtual bool Do();

private:
    EditPart* part_;
};

#endif
