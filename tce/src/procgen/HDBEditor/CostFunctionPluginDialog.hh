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
