
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
 * @file CallExplorerPluginWindow.cc
 *
 * Definition of CallExplorerPluginWindow class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen-no.spam-.tut.fi)
 * @note rating: red
 */

#include <string>
#include <wx/statline.h>
#include <wx/valgen.h>
#include <wx/spinctrl.h>
#include <boost/format.hpp>
#include <wx/wx.h>
#include <wx/docview.h>

#include "CallExplorerPlugin.hh"
#include "Conversion.hh"
#include "WxConversion.hh"
#include "WarningDialog.hh"
#include "AddressSpace.hh"
#include "ModelConstants.hh"
#include "NumberControl.hh"
#include "InformationDialog.hh"
#include "MachineTester.hh"
#include "GUITextGenerator.hh"
#include "WidgetTools.hh"
#include "ProDeTextGenerator.hh"
#include "DesignSpaceExplorer.hh"
#include "DesignSpaceExplorerPlugin.hh"
#include "EditParameterDialog.hh"
#include "Model.hh"
#include "MDFDocument.hh"

using boost::format;
using std::string;
using namespace TTAMachine;

BEGIN_EVENT_TABLE(CallExplorerPluginWindow, wxDialog)
    EVT_BUTTON(wxID_CLOSE, CallExplorerPluginWindow::onClose)
    EVT_BUTTON(ID_EDIT, CallExplorerPluginWindow::onEdit)
    EVT_BUTTON(ID_RUN, CallExplorerPluginWindow::onRun)
    EVT_LIST_ITEM_ACTIVATED(ID_PARAM_LIST, CallExplorerPluginWindow::onParamActivate)
    EVT_COMBOBOX(ID_PLUGIN_CHOOSER, CallExplorerPluginWindow::onSelectPlugin)
    EVT_LIST_ITEM_SELECTED(ID_PARAM_LIST, CallExplorerPluginWindow::onParamSelect)
    EVT_LIST_ITEM_DESELECTED(ID_PARAM_LIST, CallExplorerPluginWindow::onParamDeSelect)
END_EVENT_TABLE()


/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param addressSpace The address space to be modified with the dialog.
 */
CallExplorerPluginWindow::CallExplorerPluginWindow(
    wxWindow* parent, 
    TTAMachine::Machine& machine,
    Model& model):
    wxDialog(parent, -1, _T(""), wxDefaultPosition), machine_(machine), 
    model_(model),
    selectedParam_(-1),
    selectedPlugin_(0) {
        
    createContents(this, true, true);
    
    pluginNames_ =
        dynamic_cast<wxComboBox*>(FindWindow(ID_PLUGIN_CHOOSER));
    
    pluginDescription_ =
        dynamic_cast<wxTextCtrl*>(FindWindow(ID_DESCRIPTION_FIELD));
    
    parameterList_ =
        dynamic_cast<wxListCtrl*>(FindWindow(ID_PARAM_LIST));
    
    editButton = 
        dynamic_cast<wxButton*>(FindWindow(ID_EDIT));
    
    runButton =
        dynamic_cast<wxButton*>(FindWindow(ID_RUN));

    // Get explorer plugin names.
    std::vector<DesignSpaceExplorerPlugin*> plugins = explorer_.getPlugins();

    for (std::size_t i = 0; i < plugins.size(); i++) {
        DesignSpaceExplorerPlugin *plugin = plugins[i];

        if (plugin->producesArchitecture() &&
            !plugin->requiresHDB() && 
            !plugin->requiresSimulationData()) {
            
            plugins_.push_back(plugin);
        }

    }

    // set widget texts
    setTexts();

    TransferDataToWindow();
}


/**
 * The Destructor.
 */
CallExplorerPluginWindow::~CallExplorerPluginWindow() {
}


/**
 * Sets texts for widgets.
 */
