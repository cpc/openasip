/**
 * @file BlockImplementationDialog.hh
 *
 * Declaration of BlockImplementationDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_BLOCK_IMPLEMENTATION_DIALOG_HH
#define TTA_BLOCK_IMPLEMENTATION_DIALOG_HH

#include <set>
#include <string>
#include <wx/wx.h>

namespace TTAMachine {
    class Component;
}

namespace IDF {
    class UnitImplementationLocation;
}

class BlockImplementationDialog : public wxDialog {
public:
    BlockImplementationDialog(
        wxWindow* parent, const TTAMachine::Component& block,
        IDF::UnitImplementationLocation& impl);

    virtual ~BlockImplementationDialog();

private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    void onBrowse(wxCommandEvent& event);
    void onImplSelection(wxListEvent& event);
    void onOK(wxCommandEvent& event);
    void onHDBSelection(wxCommandEvent& event);

    const TTAMachine::Component& block_;
    IDF::UnitImplementationLocation& impl_;
    wxListCtrl* list_;
    wxChoice* hdbChoice_;

    /// Static set for hdb paths.
    static std::set<std::string> hdbs_;
    /// Static variable for the hdb choicer selection.
    static int selection_;

    enum {
        ID_HDB_CHOICE,
        ID_BROWSE,
        ID_LIST,
        ID_LINE
    };

    DECLARE_EVENT_TABLE()
};

#endif
