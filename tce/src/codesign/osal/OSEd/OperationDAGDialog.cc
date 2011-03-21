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
 * @file OperationDAGEditor.cc
 *
 * Declaration of OperationDAGEditor class.
 *
 * @author Tero Ryynänen 2008 (tero.ryynanen-no.spam-tut.fi)
 * @note rating: red
 */

#include <wx/valgen.h>
#include <boost/format.hpp>
#include <fstream>
#include <iostream>
#include <string>

#include "OperationDAGDialog.hh"
#include "OperationContainer.hh"
#include "WxConversion.hh"
#include "WidgetTools.hh"
#include "InputOperandDialog.hh"
#include "OutputOperandDialog.hh"
#include "OSEdConstants.hh"
#include "Application.hh"
#include "GUITextGenerator.hh"
#include "OSEdTextGenerator.hh"
#include "ErrorDialog.hh"
#include "ConfirmDialog.hh"
#include "CommandThread.hh"
#include "DialogPosition.hh"
#include "OSEd.hh"
#include "StringTools.hh"
#include "OperationIndex.hh"
#include "Operation.hh"
#include "Operand.hh"
#include "OperationModule.hh"
#include "ObjectState.hh"
#include "WarningDialog.hh"
#include "OperationDAG.hh"
#include "TCEString.hh"
#include "FileSystem.hh"

using std::string;
using std::vector;
using std::ifstream;
using boost::format;

BEGIN_EVENT_TABLE(OperationDAGDialog, wxDialog)

    EVT_BUTTON(ID_SAVE_DAG_BUTTON, OperationDAGDialog::onSaveDAG)
    EVT_BUTTON(ID_UNDO_DAG_BUTTON, OperationDAGDialog::onUndoDAG)
    EVT_BUTTON(ID_DELETE_DAG_BUTTON, OperationDAGDialog::onDeleteDAG)
    EVT_BUTTON(ID_NEW_DAG_BUTTON, OperationDAGDialog::onNewDAG)
    EVT_BUTTON(ID_OK_BUTTON, OperationDAGDialog::onOK)
    EVT_BUTTON(wxID_CANCEL, OperationDAGDialog::onCancel)
    EVT_TEXT(ID_EDIT_DAG, OperationDAGDialog::onDAGChange)
    EVT_CHOICE(ID_INDEX_DAG, OperationDAGDialog::onComboBoxChange)

END_EVENT_TABLE()

/**
 * Constructor.
 *
 * @param parent The parent window.
 * @param op Operation to be created or modified.
 * @param module Module in which operation belongs to.
 * @param path Path in which module belongs to.
 */
 OperationDAGDialog::OperationDAGDialog(
     wxWindow* parent,
     Operation* op ):
     wxDialog(
         parent, -1, _T(""), 
         DialogPosition::getPosition(DialogPosition::DIALOG_PROPERTIES), 
         wxDefaultSize, wxRESIZE_BORDER),
     operation_(op), 
     operationWasCreatedHere_(false),
     dotInstalled_(false) {
    
    createContents(this, true, true);

    
    dagEdit_ = dynamic_cast<wxTextCtrl*>(FindWindow(ID_EDIT_DAG));

    dagIndex_ =
        dynamic_cast<wxChoice*>(FindWindow(ID_INDEX_DAG));

    //FindWindow(ID_NAME)->SetValidator(
     //   wxTextValidator(wxFILTER_ASCII, &name_));

    // set OK button as default choice
    // FindWindow(ID_OK_BUTTON)->SetFocus();

    orig_ = operation_->saveState(); // save original operation's state

    setTexts();

    FindWindow(ID_SAVE_DAG_BUTTON)->Disable();
    FindWindow(ID_UNDO_DAG_BUTTON)->Disable();
    FindWindow(ID_DELETE_DAG_BUTTON)->Disable();
    FindWindow(ID_NEW_DAG_BUTTON)->Disable();

    updateIndex();

    dotInstalled_ = hasDot();

    updateDAG();
}

/**
 * Destructor.
 */
