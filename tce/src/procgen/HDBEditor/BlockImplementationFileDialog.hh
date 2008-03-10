/**
 * @file BlockImplementationFileDialog.hh
 *
 * Declaration of BlockImplementationFileDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_BLOCK_IMPLEMENTATION_FILE_DIALOG_HH
#define TTA_BLOCK_IMPLEMENTATION_FILE_DIALOG_HH

#include <wx/wx.h>

#include "FUImplementation.hh"


/**
 * Dialog for editing BlockImplementationFile objects.
 */
class BlockImplementationFileDialog : public wxDialog {
public:
    BlockImplementationFileDialog(
        wxWindow* parent, wxWindowID id,
        HDB::BlockImplementationFile& file);

    virtual ~BlockImplementationFileDialog();
private:
    void onBrowse(wxCommandEvent& event);
    void onOK(wxCommandEvent& event);

    /// Enumerated IDs for dialog widgets.
    enum {
        ID_LABEL_PATH = 10000,
        ID_PATH,
        ID_LABEL_FORMAT,
        ID_FORMAT,
        ID_BROWSE,
        ID_LINE
    };

    /// Creates the dialog contents.
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);

    /// BlockImplementationFile object to modify.
    HDB::BlockImplementationFile& file_;

    /// Block implementation file path.
    wxString path_;
    /// Selected implementation file type.
    int format_;

    DECLARE_EVENT_TABLE()
};

#endif
