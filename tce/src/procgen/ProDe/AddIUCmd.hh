/**
 * @file AddIUCmd.hh
 *
 * Declaration of AddIUCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_ADD_IU_CMD_HH
#define TTA_ADD_IU_CMD_HH

#include "EditorCommand.hh"

/**
 * Command for adding new immediate units to the Machine.
 *
 * Displays a immediate unit dialog and creates a new immediate unit
 * according to the dialog output.
 */
class AddIUCmd : public EditorCommand {
public:
    AddIUCmd();
    virtual bool Do();
    virtual int id() const;
    virtual AddIUCmd* create() const;
    virtual std::string shortName() const;
    virtual bool isEnabled();
};

#endif
