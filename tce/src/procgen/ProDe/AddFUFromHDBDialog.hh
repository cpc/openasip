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
 * @file AddFUFromHDBDialog.hh
 *
 * Declaration of AddFUFromHDBDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_ADD_FU_FROM_HDB_DIALOG_HH
#define TTA_ADD_FU_FROM_HDB_DIALOG_HH

#include <map>
#include <wx/wx.h>
#include <wx/listctrl.h>
#include "Machine.hh"

class Model;

namespace HDB {
    class FUArchitecture;
    class HDBManager;
}

/**
 * Dialog for adding register file architectures directly from HDB to the
 * current machine.
 */
class AddFUFromHDBDialog : public wxDialog {
public:
    AddFUFromHDBDialog(wxWindow* parent, Model* model);
    virtual ~AddFUFromHDBDialog();

private:
    virtual bool TransferDataToWindow();
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    void onListSelectionChange(wxListEvent& event);
    void onAdd(wxCommandEvent& event);
    void onClose(wxCommandEvent& event);
    bool loadHDB(const HDB::HDBManager& manager);

    /// Model of the current adf file.
    Model* model_;
    /// Immediate slot list widget.
    wxListCtrl* list_;
    /// Map of iu architectures displayed in the dialog list.
    std::map<int, HDB::FUArchitecture*> fuArchitectures_;

    enum {
	ID_LIST = 10000,
	ID_ADD,
	ID_CLOSE,
	ID_LINE
    };

    /// File filter for HDB files.
    static const wxString HDB_FILE_FILTER;

    DECLARE_EVENT_TABLE()
};
#endif