void
CallExplorerPluginWindow::setTexts() {
    ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();

    // Dialog title
    format fmt = prodeTexts->text(
        ProDeTextGenerator::TXT_CALL_EXPLORER_PLUGIN_TITLE);
    SetTitle(WxConversion::toWxString(fmt.str()));

    // Get explorer plugin names into combo box
    for (size_t i = 0; i < plugins_.size(); ++i) {
        wxString pluginName =  WxConversion::toWxString(plugins_.at(i)->name());
        pluginNames_->Append(pluginName);
    }
    
    // Set parameter list column titles
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_NAME);
    parameterList_->InsertColumn(0, WxConversion::toWxString(fmt.str()),
                           wxLIST_FORMAT_CENTER, 160);
    //fmt = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_COMPULSORY);
    //parameterList_->InsertColumn(1, WxConversion::toWxString(fmt.str()),
    //                       wxLIST_FORMAT_LEFT, 100);
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_TYPE);
    parameterList_->InsertColumn(2, WxConversion::toWxString(fmt.str()),
                           wxLIST_FORMAT_LEFT, 100);
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_VALUE);
    parameterList_->InsertColumn(3, WxConversion::toWxString(fmt.str()),
                           wxLIST_FORMAT_LEFT, 160);
}


/**
 * Transfers data from the AddressSpace object to the dialog widgets.
 *
 * @return false, if an error occured in the transfer, true otherwise.
 */
bool
CallExplorerPluginWindow::TransferDataToWindow() {

    return wxWindow::TransferDataToWindow();
}


/**
 * Validates input in the controls, and updates the AddressSpace.
 */
void
CallExplorerPluginWindow::onClose(wxCommandEvent&) {
    EndModal(wxID_CANCEL);
}

/**
 * Editing selected parameter
 */
void
CallExplorerPluginWindow::onEdit(wxCommandEvent&) {
    doEdit();
}


/**
 * Runs the plugin
 */
void 
CallExplorerPluginWindow::onRun(wxCommandEvent&) {
    
    // no plugin selected?
    if (selectedPlugin_ == 0) {
        return;
    }

    // open up a temporary dsdb
    const string dsdbFile = "tmp.dsdb";
    FileSystem::removeFileOrDirectory(dsdbFile);
    DSDBManager* dsdb = DSDBManager::createNew(dsdbFile);
    RowID archID = dsdb->addArchitecture(machine_);
    
    // no implementation?
    DSDBManager::MachineConfiguration confIn = {archID, false, -1};
    RowID confID = dsdb->addConfiguration(confIn);

    // run the plugin
    try {
        selectedPlugin_->setDSDB(*dsdb);
        std::vector<RowID> result = selectedPlugin_->explore(confID, 0);
    
        // store the new machine
        TTAMachine::Machine& machine = *dsdb->architecture(confID+result.size());
        model_.getMachine()->copyFromMachine(machine);
    } catch (KeyNotFound& e) { 
        // no new adf created by the plugin. just exit
        FileSystem::removeFileOrDirectory(dsdbFile);
        EndModal(wxID_CANCEL);
        return;
    } catch (Exception& e) {
        // error in parameters
        InformationDialog diag(this, WxConversion::toWxString(e.errorMessage()));
        diag.ShowModal();
        FileSystem::removeFileOrDirectory(dsdbFile);
        return;
    }
    
    FileSystem::removeFileOrDirectory(dsdbFile);
    EndModal(wxID_OK);
}


/**
 * Parameter was doubleclicked
 */
void
CallExplorerPluginWindow::onParamActivate(wxListEvent& event) {
    selectedParam_ = event.GetIndex();
    doEdit();    
}

/**
 * Parameter was selected
 */
void
CallExplorerPluginWindow::onParamSelect(wxListEvent& event) {
    selectedParam_ = event.GetIndex();
}

/**
 * Parameter was deselected
 */
void 
CallExplorerPluginWindow::onParamDeSelect(wxListEvent&) {
    selectedParam_ = -1;
}


/**
 * A plugin was selected
 */
void 
CallExplorerPluginWindow::onSelectPlugin(wxCommandEvent&) {
    
    string pluginName =
        WxConversion::toString(pluginNames_->GetValue());

    // get current plugin
    for (size_t i = 0; i < plugins_.size(); i++) {
        if (plugins_.at(i)->name() == pluginName) {
            selectedPlugin_ = plugins_.at(i);
            break;
        }
    }
    
    // set description field
    pluginDescription_->Clear();
    pluginDescription_->AppendText(WxConversion::toWxString(
        selectedPlugin_->description()));
    
    updateParameterList();
}


/**
 * Starts parameter editor
 */
void 
CallExplorerPluginWindow::doEdit() {  
    if (selectedParam_ == -1) {
        return;
    }

    ExplorerPluginParameter param = getParam(selectedParam_); 
    EditParameterDialog diag(this, &param);   
    diag.ShowModal();
    selectedPlugin_->giveParameter(param.name(), param.value());
    updateParameterList();
}


