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
