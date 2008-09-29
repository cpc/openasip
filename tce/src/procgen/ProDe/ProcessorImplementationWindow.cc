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
 * @file ProcessorImplementationWindow.cc
 *
 * Implementation of ProcessorImplementationWindow class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/statline.h>
#include <wx/notebook.h>
#include <wx/docview.h>

#include "ProcessorImplementationWindow.hh"
#include "IDFSerializer.hh"
#include "MachineImplementation.hh"
#include "Conversion.hh"
#include "WxConversion.hh"
#include "IDFValidator.hh"
#include "ErrorDialog.hh"
#include "WarningDialog.hh"
#include "Machine.hh"
#include "BlockImplementationDialog.hh"
#include "RegisterFile.hh"
#include "FunctionUnit.hh"
#include "ICDecoderGeneratorPlugin.hh"
#include "Environment.hh"
#include "BinaryEncoding.hh"
#include "BEMGenerator.hh"
#include "FileSystem.hh"
#include "GenerateProcessorDialog.hh"
#include "ProDe.hh"

using namespace IDF;
using namespace TTAMachine;
using namespace ProGe;
using std::vector;
using std::string;

BEGIN_EVENT_TABLE(ProcessorImplementationWindow, wxDialog)
    EVT_BUTTON(ID_SELECT_RF_IMPL, ProcessorImplementationWindow::onSelectRFImplementation)
    EVT_BUTTON(ID_SELECT_IU_IMPL, ProcessorImplementationWindow::onSelectIUImplementation)
    EVT_BUTTON(ID_SELECT_FU_IMPL, ProcessorImplementationWindow::onSelectFUImplementation)

    EVT_LIST_ITEM_FOCUSED(ID_RF_LIST, ProcessorImplementationWindow::onRFSelection)
    EVT_LIST_DELETE_ITEM(ID_RF_LIST, ProcessorImplementationWindow::onRFSelection)
    EVT_LIST_ITEM_SELECTED(ID_RF_LIST, ProcessorImplementationWindow::onRFSelection)
    EVT_LIST_ITEM_DESELECTED(ID_RF_LIST, ProcessorImplementationWindow::onRFSelection)

    EVT_LIST_ITEM_FOCUSED(ID_IU_LIST, ProcessorImplementationWindow::onIUSelection)
    EVT_LIST_DELETE_ITEM(ID_IU_LIST, ProcessorImplementationWindow::onIUSelection)
    EVT_LIST_ITEM_SELECTED(ID_IU_LIST, ProcessorImplementationWindow::onIUSelection)
    EVT_LIST_ITEM_DESELECTED(ID_IU_LIST, ProcessorImplementationWindow::onIUSelection)

    EVT_LIST_ITEM_FOCUSED(ID_FU_LIST, ProcessorImplementationWindow::onFUSelection)
    EVT_LIST_DELETE_ITEM(ID_FU_LIST, ProcessorImplementationWindow::onFUSelection)
    EVT_LIST_ITEM_SELECTED(ID_FU_LIST, ProcessorImplementationWindow::onFUSelection)
    EVT_LIST_ITEM_DESELECTED(ID_FU_LIST, ProcessorImplementationWindow::onFUSelection)

    EVT_LIST_ITEM_FOCUSED(ID_PARAMETER_LIST, ProcessorImplementationWindow::onParameterSelection)
    EVT_LIST_DELETE_ITEM(ID_PARAMETER_LIST, ProcessorImplementationWindow::onParameterSelection)
    EVT_LIST_ITEM_SELECTED(ID_PARAMETER_LIST, ProcessorImplementationWindow::onParameterSelection)
    EVT_LIST_ITEM_DESELECTED(ID_PARAMETER_LIST, ProcessorImplementationWindow::onParameterSelection)

    EVT_BUTTON(ID_BROWSE_DECOMPRESSOR, ProcessorImplementationWindow::onBrowseDecompressor)
    EVT_BUTTON(ID_BROWSE_IC_DEC_PLUGIN, ProcessorImplementationWindow::onBrowseICDecPlugin)
    EVT_BUTTON(ID_BROWSE_IC_DEC_HDB, ProcessorImplementationWindow::onBrowseICHDB)
    EVT_BUTTON(ID_SET_VALUE, ProcessorImplementationWindow::onSetParameterValue)
    EVT_BUTTON(ID_LOAD_IDF, ProcessorImplementationWindow::onLoadIDF)
    EVT_BUTTON(ID_SAVE_IDF, ProcessorImplementationWindow::onSaveIDF)
    EVT_BUTTON(ID_GENERATE, ProcessorImplementationWindow::onGenerateProcessor)
    EVT_BUTTON(ID_CLOSE, ProcessorImplementationWindow::onClose)
END_EVENT_TABLE()


