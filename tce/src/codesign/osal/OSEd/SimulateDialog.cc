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
 * @file SimulateDialog.cc
 *
 * Definition of SimulateDialog class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <wx/valgen.h>
#include <boost/format.hpp>

#include "SimulateDialog.hh"
#include "SequenceTools.hh"
#include "OSEdTextGenerator.hh"
#include "GUITextGenerator.hh"
#include "WidgetTools.hh"
#include "WxConversion.hh"
#include "OSEdConstants.hh"
#include "Conversion.hh"
#include "OperationSimulator.hh"
#include "ErrorDialog.hh"
#include "DialogPosition.hh"
#include "SimulationInfoDialog.hh"
#include "Operation.hh"
#include "DataObject.hh"
#include "OperationContext.hh"
#include "NumberControl.hh"
#include "SimValue.hh"
#include "ContainerTools.hh"
#include "OSEdMainFrame.hh"
#include "Memory.hh"
#include "OperationContainer.hh"
#include "OSEd.hh"
#include "OSEdInformer.hh"
#include "TCEString.hh"
#include "OperationBehaviorProxy.hh"

using boost::format;
using std::string;
using std::vector;

const string SimulateDialog::FORMAT_SIGNED_INT = "signed int";
const string SimulateDialog::FORMAT_UNSIGNED_INT = "unsigned int";
const string SimulateDialog::FORMAT_DOUBLE = "double";
const string SimulateDialog::FORMAT_FLOAT = "float";
const string SimulateDialog::FORMAT_BINARY = "binary";
const string SimulateDialog::FORMAT_HEXA_DECIMAL = "hexadecimal";

BEGIN_EVENT_TABLE(SimulateDialog, wxDialog)
    EVT_LIST_ITEM_SELECTED(ID_INPUT_LIST, SimulateDialog::onSelection)
    EVT_LIST_ITEM_DESELECTED(ID_INPUT_LIST, SimulateDialog::onSelection)

    EVT_BUTTON(ID_BUTTON_UPDATE, SimulateDialog::onUpdateValue)
    EVT_BUTTON(ID_BUTTON_TRIGGER, SimulateDialog::onTrigger)
    EVT_BUTTON(ID_BUTTON_ADVANCE_LOCK, SimulateDialog::onAdvanceClock)
    EVT_BUTTON(ID_BUTTON_RESET, SimulateDialog::onReset)
    EVT_BUTTON(ID_BUTTON_SHOW_HIDE_REGISTERS, SimulateDialog::showOrHideRegisters)
    EVT_BUTTON(wxID_OK, SimulateDialog::onOK)

    EVT_CHOICE(ID_FORMAT, SimulateDialog::onFormatChanged)

    EVT_CLOSE(SimulateDialog::onClose)
END_EVENT_TABLE()


const wxString SimulateDialog::DIALOG_NAME = _T("SimulateDialog");
			  
/**
 * Constructor.
 *
 * @param parent Parent window.
 * @param operation Operation to be simulated.
 */
