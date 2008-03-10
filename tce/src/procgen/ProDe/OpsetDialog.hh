/**
 * @file OpsetDialog.hh
 *
 * Declaration of OpsetDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OPSET_DIALOG_HH
#define TTA_OPSET_DIALOG_HH

#include <wx/wx.h>

namespace TTAMachine {
    class FunctionUnit;
    class HWOperation;
}

/**
 * Dialog for selecting and creating HWOperation from the operation pool.
 */
class OpsetDialog : public wxDialog {
public:
    OpsetDialog(wxWindow* parent);
    virtual ~OpsetDialog();
    TTAMachine::HWOperation* createOperation(TTAMachine::FunctionUnit& fu);
protected:
    virtual bool TransferDataToWindow();
    virtual bool TransferDataFromWindow();
private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    void onSelectOperation(wxCommandEvent& event);
    void onOK(wxCommandEvent& event);

    /// Chosen latency.
    int latency_;
    /// Name of the selected operation.
    std::string operation_;
    /// Operation list widget.
    wxListBox* operationList_;

    /// Widget IDs.
    enum {
        ID_LIST = 20000,
        ID_LATENCY,
        ID_TEXT,
        ID_LINE
    };

    DECLARE_EVENT_TABLE()
};
#endif
