/**
 * @file ImmediateSlotDialog.hh
 *
 * Declaration of ImmediateSlotDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_IMMEDIATE_SLOT_DIALOG_HH
#define TTA_IMMEDIATE_SLOT_DIALOG_HH

#include <wx/wx.h>
#include <wx/listctrl.h>

namespace TTAMachine {
    class Machine;
}


/**
 * Dialog for modifying immediate slots in a machine.
 */
class ImmediateSlotDialog : public wxDialog {
public:
    ImmediateSlotDialog(wxWindow* parent, TTAMachine::Machine* machine);
    virtual ~ImmediateSlotDialog();

private:
    virtual bool TransferDataToWindow();
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    void updateSlotList();
    void onAddSlot(wxCommandEvent& event);
    void onDeleteSlot(wxCommandEvent& event);
    void onSlotName(wxCommandEvent& event);
    void onSlotSelection(wxListEvent& event);
    void setTexts();

    /// Parent machine of the immediate slots.
    TTAMachine::Machine* machine_;
    /// Immediate slot list widget.
    wxListCtrl* slotList_;
    /// Text in the slot name widget.
    wxString slotName_;

    enum {
	ID_SLOT_LIST = 10000,
	ID_NAME,
	ID_LABEL_NAME,
	ID_LABE_NAME,
	ID_ADD_SLOT,
	ID_DELETE_SLOT,
	ID_LINE,
	ID_HELP
    };

    DECLARE_EVENT_TABLE()
};
#endif
