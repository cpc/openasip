/**
 * @file ProximExecuteFileCmd.hh
 *
 * Declaration of ProximExecuteFileCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_EXECUTE_FILE_CMD_HH
#define TTA_PROXIM_EXECUTE_FILE_CMD_HH

#include "GUICommand.hh"

class SimulatorFrontend;

/**
 * Command for executing commands from a log file.
 */
class ProximExecuteFileCmd : public GUICommand {
public:
    ProximExecuteFileCmd();
    virtual ~ProximExecuteFileCmd();
    virtual bool Do();
    virtual int id() const;
    virtual ProximExecuteFileCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();

private:
    SimulatorFrontend* simulator_;

};
#endif
