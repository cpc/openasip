/**
 * @file AddressSpaceDialog.hh
 *
 * Declaration of AddressSpaceDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_ADDRESS_SPACE_DIALOG_HH
#define TTA_ADDRESS_SPACE_DIALOG_HH

#include <wx/wx.h>
#include <wx/spinctrl.h>

class NumberControl;

namespace TTAMachine {
    class AddressSpace;
}

/**
 * Dialog for editing address space properties.
 */
class AddressSpaceDialog : public wxDialog {
public:
    AddressSpaceDialog(
        wxWindow* parent,
        TTAMachine::AddressSpace* addressSpace);
    ~AddressSpaceDialog();

private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    virtual bool TransferDataToWindow();
    void onName(wxCommandEvent& event);
    void onOK(wxCommandEvent& event);
    void onHelp(wxCommandEvent& event);
    void onMinAddress(wxCommandEvent& event);
    void onMinHex(wxCommandEvent& event);
    void onMaxAddress(wxCommandEvent& event);
    void onMaxHex(wxCommandEvent& event);
    void onMinToDec(wxCommandEvent& event);
    void onMinToHex(wxCommandEvent& event);
    void onMaxToDec(wxCommandEvent& event);
    void onMaxToHex(wxCommandEvent& event);
    void setTexts();

    /// Address space to modify with the dialog.
    TTAMachine::AddressSpace* as_;
    /// Name of the address space.
    wxString name_;
    /// The bit width of the minimum addressable word.
    int width_;
    /// The lowest address in the address space.
    wxString minAddress_;
    /// The highest address in the address space.
    wxString maxAddress_;
    /// Widget for the lowest address hex value.
    NumberControl* minControl_;
    /// Widget for the highest address hex value.
    NumberControl* maxControl_;

    /// Boxsizer containing the name widget.
    wxStaticBoxSizer* nameSizer_;
    /// Boxsizer containing the min-address widget.
    wxStaticBoxSizer* minAddressSizer_;
    /// Boxsizer containing the max-address widget.
    wxStaticBoxSizer* maxAddressSizer_;
    /// Boxsizer containing the width widget.
    wxStaticBoxSizer* widthSizer_;

    // IDs for dialog controls
    enum {
        ID_NAME = 10000,
        ID_WIDTH,
        ID_MIN_ADDRESS,
        ID_MAX_ADDRESS,
        ID_RADIO_MIN_DEC,
        ID_RADIO_MIN_HEX,
        ID_RADIO_MAX_DEC,
        ID_RADIO_MAX_HEX,
        ID_TEXT,
        ID_LINE,
        ID_HELP
    };

    DECLARE_EVENT_TABLE()
};
#endif
