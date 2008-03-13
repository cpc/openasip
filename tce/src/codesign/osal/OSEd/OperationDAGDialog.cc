/**
 * @file OperationDAGEditor.cc
 *
 * Declaration of OperationDAGEditor class.
 *
 * @author Tero Ryynänen 2008 (tero.ryynanen@tut.fi)
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
     operationWasCreatedHere_(false) {
    
    createContents(this, true, true);

    
    editDAG_ = dynamic_cast<wxTextCtrl*>(FindWindow(ID_EDIT_DAG));

    indexDAG_ =
        dynamic_cast<wxChoice*>(FindWindow(ID_INDEX_DAG));

    //FindWindow(ID_NAME)->SetValidator(
     //   wxTextValidator(wxFILTER_ASCII, &name_));

    // set OK button as default choice
    // FindWindow(ID_OK_BUTTON)->SetFocus();

    if (operation_ != NULL) {
    } else {
    }

    orig_ = operation_->saveState(); // save original operation's state

    setTexts();

    FindWindow(ID_SAVE_DAG_BUTTON)->Disable();
    FindWindow(ID_UNDO_DAG_BUTTON)->Disable();
    FindWindow(ID_DELETE_DAG_BUTTON)->Disable();
    FindWindow(ID_NEW_DAG_BUTTON)->Disable();

    updateIndex();
    updateDAG();
    this->Fit();
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

    WidgetTools::setLabel(&osedText, FindWindow(ID_SAVE_DAG_BUTTON),
                              OSEdTextGenerator::TXT_BUTTON_SAVE);
    WidgetTools::setLabel(&osedText, FindWindow(ID_UNDO_DAG_BUTTON),
                              OSEdTextGenerator::TXT_BUTTON_UNDO);
    WidgetTools::setLabel(&osedText, FindWindow(ID_DELETE_DAG_BUTTON),
                              OSEdTextGenerator::TXT_BUTTON_DELETE);
    WidgetTools::setLabel(&osedText, FindWindow(ID_NEW_DAG_BUTTON),
                              OSEdTextGenerator::TXT_BUTTON_NEW);
	
}

/**
 * Transfers data to window.
 *
 * @return True if transfer is successful.
 */
bool
OperationDAGDialog::TransferDataToWindow() {
    if (operation_ != NULL) {
        //name_ = WxConversion::toWxString(operation_->name());
    } else {
        // no operation
    }

    return wxWindow::TransferDataToWindow();
}

/**
 * Updates the DAG list.
 */
void
OperationDAGDialog::updateIndex() {

    indexDAG_->Clear();

    if (operation_ == NULL) {
        return;
    }

    if (operation_->dagCount() > 0) {
        for (int i = 0; i < operation_->dagCount(); i++) {
            wxString oper = WxConversion::toWxString(i+1);
            indexDAG_->Append(oper);
        }
    }
    else
    {
    }

    // last item is "New Dag"-option
    wxString oper = wxT("New DAG");
    indexDAG_->Append(oper);
    indexDAG_->SetSelection(0);

}

/**
 * Updates TextCtrl for DAG
 */
