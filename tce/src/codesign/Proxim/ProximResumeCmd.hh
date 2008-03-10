/**
 * @file ProximResumeCmd.hh
 *
 * Declaration of ProximResumeCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_RESUME_CMD_HH
#define TTA_PROXIM_RESUME_CMD_HH

#include "GUICommand.hh"

class SimulatorFrontend;

/**
 * Command for resuming simulation in Proxim until the simulation is finished
 * or a breakpoint is reacehed.
 */
class ProximResumeCmd : public GUICommand {
public:
    ProximResumeCmd();
    virtual ~ProximResumeCmd();
    virtual bool Do();
    virtual int id() const;
    virtual ProximResumeCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();

private:
    SimulatorFrontend* simulator_;

};
#endif
