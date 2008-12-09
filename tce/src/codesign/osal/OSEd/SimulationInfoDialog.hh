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
 * @file SimulationInfoDialog.hh
 *
 * Declaration of SimulationInfoDialog class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIMULATION_INFO_DIALOG_HH
#define TTA_SIMULATION_INFO_DIALOG_HH

#include <wx/wx.h>
#include <wx/listctrl.h>

#include "SimulateDialog.hh"
#include "OSEdInformer.hh"
#include "OSEdListener.hh"

class OperationContext;

/**
 * Dialog that holds additional information about simulation.
 */
class SimulationInfoDialog : public wxDialog, public OSEdListener {
public:
    SimulationInfoDialog(wxWindow* parent, OperationContext* context);
    virtual ~SimulationInfoDialog();

    virtual void handleEvent(OSEdInformer::EventId id);

private:
    /// Copying not allowed.
    SimulationInfoDialog(const SimulationInfoDialog&);
    /// Assignment not allowed.
    SimulationInfoDialog& operator=(const SimulationInfoDialog&);

    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    virtual bool TransferDataToWindow();
    void setTexts();
    void updateList();

    /**
     * Component ids.
     */
    enum {
        ID_LISTCTRL
    };

    /// List for showing simulation statistics.
    wxListCtrl* infoList_;
    /// Parent window.
    SimulateDialog* parent_;
    /// Operation context for retrieving simulation data.
    OperationContext* context_;
};

#endif
