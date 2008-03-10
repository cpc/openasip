/**
 * @file EditTemplatesCmd.hh
 *
 * Declaration of EditTemplatesCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_EDIT_TEMPLATES_CMD_HH
#define TTA_EDIT_TEMPLATES_CMD_HH

#include "EditorCommand.hh"

/**
 * Command for editing instruction templates.
 *
 * Opens a TemplateListDialog which lists instruction templates in the machine.
 */
class EditTemplatesCmd : public EditorCommand {
public:
    EditTemplatesCmd();
    virtual ~EditTemplatesCmd();
    virtual bool Do();
    virtual int id() const;
    virtual EditTemplatesCmd* create() const;
    virtual std::string icon() const;
    virtual std::string shortName() const;
    virtual bool isEnabled();
};

#endif
