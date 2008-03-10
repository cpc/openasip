/**
 * @file AddFUCmd.hh
 *
 * Declaration of AddFUCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_ADD_FU_CMD_HH
#define TTA_ADD_FU_CMD_HH

#include "EditorCommand.hh"

/**
 * Command for adding new function units to the Machine.
 *
 * Displays a function unit dialog and creates a new function unit
 * according to the dialog output.
 */
class AddFUCmd : public EditorCommand {
public:
    AddFUCmd();
    virtual bool Do();
    virtual int id() const;
    virtual AddFUCmd* create() const;
    virtual std::string shortName() const;
    virtual bool isEnabled();
};

#endif