OperationDAGDialog::~OperationDAGDialog() {

    // set dialog position
    int x, y;
    GetPosition(&x, &y);
    wxPoint point(x, y);
    DialogPosition::setPosition(DialogPosition::DIALOG_PROPERTIES, point);

    if (operationWasCreatedHere_) {
        delete operation_;
        operation_ = NULL;
    }
}

/**
 * Returns operation object of dialog.
 * 
 * @return Operation object of dialog.
 */
Operation* 
OperationDAGDialog::operation() const {
    return operation_;
}

/**
 * Set texts to all widgets.
 */
void
OperationDAGDialog::setTexts() {
	
    //GUITextGenerator& guiText = *GUITextGenerator::instance();
    OSEdTextGenerator& osedText = OSEdTextGenerator::instance();

    // title
    format fmt = osedText.text(OSEdTextGenerator::TXT_PROPERTY_DIALOG_TITLE);
    SetTitle(WxConversion::toWxString(fmt.str()));

    WidgetTools::setLabel(
        &osedText, FindWindow(ID_SAVE_DAG_BUTTON),
        OSEdTextGenerator::TXT_BUTTON_SAVE);
    WidgetTools::setLabel(
        &osedText, FindWindow(ID_UNDO_DAG_BUTTON),
        OSEdTextGenerator::TXT_BUTTON_UNDO);
    WidgetTools::setLabel(
        &osedText, FindWindow(ID_DELETE_DAG_BUTTON),
        OSEdTextGenerator::TXT_BUTTON_DELETE);
    WidgetTools::setLabel(
        &osedText, FindWindow(ID_NEW_DAG_BUTTON),
        OSEdTextGenerator::TXT_BUTTON_NEW);
	
}

/**
 * Transfers data to window.
 *
 * @return True if transfer is successful.
 */
bool
OperationDAGDialog::TransferDataToWindow() {
    return wxWindow::TransferDataToWindow();
}

/**
 * Updates the DAG list.
 */
void
OperationDAGDialog::updateIndex() {

    dagIndex_->Clear();

    if (operation_ == NULL) {
        return;
    }

    if (operation_->dagCount() > 0) {
        for (int i = 0; i < operation_->dagCount(); i++) {
            wxString oper = WxConversion::toWxString(i+1);
            dagIndex_->Append(oper);
        }
    }

    // last item is "New Dag"-option
    wxString oper = wxT("New DAG");
    dagIndex_->Append(oper);
    dagIndex_->SetSelection(0);

}

/**
 * Tests whether user has program dot is installed.
 *
 * @return True if dot is installed.
 */
bool
OperationDAGDialog::hasDot() {

    if (system("which dot > /dev/null 2>&1") != 0) {
        wxString caption(wxString::FromAscii("Warning"));
        wxString message(
                wxString::FromAscii(
                    "You must have dot installed to display DAGs."));
        wxMessageBox(message, caption);
        dagImageStaticBoxSizer_->Show(false);
        Layout();
        return false; // cannot display DAG without dot
    } else {
        return true;
    }
}

/**
 * Updates TextCtrl for DAG
 */
