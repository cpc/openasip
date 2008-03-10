/**
 * @file VerifyMachineCmd.hh
 *
 * Declaration of VerifyMachineCmd class.
 *
 * @author Tommi Rantanen 2004 (tommi.rantanen@tut.fi)
 */

#ifndef TTA_VERIFY_MACHINE_CMD_HH
#define TTA_VERIFY_MACHINE_CMD_HH

#include <string>

#include "EditorCommand.hh"

/**
 * wxCommand for editing connections.
 */
class VerifyMachineCmd : public EditorCommand {
public:
    VerifyMachineCmd();
    virtual ~VerifyMachineCmd();
    virtual bool Do();
    virtual int id() const;
    virtual VerifyMachineCmd* create() const;
    virtual std::string icon() const;
    virtual std::string shortName() const;
    virtual bool isEnabled();
};

#endif
