/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file GUICommand.hh
 *
 * Declaration of GUICommand class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel-no.spam-cs.tut.fi)
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
