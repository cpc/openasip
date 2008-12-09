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
 * @file ProDeBusOrderDialog.hh
 *
 * Declaration of ProDeBusOrderDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PRODE_BUS_ORDER_DIALOG_HH
#define TTA_PRODE_BUS_ORDER_DIALOG_HH

#include <wx/wx.h>
#include <wx/listctrl.h>

namespace TTAMachine {
    class Machine;
}

class wxListCtrl;

/**
 * Dialog for modifying order of transport buses in a machine.
 */
class ProDeBusOrderDialog : public wxDialog {
public:
    ProDeBusOrderDialog(wxWindow* parent, TTAMachine::Machine& machine);
    virtual ~ProDeBusOrderDialog();
private:
    void onUp(wxCommandEvent& event);
    void onDown(wxCommandEvent& event);
    void onBusSelectionChanged(wxListEvent& event);
    void updateBusList();
    int selectedBus() const;
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);

    // Widget IDs.
    enum {
        ID_UP = 10000,
        ID_DOWN,
        ID_LINE,
        ID_LIST
    };

    /// Machine containing the buses to sort.
    TTAMachine::Machine& machine_;
    /// List widget for the buses.
    wxListCtrl* list_;

    DECLARE_EVENT_TABLE()
};

#endif