void
OperationDAGDialog::updateDAG() {

    dagEdit_->Clear();

    // get selected item from ComboBox
    int index = dagIndex_->GetSelection();
    
    // check that index is legal
    if (operation_ != NULL && index < operation_->dagCount()) {
        std::string code = operation_->dagCode(index);
        std::ostream dagCode(dagEdit_);
        dagCode << code;
        dagCode.flush();

        FindWindow(ID_UNDO_DAG_BUTTON)->Disable();
        FindWindow(ID_SAVE_DAG_BUTTON)->Disable();
        FindWindow(ID_DELETE_DAG_BUTTON)->Enable();
        FindWindow(ID_NEW_DAG_BUTTON)->Enable();

        OperationDAG& currentDAG = operation_->dag(index);
        if (operation_->dagError(index) != "") {
            wxString message(
                wxString::FromAscii(
                    operation_->dagError(index).c_str()));
            wxString caption(wxString::FromAscii("Warning"));
            wxMessageBox(message, caption);
            dagImageStaticBoxSizer_->Show(false);
            Layout();
        }

        std::string temp = FileSystem::createTempDirectory();
        std::string pngDag = temp + "/dag.png";
        std::string dotDag = temp + "/dag.dot";

        if( !dotInstalled_ )
        {
            // cannot show DAG without dot
            dagImageStaticBoxSizer_->Show(false);
            Layout();
            return;
        }

        if (currentDAG.isNull() == true) { // compile error

            std::string errText = operation_->dagError(index);
            
            // strip carriage returns from errText
            while (true) {
                const int pos = errText.find('\n');
                if (pos == -1) {
                    break;
                }
                errText.replace(pos, 1,"\\n");
            }

            // command to generate error message
            std::string dotCmd = 
                std::string("echo 'digraph G {n140545368 [label=\"") + errText + 
                "\", shape=plaintext, fontsize=12]; }' | dot -Tpng > " + pngDag;

            if (system(dotCmd.c_str()) != 0) {
                debugLog("Error executing dot.");
            }
	      
            delete dotImage_;
            dotImage_ = new wxBitmap(100,100);
            wxString wxTemp(wxString::FromAscii(pngDag.c_str()));
            dotImage_->LoadFile(wxTemp, wxBITMAP_TYPE_PNG);
            dagStaticBitmap_->SetBitmap(*dotImage_);

            unsigned int width = dotImage_->GetWidth();
            unsigned int height = dotImage_->GetHeight();
            
            // calculate virtual area and rounding the result up
            width = static_cast<unsigned int>(float(width) / 20 + 0.5);
            height = static_cast<unsigned int>(float(height) / 20 + 0.5);

            dagWindow_->SetSize(-1, -1, width, height);
            dagWindow_->SetScrollbars(20, 20, -1, -1);
            dagImageStaticBoxSizer_->Show(true);

        } else {

            // generate dot
            currentDAG.writeToDotFile(dotDag);

            // generate png from dot
            std::string dotCmd = "dot -Tpng " + dotDag + " > " + pngDag;
            if (system(dotCmd.c_str()) != 0) {
                debugLog("Error executing dot.");
            }

            delete dotImage_;
            dotImage_ = new wxBitmap(300,100);
            wxString wxTemp(wxString::FromAscii(pngDag.c_str()));
            dotImage_->LoadFile(wxTemp, wxBITMAP_TYPE_PNG);
            dagStaticBitmap_->SetBitmap(*dotImage_);
            dagImageStaticBoxSizer_->Show(true);

            unsigned int width = dotImage_->GetWidth();
            unsigned int height = dotImage_->GetHeight();
            
            // calculate virtual area and rounding the result up
            width = static_cast<unsigned int>(float(width) / 20 + 0.5);
            height = static_cast<unsigned int>(float(height) / 20 + 0.5);

            dagWindow_->SetSize(50, 50);
            dagWindow_->SetScrollbars(20, 20, width, height);
        }
        FileSystem::removeFileOrDirectory(temp);

    } else {

        // new dag => do not update, just disable the buttons
        FindWindow(ID_UNDO_DAG_BUTTON)->Disable();
        FindWindow(ID_SAVE_DAG_BUTTON)->Disable();
        FindWindow(ID_DELETE_DAG_BUTTON)->Disable();
        FindWindow(ID_NEW_DAG_BUTTON)->Disable();

        dagImageStaticBoxSizer_->Show(false);
    }
    Layout();
}

/**
 * Handles the event when OK button is pushed.
 */
void 
OperationDAGDialog::onOK(wxCommandEvent&) {
    EndModal(wxID_OK);
}


/**
 * Handles the event when Cancel button is pushed.
 */
void 
OperationDAGDialog::onCancel(wxCommandEvent&) {
    operation_->loadState(orig_);
    EndModal(wxID_CANCEL);
}

/**
 * Handles the event when Save button is pushed.
 */
