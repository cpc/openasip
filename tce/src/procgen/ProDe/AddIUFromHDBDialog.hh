/**
 * @file AddRFFromHDBDialog.hh
 *
 * Declaration of AddRFFromHDBDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_ADD_IU_FROM_HDB_DIALOG_HH
#define TTA_ADD_IU_FROM_HDB_DIALOG_HH

#include <map>
#include <wx/wx.h>
#include <wx/listctrl.h>
#include "Machine.hh"

class Model;

namespace HDB {
    class RFArchitecture;
}

/**
 * Dialog for adding immediate unit architectures directly from HDB to the
 * current machine.
 */
class AddIUFromHDBDialog : public wxDialog {
public:
    AddIUFromHDBDialog(wxWindow* parent, Model* model);
    virtual ~AddIUFromHDBDialog();

private:
    virtual bool TransferDataToWindow();
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    void onListSelectionChange(wxListEvent& event);
    void onAdd(wxCommandEvent& event);
    void onClose(wxCommandEvent& event);
    bool loadHDB(const std::string& path);

    /// Model of the current adf file.
    Model* model_;
    /// Immediate slot list widget.
    wxListCtrl* list_;
    /// Map of iu architectures displayed in the dialog list.
    std::map<int, HDB::RFArchitecture*> iuArchitectures_;

    enum {
	ID_LIST = 10000,
	ID_ADD,
	ID_CLOSE,
	ID_LINE
    };

    /// Default size for the IU, if the size is parameterized in the HDB.
    static const int DEFAULT_SIZE;
    /// Default width for the IU, if the size is parameterized in the HDB.
    static const int DEFAULT_WIDTH;
    /// Default extension mode for the immediate unit.
    static const TTAMachine::Machine::Extension DEFAULT_EXTENSION_MODE;
    /// File filter for HDB files.
    static const wxString HDB_FILE_FILTER;

    DECLARE_EVENT_TABLE()
};
#endif
