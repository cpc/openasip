/**
 * @file HDBEditorModifyCmd.hh
 *
 * Declaration of HDBEditorModifyCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_HDB_EDITOR_MODIFY_CMD_HH
#define TTA_HDB_EDITOR_MODIFY_CMD_HH

#include "GUICommand.hh"

/**
 * Command for modifying HDB items.
 */
class HDBEditorModifyCmd : public GUICommand {
public:
    HDBEditorModifyCmd();
    virtual ~HDBEditorModifyCmd();
    virtual bool Do();
    virtual int id() const;
    virtual HDBEditorModifyCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();

};

#endif
