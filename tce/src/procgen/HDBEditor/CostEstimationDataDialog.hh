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
 * @file CostEstimationDataDialog.hh
 *
 * Declaration of CostEstimationDataDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_COST_ESTIMATION_DATA_DIALOG_HH
#define TTA_COST_ESTIMATION_DATA_DIALOG_HH

#include <wx/wx.h>
#include "DBTypes.hh"

namespace HDB {
    class HDBManager;
}

/**
 * Dialog for adding and modifying cost function plugin data.
 */
class CostEstimationDataDialog : public wxDialog {
public:
    CostEstimationDataDialog(
        wxWindow* parent, wxWindowID id, HDB::HDBManager& hdb, RowID pluginID,
        RowID dataID);

    virtual ~CostEstimationDataDialog();

private:
    virtual bool TransferDataToWindow();
    void onOK(wxCommandEvent& event);
    void onEntryTypeSelection(wxCommandEvent& event);

    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);

    /// widget IDs
    enum {
        ID_ENTRY_TYPE = 10000,
        ID_ENTRY_ID,
        ID_NAME,
        ID_VALUE,
        ID_TEXT,
        ID_LINE
    };

    /// HDB Containing the data.
    HDB::HDBManager& hdb_;
    /// ID of the data's parent plugin.
    RowID pluginID_;
    /// ID of the data to modify, or -1 if new data is being added.
    RowID dataID_;
    /// Entry reference type choice widget.
    wxChoice* typeChoice_;
    /// Entry reference id choice widget.
    wxChoice* idChoice_;
    /// Name of the data.
    wxString name_;
    /// Value of the data.
    wxString value_;

    static const wxString ENTRY_TYPE_NONE;
    static const wxString ENTRY_TYPE_FU;
    static const wxString ENTRY_TYPE_RF;
    static const wxString ENTRY_TYPE_BUS;
    static const wxString ENTRY_TYPE_SOCKET;

    DECLARE_EVENT_TABLE()
};

#endif