/**
 * Constructor.
 *
 * @param parent Parent window of the page.
 * @param machine Processor architecture.
 * @param impl Processor implementation definition.
 */
ProcessorImplementationWindow::ProcessorImplementationWindow(
    wxWindow* parent, TTAMachine::Machine& machine,
    IDF::MachineImplementation& impl):
    wxDialog(parent, -1, _T("Processor Implementation"),
             wxDefaultPosition, wxDefaultSize,
             wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
    machine_(machine), impl_(impl), plugin_(NULL) {

    createContents(this, true, true);
    SetMinSize(wxSize(600, 500));

    // Plugin name is always fixed, thus disable editing
    dynamic_cast<wxTextCtrl*>(FindWindow(ID_IC_DEC_PLUGIN_NAME))->
       Enable(false);

    fuList_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_FU_LIST));
    rfList_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_RF_LIST));
    iuList_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_IU_LIST));
    parameterList_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_PARAMETER_LIST));

    // Add FU and RF list columns.
    fuList_->InsertColumn(0, _T("FU"), wxLIST_FORMAT_LEFT, 150);
    fuList_->InsertColumn(1, _T("ID"), wxLIST_FORMAT_LEFT, 40);
    fuList_->InsertColumn(2, _T("HDB"), wxLIST_FORMAT_LEFT, 400);

    rfList_->InsertColumn(0, _T("RF"), wxLIST_FORMAT_LEFT, 150);
    rfList_->InsertColumn(1, _T("ID"), wxLIST_FORMAT_LEFT, 40);
    rfList_->InsertColumn(2, _T("HDB"), wxLIST_FORMAT_LEFT, 400);

    iuList_->InsertColumn(0, _T("IU"), wxLIST_FORMAT_LEFT, 150);
    iuList_->InsertColumn(1, _T("ID"), wxLIST_FORMAT_LEFT, 40);
    iuList_->InsertColumn(2, _T("HDB"), wxLIST_FORMAT_LEFT, 400);

    parameterList_->InsertColumn(0, _T("Name"), wxLIST_FORMAT_LEFT, 150);
    parameterList_->InsertColumn(1, _T("Value"), wxLIST_FORMAT_LEFT, 100);
    parameterList_->InsertColumn(
        2, _T("Description"), wxLIST_FORMAT_LEFT, 300);

    // Disable conditional buttons by default.
    FindWindow(ID_SELECT_RF_IMPL)->Disable();
    FindWindow(ID_SELECT_IU_IMPL)->Disable();
    FindWindow(ID_SELECT_FU_IMPL)->Disable();
    FindWindow(ID_SET_VALUE)->Disable();

    // Load default plugin
    std::string pluginFile = Environment::defaultICDecoderPlugin();
    std::string pluginName = FileSystem::fileNameBody(pluginFile);
    assert(pluginName.length() > 6);
    pluginName = pluginName.substr(0, pluginName.length() - 6);

    loadICDecoderPlugin(pluginName, pluginFile);

}


/**
 * Destructor.
 */
ProcessorImplementationWindow::~ProcessorImplementationWindow() {
}

/**
 * Transfers data from the current MachineImplementation object to the
 * dialog widgets.
 */
