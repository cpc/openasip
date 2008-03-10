/**
 * @file ProximRunCmd.hh
 *
 * Declaration of ProximRunCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_RUN_CMD_HH
#define TTA_PROXIM_RUN_CMD_HH

#include "GUICommand.hh"

class SimulatorFrontend;

/**
 * Command for starting simulation run in Proxim.
 */
class ProximRunCmd : public GUICommand {
public:
    ProximRunCmd();
    virtual ~ProximRunCmd();
    virtual bool Do();
    virtual int id() const;
    virtual ProximRunCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();

private:
    SimulatorFrontend* simulator_;

};
#endif
