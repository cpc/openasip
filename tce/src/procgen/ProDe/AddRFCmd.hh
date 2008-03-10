/**
 * @file AddRFCmd.hh
 *
 * Declaration of AddRFCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_ADD_RF_CMD_HH
#define TTA_ADD_RF_CMD_HH

#include "EditorCommand.hh"

/**
 * Command for adding new register files to the Machine.
 *
 * Displays a register file dialog and creates a new register file
 * according to the dialog output.
 */
class AddRFCmd : public EditorCommand {
public:
    AddRFCmd();
    virtual bool Do();
    virtual int id() const;
    virtual AddRFCmd* create() const;
    virtual std::string shortName() const;
    virtual bool isEnabled();
};

#endif
