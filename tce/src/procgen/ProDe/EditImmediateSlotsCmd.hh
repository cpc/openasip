/**
 * @file EditImmediateSlotsCmd.hh
 *
 * Declaration of EditImmediateSlotsCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_EDIT_IMMEDIATE_SLOTS_CMD_HH
#define TTA_EDIT_IMMEDIATE_SLOTS_CMD_HH

#include "EditorCommand.hh"

/**
 * wxCommand for editing machine's immediate slots.
 */
class EditImmediateSlotsCmd : public EditorCommand {
public:
    EditImmediateSlotsCmd();
    virtual ~EditImmediateSlotsCmd();
    virtual bool Do();
    virtual int id() const;
    virtual EditImmediateSlotsCmd* create() const;
    virtual std::string icon() const;
    virtual std::string shortName() const;
    virtual bool isEnabled();
};

#endif
