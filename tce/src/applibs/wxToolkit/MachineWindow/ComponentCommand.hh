/**
 * @file ComponentCommand.hh
 * 
 * Declaration of ComponentCommand class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_COMPONENT_COMMAND_HH
#define TTA_COMPONENT_COMMAND_HH

#include <wx/wx.h>
#include <wx/cmdproc.h>

class wxWindow;

/**
 * Base class for the commands created by component EditPolicies.
 *
 * Setting the parent window allows command to popup dialogs.
 */
class ComponentCommand {
public:
    ComponentCommand();
    virtual ~ComponentCommand();

    virtual bool Do() = 0;

    wxWindow* parentWindow();
    void setParentWindow(wxWindow* window);

private:
    /// Parent window of the command.
    wxWindow* parentWindow_;
};

#endif
