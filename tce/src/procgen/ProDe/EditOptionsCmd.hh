/**
 * @file EditOptionsCmd.hh
 *
 * Declaration of EditOptionsCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_EDIT_OPTIONS_CMD_HH
#define TTA_EDIT_OPTIONS_CMD_HH

#include "EditorCommand.hh"

/**
 * wxCommand for editing editor options.
 *
 * Creates and shows an OptionsDialog.
 */
class EditOptionsCmd : public EditorCommand {
public:
    EditOptionsCmd();
    virtual ~EditOptionsCmd();
    virtual bool Do();
    virtual int id() const;
    virtual EditOptionsCmd* create() const;
    virtual std::string shortName() const;
    virtual std::string icon() const;
    virtual bool isEnabled();
};

#endif
