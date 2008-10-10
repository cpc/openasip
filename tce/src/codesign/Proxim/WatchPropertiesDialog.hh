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
 * @file WatchPropertiesDialog.hh
 *
 * Declaration of WatchPropertiesDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_WATCH_PROPERTIES_DIALOG_HH
#define TTA_WATCH_PROPERTIES_DIALOG_HH

#include <wx/wx.h>

class StopPointManager;

/**
 * Dialog for modifying watchpoint properties.
 */
class WatchPropertiesDialog : public wxDialog {
public:
    WatchPropertiesDialog(
        wxWindow* parent, wxWindowID id,
        StopPointManager& manager, int handle);
    ~WatchPropertiesDialog();
private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool call_sizer);
    virtual bool TransferDataToWindow();
    void onOK(wxCommandEvent& event);

    /// Widget IDs.
    enum {
        ID_LABEL_WATCH = 10000,
        ID_LABEL_EXPRESSION,
        ID_LABEL_CONDITION,
        ID_LABEL_IGNORE_COUNT,

        ID_WATCH_HANDLE,
        ID_EXPRESSION,
        ID_CONDITION,
        ID_IGNORE_COUNT,
        ID_LINE
    };

    /// Stop point manager of the simulator.
    StopPointManager& manager_;
    /// Handle of the watch to modify.
    int handle_;

    /// Text widget for the watch expression script.
    wxTextCtrl* expressionCtrl_;
    /// Text widget fot the watch condition script.
    wxTextCtrl* conditionCtrl_;
    /// Spin button widget for the watch ignore count.
    wxSpinCtrl* ignoreCtrl_;

    DECLARE_EVENT_TABLE()
};
#endif
