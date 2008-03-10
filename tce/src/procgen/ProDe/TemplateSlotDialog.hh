/**
 * @file TemplateSlotDialog.hh
 *
 * Declaration of TemplateSlotDialog.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_TEMPLATE_SLOT_DIALOG_HH
#define TTA_TEMPLATE_SLOT_DIALOG_HH

#include <wx/wx.h>
#include <wx/spinctrl.h>

namespace TTAMachine {
    class InstructionTemplate;
    class Bus;
    class ImmediateUnit;
}

/**
 * Dialog for editing telplate slot properties.
 */
class TemplateSlotDialog : public wxDialog {
public:
    TemplateSlotDialog(
        wxWindow* parent,
        TTAMachine::InstructionTemplate* it,
        TTAMachine::Bus* slot = NULL);

    virtual ~TemplateSlotDialog();

private:
    wxSizer* createContents(wxWindow *parent, bool call_fit, bool set_sizer);
    virtual bool TransferDataToWindow();
    void onOK(wxCommandEvent& event);
    void setTexts();

    /// Instruction template slot to edit.
    TTAMachine::Bus* slot_;
    /// Parent instruction template of the slot.
    TTAMachine::InstructionTemplate* it_;
    /// Choice control for the slot.
    wxChoice* slotChoice_;
    /// Choice control fot the destination IU.
    wxChoice* destinationChoice_;
    /// Spin control for the width.
    wxSpinCtrl* widthCtrl_;
    /// Bit width of the slot.
    int width_;
    /// Destination immediate unit.
    TTAMachine::ImmediateUnit* destination_;

    // enumerated IDs for dialog widgets
    enum {
        ID_LABEL_SLOT = 10000,
        ID_SLOT,
        ID_LABEL_WIDTH,
        ID_WIDTH,
        ID_LABEL_DESTINATION,
        ID_DESTINATION,
        ID_LINE,
        ID_HELP
    };

    DECLARE_EVENT_TABLE()
};
#endif
