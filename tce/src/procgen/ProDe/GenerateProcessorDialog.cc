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
 * @file GenerateProcessorDialog.cc
 *
 * Implementation of GenerateProcessorDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include <vector>
#include <sstream>

#include <wx/wx.h>
#include <wx/dirdlg.h>
#include <wx/statline.h>
#include <wx/filedlg.h>

#include "GenerateProcessorDialog.hh"
#include "WxConversion.hh"
#include "BinaryEncoding.hh"
#include "BEMSerializer.hh"
#include "BEMGenerator.hh"
#include "ErrorDialog.hh"
#include "BEMValidator.hh"
#include "FileSystem.hh"
#include "MachineImplementation.hh"
#include "ICDecoderGeneratorPlugin.hh"
#include "ProcessorGenerator.hh"
#include "PluginTools.hh"
#include "InformationDialog.hh"
#include "ProGeScriptGenerator.hh"
#include "ProGeTestBenchGenerator.hh"

using std::vector;
using std::string;

BEGIN_EVENT_TABLE(GenerateProcessorDialog, wxDialog)
    EVT_BUTTON(ID_BROWSE_BEM_LOAD, GenerateProcessorDialog::onBrowseBEMLoad)
    EVT_BUTTON(ID_BROWSE_BEM_SAVE, GenerateProcessorDialog::onBrowseBEMSave)
    EVT_BUTTON(ID_BROWSE_TARGET, GenerateProcessorDialog::onBrowseTargetDir)
    EVT_BUTTON(wxID_OK, GenerateProcessorDialog::onOK)
END_EVENT_TABLE()


/**
 * The Constructor.
 *
 * @param paren Parent window of the dialog.
 * @param machine Processor architecture.
 * @param imp Processor implementation definition.
 */
GenerateProcessorDialog::GenerateProcessorDialog(
    wxWindow* parent,
    TTAMachine::Machine& machine,
    const IDF::MachineImplementation& impl) :
    wxDialog(parent, -1, _T("Generate Processor")),
    machine_(machine), impl_(impl), bem_(NULL) {

    createContents(this, true, true);
}


/**
 * The Destructor.
 */
GenerateProcessorDialog::~GenerateProcessorDialog() {
    if (bem_ != NULL) {
        delete bem_;
    }
}

/**
 * Event handler for the OK button.
 */
