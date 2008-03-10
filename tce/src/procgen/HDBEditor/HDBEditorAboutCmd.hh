/**
 * @file HDBEditorAboutCmd.hh
 *
 * Declaration of HDBEditorAboutCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_HDB_EDITOR_ABOUT_CMD_HH
#define TTA_HDB_EDITOR_ABOUT_CMD_HH

#include "GUICommand.hh"


/**
 * Command for displaying HDBEditor about dialog.
 */
class HDBEditorAboutCmd : public GUICommand {
public:
    HDBEditorAboutCmd();
    virtual ~HDBEditorAboutCmd();
    virtual bool Do();
    virtual int id() const;
    virtual HDBEditorAboutCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();
};

#endif