SimulateDialog::SimulateDialog(
    wxWindow* parent, 
    Operation* operation,
    const std::string& pathName,
    const std::string& modName) :
    wxDialog(parent, -1, _T(""),
             DialogPosition::getPosition(DialogPosition::DIALOG_SIMULATE), 
             wxDefaultSize, wxRESIZE_BORDER, DIALOG_NAME),
    operation_(operation), pathName_(pathName), modName_(modName), clock_(0) {

    createContents(this, true, true);
	
    for (int i = 0; i < operation_->numberOfInputs(); i++) {
        inputs_.push_back(new DataObject());
        inputs_[i]->setString("0");
    }
	
    for (int i = 0; i < operation_->numberOfOutputs(); i++) {
        outputs_.push_back(new DataObject());
        outputs_[i]->setString("0");
    }
	
    inputList_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_INPUT_LIST));
    outputList_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_OUTPUT_LIST));
    updateValue_ = dynamic_cast<NumberControl*>(FindWindow(ID_VALUE));
    format_ = dynamic_cast<wxChoice*>(FindWindow(ID_FORMAT));
    //triggerBM_ = dynamic_cast<wxStaticBitmap*>(FindWindow(ID_BITMAP_READY));

    //lateResultBM_ = 
    //    dynamic_cast<wxStaticBitmap*>(FindWindow(ID_BITMAP_UPDATE));
       
    FindWindow(wxID_OK)->SetFocus();

    format_->Append(WxConversion::toWxString(FORMAT_SIGNED_INT));
    format_->Append(WxConversion::toWxString(FORMAT_UNSIGNED_INT));
    format_->Append(WxConversion::toWxString(FORMAT_DOUBLE));
    format_->Append(WxConversion::toWxString(FORMAT_FLOAT));
    format_->Append(WxConversion::toWxString(FORMAT_BINARY));
    format_->Append(WxConversion::toWxString(FORMAT_HEXA_DECIMAL));
	
    format_->SetSelection(0);
	
    setTexts();

    infoDialog_ = new SimulationInfoDialog(
        this, &OperationContainer::operationContext());
    OSEdInformer* informer = wxGetApp().mainFrame()->informer();
    informer->registerListener(OSEdInformer::EVENT_REGISTER, infoDialog_);
    informer->registerListener(OSEdInformer::EVENT_RESET, infoDialog_);

    /* Ensure we won't refresh the behavior from the plugin file again
       during the simulation. This leads to crashes due to using a state
       instance created by a behavior definition created by a previously
       loaded behavior instance. This effectively "freezes" the behavior
       to the one loaded in the next behavior function call. */
    behaviorProxy_ = 
        dynamic_cast<OperationBehaviorProxy*>(&operation->behavior());
    assert(behaviorProxy_ != NULL);
    behaviorProxy_->setAlwaysReloadBehavior(false);
}

/**
 * Destructor.
 */
SimulateDialog::~SimulateDialog() {
    
    SequenceTools::deleteAllItems(inputs_);
    SequenceTools::deleteAllItems(outputs_);
    
    int x, y;
    GetPosition(&x, &y);
    wxPoint point(x, y);
    DialogPosition::setPosition(DialogPosition::DIALOG_SIMULATE, point);

    /* Ensure we won't use the old state object the next time because
       the behavior will be reloaded from the dynlib. It would crash. */
    OperationBehavior& behavior = operation_->behavior();
    behavior.deleteState(OperationContainer::operationContext());

    /* "Unfreeze" the OperationBehavior. Now it will be reloaded always
       when accessing the methods to enable the dynamic menu items in 
       the operation list. */
    behaviorProxy_->setAlwaysReloadBehavior(true);
    behaviorProxy_->uninitializeBehavior();

}

/**
 * Transfers data to window.
 */
bool
SimulateDialog::TransferDataToWindow() {
    updateLists();
    return wxWindow::TransferDataToWindow();
}

/**
 * Set texts to widgets.
 */
