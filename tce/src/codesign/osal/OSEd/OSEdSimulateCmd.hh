/**
 * @file OSEdSimulateCmd.hh
 *
 * Declaration of OSEdSimulateCmd class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OSED_SIMULATE_CMD_HH
#define TTA_OSED_SIMULATE_CMD_HH

#include <string>

#include "GUICommand.hh"

/**
 * Executes the command when operation is simulated.
 */
class OSEdSimulateCmd : public GUICommand {
public:
    OSEdSimulateCmd();
    virtual ~OSEdSimulateCmd();
	
    virtual int id() const;
    virtual GUICommand* create() const;
    virtual bool Do();
    virtual bool isEnabled();
    virtual std::string icon() const;

private:
    /// Copying not allowed.
    OSEdSimulateCmd(const OSEdSimulateCmd&);
    /// Assignment not allowed.
    OSEdSimulateCmd& operator=(const OSEdSimulateCmd&);

};

#endif