void
GenerateProcessorDialog::onOK(wxCommandEvent&) {

    // Binary encoding map.
    if (dynamic_cast<wxRadioButton*>(
            FindWindow(ID_GENERATE_BEM))->GetValue()) {

        // Generate new bem.
        if (bem_ != NULL) {
            delete bem_;
            bem_ = NULL;
        }
        BEMGenerator generator(machine_);
        bem_ = generator.generate();

        if (dynamic_cast<wxCheckBox*>(FindWindow(ID_SAVE_BEM))->IsChecked()) {
            wxTextCtrl* ctrl =
                dynamic_cast<wxTextCtrl*>(FindWindow(ID_BEM_SAVE_PATH));
            string bemPath = WxConversion::toString(ctrl->GetValue());
            if (bemPath != "") {
                BEMSerializer serializer;
                serializer.setDestinationFile(bemPath);
                try {
                    serializer.writeBinaryEncoding(*bem_);
                } catch (Exception& e) {
                    wxString message = _T("Error saving BEM:");
                    message.Append(WxConversion::toWxString(e.errorMessage()));
                    ErrorDialog dialog(this, message);
                    dialog.ShowModal();
                    return;
                }
            } else {
                wxString message = _T("BEM target file not defined.");
                ErrorDialog dialog(this, message);
                dialog.ShowModal();
                return;
            }
        }
        
    } else {
        if (bem_ == NULL) {
            wxString message = _T("Error: No binary encoding map loaded.");
            ErrorDialog dialog(this, message);
            dialog.ShowModal();
            return;
        }
    }

    // Target directory.
    wxTextCtrl* ctrl = dynamic_cast<wxTextCtrl*>(FindWindow(ID_TARGET));
    string targetDir = WxConversion::toString(ctrl->GetValue());
    if (!FileSystem::fileExists(targetDir) ||
        !FileSystem::fileIsDirectory(targetDir)) {

        wxString message = _T("Target directory '");
        message.Append(WxConversion::toWxString(targetDir));
        message.Append(_T("' does not exist."));
        ErrorDialog dialog(this, message);
        dialog.ShowModal();
        return;
    }

    // validate BEM against machine
    BEMValidator bemValidator(*bem_, machine_);
    if (!bemValidator.validate()) {
        wxString message = _T("Error in Binary Encoding Map:\n");
        for (int i = 0; i < bemValidator.errorCount(); i++) {
            message.Append(
                WxConversion::toWxString(bemValidator.errorMessage(i)));
            message.Append(_T("\n"));
        }
        ErrorDialog dialog(this, message);
        dialog.ShowModal();
        return;
    }

    ProGe::ICDecoderGeneratorPlugin* plugin = NULL;
    
    // Load IC/Decoder generator plugin.
    if (!impl_.hasICDecoderPluginFile() ||
        !impl_.hasICDecoderPluginName()) {

        wxString message = _T("IC/decoder generator plugin not defined");
        ErrorDialog dialog(this, message);
        dialog.ShowModal();
        return;
    } else {
        plugin = loadICDecoderGeneratorPlugin(
            impl_.icDecoderPluginFile(), impl_.icDecoderPluginName());

        if (plugin == NULL) return;
    }

    // Set plugin parameters.
    for (unsigned i = 0; i < impl_.icDecoderParameterCount(); i++) {
        plugin->setParameter(
            impl_.icDecoderParameterName(i),
            impl_.icDecoderParameterValue(i));
    }

    string errorMessages;
    std::ostringstream errStream(errorMessages);

    ProGe::ProcessorGenerator generator;
    try {
        generator.generateProcessor(
            ProGe::VHDL, machine_, impl_, *plugin, 1, targetDir, errStream);
    } catch (Exception& e) {
        wxString message = WxConversion::toWxString(e.errorMessage());
        ErrorDialog dialog(this, message);
        dialog.ShowModal();
        return;
    }

    // generate test bench for simulating
    string testbenchDir = targetDir + FileSystem::DIRECTORY_SEPARATOR +
        "tb";
    try {
        ProGeTestBenchGenerator::ProGeTestBenchGenerator tbGen =
            ProGeTestBenchGenerator::ProGeTestBenchGenerator();
        tbGen.generate(machine_, impl_, testbenchDir, targetDir);
    } catch (const Exception& e) {
        string errorMsg = "Warning: Processor Generator failed to "
                          "generate a test bench.\n"; 
        wxString message = WxConversion::toWxString(errorMsg + 
            e.errorMessage());
        ErrorDialog dialog(this, message);
        dialog.ShowModal();
    }

    // generate vhdl compilation and simulation scripts
    try {
        ProGeScriptGenerator::ProGeScriptGenerator sGen(targetDir, targetDir, 
                testbenchDir);
        sGen.generateAll();
    } 
    catch (const Exception& e) {
        string errorMsg = "Warning: Processor Generator failed to "
                          "generate a simulation/compilation scripts.\n";
        wxString message = WxConversion::toWxString(errorMsg + 
            e.errorMessage());
        ErrorDialog dialog(this, message);
        dialog.ShowModal();
    }

    if (errorMessages != "") {
        ErrorDialog dialog(this, WxConversion::toWxString(errorMessages));
        dialog.ShowModal();
        return;
    }

    delete plugin;

    wxString message = _T("Processor was succesfully generated.");
    InformationDialog dialog(this, message);
    dialog.ShowModal();

    EndModal(wxID_OK);
}

/**
 * Event handler for the bem saving Browse... button.
 */
void
GenerateProcessorDialog::onBrowseBEMSave(wxCommandEvent&) {

    wxString fileTypes = _T("Binary Encoding Map (.bem)|*.bem|");
    wxFileDialog dialog(
        this, _T("Choose a file"), _T("."), _T(""), fileTypes,
        wxSAVE | wxOVERWRITE_PROMPT);

    if (dialog.ShowModal() == wxID_OK) {
        wxTextCtrl* ctrl = dynamic_cast<wxTextCtrl*>(
            FindWindow(ID_BEM_SAVE_PATH));
        ctrl->SetValue(dialog.GetPath());
    }
}


/**
 * Event handler for the ben loading Browse... button.
 */
void
GenerateProcessorDialog::onBrowseBEMLoad(wxCommandEvent&) {

    wxTextCtrl* ctrl = dynamic_cast<wxTextCtrl*>(FindWindow(ID_BEM_LOAD_PATH));

    wxFileDialog dialog(
        this, _T("Choose a file"), _T(""), _T(""),
        _T("Binary Encoding Maps (*.bem)|*.bem|All files|*"),
        (wxOPEN | wxFILE_MUST_EXIST));

    if (dialog.ShowModal() == wxID_OK) {
        if (bem_ != NULL) {
            delete bem_;
            bem_ = NULL;
            ctrl->SetValue(_T(""));
        }
        string path = WxConversion::toString(dialog.GetPath());
        try {
            BEMSerializer serializer;
            serializer.setSourceFile(path);
            bem_ = serializer.readBinaryEncoding();
        } catch (Exception& e) {
            wxString message = _T("Error loading binary encoding map:\n");
            message.Append(WxConversion::toWxString(e.errorMessage()));
            ErrorDialog dialog(this, message);
            dialog.ShowModal();
            return;
        }
        ctrl->SetValue(dialog.GetPath());
    }
}


/**
 * Event handler for the target directory Browse... button.
 */