void
SimulateDialog::setTexts() {

    GUITextGenerator& guiText = *GUITextGenerator::instance();
    OSEdTextGenerator& osedText = OSEdTextGenerator::instance();

    // title
    format fmt = osedText.text(OSEdTextGenerator::TXT_SIMULATE_DIALOG_TITLE);
    fmt % operation_->name();
    SetTitle(WxConversion::toWxString(fmt.str()));

    // static texts
    //WidgetTools::setLabel(&osedText, FindWindow(ID_TEXT_READY),
    //                      OSEdTextGenerator::TXT_LABEL_READY);
	
    //WidgetTools::setLabel(&osedText, FindWindow(ID_TEXT_UPDATE),
    //                      OSEdTextGenerator::TXT_LABEL_UPDATE);

    WidgetTools::setLabel(&osedText, FindWindow(ID_TEXT_CLOCK),
                          OSEdTextGenerator::TXT_LABEL_CLOCK);
	
    WidgetTools::setLabel(&osedText, FindWindow(ID_FORMAT_TEXT),
                          OSEdTextGenerator::TXT_LABEL_FORMAT);

    // buttons
    WidgetTools::setLabel(&guiText, FindWindow(wxID_OK),
                          GUITextGenerator::TXT_BUTTON_OK);

    WidgetTools::setLabel(&osedText, FindWindow(ID_BUTTON_RESET),
                          OSEdTextGenerator::TXT_BUTTON_RESET);

    WidgetTools::setLabel(&osedText, FindWindow(ID_BUTTON_TRIGGER),
                          OSEdTextGenerator::TXT_BUTTON_TRIGGER);

    WidgetTools::setLabel(&osedText, FindWindow(ID_BUTTON_ADVANCE_LOCK),
                          OSEdTextGenerator::TXT_BUTTON_ADVANCE_LOCK);

    WidgetTools::setLabel(&osedText, FindWindow(ID_BUTTON_SHOW_HIDE_REGISTERS),
                          OSEdTextGenerator::TXT_BUTTON_SHOW_REGISTERS);

    WidgetTools::setLabel(&osedText, FindWindow(ID_BUTTON_UPDATE),
                          OSEdTextGenerator::TXT_BUTTON_UPDATE);

    // column titles
    fmt = osedText.text(OSEdTextGenerator::TXT_COLUMN_OPERAND);
    inputList_->InsertColumn(
        0, WxConversion::toWxString(fmt.str()), wxLIST_FORMAT_LEFT,
        OSEdConstants::DEFAULT_COLUMN_WIDTH/2);

    outputList_->InsertColumn(
        0, WxConversion::toWxString(fmt.str()), wxLIST_FORMAT_LEFT,
        OSEdConstants::DEFAULT_COLUMN_WIDTH/2);

    fmt = osedText.text(OSEdTextGenerator::TXT_COLUMN_VALUE);
    inputList_->InsertColumn(
        1, WxConversion::toWxString(fmt.str()), wxLIST_FORMAT_LEFT,
        OSEdConstants::DEFAULT_COLUMN_WIDTH);
	
    outputList_->InsertColumn(
        1, WxConversion::toWxString(fmt.str()), wxLIST_FORMAT_LEFT,
        OSEdConstants::DEFAULT_COLUMN_WIDTH);
	
    // box sizers
    fmt = osedText.text(OSEdTextGenerator::TXT_BOX_INPUT_VALUES);
    WidgetTools::setWidgetLabel(inputSizer_, fmt.str());

    fmt = osedText.text(OSEdTextGenerator::TXT_BOX_OUTPUT_VALUES);
    WidgetTools::setWidgetLabel(outputSizer_, fmt.str());
}

/**
 * Updates input and output value lists.
 */
void
SimulateDialog::updateLists() {
	
    inputList_->DeleteAllItems();
    outputList_->DeleteAllItems();
    for (unsigned int i = 0; i < inputs_.size(); i++) {
        wxString id = WxConversion::toWxString(i + 1);
        inputList_->InsertItem(i, id);
        inputList_->SetItem(
            i, 1, WxConversion::toWxString(inputs_[i]->stringValue()));
    }
	
    int k = inputs_.size() + 1;
    for (unsigned int i = 0; i < outputs_.size(); i++) {
        wxString id = WxConversion::toWxString(k);
        outputList_->InsertItem(i, id);
        outputList_->SetItem(
            i, 1, WxConversion::toWxString(outputs_[i]->stringValue()));
        k++;
    }

    wxListEvent dummy;
    onSelection(dummy);
}

/**
 * Handles the event when a value is selected or deselected on input list.
 */
void
SimulateDialog::onSelection(wxListEvent&) {
    if (inputList_->GetSelectedItemCount() != 1) {
        FindWindow(ID_BUTTON_UPDATE)->Disable();
    } else {
        FindWindow(ID_BUTTON_UPDATE)->Enable();
    }
}

/**
 * Handles the event when input value is updated.
 */
void
SimulateDialog::onUpdateValue(wxCommandEvent&) {
    TransferDataFromWindow();
    string inputString = WidgetTools::lcStringSelection(inputList_, 0);
    if (inputString != "") {
        int input = Conversion::toInt(inputString);
        SimValue* temp = new SimValue(32);
        if (updateValue_->mode() == NumberControl::MODE_DOUBLE) {
            double value = updateValue_->doubleValue();
            *temp = value;
        } else {
            int value = updateValue_->intValue();
            *temp = value;
        }
        inputs_[input - 1]->setString(formattedValue(temp));
        delete temp;
        updateLists();
    }
}

