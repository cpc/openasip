/**
 * @file AddBridgeCmd.hh
 *
 * Declaration of AddBridgeCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_ADD_BRIDGE_CMD_HH
#define TTA_ADD_BRIDGE_CMD_HH

#include "EditorCommand.hh"

/**
 * Command for adding new bridges to the Machine.
 *
 * Displays a bridge dialog and creates a new bridge according to the
 * dialog output.
 */
class AddBridgeCmd : public EditorCommand {
public:
    AddBridgeCmd();
    virtual ~AddBridgeCmd();
    virtual bool Do();
    virtual int id() const;
    virtual AddBridgeCmd* create() const;
    virtual std::string shortName() const;
    virtual bool isEnabled();
};

#endif
