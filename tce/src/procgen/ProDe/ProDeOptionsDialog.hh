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
 * @file ProDeOptionsDialog.hh
 *
 * Declaration of ProDeOptionsDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PRODE_OPTIONS_DIALOG_HH
#define TTA_PRODE_OPTIONS_DIALOG_HH

#include <string>
#include <wx/wx.h>
#include <wx/spinctrl.h>
#include "OptionsDialog.hh"

class ProDeOptions;
class wxListCtrl;
class CommandRegistry;

/**
 * Dialog for editing ProDeOptions.
 */
class ProDeOptionsDialog : public OptionsDialog {
public:
    ProDeOptionsDialog(
        wxWindow* parent,
        ProDeOptions& options,
        CommandRegistry& registry);

    virtual ~ProDeOptionsDialog();

private:
    void readProDeOptions();
    void writeProDeOptions();
    void onOK(wxCommandEvent& event);
    void onHelp(wxCommandEvent& event);

    wxSizer* createGeneralPage(
        wxWindow* parent, bool callFit, bool set_sizer);

    /// parent window of the dialog
    wxWindow* parent_;
    /// current editor options
    ProDeOptions& options_;

    /// Undo levels.
    wxSpinCtrl* undoStackSize_;

    // control IDs
    enum {
        ID_UNDO_LEVELS,
        ID_LABEL_UNDO_LEVELS
    };

    /// The event table for the controls of the dialog.
    DECLARE_EVENT_TABLE()
};

#endif
