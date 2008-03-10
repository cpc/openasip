/**
 * @file ProximKillCmd.hh
 *
 * Declaration of ProximKillCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_KILL_CMD_HH
#define TTA_PROXIM_KILL_CMD_HH

#include "GUICommand.hh"

class SimulatorFrontend;

/**
 * Command for terminating simulation in Proxim.
 */
class ProximKillCmd : public GUICommand {
public:
    ProximKillCmd();
    virtual ~ProximKillCmd();
    virtual bool Do();
    virtual int id() const;
    virtual ProximKillCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();

private:
    SimulatorFrontend* simulator_;

};
#endif
