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
 * @file CostFunctionPluginDialog.hh
 *
 * Declaration of CostFunctionPluginDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_COST_FUNCTION_PLUGIN_DIALOG_HH
#define TTA_COST_FUNCTION_PLUGIN_DIALOG_HH

#include <wx/wx.h>

namespace HDB {
    class HDBManager;
}

/**
 * Dialog for modifying CostFunctionPlugins.
 */
class CostFunctionPluginDialog : public wxDialog {
public:
    CostFunctionPluginDialog(
        wxWindow* parent, wxWindowID id, HDB::HDBManager& hdb, int pluginID);

    virtual ~CostFunctionPluginDialog();
private:
    virtual bool TransferDataToWindow();
    void onBrowse(wxCommandEvent& event);
    void onAdd(wxCommandEvent& event);
    void onModify(wxCommandEvent& event);
    void onDelete(wxCommandEvent& event);
    void onOK(wxCommandEvent& event);

    void onDataSelection(wxListEvent& event);
    int selectedData();

    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);

    /// Pointer to the data list widget.
    wxListCtrl* list_;

    /// HDB containing the plugin.
    HDB::HDBManager& hdb_;
    /// Plugin id in the HDB.
    int id_;
    /// Plugin name.
    wxString name_;
    /// Plugin file path.
    wxString path_;
    /// Plugin description string.
    wxString description_;

    /// Pointer to the type choice widget.
    wxChoice* typeChoice_;

    /// FU Cost estimator string for the type choicer widget.
    static const wxString TYPE_COST_FU;
    /// RF Cost estimator string for the type choicer widget.
    static const wxString TYPE_COST_RF;
    /// Decompressor cost estimator string for the type choicer widget.
    static const wxString TYPE_COST_DECOMP;
    /// IC&decoder Cost estimator string for the type choicer widget.
    static const wxString TYPE_COST_ICDEC;

    /// Widget ids.
    enum {
        ID_LIST = 10000,
        ID_NAME,
        ID_PATH,
        ID_TYPE,
        ID_BROWSE,
        ID_DESCRIPTION,
        ID_ADD,
        ID_MODIFY,
        ID_DELETE,
        ID_TEXT,
        ID_LINE
    };

    DECLARE_EVENT_TABLE()
};

#endif
