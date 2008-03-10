/**
 * @file AddRFFromHDBDialog.hh
 *
 * Declaration of AddRFFromHDBDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_ADD_RF_FROM_HDB_DIALOG_HH
#define TTA_ADD_RF_FROM_HDB_DIALOG_HH

#include <map>
#include <wx/wx.h>
#include <wx/listctrl.h>

class Model;

namespace HDB {
    class RFArchitecture;
    class HDBManager;
}

/**
 * Dialog for adding register file architectures directly from HDB to the
 * current machine.
 */
class AddRFFromHDBDialog : public wxDialog {
public:
    AddRFFromHDBDialog(wxWindow* parent, Model* model);
    virtual ~AddRFFromHDBDialog();

private:
    virtual bool TransferDataToWindow();
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    void onListSelectionChange(wxListEvent& event);
    void onAdd(wxCommandEvent& event);
    void onClose(wxCommandEvent& event);
    bool loadHDB(const HDB::HDBManager& manager);

    /// Model of the current adf file.
    Model* model_;
    /// Immediate slot list widget.
    wxListCtrl* list_;
    /// Map of rf architectures displayed in the dialog list.
    std::map<int, HDB::RFArchitecture*> rfArchitectures_;

    enum {
	ID_LIST = 10000,
	ID_ADD,
	ID_CLOSE,
	ID_LINE
    };

    /// Default size for the rf, if the size is parameterized in the HDB.
    static const int DEFAULT_SIZE;
    /// Default bit width for the rf, if the size is parameterized in the HDB.
    static const int DEFAULT_WIDTH;
    /// File filter for the HDB files.
    static const wxString HDB_FILE_FILTER;

    DECLARE_EVENT_TABLE()
};
#endif
