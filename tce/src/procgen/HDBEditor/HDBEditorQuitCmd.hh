/**
 * @file HDBEditorQuitCmd.hh
 *
 * Declaration of HDBEditorQuitCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_HDB_EDITOR_QUIT_CMD_HH
#define TTA_HDB_EDITOR_QUIT_CMD_HH

#include "GUICommand.hh"

/**
 * Command for quiting HDBEditor.
 */
class HDBEditorQuitCmd : public GUICommand {
public:
    HDBEditorQuitCmd();
    virtual ~HDBEditorQuitCmd();
    virtual bool Do();
    virtual int id() const;
    virtual HDBEditorQuitCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();

};
#endif
