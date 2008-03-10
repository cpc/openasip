/**
 * @file HDBEditorDeleteCmd.hh
 *
 * Declaration of HDBEditorDeleteCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_HDB_EDITOR_DELETE_CMD_HH
#define TTA_HDB_EDITOR_DELETE_CMD_CC

#include "GUICommand.hh"

/**
 * Command for deleting entries from the HDB.
 */
class HDBEditorDeleteCmd : public GUICommand {
public:
    HDBEditorDeleteCmd();
    virtual ~HDBEditorDeleteCmd();
    virtual bool Do();
    virtual int id() const;
    virtual HDBEditorDeleteCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();

};

#endif