bool
ProcessorImplementationWindow::TransferDataToWindow() {

    fuList_->DeleteAllItems();
    rfList_->DeleteAllItems();
    iuList_->DeleteAllItems();
    parameterList_->DeleteAllItems();

    // FU implementation list.
    Machine::FunctionUnitNavigator fuNav =
        machine_.functionUnitNavigator();

    for (int i = 0; i < fuNav.count(); i++) {
        string fuName = fuNav.item(i)->name();
        fuList_->InsertItem(i, WxConversion::toWxString(fuName));
        if (impl_.hasFUImplementation(fuName)) {
            const FUImplementationLocation fuImpl =
                impl_.fuImplementation(fuName);

            string hdb;
            int id = 0;
            try {
                hdb = fuImpl.hdbFile();
                id = fuImpl.id();
            } catch (FileNotFound& e) {
                hdb = "Warning: " + e.errorMessage();
            }

            fuList_->SetItem(i, 1, WxConversion::toWxString(id));
            fuList_->SetItem(i, 2, WxConversion::toWxString(hdb));
        }
    }

    // RF implementation list.
    const Machine::RegisterFileNavigator rfNav =
        machine_.registerFileNavigator();

    for (int i = 0; i < rfNav.count(); i++) {
        string rfName = rfNav.item(i)->name();
        rfList_->InsertItem(i, WxConversion::toWxString(rfName));
        if (impl_.hasRFImplementation(rfName)) {
            const RFImplementationLocation rfImpl =
                impl_.rfImplementation(rfName);

            string hdb;
            int id = 0;
            try {
                hdb = rfImpl.hdbFile();
                id = rfImpl.id();
            } catch (FileNotFound& e) {
                hdb = "Warning: " + e.errorMessage();
            }
            rfList_->SetItem(i, 1, WxConversion::toWxString(id));
            rfList_->SetItem(i, 2, WxConversion::toWxString(hdb));
        }
    }

    // IU implementation list.
    const Machine::ImmediateUnitNavigator iuNav =
        machine_.immediateUnitNavigator();

    for (int i = 0; i < iuNav.count(); i++) {
        string iuName = iuNav.item(i)->name();
        iuList_->InsertItem(i, WxConversion::toWxString(iuName));
        if (impl_.hasIUImplementation(iuName)) {
            const RFImplementationLocation iuImpl =
                impl_.iuImplementation(iuName);

            string hdb;
            int id = 0;
            try {
                hdb = iuImpl.hdbFile();
                id = iuImpl.id();
            } catch (FileNotFound& e) {
                hdb = "Warning: " + e.errorMessage();
            }
            iuList_->SetItem(i, 1, WxConversion::toWxString(id));
            iuList_->SetItem(i, 2, WxConversion::toWxString(hdb));
        }
    }

    if (impl_.hasICDecoderPluginFile()) {
        dynamic_cast<wxTextCtrl*>(FindWindow(ID_IC_DEC_PLUGIN_FILE))->
            SetValue(WxConversion::toWxString(impl_.icDecoderPluginFile()));
    }

    if (impl_.hasICDecoderPluginName()) {
        dynamic_cast<wxTextCtrl*>(FindWindow(ID_IC_DEC_PLUGIN_NAME))->
            SetValue(WxConversion::toWxString(impl_.icDecoderPluginName()));
    }
    
    if (impl_.hasICDecoderHDB()) {
        dynamic_cast<wxTextCtrl*>(FindWindow(ID_IC_DEC_HDB_FILE))->
            SetValue(WxConversion::toWxString(impl_.icDecoderHDB()));
    }
    

    wxTextCtrl* descCtrl = dynamic_cast<wxTextCtrl*>(
        FindWindow(ID_IC_DEC_PLUGIN_DESC));

    // Set IC&decoder generator plugin attributes.
    if (plugin_ != NULL) {

        descCtrl->SetValue(
            WxConversion::toWxString(plugin_->pluginDescription()));

        // IC / Decoder plugin parameters.
        for (int i = 0; i < plugin_->recognizedParameterCount(); i++) {
            string paramName = plugin_->recognizedParameter(i);
            string paramValue =
                impl_.icDecoderParameterValue(plugin_->recognizedParameter(i));
            string paramDesc = plugin_->parameterDescription(paramName);
            parameterList_->InsertItem(i, WxConversion::toWxString(paramName));
            parameterList_->SetItem(
                i, 1, WxConversion::toWxString(paramValue));
            parameterList_->SetItem(
                i, 2, WxConversion::toWxString(paramDesc));
        }
        parameterList_->SetColumnWidth(2, wxLIST_AUTOSIZE);
    } else {
        descCtrl->SetValue(_T("\n   No IC / Decoder plugin loaded."));
    }

    if (impl_.hasDecompressorFile()) {
        try {
            dynamic_cast<wxTextCtrl*>(FindWindow(ID_DECOMPRESSOR_PATH))->
                SetValue(WxConversion::toWxString(impl_.decompressorFile()));
        } catch (FileNotFound& e) {
            wxString message =
                _T("Unable to set decompressor block file:\n");

            WarningDialog dialog(this, message);
            dialog.ShowModal();
        }
    }

    return true;
}

/**
 * Event handler for the RF Select implementation button.
 */
