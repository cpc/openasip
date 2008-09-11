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
 * @file TemplateListDialog.hh
 *
 * Declaration of TemplateListDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_TEMPLATE_LIST_DIALOG_HH
#define TTA_TEMPLATE_LIST_DIALOG_HH

#include <wx/wx.h>
#include <wx/listctrl.h>

class wxListCtrl;

namespace TTAMachine {
    class Machine;
    class InstructionTemplate;
    class Bus;
}

/**
 * Dialog for listing and editing instruction templates in a machine.
 */
class TemplateListDialog : public wxDialog {
public:
    TemplateListDialog(wxWindow* parent, TTAMachine::Machine* machine);
    virtual ~TemplateListDialog();

private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    virtual bool TransferDataToWindow();
    void updateSlotList();
    void onTemplateSelection(wxListEvent& event);
    void onSlotSelection(wxListEvent& event);
    void onTemplateName(wxCommandEvent& event);
    void onAddTemplate(wxCommandEvent& event);
    void onDeleteTemplate(wxCommandEvent& event);
    void onAddSlot(wxCommandEvent& event);
    void onEditSlot(wxCommandEvent& event);
    void onDeleteSlot(wxCommandEvent& event);
    void setTexts();
    TTAMachine::InstructionTemplate* selectedTemplate();
    std::string selectedSlot();

    /// Parent machine of the instruction templates.
    TTAMachine::Machine* machine_;
    /// Box sizer around the template list.
    wxStaticBoxSizer* templateSizer_;
    /// Box sizer around the slot list.
    wxStaticBoxSizer* slotSizer_;
    /// Widget for list of templates.
    wxListCtrl* templateList_;
    /// Widget for list of slots in the selected template.
    wxListCtrl* slotList_;
    /// Name of the new template.
    wxString templateName_;

    // enumerated IDs for dialog widgets
    enum {
        ID_TEMPLATE_LIST = 10000,
        ID_SLOT_LIST,
        ID_LINE,
        ID_HELP,
        ID_ADD_TEMPLATE,
        ID_DELETE_TEMPLATE,
        ID_ADD_SLOT,
        ID_EDIT_SLOT,
        ID_DELETE_SLOT,
        ID_NAME,
        ID_LABEL_NAME
    };

    DECLARE_EVENT_TABLE()
};
#endif
