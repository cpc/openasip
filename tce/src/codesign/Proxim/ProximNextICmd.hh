/**
 * @file ProximNextICmd.hh
 *
 * Declaration of ProximNextICmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_NEXTI_CMD_HH
#define TTA_PROXIM_NEXTI_CMD_HH

#include "GUICommand.hh"

class SimulatorFrontend;

/**
 * Proxim simulation command for advancing to the next machine instruction
 * in current procedure.
 */
class ProximNextICmd : public GUICommand {
public:
    ProximNextICmd();
    virtual ~ProximNextICmd();
    virtual bool Do();
    virtual int id() const;
    virtual ProximNextICmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();

private:
    SimulatorFrontend* simulator_;

};
#endif