/**
 * Handles the event when Reset button is pushed.
 *
 * Reset sets all values to their defaults.
 */
void
SimulateDialog::onReset(wxCommandEvent&) {
    
    OperationBehavior& behavior = operation_->behavior();
    behavior.deleteState(OperationContainer::operationContext());
    
    for (size_t i = 0; i < inputs_.size(); i++) {
        inputs_[i]->setString("0");
    }
    
    for (size_t i = 0; i < outputs_.size(); i++) {
        outputs_[i]->setString("0");
    }

    //triggerBM_->SetBitmap(createBitmap(2));
    //lateResultBM_->SetBitmap(createBitmap(2));
    clock_ = 0;
    FindWindow(ID_TEXT_CLOCK_VALUE)->
        SetLabel(WxConversion::toWxString(clock_));
    updateLists();
    OSEdInformer* informer = wxGetApp().mainFrame()->informer();
    informer->handleEvent(OSEdInformer::EVENT_RESET);
}

/**
 * Handles the event when Trigger button is pushed.
 */
void
SimulateDialog::onTrigger(wxCommandEvent&) {

    OperationSimulator& simulator = OperationSimulator::instance();
    vector<SimValue*> outputs;
    vector<DataObject> inputs;
    for (size_t i = 0; i < inputs_.size(); i++) {
        inputs.push_back(*inputs_[i]);
    }

    string result = "";
    
    createState();
    OperationContext& context = OperationContainer::operationContext();
    if (simulator.simulateTrigger(
            *operation_, inputs, outputs, context, 32, result)) {
		
        //triggerBM_->SetBitmap(createBitmap(1));
        setOutputValues(outputs);
        updateLists();
    } else {
        //triggerBM_->SetBitmap(createBitmap(0));
        if (result != "") {
            ErrorDialog dialog(this, WxConversion::toWxString(result));
            dialog.ShowModal();
        }
    }
    SequenceTools::deleteAllItems(outputs);
    OSEdInformer* informer = wxGetApp().mainFrame()->informer();
    informer->handleEvent(OSEdInformer::EVENT_REGISTER);
    informer->handleEvent(OSEdInformer::EVENT_MEMORY);
}

/**
 * Handles the event when advance clock button is pushed.
 */
void
SimulateDialog::onAdvanceClock(wxCommandEvent&) {    
    createState();
    OperationContainer::operationContext().advanceClock();
    OperationContainer::memory().advanceClock();
    clock_++;
    FindWindow(ID_TEXT_CLOCK_VALUE)->SetLabel(WxConversion::toWxString(clock_));
    OSEdInformer* informer = wxGetApp().mainFrame()->informer();
    informer->handleEvent(OSEdInformer::EVENT_MEMORY);
}

/**
 * Handles the event when intput and output format is changed.
 */
void
SimulateDialog::onFormatChanged(wxCommandEvent&) {
    
    vector<SimValue*> outputs;

    OperationSimulator& simulator = OperationSimulator::instance();
    string result = "";
    
    for (size_t i = 0; i < inputs_.size(); i++) {
        SimValue* sim = new SimValue(32);
        string value = inputs_[i]->stringValue();
        simulator.initializeSimValue(value, sim, result);
        outputs.push_back(sim);
    }
    
    for (size_t i = 0; i < outputs_.size(); i++) {
        SimValue* sim = new SimValue(32);
        string value = outputs_[i]->stringValue();
        simulator.initializeSimValue(value, sim, result);
        outputs.push_back(sim);
    }

    setInputValues(outputs);
    setOutputValues(outputs);
    updateLists();
    SequenceTools::deleteAllItems(outputs);
    OSEdInformer* informer = wxGetApp().mainFrame()->informer();
    informer->handleEvent(OSEdInformer::EVENT_REGISTER);
}

/**
 * Handles the event when registers are shown or hidden.
 */
