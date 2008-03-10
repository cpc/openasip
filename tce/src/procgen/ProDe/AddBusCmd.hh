/**
 * @file AddBusCmd.hh
 *
 * Declaration of AddBusCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_ADD_BUS_CMD_HH
#define TTA_ADD_BUS_CMD_HH

#include "EditorCommand.hh"

/**
 * Command for adding new buses to the Machine.
 *
 * Displays a bus dialog and creates a new bus according to the dialog
 * output.
 */
class AddBusCmd : public EditorCommand {
public:
    AddBusCmd();
    virtual bool Do();
    virtual int id() const;
    virtual AddBusCmd* create() const;
    virtual std::string shortName() const;
    virtual bool isEnabled();
};

#endif
