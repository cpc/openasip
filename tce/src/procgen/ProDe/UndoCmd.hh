/**
 * @file UndoCmd.hh
 *
 * Declaration of UndoCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_UNDO_CMD_HH
#define TTA_UNDO_CMD_HH

#include <string>
#include "EditorCommand.hh"

/**
 * EditorCommand for undoing modifications to the machine.
 */
class UndoCmd : public EditorCommand {
public:
    UndoCmd();
    virtual ~UndoCmd();
    virtual bool Do();
    virtual int id() const;
    virtual UndoCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();
};

#endif