void
SimulateDialog::showOrHideRegisters(wxCommandEvent&) {
    wxButton* button = 
        dynamic_cast<wxButton*>(FindWindow(ID_BUTTON_SHOW_HIDE_REGISTERS));
    OSEdTextGenerator& texts = OSEdTextGenerator::instance();
    format fmt = texts.text(OSEdTextGenerator::TXT_BUTTON_SHOW_REGISTERS);
    if (button->GetLabel() == WxConversion::toWxString(fmt.str())) {
        int x, y;
        GetPosition(&x, &y);
        int width, height;
        GetSize(&width, &height);
        y += height + 25;
        infoDialog_->Move(x, y);
        infoDialog_->Show(true);
        fmt = texts.text(OSEdTextGenerator::TXT_BUTTON_HIDE_REGISTERS);
        button->SetLabel(WxConversion::toWxString(fmt.str()));
    } else {
        infoDialog_->Show(false);
        button->SetLabel(WxConversion::toWxString(fmt.str()));
    }
}

/**
 * Handles the event when OK button is pushed.
 */
void
SimulateDialog::onOK(wxCommandEvent&) {
    OSEdInformer* informer = wxGetApp().mainFrame()->informer();
    informer->unregisterListener(OSEdInformer::EVENT_REGISTER, infoDialog_);
    informer->unregisterListener(OSEdInformer::EVENT_RESET, infoDialog_);
    infoDialog_->Destroy();
    Destroy();
}

/**
 * Handles the event when window is closed.
 *
 * @param event Event to be handled.
 */
void
SimulateDialog::onClose(wxCloseEvent& event){
    OSEdInformer* informer = wxGetApp().mainFrame()->informer();
    informer->unregisterListener(OSEdInformer::EVENT_REGISTER, infoDialog_);
    informer->unregisterListener(OSEdInformer::EVENT_RESET, infoDialog_);
    infoDialog_->Destroy();
    event.Skip();
}

/**
 * Sets the input values after the format has changed.
 *
 * @param inputs Input values of operation.
 */
void
SimulateDialog::setInputValues(std::vector<SimValue*> inputs) {

    string inputFormat = WxConversion::toString(format_->GetStringSelection());

    for (size_t i = 0; i < inputs_.size(); i++) {
        SimValue* current = inputs[i];
        inputs_[i]->setString(formattedValue(current));
    }
}

/**
 * Sets the output values of the operation after executing a command.
 *
 * @param outputs Output values of operation.
 */
void
SimulateDialog::setOutputValues(std::vector<SimValue*> outputs) {

    int k = 0;
    size_t upperBound = inputs_.size() + outputs_.size();
    for (size_t i = inputs_.size(); i < upperBound; i++) {
        SimValue* current = outputs[i];
        outputs_[k]->setString(formattedValue(current));
        k++;
    }
}

/**
 * Formats the value of the SimValue.
 *
 * @param value Value to be formatted.
 */
string
SimulateDialog::formattedValue(SimValue* value) {

    string format = WxConversion::toString(format_->GetStringSelection());
    if (format == FORMAT_SIGNED_INT) {
        SIntWord intSigned = value->sIntWordValue();
        return Conversion::toString(static_cast<int>(intSigned));
    } else if (format == FORMAT_UNSIGNED_INT) {
        UIntWord intUnsigned = value->uIntWordValue();
        return Conversion::toString(intUnsigned);
    } else if (format == FORMAT_DOUBLE) {
        DoubleWord doubleWord = value->doubleWordValue();
        return Conversion::toString(doubleWord);
    } else if (format == FORMAT_FLOAT) {
        FloatWord floatWord = value->floatWordValue();
        string floatString = Conversion::toString(floatWord);
        if (floatString.find(".") == string::npos) {
            floatString += ".0";
        }
        floatString += "f";
        return floatString;
    } else if (format == FORMAT_BINARY) {
        UIntWord binary = value->uIntWordValue();
        return Conversion::toBinString(static_cast<int>(binary));
    } else if (format == FORMAT_HEXA_DECIMAL) {
        UIntWord hex = value->uIntWordValue();
        return Conversion::toHexString(hex);
    }
    return "";
}

/**
 * Creates the state for the operation.
 */
void
SimulateDialog::createState() {
    OperationBehavior& beh = operation_->behavior();
    beh.createState(OperationContainer::operationContext());
}

