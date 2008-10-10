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
 * @file BusDialog.hh
 *
 * Declaration of BusDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 */

#ifndef TTA_BUS_DIALOG_HH
#define TTA_BUS_DIALOG_HH

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/spinctrl.h>
#include <wx/valgen.h>
#include <vector>

using std::vector;

namespace TTAMachine {
    class Bus;
    class RegisterGuard;
    class PortGuard;
    class Segment;
}

/**
 * Dialog for querying transport bus parameters from the user.
 */
class BusDialog : public wxDialog {
public:
    BusDialog(wxWindow* parent, TTAMachine::Bus* bus);
    virtual ~BusDialog();

private:
    virtual bool TransferDataToWindow();
    void onOK(wxCommandEvent&);
    void onHelp(wxCommandEvent& event);
    void onBusName(wxCommandEvent&);
    void onAddSegment(wxCommandEvent&);
    void onSegmentName(wxCommandEvent&);
    void onSegmentSelection(wxListEvent&);
    void onSegmentRightClick(wxListEvent& event);
    void onDeleteSegment(wxCommandEvent&);
    void onSegmentUp(wxCommandEvent&);
    void onSegmentDown(wxCommandEvent&);
    void setTexts();

    void onRFGuardSelection(wxListEvent&);
    void onAddRFGuard(wxCommandEvent&);
    void onDeleteRFGuard(wxCommandEvent&);
    void onEditRFGuard(wxCommandEvent&);
    void onFUGuardSelection(wxListEvent&);
    void onAddFUGuard(wxCommandEvent&);
    void onDeleteFUGuard(wxCommandEvent&);
    void onEditFUGuard(wxCommandEvent&);
    void onUnconditionalGuard(wxCommandEvent&);
    void onWidth(wxSpinEvent&);
    void updateSegmentList();
    void updateGuardLists();

    void onRFGuardRightClick(wxListEvent& event);
    void onActivateRFGuard(wxListEvent& event);
    void onFUGuardRightClick(wxListEvent& event);
    void onActivateFUGuard(wxListEvent& event);

    TTAMachine::Segment* selectedSegment() const;
    TTAMachine::RegisterGuard* selectedRFGuard() const;
    TTAMachine::PortGuard* selectedFUGuard() const;
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);

    /// Transport Bus to edit.
    TTAMachine::Bus* bus_;
    /// Name of the bus.
    wxString name_;
    /// Width of the bus.
    int width_;
    /// Short immediate width
    int immWidth_;
    /// Name for a new segment.
    wxString newSegName_;

    /// List widget for the segment list.
    wxListCtrl* segList_;
    /// Check box for always true unconditional guard.
    wxCheckBox* alwaysTrueGuard_;
    /// Check box for always false unconditional guard.
    wxCheckBox* alwaysFalseGuard_;
    /// Radio box for immediate extension.
    wxRadioBox* immediateExtensionBox_;
    /// Register file guard list control.
    wxListCtrl* rfGuardList_;
    /// Function unit port guard list control.
    wxListCtrl* fuGuardList_;

    /// Static boxsizer for the bus properties.
    wxStaticBoxSizer* busSizer_;
    /// Static boxsizer for the short immediate properties.
    wxStaticBoxSizer* siSizer_;
    /// Static boxsizer for the register guard properties.
    wxStaticBoxSizer* registerGuardSizer_;
    /// Static boxsizer for the port guard properties.
    wxStaticBoxSizer* portGuardSizer_;
    /// Static boxsizer for the segment properties.
    wxStaticBoxSizer* segmentSizer_;


    /// list of register guards
    vector<TTAMachine::RegisterGuard*> registerGuards_;
    /// list of port guards
    vector<TTAMachine::PortGuard*> portGuards_;

    /// enumerated IDs for dialog controls
    enum {
        ID_BUS_NAME = 10000,
        ID_BUS_WIDTH,
        ID_SI_WIDTH,
        ID_SI_EXTENSION,
        ID_TRUE_GUARD,
        ID_FALSE_GUARD,
        ID_SEGMENT_LIST,
        ID_SEGMENT_NAME,
        ID_SEGMENT_UP,
        ID_SEGMENT_DOWN,
        ID_ADD_SEGMENT,
        ID_DELETE_SEGMENT,
        ID_RF_GUARD_LIST,
        ID_ADD_RF_GUARD,
        ID_EDIT_RF_GUARD,
        ID_DELETE_RF_GUARD,
        ID_FU_GUARD_LIST,
        ID_ADD_FU_GUARD,
        ID_EDIT_FU_GUARD,
        ID_DELETE_FU_GUARD,
        ID_HELP,
        ID_LABEL_BUS_NAME,
        ID_LABEL_BUS_WIDTH,
        ID_LABEL_WIDTH,
        ID_LABEL_SI_WIDTH,
        ID_LABEL_SEGMENT_NAME
    };

    /// The event table for the controls of the dialog.
    DECLARE_EVENT_TABLE()
};
#endif
