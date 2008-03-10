/**
 * @file AddASCmd.hh
 *
 * Declaration of AddASCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_ADD_AS_CMD_HH
#define TTA_ADD_AS_CMD_HH

#include "EditorCommand.hh"

/**
 * Command for adding new address spaces to the Machine.
 *
 * Displays a address space dialog and creates a new address space
 * according to the dialog output.
 */
class AddASCmd : public EditorCommand {
public:
    AddASCmd();
    virtual bool Do();
    virtual int id() const;
    virtual AddASCmd* create() const;
    virtual std::string shortName() const;
    virtual bool isEnabled();
};

#endif