void
ProcessorImplementationWindow::onSelectRFImplementation(wxCommandEvent&) {

    long item = -1;
    item = rfList_->GetNextItem(
        item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    if (item == -1) {
        // No rf selected.
        return;
    }

    const RegisterFile& rf = *machine_.registerFileNavigator().item(item);
    if (impl_.hasRFImplementation(rf.name())) {
        BlockImplementationDialog dialog(
            this, rf, impl_.rfImplementation(rf.name()));

        dialog.ShowModal();
    } else {
        UnitImplementationLocation* location =
            new RFImplementationLocation("", -1, rf.name());

        BlockImplementationDialog dialog(this, rf, *location);
        if (dialog.ShowModal() == wxID_OK) {
            impl_.addRFImplementation(location);
        } else {
            delete location;
        }
    }
    TransferDataToWindow();
}

/**
 * Event handler for the IU Select implementation button.
 */
void
ProcessorImplementationWindow::onSelectIUImplementation(wxCommandEvent&) {

    long item = -1;
    item = iuList_->GetNextItem(
        item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    if (item == -1) {
        // No iu selected.
        return;
    }

    const ImmediateUnit& iu = *machine_.immediateUnitNavigator().item(item);
    if (impl_.hasIUImplementation(iu.name())) {
        BlockImplementationDialog dialog(
            this, iu, impl_.iuImplementation(iu.name()));

        dialog.ShowModal();
    } else {
        UnitImplementationLocation* location =
            new RFImplementationLocation("", -1, iu.name());

        BlockImplementationDialog dialog(this, iu, *location);
        if (dialog.ShowModal() == wxID_OK) {
            impl_.addIUImplementation(location);
        } else {
            delete location;
        }
    }
    TransferDataToWindow();
}


/**
 * Event handler for the FU Select implementation button.
 */
void
ProcessorImplementationWindow::onSelectFUImplementation(wxCommandEvent&) {

    long item = -1;
    item = fuList_->GetNextItem(
        item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    if (item == -1) {
        // No fu selected.
        return;
    }

    const FunctionUnit& fu = *machine_.functionUnitNavigator().item(item);

    if (impl_.hasFUImplementation(fu.name())) {
        BlockImplementationDialog dialog(
            this, fu, impl_.fuImplementation(fu.name()));

        dialog.ShowModal();
    } else {
        UnitImplementationLocation* location =
            new FUImplementationLocation("", -1, fu.name());

        BlockImplementationDialog dialog(this, fu, *location);
        if (dialog.ShowModal() == wxID_OK) {
            impl_.addFUImplementation(location);
        } else {
            delete location;
        }
    }
    TransferDataToWindow();
}


/**
 * Event handler for the RF list selection changes.
 *
 * Enables and disables the select RF implementation button.
 */
void
ProcessorImplementationWindow::onRFSelection(wxListEvent&) {
    if (rfList_->GetSelectedItemCount() == 1) {
        FindWindow(ID_SELECT_RF_IMPL)->Enable();
    } else {
        FindWindow(ID_SELECT_RF_IMPL)->Disable();
    }
}

/**
 * Event handler for the IU list selection changes.
 *
 * Enables and disables the select IU implementation button.
 */
void
ProcessorImplementationWindow::onIUSelection(wxListEvent&) {
    if (iuList_->GetSelectedItemCount() == 1) {
        FindWindow(ID_SELECT_IU_IMPL)->Enable();
    } else {
        FindWindow(ID_SELECT_IU_IMPL)->Disable();
    }
}

/**
 * Event handler for the FU list selection changes.
 *
 * Enables and disables the select FU implementation button.
 */
void
ProcessorImplementationWindow::onFUSelection(wxListEvent&) {
    if (fuList_->GetSelectedItemCount() == 1) {
        FindWindow(ID_SELECT_FU_IMPL)->Enable();
    } else {
        FindWindow(ID_SELECT_FU_IMPL)->Disable();
    }
}

/**
 * Event handler for the ic/decoder parameter list selection changes.
 *
 * Enables and disables the set parameter value button.
 */
void
ProcessorImplementationWindow::onParameterSelection(wxListEvent&) {
    if (parameterList_->GetSelectedItemCount() == 1) {
        FindWindow(ID_SET_VALUE)->Enable();
    } else {
        FindWindow(ID_SET_VALUE)->Disable();
    }
}

/**
 * Event handler for the Load IDF... button.
 */
void
ProcessorImplementationWindow::onLoadIDF(wxCommandEvent&) {

    wxFileDialog dialog(
        this, _T("Choose a file"), _T(""), _T(""),
        _T("Implemenation Definition Files files (*.idf)|*.idf|All files|*"),
        (wxOPEN | wxFILE_MUST_EXIST));

    if (dialog.ShowModal() == wxID_OK) {

        ObjectState* old =impl_.saveState();

        string idfFile =
            WxConversion::toString(dialog.GetPath());

        IDFSerializer serializer;
        serializer.setSourceFile(idfFile);
        
        try {
            ObjectState* newIDF = serializer.readState();
            impl_.loadState(newIDF);
            delete newIDF;
        } catch (Exception& e) {
            impl_.loadState(old);
            wxString message = _T("Error reading idf '");
            message.Append(WxConversion::toWxString(idfFile));
            message.Append(_T("':\n"));
            message.Append(WxConversion::toWxString(e.errorMessage()));
            ErrorDialog errorDialog(this, message);
            errorDialog.ShowModal();
            return;
        }

        IDFValidator validator(impl_, machine_);
        if (!validator.validate()) {
            wxString message = _T("Warning:\n");
            for (int i = 0; i < validator.errorCount(); i++) {
                message.Append(
                    WxConversion::toWxString(validator.errorMessage(i)));

                message.Append(_T("\n"));
            }
            WarningDialog warningDialog(this, message);
            warningDialog.ShowModal();

            // Remove implementations of units that doesn't exist in the
            // architecture.
            IDFValidator::removeUnknownImplementations(impl_, machine_);
        }

        if (impl_.hasICDecoderPluginName() &&
            impl_.hasICDecoderPluginFile()) {

            loadICDecoderPlugin(
                impl_.icDecoderPluginName(),
                impl_.icDecoderPluginFile());
        }

        TransferDataToWindow();
        delete old;
    }
}


/**
 * Event handler for the Close button.
 */
void
ProcessorImplementationWindow::onClose(wxCommandEvent&) {
    Close();
}


/**
 * Event handler for the Save IDF button.
 */
void
ProcessorImplementationWindow::onSaveIDF(wxCommandEvent&) {

    wxString message = _T("Save implementation.");
    wxString defaultDir = _T(".");

    wxDocument* doc = wxGetApp().docManager()->GetCurrentDocument();
    wxString defaultFile;
    if (doc != NULL) {
        defaultFile = doc->GetFilename();
        defaultFile.erase(defaultFile.rfind('.'));
        defaultFile += _T(".idf");
    } else {
        defaultFile = _T(".idf");
    }

    wxString fileTypes = _T("Implementation Definition File (.idf)|*.idf|");

    wxFileDialog dialog(
        this, message, defaultDir, defaultFile, fileTypes,
        wxSAVE | wxOVERWRITE_PROMPT);

    if (dialog.ShowModal() == wxID_OK) {
        string path = WxConversion::toString(dialog.GetPath());
        try {
            IDFSerializer serializer;
            serializer.setDestinationFile(path);
            serializer.writeMachineImplementation(impl_);
        } catch (Exception& e) {
            wxString message = _T("Error writing '");
            message.Append(WxConversion::toWxString(path));
            message.Append(_T("':"));
            message.Append(WxConversion::toWxString(e.errorMessage()));
            ErrorDialog dialog(this, message);
            dialog.ShowModal();
        }
    }
}


/**
 * Event handler for the decompressor block Browse... button.
 */
void
ProcessorImplementationWindow::onBrowseDecompressor(wxCommandEvent&) {
    wxFileDialog dialog(
        this, _T("Choose a file"), _T(""), _T(""),
        _T("Decompressor block files (*.vhdl;*.vhd)|*.vhdl;*.vhd|"
           "All files|*"),
        (wxOPEN | wxFILE_MUST_EXIST));

    if (dialog.ShowModal() == wxID_OK) {
        string decompressorFile =
            WxConversion::toString(dialog.GetPath());

        impl_.setDecompressorFile(decompressorFile);
    }

    TransferDataToWindow();
}


/**
 * Event handler for the ic/decoder plugin Browse... button.
 */
void
ProcessorImplementationWindow::onBrowseICDecPlugin(wxCommandEvent&) {

    wxFileDialog dialog(
        this, _T("Choose a file"), _T(""), _T(""),
        _T("IC/Decoder plugins (*.so)|*.so|All files|*.*"),
        (wxOPEN | wxFILE_MUST_EXIST));

    if (dialog.ShowModal() == wxID_OK) {
        string pluginFile =
            WxConversion::toString(dialog.GetPath());

        // An ugly way to determine the plugin name which should be the
        // file name minus the "Plugin.so" ending.
        string pluginName = FileSystem::fileOfPath(pluginFile);
        pluginName = FileSystem::fileNameBody(pluginFile);
        if (pluginName.length() < 6 ||
            pluginName.substr(pluginName.length() - 6) != "Plugin") {

            wxString message = _T("Unable to determine plugin name.\n");
            message.Append(_T("Plugin file must be named "));
            message.Append(_T("'<plugin name>Plugin.so'"));
            ErrorDialog dialog(this, message);
            dialog.ShowModal();
            return;
        }
        pluginName = pluginName.substr(0, pluginName.length() - 6);
        loadICDecoderPlugin(pluginName, pluginFile);
        impl_.clearICDecoderParameters();           
    }

    TransferDataToWindow();
}


/**
 * Event handler for the ic/ hdb Browse... button.
 */
void
ProcessorImplementationWindow::onBrowseICHDB(wxCommandEvent&) {

    wxFileDialog dialog(
        this, _T("Choose a file"), _T(""), _T(""),
        _T("HDB Files (*.hdb)|*.hdb|All files|*.*"),
        (wxOPEN | wxFILE_MUST_EXIST));

    if (dialog.ShowModal() == wxID_OK) {
        string hdbFile = WxConversion::toString(dialog.GetPath());
        impl_.setICDecoderHDB(hdbFile);         
    }

    TransferDataToWindow();
}


/**
 * Event handler for the ic/decoder parameter value setting button.
 */
void
ProcessorImplementationWindow::onSetParameterValue(wxCommandEvent&) {

    long item = -1;
    item = parameterList_->GetNextItem(
        item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    
    if (item < 0) return;

    wxTextCtrl* valueCtrl = dynamic_cast<wxTextCtrl*>(
        FindWindow(ID_PARAMETER_VALUE));

    std::string name = plugin_->recognizedParameter(item);
    std::string value = WxConversion::toString(valueCtrl->GetValue());
    impl_.setICDecoderParameter(name, value);
    TransferDataToWindow();
}


/**
 * Loads an IC/Decoder plugin.
 *
 * @param pluginName Name of the plugin.
 * @param pluginFile Full path to the plugin file.
 */
void
ProcessorImplementationWindow::loadICDecoderPlugin(
    const string& pluginName,
    const string& pluginFile) {

    if (plugin_ != NULL) {
        delete plugin_;
        plugin_ = NULL;
    }

    // initialize the plugin tool
    vector<string> pluginPaths = Environment::icDecoderPluginPaths();
    for (vector<string>::const_iterator iter = pluginPaths.begin();
         iter != pluginPaths.end(); iter++) {
        try {
            pluginTool_.addSearchPath(*iter);
        } catch (const FileNotFound&) {
        }
    }

    try {
        pluginTool_.registerModule(pluginFile);
    } catch (const FileNotFound&) {
        wxString message = _T("Plugin file '");
        message.Append(WxConversion::toWxString(pluginFile));
        message.Append(_T("' doesn't exist"));
        ErrorDialog dialog(this, message);
        dialog.ShowModal();
        return;
    } catch (Exception& e) {
        wxString message = _T("Error loading plugin file '");
        message.Append(WxConversion::toWxString(pluginFile));
        message.Append(_T("':\n"));
        message.Append(WxConversion::toWxString(e.errorMessage()));
        ErrorDialog dialog(this, message);
        dialog.ShowModal();
        return;
    }

    ICDecoderGeneratorPlugin* (*creator)(
        TTAMachine::Machine&, BinaryEncoding&);

    try {
        pluginTool_.importSymbol(
            "create_generator_plugin_" + pluginName, creator, pluginFile);

        BEMGenerator generator(machine_);
        BinaryEncoding* bem = generator.generate();
        plugin_ = creator(machine_, *bem);
        delete bem;
    } catch (Exception& e) {
        wxString message = _T("Error loading plugin '");
        message.Append(WxConversion::toWxString(pluginName));
        message.Append(_T("' from '"));
        message.Append(WxConversion::toWxString(pluginFile));
        message.Append(_T("':\n"));
        message.Append(WxConversion::toWxString(e.errorMessage()));
        ErrorDialog dialog(this, message);
        dialog.ShowModal();
        return;
    }
    impl_.setICDecoderPluginFile(pluginFile);
    impl_.setICDecoderPluginName(pluginName);
    assert(plugin_ != NULL);
    return;
}

/**
 * Event handler for the Generate Processor... button.
 */
void
ProcessorImplementationWindow::onGenerateProcessor(wxCommandEvent&) {

    IDFValidator validator(impl_, machine_);
    if (!validator.validate()) {
        wxString message;
        for (int i = 0; i < validator.errorCount(); i++) {
            message.Append(
                WxConversion::toWxString(validator.errorMessage(i)));

            message.Append(_T("\n"));
        }
        ErrorDialog dialog(this, message);
        dialog.ShowModal();
        return;
    }

    GenerateProcessorDialog dialog(this, machine_, impl_);
    dialog.ShowModal();
}


/**
 * Creates the dialog widgets.
 *
 * Code geenrated by wxDesigner. Do not modify manually.
 *
 * @param parent Parent window of the created widget hierarchy.
 * @param call_fit Fit parent window size.
 * @param set_sizer Set the created top level sizer as the parent win contents.
 */

wxSizer *ProcessorImplementationWindow::createContents( wxWindow *parent, bool call_fit, bool set_sizer )
{
    wxFlexGridSizer *item0 = new wxFlexGridSizer( 1, 0, 0 );
    item0->AddGrowableCol( 0 );
    item0->AddGrowableRow( 0 );

    wxNotebook *item2 = new wxNotebook( parent, ID_NOTEBOOK, wxDefaultPosition, wxDefaultSize, 0 );
#if !wxCHECK_VERSION(2,5,2)
    wxNotebookSizer *item1 = new wxNotebookSizer( item2 );
#else
    wxWindow *item1 = item2;
#endif

    wxPanel *item3 = new wxPanel( item2, -1 );
    ProcessorImplementationWindow::registerFilePage( item3, FALSE );
    item2->AddPage( item3, wxT("Register Files") );

    wxPanel *item4 = new wxPanel( item2, -1 );
    ProcessorImplementationWindow::immediateUnitPage( item4, FALSE );
    item2->AddPage( item4, wxT("Immediate Units") );

    wxPanel *item5 = new wxPanel( item2, -1 );
    ProcessorImplementationWindow::functionUnitPage( item5, FALSE );
    item2->AddPage( item5, wxT("Function Units") );

    wxPanel *item6 = new wxPanel( item2, -1 );
    ProcessorImplementationWindow::decompressionPage( item6, FALSE );
    item2->AddPage( item6, wxT("Instruction Compression") );

    wxPanel *item7 = new wxPanel( item2, -1 );
    ProcessorImplementationWindow::icDecoderPluginPage( item7, FALSE );
    item2->AddPage( item7, wxT("IC / Decoder Plugin") );

    item0->Add( item1, 0, wxADJUST_MINSIZE|wxFIXED_MINSIZE|wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticLine *item8 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(200,-1), wxLI_HORIZONTAL );
    item0->Add( item8, 0, wxADJUST_MINSIZE|wxFIXED_MINSIZE|wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxGridSizer *item9 = new wxGridSizer( 2, 0, 0 );

    wxBoxSizer *item10 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item11 = new wxButton( parent, ID_LOAD_IDF, wxT("Load IDF..."), wxDefaultPosition, wxDefaultSize, 0 );
    item10->Add( item11, 0, wxADJUST_MINSIZE|wxFIXED_MINSIZE|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxButton *item12 = new wxButton( parent, ID_SAVE_IDF, wxT("Save IDF..."), wxDefaultPosition, wxDefaultSize, 0 );
    item10->Add( item12, 0, wxADJUST_MINSIZE|wxFIXED_MINSIZE|wxALIGN_CENTER|wxALL, 5 );

    wxButton *item13 = new wxButton( parent, ID_GENERATE, wxT("Generate Processor"), wxDefaultPosition, wxDefaultSize, 0 );
    item10->Add( item13, 0, wxADJUST_MINSIZE|wxFIXED_MINSIZE|wxALIGN_CENTER|wxALL, 5 );

    item9->Add( item10, 0, wxADJUST_MINSIZE|wxFIXED_MINSIZE|wxALIGN_CENTER_VERTICAL, 5 );

    wxBoxSizer *item14 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item15 = new wxButton( parent, ID_CLOSE, wxT("Close"), wxDefaultPosition, wxDefaultSize, 0 );
    item14->Add( item15, 0, wxADJUST_MINSIZE|wxFIXED_MINSIZE|wxALIGN_CENTER|wxALL, 5 );

    item9->Add( item14, 0, wxADJUST_MINSIZE|wxFIXED_MINSIZE|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

    item0->Add( item9, 0, wxADJUST_MINSIZE|wxFIXED_MINSIZE|wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}

wxSizer *ProcessorImplementationWindow::registerFilePage( wxWindow *parent, bool call_fit, bool set_sizer )
{
    wxFlexGridSizer *item0 = new wxFlexGridSizer( 1, 0, 0 );
    item0->AddGrowableCol( 0 );
    item0->AddGrowableRow( 0 );

    wxListCtrl *item1 = new wxListCtrl( parent, ID_RF_LIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL|wxSUNKEN_BORDER );
    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxButton *item2 = new wxButton( parent, ID_SELECT_RF_IMPL, wxT("Select implementation..."), wxDefaultPosition, wxDefaultSize, 0 );
    item0->Add( item2, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}

wxSizer *ProcessorImplementationWindow::functionUnitPage( wxWindow *parent, bool call_fit, bool set_sizer )
{
    wxFlexGridSizer *item0 = new wxFlexGridSizer( 1, 0, 0 );
    item0->AddGrowableCol( 0 );
    item0->AddGrowableRow( 0 );

    wxListCtrl *item1 = new wxListCtrl( parent, ID_FU_LIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL|wxSUNKEN_BORDER );
    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxButton *item2 = new wxButton( parent, ID_SELECT_FU_IMPL, wxT("Select implementation..."), wxDefaultPosition, wxDefaultSize, 0 );
    item0->Add( item2, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}

wxSizer *ProcessorImplementationWindow::decompressionPage( wxWindow *parent, bool call_fit, bool set_sizer )
{
    wxFlexGridSizer *item0 = new wxFlexGridSizer( 1, 0, 0 );
    item0->AddGrowableCol( 0 );

    item0->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticBox *item2 = new wxStaticBox( parent, -1, wxT("Instruction Decompressor:") );
    wxStaticBoxSizer *item1 = new wxStaticBoxSizer( item2, wxVERTICAL );

    wxFlexGridSizer *item3 = new wxFlexGridSizer( 2, 0, 0 );

    wxStaticText *item4 = new wxStaticText( parent, ID_TEXT, wxT("Decompressor block file:"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( item4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item3->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxTextCtrl *item5 = new wxTextCtrl( parent, ID_DECOMPRESSOR_PATH, wxT(""), wxDefaultPosition, wxSize(250,-1), wxTE_READONLY );
    item3->Add( item5, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item6 = new wxButton( parent, ID_BROWSE_DECOMPRESSOR, wxT("Browse..."), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( item6, 0, wxALIGN_CENTER|wxALL, 5 );

    item1->Add( item3, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}

wxSizer *ProcessorImplementationWindow::icDecoderPluginPage( wxWindow *parent, bool call_fit, bool set_sizer )
{
    wxFlexGridSizer *item0 = new wxFlexGridSizer( 1, 0, 0 );
    item0->AddGrowableCol( 0 );
    item0->AddGrowableRow( 0 );

    wxFlexGridSizer *item1 = new wxFlexGridSizer( 1, 0, 0 );
    item1->AddGrowableCol( 0 );
    item1->AddGrowableRow( 1 );

    wxFlexGridSizer *item2 = new wxFlexGridSizer( 3, 0, 0 );

    wxStaticText *item3 = new wxStaticText( parent, ID_TEXT, wxT("Plugin file:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item3, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item4 = new wxTextCtrl( parent, ID_IC_DEC_PLUGIN_FILE, wxT(""), wxDefaultPosition, wxSize(80,-1), wxTE_READONLY );
    item2->Add( item4, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxButton *item5 = new wxButton( parent, ID_BROWSE_IC_DEC_PLUGIN, wxT("Browse..."), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item5, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item6 = new wxStaticText( parent, ID_TEXT, wxT("HDB file:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item6, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item7 = new wxTextCtrl( parent, ID_IC_DEC_HDB_FILE, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item2->Add( item7, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxButton *item8 = new wxButton( parent, ID_BROWSE_IC_DEC_HDB, wxT("Browse..."), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item8, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item9 = new wxStaticText( parent, ID_TEXT, wxT("Plugin name:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item9, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item10 = new wxTextCtrl( parent, ID_IC_DEC_PLUGIN_NAME, wxT(""), wxDefaultPosition, wxSize(80,-1), wxTE_READONLY );
    item2->Add( item10, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item2->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item11 = new wxStaticText( parent, ID_TEXT, wxT("Description:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item11, 0, wxALIGN_RIGHT|wxALL, 5 );

    wxTextCtrl *item12 = new wxTextCtrl( parent, ID_IC_DEC_PLUGIN_DESC, wxT(""), wxDefaultPosition, wxSize(320,80), wxTE_MULTILINE|wxTE_READONLY );
    item2->Add( item12, 0, wxGROW|wxALL, 5 );

    item1->Add( item2, 0, wxFIXED_MINSIZE|wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxFlexGridSizer *item13 = new wxFlexGridSizer( 1, 0, 0 );
    item13->AddGrowableCol( 0 );
    item13->AddGrowableRow( 1 );

    wxStaticText *item14 = new wxStaticText( parent, ID_TEXT, wxT("Plugin parameters:"), wxDefaultPosition, wxDefaultSize, 0 );
    item13->Add( item14, 0, wxALL, 5 );

    wxListCtrl *item15 = new wxListCtrl( parent, ID_PARAMETER_LIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL|wxSUNKEN_BORDER );
    item13->Add( item15, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item16 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText *item17 = new wxStaticText( parent, ID_TEXT, wxT("New value:"), wxDefaultPosition, wxDefaultSize, 0 );
    item16->Add( item17, 0, wxALIGN_CENTER|wxALL, 5 );

    wxTextCtrl *item18 = new wxTextCtrl( parent, ID_PARAMETER_VALUE, wxT(""), wxDefaultPosition, wxSize(200,-1), 0 );
    item16->Add( item18, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item19 = new wxButton( parent, ID_SET_VALUE, wxT("Set"), wxDefaultPosition, wxDefaultSize, 0 );
    item16->Add( item19, 0, wxALIGN_CENTER|wxALL, 5 );

    item13->Add( item16, 0, wxALIGN_CENTER, 5 );

    item1->Add( item13, 0, wxFIXED_MINSIZE|wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}

wxSizer *ProcessorImplementationWindow::immediateUnitPage( wxWindow *parent, bool call_fit, bool set_sizer )
{
    wxFlexGridSizer *item0 = new wxFlexGridSizer( 1, 0, 0 );
    item0->AddGrowableCol( 0 );
    item0->AddGrowableRow( 0 );

    wxListCtrl *item1 = new wxListCtrl( parent, ID_IU_LIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL|wxSUNKEN_BORDER );
    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxButton *item2 = new wxButton( parent, ID_SELECT_IU_IMPL, wxT("Select implementation..."), wxDefaultPosition, wxDefaultSize, 0 );
    item0->Add( item2, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}
