/**
 * @file ProximQuitCmd.hh
 *
 * Declaration of ProximQuitCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_QUIT_CMD_HH
#define TTA_PROXIM_QUIT_CMD_HH

#include "GUICommand.hh"

class SimulatorFrontend;

/**
 * Command for quiting Proxim.
 */
class ProximQuitCmd : public GUICommand {
public:
    ProximQuitCmd();
    virtual ~ProximQuitCmd();
    virtual bool Do();
    virtual int id() const;
    virtual ProximQuitCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();

private:
    SimulatorFrontend* simulator_;

};
#endif
