/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file EditorCommand.hh
 *
 * Declaration of EditorCommand class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel-no.spam-cs.tut.fi)
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
