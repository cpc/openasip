/*
    Copyright (c) 2002-2009 Tampere University of Technology.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
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
