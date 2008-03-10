/**
 * @file EditorCommand.hh
 *
 * Declaration of EditorCommand class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_EDITOR_COMMAND_HH
#define TTA_EDITOR_COMMAND_HH

#include <wx/wx.h>
#include <string>

#include "GUICommand.hh"

class wxWindow;
class wxView;

/**
 * Base class for editor commands, which can be added to menus or toolbar.
 */
class EditorCommand : public GUICommand {
public:
    EditorCommand(std::string name, wxWindow* parent = NULL);
    virtual ~EditorCommand();

    void setView(wxView* view);
    wxView* view() const;
    virtual std::string icon() const;
    virtual bool isEnabled();

private:
    /// The view assigned for the command.
    wxView* view_;
};

#endif
