/**
 * @file FUArchitectureDialog.hh
 *
 * Declaration of FUArchParamtersDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_FU_ARCHITECTURE_DIALOG_HH
#define TTA_FU_ARCHITECTURE_DIALOG_HH

#include <wx/wx.h>

namespace HDB {
    class FUArchitecture;
}


/**
 * Dialog for defining FU architecture paramters to HDB.
 */
class FUArchitectureDialog : public wxDialog {
public:
    FUArchitectureDialog(
        wxWindow* parent, wxWindowID id, HDB::FUArchitecture& arch);
    virtual ~FUArchitectureDialog();

private:
    void onOK(wxCommandEvent& event);
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    void update();

    /// Widget IDs.
    enum {
        ID_PARAM_WIDTH_LIST = 10000,
        ID_GUARD_SUPPORT_LIST,
        ID_LINE
    };

    HDB::FUArchitecture& arch_;

    wxCheckListBox* paramWidthList_;
    wxCheckListBox* guardSupportList_;

    DECLARE_EVENT_TABLE()
};

#endif
