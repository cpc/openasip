/**
 * @file AddModuleDialog.hh
 *
 * Declaration of AddModuleDialog class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_ADD_MODULE_DIALOG_HH
#define TTA_ADD_MODULE_DIALOG_HH

#include <wx/wx.h>
#include <string>

/**
 * Dialog for adding new modules.
 */
class AddModuleDialog : public wxDialog {
public:
    AddModuleDialog(wxWindow* parent, std::string path);
    virtual ~AddModuleDialog();

    std::string name() const;

private:
    /// Copying not allowed.
    AddModuleDialog(const AddModuleDialog&);
    /// Assignment not allowed.
    AddModuleDialog& operator=(const AddModuleDialog&);

    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    void onOk(wxCommandEvent& event);
    void setTexts();
    
    /**
     * Widget ids.
     */
    enum {
        ID_TEXT,
        ID_MODULE_NAME
    };
    
    /// Path in which module is added.
    std::string path_;
    /// Name of the module.
    wxString name_;
    
    DECLARE_EVENT_TABLE()
};

#endif