/**
 * Updates parameter list
 */
void
CallExplorerPluginWindow::updateParameterList() {

    parameterList_->DeleteAllItems();
    
    DesignSpaceExplorerPlugin::ParameterMap params = selectedPlugin_->parameters();
    DesignSpaceExplorerPlugin::ParameterMap::iterator it = params.begin();

    for (int i =0; it != params.end(); ++it, ++i) {
        ExplorerPluginParameter parameter = it->second;

        string name = parameter.name();
        string type = parameter.typeAsString();
        string value = parameter.value();
        //string compulsory = parameter.isCompulsory() ? "x" : "";

        parameterList_->InsertItem(i, WxConversion::toWxString(name), 0);
        //parameterList_->SetItem(i, 1, WxConversion::toWxString(compulsory));
        parameterList_->SetItem(i, 1, WxConversion::toWxString(type));
        parameterList_->SetItem(i, 2, WxConversion::toWxString(value));
    }
}


/**
 * Gets param from param index
 */
ExplorerPluginParameter 
CallExplorerPluginWindow::getParam(int paramNum) {
    DesignSpaceExplorerPlugin::ParameterMap params = selectedPlugin_->parameters();
    DesignSpaceExplorerPlugin::ParameterMap::iterator it = params.begin();

    for (int i =0; it != params.end(); ++it, ++i) {
        if (i == paramNum) {
            return it->second;
        }
    }
    
    return ExplorerPluginParameter("", INT, false, "");
}


/**
 * Creates the dialog window contents.
 *
 * This method was generated with wxDesigner, thus the ugly code and
 * too long lines.
 *
 * @return Main sizer of the created contents.
 * @param parent The dialog window.
 * @param call_fit If true, fits the contents inside the dialog.
 * @param set_sizer If true, sets the main sizer as dialog contents.
 */
wxSizer*
CallExplorerPluginWindow::createContents(
    wxWindow* parent,
    bool call_fit,
    bool set_sizer) {
        
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxFlexGridSizer *item1 = new wxFlexGridSizer( 2, 0, 0 );
    item1->AddGrowableCol( 1 );

    wxStaticText *item2 = new wxStaticText( parent, ID_TEXT, wxT("Explorer Plugin:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item2, 0, wxFIXED_MINSIZE|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxString *strs3 = (wxString*) NULL;
    wxComboBox *item3 = new wxComboBox( parent, ID_PLUGIN_CHOOSER, wxT(""), wxDefaultPosition, wxDefaultSize, 0, strs3, wxCB_DROPDOWN );
    item1->Add( item3, 0, wxGROW|wxALL, 5 );

    wxStaticText *item4 = new wxStaticText( parent, ID_TEXT, wxT("Plugin Description:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item4, 0, wxFIXED_MINSIZE|wxALL, 5 );

    wxTextCtrl *item5 = new wxTextCtrl( parent, ID_DESCRIPTION_FIELD, wxT(""), wxDefaultPosition, wxSize(240,150), wxTE_MULTILINE|wxTE_READONLY );
    item1->Add( item5, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item6 = new wxStaticText( parent, ID_TEXT, wxT("Plugin Parameters:"), wxDefaultPosition, wxDefaultSize, 0 );
    item0->Add( item6, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxListCtrl *item7 = new wxListCtrl( parent, ID_PARAM_LIST, wxDefaultPosition, wxSize(400,200), wxLC_REPORT|wxSUNKEN_BORDER );
    item0->Add( item7, 0, wxALIGN_CENTER|wxALL, 5 );

    wxGridSizer *item8 = new wxGridSizer( 3, 0, 0 );

    item0->Add( item8, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxGridSizer *item9 = new wxGridSizer( 3, 0, 0 );

    wxButton *item10 = new wxButton( parent, ID_EDIT, wxT("&Edit Parameter..."), wxDefaultPosition, wxDefaultSize, 0 );
    item9->Add( item10, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxButton *item11 = new wxButton( parent, ID_RUN, wxT("&Run Plugin"), wxDefaultPosition, wxDefaultSize, 0 );
    item9->Add( item11, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item12 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item13 = new wxButton( parent, wxID_CLOSE, wxT("&Close"), wxDefaultPosition, wxDefaultSize, 0 );
    item12->Add( item13, 0, wxALIGN_CENTER|wxALL, 5 );

    item9->Add( item12, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item9, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}