/**
 * Creates the contents of the dialog.
 *
 * NOTE! This function was generated by wxDesigner.
 *
 * @param parent Parent window.
 * @param call_fit If true fits the contenst inside the dialog.
 * @param set_sizer If true sets the main sizer as dialog contents. 
 * @return The created sizer.
 */
wxSizer*
SimulateDialog::createContents(
    wxWindow *parent,
    bool call_fit, 
    bool set_sizer ) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *item1 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticBox *item3 = new wxStaticBox( parent, -1, wxT("Input values") );
    wxStaticBoxSizer *item2 = new wxStaticBoxSizer( item3, wxVERTICAL );
    inputSizer_ = item2;

    wxListCtrl *item4 = new wxListCtrl( parent, ID_INPUT_LIST, wxDefaultPosition, wxSize(160,120), wxLC_REPORT|wxSUNKEN_BORDER );
    item2->Add( item4, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item5 = new wxBoxSizer( wxHORIZONTAL );

    NumberControl *item6 = new NumberControl(
        parent, ID_VALUE, wxDefaultPosition, wxSize(140, 20), 
        NumberControl::MODE_BINARY | NumberControl::MODE_INT | 
        NumberControl::MODE_UNSIGNED | NumberControl::MODE_HEXADECIMAL |
        NumberControl::MODE_FLOAT | NumberControl::MODE_DOUBLE);
    
    item5->Add( item6, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item7 = new wxButton( parent, ID_BUTTON_UPDATE, wxT("Update"), wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add( item7, 0, wxALIGN_CENTER|wxALL, 5 );

    item2->Add( item5, 0, wxALIGN_CENTER|wxALL, 5 );

    item1->Add( item2, 1, wxGROW|wxALIGN_CENTER|wxALL, 5 );

    wxStaticBox *item9 = new wxStaticBox( parent, -1, wxT("Output values") );
    wxStaticBoxSizer *item8 = new wxStaticBoxSizer( item9, wxVERTICAL );
    outputSizer_ = item8;

    wxListCtrl *item10 = new wxListCtrl( parent, ID_OUTPUT_LIST, wxDefaultPosition, wxSize(200,120), wxLC_REPORT|wxSUNKEN_BORDER );
    item8->Add( item10, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item1->Add( item8, 1, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    wxGridSizer *item11 = new wxGridSizer( 2, 0, 0 );

    //wxStaticText *item12 = new wxStaticText( parent, ID_TEXT_READY, wxT("Ready"), wxDefaultPosition, wxDefaultSize, 0 );
    //item11->Add( item12, 0, wxALIGN_CENTER|wxALL, 5 );

    //wxStaticBitmap *item13 = new wxStaticBitmap( parent, ID_BITMAP_READY, createBitmap( 2 ), wxDefaultPosition, wxDefaultSize );
    //item11->Add( item13, 0, wxALIGN_CENTER|wxALL, 5 );

    //wxStaticText *item14 = new wxStaticText( parent, ID_TEXT_UPDATE, wxT("Update"), wxDefaultPosition, wxDefaultSize, 0 );
    //item11->Add( item14, 0, wxALIGN_CENTER|wxALL, 5 );

    //wxStaticBitmap *item15 = new wxStaticBitmap( parent, ID_BITMAP_UPDATE, createBitmap( 2 ), wxDefaultPosition, wxDefaultSize );
    //item11->Add( item15, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item16 = new wxStaticText( parent, ID_TEXT_CLOCK, wxT("Clock count:"), wxDefaultPosition, wxDefaultSize, 0 );
    item11->Add( item16, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item17 = new wxStaticText( parent, ID_TEXT_CLOCK_VALUE, wxT("0"), wxDefaultPosition, wxDefaultSize, 0 );
    item11->Add( item17, 0, wxALIGN_CENTER|wxALL, 5 );

    item1->Add( item11, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    item0->Add( item1, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item18 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText *item19 = new wxStaticText( parent, ID_FORMAT_TEXT, wxT("Format:"), wxDefaultPosition, wxDefaultSize, 0 );
    item18->Add( item19, 0, wxALIGN_CENTER|wxALL, 5 );

    wxString *strs20 = (wxString*) NULL;
    wxChoice *item20 = new wxChoice( parent, ID_FORMAT, wxDefaultPosition, wxSize(130,-1), 0, strs20, 0 );
    item18->Add( item20, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item18, 0, wxALIGN_CENTER|wxALL, 5 );

    wxBoxSizer *item21 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item22 = new wxButton( parent, ID_BUTTON_RESET, wxT("Reset"), wxDefaultPosition, wxDefaultSize, 0 );
    item21->Add( item22, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item23 = new wxButton( parent, ID_BUTTON_TRIGGER, wxT("Trigger"), wxDefaultPosition, wxDefaultSize, 0 );
    item21->Add( item23, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item25 = new wxButton( parent, ID_BUTTON_ADVANCE_LOCK, wxT("Advance Clock"), wxDefaultPosition, wxDefaultSize, 0 );
    item21->Add( item25, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item26 = new wxButton( parent, ID_BUTTON_SHOW_HIDE_REGISTERS, wxT("Show registers"), wxDefaultPosition, wxDefaultSize, 0 );
    item21->Add( item26, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item27 = new wxButton( parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item21->Add( item27, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item21, 0, wxALIGN_CENTER|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;

}

/**
 *
 */
wxBitmap 
SimulateDialog::createBitmap(unsigned int index) {
	
    if (index == 0)
    {
        /* XPM */
        static const char *xpm_data[] = {
            /* columns rows colors chars-per-pixel */
            "16 15 2 1",
            "  c None",
            "a c #FF0000",
            /* pixels */
            "                ",
            " aaaaaaaaaaaaaa ",
            " aaaaaaaaaaaaaa ",
            " aaaaaaaaaaaaaa ",
            " aaaaaaaaaaaaaa ",
            " aaaaaaaaaaaaaa ",
            " aaaaaaaaaaaaaa ",
            " aaaaaaaaaaaaaa ",
            " aaaaaaaaaaaaaa ",
            " aaaaaaaaaaaaaa ",
            " aaaaaaaaaaaaaa ",
            " aaaaaaaaaaaaaa ",
            " aaaaaaaaaaaaaa ",
            " aaaaaaaaaaaaaa ",
            "                "
        };
        wxBitmap bitmap( xpm_data );
        return bitmap;
    }
    if (index == 1)
    {
        /* XPM */
        static const char *xpm_data[] = {
            /* columns rows colors chars-per-pixel */
            "16 15 2 1",
            "  c None",
            "b c #00FF00",
            /* pixels */
            "                ",
            " bbbbbbbbbbbbbb ",
            " bbbbbbbbbbbbbb ",
            " bbbbbbbbbbbbbb ",
            " bbbbbbbbbbbbbb ",
            " bbbbbbbbbbbbbb ",
            " bbbbbbbbbbbbbb ",
            " bbbbbbbbbbbbbb ",
            " bbbbbbbbbbbbbb ",
            " bbbbbbbbbbbbbb ",
            " bbbbbbbbbbbbbb ",
            " bbbbbbbbbbbbbb ",
            " bbbbbbbbbbbbbb ",
            " bbbbbbbbbbbbbb ",
            "                "
        };
        wxBitmap bitmap( xpm_data );
        return bitmap;
    }
    if (index == 2)
    {
        /* XPM */
        static const char *xpm_data[] = {
            /* columns rows colors chars-per-pixel */
            "16 15 2 1",
            "  c None",
            "a c Black",
            /* pixels */
            "                ",
            " aaaaaaaaaaaaaa ",
            " aaaaaaaaaaaaaa ",
            " aaaaaaaaaaaaaa ",
            " aaaaaaaaaaaaaa ",
            " aaaaaaaaaaaaaa ",
            " aaaaaaaaaaaaaa ",
            " aaaaaaaaaaaaaa ",
            " aaaaaaaaaaaaaa ",
            " aaaaaaaaaaaaaa ",
            " aaaaaaaaaaaaaa ",
            " aaaaaaaaaaaaaa ",
            " aaaaaaaaaaaaaa ",
            " aaaaaaaaaaaaaa ",
            "                "
        };
        wxBitmap bitmap( xpm_data );
        return bitmap;
    }
    return wxNullBitmap;
}
