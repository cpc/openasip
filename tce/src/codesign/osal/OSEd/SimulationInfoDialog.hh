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
