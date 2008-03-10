/**
 * @file ProDeOptionsDialog.hh
 *
 * Declaration of ProDeOptionsDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PRODE_OPTIONS_DIALOG_HH
#define TTA_PRODE_OPTIONS_DIALOG_HH

#include <string>
#include <wx/wx.h>
#include <wx/spinctrl.h>
#include "OptionsDialog.hh"

class ProDeOptions;
class wxListCtrl;
class CommandRegistry;

/**
 * Dialog for editing ProDeOptions.
 */
class ProDeOptionsDialog : public OptionsDialog {
public:
    ProDeOptionsDialog(
        wxWindow* parent,
        ProDeOptions& options,
        CommandRegistry& registry);

    virtual ~ProDeOptionsDialog();

private:
    void readProDeOptions();
    void writeProDeOptions();
    void onOK(wxCommandEvent& event);
    void onHelp(wxCommandEvent& event);

    wxSizer* createGeneralPage(
        wxWindow* parent, bool callFit, bool set_sizer);

    /// parent window of the dialog
    wxWindow* parent_;
    /// current editor options
    ProDeOptions& options_;

    /// Undo levels.
    wxSpinCtrl* undoStackSize_;

    // control IDs
    enum {
        ID_UNDO_LEVELS,
        ID_LABEL_UNDO_LEVELS
    };

    /// The event table for the controls of the dialog.
    DECLARE_EVENT_TABLE()
};

#endif
