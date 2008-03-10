/**
 * @file ProximComponentStatusCmd.hh
 *
 * Declaration of ProximComponentStatusCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_COMPONENT_STATUS_CMD_HH
#define TTA_PROXIM_COMPONENT_STATUS_CMD_HH

#include <string>
#include "ComponentCommand.hh"

/**
 * Command for showing component status in the ProximMachineStatus Window.
 */
class ProximComponentStatusCmd : public ComponentCommand {
public:
    ProximComponentStatusCmd(std::string status);
    virtual ~ProximComponentStatusCmd();
    virtual bool Do();
private:
    std::string status_;
};
#endif
