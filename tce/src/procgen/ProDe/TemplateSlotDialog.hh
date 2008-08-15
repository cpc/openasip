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
 * @file TemplateSlotDialog.hh
 *
 * Declaration of TemplateSlotDialog.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_TEMPLATE_SLOT_DIALOG_HH
#define TTA_TEMPLATE_SLOT_DIALOG_HH

#include <wx/wx.h>
#include <wx/spinctrl.h>

namespace TTAMachine {
    class InstructionTemplate;
    class Bus;
    class ImmediateUnit;
}

/**
 * Dialog for editing telplate slot properties.
 */
class TemplateSlotDialog : public wxDialog {
public:
    TemplateSlotDialog(
        wxWindow* parent,
        TTAMachine::InstructionTemplate* it,
        TTAMachine::Bus* slot = NULL);

    virtual ~TemplateSlotDialog();

private:
    wxSizer* createContents(wxWindow *parent, bool call_fit, bool set_sizer);
    virtual bool TransferDataToWindow();
    void onOK(wxCommandEvent& event);
    void setTexts();

    /// Instruction template slot to edit.
    TTAMachine::Bus* slot_;
    /// Parent instruction template of the slot.
    TTAMachine::InstructionTemplate* it_;
    /// Choice control for the slot.
    wxChoice* slotChoice_;
    /// Choice control fot the destination IU.
    wxChoice* destinationChoice_;
    /// Spin control for the width.
    wxSpinCtrl* widthCtrl_;
    /// Bit width of the slot.
    int width_;
    /// Destination immediate unit.
    TTAMachine::ImmediateUnit* destination_;

    // enumerated IDs for dialog widgets
    enum {
        ID_LABEL_SLOT = 10000,
        ID_SLOT,
        ID_LABEL_WIDTH,
        ID_WIDTH,
        ID_LABEL_DESTINATION,
        ID_DESTINATION,
        ID_LINE,
        ID_HELP
    };

    DECLARE_EVENT_TABLE()
};
#endif
