/**
 * @file ResultDialog.hh
 *
 * Declaration of ResultDialog class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_RESULT_DIALOG_HH
#define TTA_RESULT_DIALOG_HH

#include <string>
#include <vector>
#include <wx/wx.h>
#include <wx/textctrl.h>

/**
 * Dialog that shows operation build results.
 */
class ResultDialog : public wxDialog {
public:
    ResultDialog(
        wxWindow* parent, 
        std::vector<std::string> output,
        const std::string& title,
        const std::string& module = "");
    virtual ~ResultDialog();

private:
    /// Copying not allowed.
    ResultDialog(const ResultDialog&);
    /// Assignment not allowed.
    ResultDialog operator=(const ResultDialog&);
    
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    void setTexts();
    virtual bool TransferDataToWindow();
	void onOpen(wxCommandEvent& event);

    /**
     * Widget ids.
     */
    enum {
        ID_RESULT,
        ID_BUTTON_OPEN
    };

    /// Result window pointer.
    wxTextCtrl* result_;
    /// Output of compilation.
    std::vector<std::string> output_;
    /// Module which was compiled or empty string, if all modules were
    /// compiled.
    std::string module_;

    DECLARE_EVENT_TABLE()
};

#endif
