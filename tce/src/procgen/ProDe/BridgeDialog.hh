/**
 * @file BridgeDialog.hh
 *
 * Declaration of BridgeDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_BRIDGE_DIALOG_HH
#define TTA_BRIDGE_DIALOG_HH

#include <wx/wx.h>
#include <wx/statline.h>

namespace TTAMachine {
    class Bridge;
    class Machine;
}

/**
 * Dialog for querying bridge parameters from the user.
 */
class BridgeDialog : public wxDialog {
public:
    BridgeDialog(
        wxWindow* parent,
        TTAMachine::Bridge* bridge,
        TTAMachine::Bridge* opposite);
    virtual ~BridgeDialog();

private:
    wxSizer* createContents(wxWindow* parent,
			    bool call_fit,
			    bool set_sizer);

    virtual bool TransferDataToWindow();
    void setTexts();
    void updateBusChoices();
    void onOK(wxCommandEvent&);
    void onCancel(wxCommandEvent&);
    void onHelp(wxCommandEvent& event);
    void onName(wxCommandEvent&);

    void onInputBus(wxCommandEvent&);
    void onOutputBus(wxCommandEvent&);
    void onBidirectional(wxCommandEvent&);

    /// Parent machine of the bridge(s).
    TTAMachine::Machine* machine_;
    /// Name of the bridge.
    wxString name_;
    /// Name of the opposite bridge.
    wxString oppositeName_;

    /// Name of the bridge input bus.
    wxString inputBus_;
    /// Name of the bridge output bus.
    wxString outputBus_;

    /// Opposite name control.
    wxTextCtrl* oppositeNameCtrl_;
    /// Input bus choice control.
    wxChoice* inputBusChoice_;
    /// Output bus choice control.
    wxChoice* outputBusChoice_;
    /// Indicates whether the bridge is bidirectional.
    wxCheckBox* bidirectionalBox_;

    /// Indicates whether we are adding a bridge or not.
    bool adding_;
    /// Indicates whether we are modifying a bidirectional bridge or not.
    bool bidirectional_;

    // IDs for dialog controls
    enum {
        ID_NAME = 10000,
        ID_OUTPUT_BUS,
        ID_INPUT_BUS,
        ID_OPPOSITE_BRIDGE,
        ID_BIDIRECTIONAL,
        ID_HELP,
        ID_LABEL_NAME,
        ID_LABEL_OPPOSITE_NAME,
        ID_LABEL_INPUT_BUS,
        ID_LABEL_OUTPUT_BUS,
        ID_LINE
    };

    /// The event table for the controls of the dialog.
    DECLARE_EVENT_TABLE()
};
#endif
