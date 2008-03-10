/**
 * @file GUICommand.hh
 *
 * Declaration of GUICommand class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_GUI_COMMAND_HH
#define TTA_GUI_COMMAND_HH

#include <wx/wx.h>
#include <string>

class wxWindow;

/**
 * Base class for editor commands, which can be added to menus or toolbar.
 */
class GUICommand {
public:
    GUICommand(std::string name, wxWindow* parent);
    virtual ~GUICommand();

    /**
     * Returns the ID of the command.
     *
     * @return The ID of the command.
     */
    virtual int id() const = 0;

    /**
     * Creates a new GUICommand instance.
     *
     * @return New GUICommand instance.
     */
    virtual GUICommand* create() const = 0;

    /**
     * Executes the command.
     *
     * @return True, if the command was succesfully executed, false otherwise.
     */
    virtual bool Do() = 0;

    /**
     * Returns true if the command is executable, false otherwise.
     *
     * @return True, if the command is executable, false otherwise.
     */
    virtual bool isEnabled() = 0;

    /**
     * Returns name of the command icon file.
     *
     * @return Name of the command icon file.
     */
    virtual std::string icon() const = 0;

    virtual bool isChecked() const;
    virtual std::string shortName() const;
    void setParentWindow(wxWindow* view);
    wxWindow* parentWindow() const;
    std::string name() const;

private:
    /// Parent window of the command.
    wxWindow* parent_;
    /// Name of the command.
    std::string name_;
};

#endif
