/*
    Copyright (c) 2002-2009 Tampere University of Technology.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file AddRFFromHDBDialog.hh
 *
 * Declaration of AddRFFromHDBDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_ADD_RF_FROM_HDB_DIALOG_HH
#define TTA_ADD_RF_FROM_HDB_DIALOG_HH

#include <map>
#include <wx/wx.h>
#include <wx/listctrl.h>

class Model;

namespace HDB {
    class RFArchitecture;
    class HDBManager;
}

/**
 * Dialog for adding register file architectures directly from HDB to the
 * current machine.
 */
class AddRFFromHDBDialog : public wxDialog {
public:
    AddRFFromHDBDialog(wxWindow* parent, Model* model);
    virtual ~AddRFFromHDBDialog();

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
    /// Map of rf architectures displayed in the dialog list.
    std::map<int, HDB::RFArchitecture*> rfArchitectures_;

    enum {
	ID_LIST = 10000,
	ID_ADD,
	ID_CLOSE,
	ID_LINE
    };

    /// Default size for the rf, if the size is parameterized in the HDB.
    static const int DEFAULT_SIZE;
    /// Default bit width for the rf, if the size is parameterized in the HDB.
    static const int DEFAULT_WIDTH;
    /// File filter for the HDB files.
    static const wxString HDB_FILE_FILTER;

    DECLARE_EVENT_TABLE()
};
#endif
