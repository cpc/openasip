/**
 * @file AddGCUCmd.hh
 *
 * Declaration of AddGCUCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_ADD_GCU_CMD_HH
#define TTA_ADD_GCU_CMD_HH

#include "EditorCommand.hh"

/**
 * Command for adding new global control unit to the Machine.
 *
 * Displays a global control unit dialog and creates a new global
 * control unit according to the dialog output.
 */
class AddGCUCmd : public EditorCommand {
public:
    AddGCUCmd();
    virtual bool Do();
    virtual int id() const;
    virtual AddGCUCmd* create() const;
    virtual std::string shortName() const;
    virtual bool isEnabled();

private:
    /// Name for the return address port.
    static const std::string RA_PORT_NAME;
    /// Name for the jump/call operand port.
    static const std::string OP_PORT_NAME;
    /// Name string for the jump operation.
    static const std::string OPNAME_JUMP;
    /// Name string for the call operation.
    static const std::string OPNAME_CALL;
};

#endif
