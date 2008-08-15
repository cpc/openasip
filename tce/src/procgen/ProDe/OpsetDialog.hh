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