void
OperationDAGDialog::updateDAG() {

    editDAG_->Clear();

    // get selected item from ComboBox
    int index = indexDAG_->GetSelection();
    
    // check that index is legal
    if (operation_ != NULL && index < operation_->dagCount()) {
        std::string code = operation_->dagCode(index);
        std::ostream dagCode(editDAG_);
        dagCode << code;
        dagCode.flush();

        FindWindow(ID_UNDO_DAG_BUTTON)->Disable();
        FindWindow(ID_SAVE_DAG_BUTTON)->Disable();
        FindWindow(ID_DELETE_DAG_BUTTON)->Enable();
        FindWindow(ID_NEW_DAG_BUTTON)->Enable();

        OperationDAG& currentDAG = operation_->dag(index);

        std::string temp = FileSystem::createTempDirectory();
        std::string pngDag = temp + "/dag.png";
        std::string dotDag = temp + "/dag.dot";

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
            std::string dotCmd = "echo 'digraph G {n140545368 [label=\"" + errText + "\", shape=ellipse, fontsize=12]; }' | dot -Tpng > " + pngDag;

            system(dotCmd.c_str());

            delete dotImage_;
            dotImage_ = new wxBitmap(100,100);
            wxString wxTemp(wxString::FromAscii(pngDag.c_str()));
            dotImage_->LoadFile(wxTemp, wxBITMAP_TYPE_PNG);
            dagStaticBitmap_->SetBitmap(*dotImage_);
        }
        else // show graph
        {
            // generate dot
            currentDAG.writeToDotFile(dotDag);

            // generate png from dot
            std::string dotCmd = "dot -Tpng " + dotDag + " > " + pngDag;
            system(dotCmd.c_str());

            delete dotImage_;
            dotImage_ = new wxBitmap(100,100);
            wxString wxTemp(wxString::FromAscii(pngDag.c_str()));
            dotImage_->LoadFile(wxTemp, wxBITMAP_TYPE_PNG);
            dagStaticBitmap_->SetBitmap(*dotImage_);
        }
        FileSystem::removeFileOrDirectory(temp);
    }
    else // new dag => do not update, just disable the buttons
    {
        FindWindow(ID_UNDO_DAG_BUTTON)->Disable();
        FindWindow(ID_SAVE_DAG_BUTTON)->Disable();
        FindWindow(ID_DELETE_DAG_BUTTON)->Disable();
        FindWindow(ID_NEW_DAG_BUTTON)->Disable();
        wxImage *temp = new wxImage(100, 100, true);

        delete dotImage_;
        dotImage_ = new wxBitmap(*temp);
        dagStaticBitmap_->SetBitmap(*dotImage_);

    }

}
/**
 * Handles the event when OK button is pushed.
 */
void 
OperationDAGDialog::onOK(wxCommandEvent&)
{
    EndModal(wxID_OK);
}

/**
 * Handles the event when Save button is pushed.
 */
void
OperationDAGDialog::onSaveDAG(wxCommandEvent&)
{
    if(operation_ == NULL) {
        return;
    }

    int index = indexDAG_->GetSelection();

    std::string code("");
    wxString wxTemp;

    for (int i = 0; i < editDAG_->GetNumberOfLines(); ++i) {
        wxTemp = editDAG_->GetLineText(i);

#if wxCHECK_VERSION(2, 6, 0)
        std::string stdTemp(wxTemp.mb_str());
#else
        std::string stdTemp(wxTemp.c_str());
#endif

        code += stdTemp;
        code += "\n";
    }

    if (operation_->dagCount() == index) {
        operation_->addDag(code);
    }
    else {
        operation_->setDagCode(index, code);
    }

    updateIndex();
    indexDAG_->SetSelection(index);
    updateDAG();
    FindWindow(ID_SAVE_DAG_BUTTON)->Disable();
    FindWindow(ID_UNDO_DAG_BUTTON)->Disable();
    FindWindow(ID_DELETE_DAG_BUTTON)->Enable();
}

/**
 * Handles the event when Undo button is pushed.
 */
