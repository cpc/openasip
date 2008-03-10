/**
 * @file AddressSpacesDialog.hh
 *
 * Declaration of AddressSpacesDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_ADDRESS_SPACES_DIALOG_HH
#define TTA_ADDRESS_SPACES_DIALOG_HH

#include <wx/wx.h>
#include <wx/listctrl.h>

#include "Machine.hh"

class wxListCtrl;

/**
 * Dialog for listing and editing address spaces of the machine.
 */
class AddressSpacesDialog : public wxDialog {
public:
    AddressSpacesDialog(wxWindow* parent, TTAMachine::Machine* machine);
    ~AddressSpacesDialog();

private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    TTAMachine::AddressSpace* selectedAS();
    void updateASList();
    void onAdd(wxCommandEvent& event);
    void onActivateAS(wxListEvent& event);
    void onEdit(wxCommandEvent& event);
    void onDelete(wxCommandEvent& event);
    void onASSelection(wxListEvent& event);
    void onASRightClick(wxListEvent& event);
    void setTexts();

    /// Machine containing the address spaces to list.
    TTAMachine::Machine* machine_;
    /// List control for listing the address spaces.
    wxListCtrl* asList_;

    // enumerated ids for dialog controls
    enum {
        ID_ADD = 10000,
        ID_EDIT,
        ID_HELP,
        ID_DELETE,
        ID_LIST,
        ID_LINE
    };

    DECLARE_EVENT_TABLE()
};
#endif