void
OperationDAGDialog::onSaveDAG(wxCommandEvent&) {
    if(operation_ == NULL) {
        return;
    }

    int index = dagIndex_->GetSelection();

    std::string code("");
    wxString wxTemp;

    for (int i = 0; i < dagEdit_->GetNumberOfLines(); ++i) {
        wxTemp = dagEdit_->GetLineText(i);

#if wxCHECK_VERSION(2, 6, 0)
        std::string stdTemp(wxTemp.mb_str());
#else
        std::string stdTemp(wxTemp.c_str());
#endif

        code += stdTemp;
        code += '\n';
    }

    if (operation_->dagCount() == index) {
        operation_->addDag(code);
    } else {
        operation_->setDagCode(index, code.c_str());
    }

    updateIndex();
    dagIndex_->SetSelection(index);
    updateDAG();
    FindWindow(ID_SAVE_DAG_BUTTON)->Disable();
    FindWindow(ID_UNDO_DAG_BUTTON)->Disable();
    FindWindow(ID_DELETE_DAG_BUTTON)->Enable();
}

/**
 * Handles the event when Undo button is pushed.
 */
void
OperationDAGDialog::onUndoDAG(wxCommandEvent&) {
    std::iostream dagCode(dagEdit_);

    int index = dagIndex_->GetSelection();
    if (operation_->dagCount() == index) { // new operation
        dagEdit_->Clear();
        FindWindow(ID_DELETE_DAG_BUTTON)->Disable();
    } else {
        updateDAG();
        FindWindow(ID_DELETE_DAG_BUTTON)->Enable();
    }
    FindWindow(ID_SAVE_DAG_BUTTON)->Disable();
    FindWindow(ID_UNDO_DAG_BUTTON)->Disable();
}

/**
 * Handles the event when Delete button is pushed.
 */
void
OperationDAGDialog::onDeleteDAG(wxCommandEvent&) {
    int index = dagIndex_->GetSelection();
    if (operation_->dagCount() > index) {
        dagEdit_->Clear();
        operation_->removeDag(index);
    }

    updateIndex();

    if (index > 0) {
        index--;
    }

    dagIndex_->SetSelection(index);
    updateDAG();
}

/**
 * Handles the event when New button is pushed.
 */
void
OperationDAGDialog::onNewDAG(wxCommandEvent&) {
    int index = operation_->dagCount();
    dagEdit_->Clear();
    dagIndex_->SetSelection(index);
    FindWindow(ID_SAVE_DAG_BUTTON)->Disable();
    FindWindow(ID_UNDO_DAG_BUTTON)->Disable();
    FindWindow(ID_DELETE_DAG_BUTTON)->Disable();
    FindWindow(ID_NEW_DAG_BUTTON)->Disable();
    dagImageStaticBoxSizer_->Show(false);
}

/**
 * Handles the event when DAG is changed
 */
void
OperationDAGDialog::onDAGChange(wxCommandEvent&) {
    FindWindow(ID_UNDO_DAG_BUTTON)->Enable();
    FindWindow(ID_SAVE_DAG_BUTTON)->Enable();
}

/**
 * Handles the event when ComboBox is changed
 */
void
OperationDAGDialog::onComboBoxChange(wxCommandEvent&) {
    updateDAG();
}

/**
 * Creates the contents of the dialog.
 *
 * @param parent Parent window.
 * @param call_fit If true, fits the contents inside the dialog.
 * @param set_sizer If true, sets the main sizer as dialog contents.
 * @return The created sizer.
 */
