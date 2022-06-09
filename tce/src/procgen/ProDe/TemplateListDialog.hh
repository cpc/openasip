/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file TemplateListDialog.hh
 *
 * Declaration of TemplateListDialog class.
 *
 * @author Veli-Pekka Jääskeläinen 2004 (vjaaskel-no.spam-cs.tut.fi)
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
