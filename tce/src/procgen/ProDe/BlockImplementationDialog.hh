/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
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