wxSizer*
OperationDAGDialog::createContents(
    wxWindow* parent, 
    bool call_fit, 
    bool set_sizer) {

    // Window for scrolling DAG image
    wxSize size(300, 400);
    dagWindow_ = new wxScrolledWindow(parent, -1, wxDefaultPosition, size);

    // Window sizers
    wxBoxSizer *item0 = new wxBoxSizer(wxVERTICAL);

    // Static box for DAG code
    wxStaticBox *dagStaticBox = new wxStaticBox(parent, -1, wxT("Code"));
    wxStaticBoxSizer *dagStaticBoxSizer =
        new wxStaticBoxSizer(dagStaticBox, wxVERTICAL);

    // DAG editor
    wxTextCtrl* editDAG =
        new wxTextCtrl(
                parent, ID_EDIT_DAG, wxT(""), wxDefaultPosition,
                wxSize(250,365), wxTE_MULTILINE);

    // Static box for DAG image
    wxStaticBox *dagImageStaticBox = new wxStaticBox(parent, -1, wxT("DAG"));
    dagImageStaticBoxSizer_ =
        new wxStaticBoxSizer(dagImageStaticBox, wxVERTICAL);

    dagImageStaticBoxSizer_->Add(dagWindow_, 5, wxALIGN_TOP|wxALL|wxGROW, 5);
    dagImageStaticBoxSizer_->SetDimension(-1, -1, 550, 550);

    // DAG image
    dotImage_ = new wxBitmap(300,100);
    dagStaticBitmap_ =
        new wxStaticBitmap(dagWindow_, ID_DAG_IMAGE, *dotImage_);
    
    dagWindow_->SetSize(-1, -1, 50, 50);
    dagWindow_->SetScrollbars(20, 20, 20, 20);
    dagImageStaticBoxSizer_->Show(true);

    // Add DAG editor to DAG code sizer
    dagStaticBoxSizer->Add(editDAG, 0, wxALIGN_TOP|wxALL|wxGROW, 5);
    
    // Add DAG code to page sizer
    wxBoxSizer *pageSizer = new wxBoxSizer(wxHORIZONTAL);
    pageSizer->Add(dagStaticBoxSizer, 0, wxALIGN_TOP|wxTOP, 10);
    pageSizer->Add(dagImageStaticBoxSizer_, 0, wxALIGN_TOP|wxTOP, 10);

    // Add page sizer to window sizer
    item0->Add(pageSizer, 0, wxALIGN_CENTER|wxALL, 5);

    wxString strs9[] = 
        {
            wxT("id: 1")
        };


    // ComboBox for choosing DAG
    wxChoice *indexDAG =
        new wxChoice(
                parent, ID_INDEX_DAG, wxDefaultPosition, wxSize(150,-1), 1,
                strs9);
    wxButton *saveDAG = 
        new wxButton(
                parent, ID_SAVE_DAG_BUTTON, wxT("Save"), wxDefaultPosition,
                wxDefaultSize, 0);
    wxButton *undoDAG = 
        new wxButton(
                parent, ID_UNDO_DAG_BUTTON, wxT("Undo"), wxDefaultPosition,
                wxDefaultSize, 0);
    wxButton *deleteDAG = 
        new wxButton(
                parent, ID_DELETE_DAG_BUTTON, wxT("Delete"), wxDefaultPosition,
                wxDefaultSize, 0);
    wxButton *newDAG = 
        new wxButton(
                parent, ID_NEW_DAG_BUTTON, wxT("New"), wxDefaultPosition,
                wxDefaultSize, 0);

    wxBoxSizer *dagToolsSizer = new wxBoxSizer(wxHORIZONTAL);
    dagToolsSizer->Add(indexDAG, 0, wxALIGN_CENTER|wxALL, 5);

    wxGridSizer *dagButtonSizer = new wxGridSizer(2, 0, 0);

    dagButtonSizer->Add(saveDAG, 0, wxALIGN_CENTER|wxALL, 5);
    dagButtonSizer->Add(undoDAG, 0, wxALIGN_CENTER|wxALL, 5);
    dagButtonSizer->Add(deleteDAG, 0, wxALIGN_CENTER|wxALL, 5);
    dagButtonSizer->Add(newDAG, 0, wxALIGN_CENTER|wxALL, 5);

    dagToolsSizer->Add(dagButtonSizer, 0, wxALIGN_CENTER|wxALL, 5);
    dagStaticBoxSizer->Add(dagToolsSizer, 0, wxALIGN_TOP|wxALL, 5);

    wxBoxSizer *controlButtonSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton *OK =
        new wxButton(
                parent, ID_OK_BUTTON, wxT("Ok"), wxDefaultPosition,
                wxDefaultSize, 0);
    wxButton *Cancel =
        new wxButton(
                parent, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition,
                wxDefaultSize, 0);

    controlButtonSizer->Add(OK, 0, wxALIGN_RIGHT|wxALL, 5);
    controlButtonSizer->Add(Cancel, 0, wxALIGN_RIGHT|wxALL, 5);
    item0->Add(
            controlButtonSizer, 0,
            wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    if (set_sizer) {
        parent->SetSizer(item0);
        if (call_fit) {
            item0->SetSizeHints(parent);
        }
    }

    
    return item0;
}