void
OperationDAGDialog::onUndoDAG(wxCommandEvent&)
{
    std::iostream dagCode(editDAG_);

    int index = indexDAG_->GetSelection();
    if (operation_->dagCount() == index) { // new operation
        editDAG_->Clear();
        FindWindow(ID_DELETE_DAG_BUTTON)->Disable();
    }
    else {
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
OperationDAGDialog::onDeleteDAG(wxCommandEvent&)
{
    int index = indexDAG_->GetSelection();
    if (operation_->dagCount() > index) {
        editDAG_->Clear();
        operation_->removeDag(index);
    }

    updateIndex();

    if (index > 0) {
        index--;
    }

    indexDAG_->SetSelection(index);
    updateDAG();
}

/**
 * Handles the event when New button is pushed.
 */
void
OperationDAGDialog::onNewDAG(wxCommandEvent&)
{
    int index = operation_->dagCount();
    editDAG_->Clear();
    indexDAG_->SetSelection(index);
    FindWindow(ID_SAVE_DAG_BUTTON)->Disable();
    FindWindow(ID_UNDO_DAG_BUTTON)->Disable();
    FindWindow(ID_DELETE_DAG_BUTTON)->Disable();
    FindWindow(ID_NEW_DAG_BUTTON)->Disable();
}

/**
 * Handles the event when DAG is changed
 */
void
OperationDAGDialog::onDAGChange(wxCommandEvent&)
{
    FindWindow(ID_UNDO_DAG_BUTTON)->Enable();
    FindWindow(ID_SAVE_DAG_BUTTON)->Enable();
}

/**
 * Handles the event when ComboBox is changed
 */
void
OperationDAGDialog::onComboBoxChange(wxCommandEvent&)
{
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

    wxBoxSizer *item0 = new wxBoxSizer(wxVERTICAL);

    wxStaticBox *dagStaticBox = new wxStaticBox(parent, -1, wxT("Code"));
    wxStaticBoxSizer *dagStaticBoxSizer = new wxStaticBoxSizer(dagStaticBox, wxVERTICAL);

    // DAG editor
    wxTextCtrl* editDAG = new wxTextCtrl(parent, ID_EDIT_DAG, wxT(""), wxDefaultPosition, wxSize(250,365), wxTE_MULTILINE);

    wxStaticBox *dagCodeStaticBox = new wxStaticBox(parent, -1, wxT("DAG"));
    wxStaticBoxSizer *dagCodeStaticBoxSizer = new wxStaticBoxSizer(dagCodeStaticBox, wxVERTICAL);

    dotImage_ = new wxBitmap(100,100);
    dagStaticBitmap_ = new wxStaticBitmap(parent, ID_DAG_IMAGE, *dotImage_);
    dagStaticBoxSizer->Add(editDAG, 0, wxALIGN_TOP|wxALL|wxGROW, 5);
    dagCodeStaticBoxSizer->Add(dagStaticBitmap_, 5, wxALIGN_TOP|wxALL|wxGROW, 5);

    wxBoxSizer *pageSizer = new wxBoxSizer(wxHORIZONTAL);
    //pageSizer->Add(item8, 0, wxALIGN_TOP|wxALL, 5);
    pageSizer->Add(dagStaticBoxSizer, 0, wxALIGN_TOP|wxTOP, 10);
    pageSizer->Add(dagCodeStaticBoxSizer, 5, wxALIGN_TOP|wxTOP, 10);

    item0->Add(pageSizer, 0, wxALIGN_CENTER|wxALL, 5);

    wxString strs9[] = 
        {
            wxT("id: 1")
        };

    // ComboBox for choosing DAG
    //wxComboBox *indexDAG = new wxComboBox(parent, ID_INDEX_DAG, wxT(""), wxDefaultPosition, wxSize(150,-1), 1, strs9, wxCB_DROPDOWN);
    wxChoice *indexDAG = new wxChoice(parent, ID_INDEX_DAG, wxDefaultPosition, wxSize(150,-1), 1, strs9);
    wxButton *saveDAG = new wxButton(parent, ID_SAVE_DAG_BUTTON, wxT("Save"), wxDefaultPosition, wxDefaultSize, 0);
    wxButton *undoDAG = new wxButton(parent, ID_UNDO_DAG_BUTTON, wxT("Undo"), wxDefaultPosition, wxDefaultSize, 0);
    wxButton *deleteDAG = new wxButton(parent, ID_DELETE_DAG_BUTTON, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0);
    wxButton *newDAG = new wxButton(parent, ID_NEW_DAG_BUTTON, wxT("New"), wxDefaultPosition, wxDefaultSize, 0);

    wxBoxSizer *dagToolsSizer = new wxBoxSizer(wxHORIZONTAL);
    dagToolsSizer->Add(indexDAG, 0, wxALIGN_CENTER|wxALL, 5);

    wxGridSizer *dagButtonSizer = new wxGridSizer(2, 0, 0);

    dagButtonSizer->Add(saveDAG, 0, wxALIGN_CENTER|wxALL, 5);
    dagButtonSizer->Add(undoDAG, 0, wxALIGN_CENTER|wxALL, 5);
    dagButtonSizer->Add(deleteDAG, 0, wxALIGN_CENTER|wxALL, 5);
    dagButtonSizer->Add(newDAG, 0, wxALIGN_CENTER|wxALL, 5);

    dagToolsSizer->Add(dagButtonSizer, 0, wxALIGN_CENTER|wxALL, 5);

    wxButton *OK = new wxButton(parent, ID_OK_BUTTON, wxT("Ok"), wxDefaultPosition, wxDefaultSize, 0);
    dagToolsSizer->Add(OK, 0, wxALIGN_CENTER|wxALL, 5);


    dagStaticBoxSizer->Add(dagToolsSizer, 0, wxALIGN_TOP|wxALL, 5);





    if (set_sizer)
    {
        parent->SetSizer(item0);
        if (call_fit)
            item0->SetSizeHints(parent);
    }
    
    return item0;
}

