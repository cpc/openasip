/**
 * @file ProximOpenMachineCmd.hh
 *
 * Declaration of ProximOpenMachineCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_OPEN_MACHINE_CMD_HH
#define TTA_PROXIM_OPEN_MACHINE_CMD_HH

#include "GUICommand.hh"

/**
 * Command for opening a machine desctiption file in Proxim.
 *
 * The command displays a file dialog for choosing the machine file to open.
 */
class ProximOpenMachineCmd : public GUICommand {
public:
    ProximOpenMachineCmd();
    virtual ~ProximOpenMachineCmd();
    virtual bool Do();
    virtual int id() const;
    virtual ProximOpenMachineCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();
    virtual std::string shortName() const;
};
#endif