void
GenerateProcessorDialog::onBrowseTargetDir(wxCommandEvent&) {

    wxDirDialog dialog(
        this, _T("Choose a directory"), _T(""), wxDD_NEW_DIR_BUTTON);

    if (dialog.ShowModal() == wxID_OK) {
        wxTextCtrl* ctrl = dynamic_cast<wxTextCtrl*>(FindWindow(ID_TARGET));
        ctrl->SetValue(dialog.GetPath());
    }
}

/**
 * Loads the given IC/decoder generator plugin.
 *
 * @param pluginFile The file that implements the plugin.
 * @param pluginName Name of the plugin.
 */
ProGe::ICDecoderGeneratorPlugin*
GenerateProcessorDialog::loadICDecoderGeneratorPlugin(
    const string& pluginFile,
    const string& pluginName) {

    PluginTools pluginTool;

    // initialize the plugin tool
    vector<string> pluginPaths = Environment::icDecoderPluginPaths();
    for (vector<string>::const_iterator iter = pluginPaths.begin();
         iter != pluginPaths.end(); iter++) {
        try {
            pluginTool.addSearchPath(*iter);
        } catch (const FileNotFound&) {
        }
    }

    try {
        pluginTool.registerModule(pluginFile);
    } catch (Exception& e) {
        wxString message = _T("Error loading IC/Decoder generator plugin:\n");
        message.Append(WxConversion::toWxString(e.errorMessage()));
        ErrorDialog dialog(this, message);
        dialog.ShowModal();
        return NULL;
    }

    ProGe::ICDecoderGeneratorPlugin* (*creator)(
        TTAMachine::Machine&, BinaryEncoding&);

    pluginTool.importSymbol(
        "create_generator_plugin_" + pluginName, creator, pluginFile);

    ProGe::ICDecoderGeneratorPlugin* plugin = creator(machine_, *bem_);
    return plugin;
}

/**
 * Craetes the dialog contents.
 */
wxSizer*
GenerateProcessorDialog::createContents(
    wxWindow *parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *item1 = new wxBoxSizer( wxVERTICAL );

    wxStaticBox *item3 = new wxStaticBox( parent, -1, wxT("Binary Encoding Map:") );
    wxStaticBoxSizer *item2 = new wxStaticBoxSizer( item3, wxVERTICAL );

    wxRadioButton *item4 = new wxRadioButton( parent, ID_GENERATE_BEM, wxT("Generate new"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    item4->SetValue( TRUE );
    item2->Add( item4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item5 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *item6 = new wxBoxSizer( wxHORIZONTAL );

    wxCheckBox *item7 = new wxCheckBox( parent, ID_SAVE_BEM, wxT("Save to file:"), wxDefaultPosition, wxDefaultSize, 0 );
    item6->Add( item7, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );

    item5->Add( item6, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item8 = new wxBoxSizer( wxHORIZONTAL );

    wxTextCtrl *item9 = new wxTextCtrl( parent, ID_BEM_SAVE_PATH, wxT(""), wxDefaultPosition, wxSize(300,-1), 0 );
    item8->Add( item9, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item10 = new wxButton( parent, ID_BROWSE_BEM_SAVE, wxT("Browse..."), wxDefaultPosition, wxDefaultSize, 0 );
    item8->Add( item10, 0, wxALIGN_CENTER|wxALL, 5 );

    item5->Add( item8, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item2->Add( item5, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxRadioButton *item11 = new wxRadioButton( parent, ID_LOAD_BEM, wxT("Load from file"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item11, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item12 = new wxBoxSizer( wxHORIZONTAL );

    wxTextCtrl *item13 = new wxTextCtrl( parent, ID_BEM_LOAD_PATH, wxT(""), wxDefaultPosition, wxSize(300,-1), 0 );
    item12->Add( item13, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item14 = new wxButton( parent, ID_BROWSE_BEM_LOAD, wxT("Browse..."), wxDefaultPosition, wxDefaultSize, 0 );
    item12->Add( item14, 0, wxALIGN_CENTER|wxALL, 5 );

    item2->Add( item12, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item1->Add( item2, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item1->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticBox *item16 = new wxStaticBox( parent, -1, wxT("Target directory:") );
    wxStaticBoxSizer *item15 = new wxStaticBoxSizer( item16, wxVERTICAL );

    wxBoxSizer *item17 = new wxBoxSizer( wxHORIZONTAL );

    wxTextCtrl *item18 = new wxTextCtrl( parent, ID_TARGET, wxT(""), wxDefaultPosition, wxSize(300,-1), 0 );
    item17->Add( item18, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item19 = new wxButton( parent, ID_BROWSE_TARGET, wxT("Browse..."), wxDefaultPosition, wxDefaultSize, 0 );
    item17->Add( item19, 0, wxALIGN_CENTER|wxALL, 5 );

    item15->Add( item17, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item1->Add( item15, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticLine *item20 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item1->Add( item20, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item21 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item22 = new wxButton( parent, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item21->Add( item22, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item23 = new wxButton( parent, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item21->Add( item23, 0, wxALIGN_CENTER|wxALL, 5 );

    item1->Add( item21, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}
