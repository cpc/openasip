/**
 * @file ProximStepICmd.hh
 *
 * Declaration of ProximStepICmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_STEPI_CMD_HH
#define TTA_PROXIM_STEPI_CMD_HH

#include "GUICommand.hh"

class SimulatorFrontend;

/**
 * Command for advancing to the next machine instruction in Proxim simulation.
 */
class ProximStepICmd : public GUICommand {
public:
    ProximStepICmd();
    virtual ~ProximStepICmd();
    virtual bool Do();
    virtual int id() const;
    virtual ProximStepICmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();

private:
    SimulatorFrontend* simulator_;

};
#endif
