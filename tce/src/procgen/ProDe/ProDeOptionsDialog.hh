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
