/*
    Copyright (c) 2002-2021 Tampere University.

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
 * @file ProcessorImplementationWindow.cc
 *
 * Implementation of ProcessorImplementationWindow class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2021
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
#include "ObjectState.hh"
#include "AutoSelectImplementationsDialog.hh"

#if wxCHECK_VERSION(3, 0, 0)
    #define wxSAVE wxFD_SAVE
    #define wxOVERWRITE_PROMPT wxFD_OVERWRITE_PROMPT
    #define wxOPEN wxFD_OPEN
    #define wxFILE_MUST_EXIST wxFD_FILE_MUST_EXIST
#endif

using namespace IDF;
using namespace TTAMachine;
using namespace ProGe;
using std::vector;
using std::string;

BEGIN_EVENT_TABLE(ProcessorImplementationWindow, wxDialog)
    EVT_BUTTON(ID_SELECT_RF_IMPL, ProcessorImplementationWindow::onSelectRFImplementation)
    EVT_BUTTON(ID_SELECT_IU_IMPL, ProcessorImplementationWindow::onSelectIUImplementation)
    EVT_BUTTON(ID_SELECT_FU_IMPL, ProcessorImplementationWindow::onSelectFUImplementation)
    EVT_BUTTON(ID_AUTO_SELECT_IMPL, ProcessorImplementationWindow::onAutoSelectImplementations)

    EVT_LIST_ITEM_FOCUSED(ID_RF_LIST, ProcessorImplementationWindow::onRFSelection)
    EVT_LIST_DELETE_ITEM(ID_RF_LIST, ProcessorImplementationWindow::onRFSelection)
    EVT_LIST_ITEM_SELECTED(ID_RF_LIST, ProcessorImplementationWindow::onRFSelection)
    EVT_LIST_ITEM_DESELECTED(ID_RF_LIST, ProcessorImplementationWindow::onRFSelection)
    EVT_LIST_ITEM_ACTIVATED(ID_RF_LIST, ProcessorImplementationWindow::onRFActivation)

    EVT_LIST_ITEM_FOCUSED(ID_IU_LIST, ProcessorImplementationWindow::onIUSelection)
    EVT_LIST_DELETE_ITEM(ID_IU_LIST, ProcessorImplementationWindow::onIUSelection)
    EVT_LIST_ITEM_SELECTED(ID_IU_LIST, ProcessorImplementationWindow::onIUSelection)
    EVT_LIST_ITEM_DESELECTED(ID_IU_LIST, ProcessorImplementationWindow::onIUSelection)
    EVT_LIST_ITEM_ACTIVATED(ID_IU_LIST, ProcessorImplementationWindow::onIUActivation)

    EVT_LIST_ITEM_FOCUSED(ID_FU_LIST, ProcessorImplementationWindow::onFUSelection)
    EVT_LIST_DELETE_ITEM(ID_FU_LIST, ProcessorImplementationWindow::onFUSelection)
    EVT_LIST_ITEM_SELECTED(ID_FU_LIST, ProcessorImplementationWindow::onFUSelection)
    EVT_LIST_ITEM_DESELECTED(ID_FU_LIST, ProcessorImplementationWindow::onFUSelection)
    EVT_LIST_ITEM_ACTIVATED(ID_FU_LIST, ProcessorImplementationWindow::onFUActivation)

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
    machine_(machine), impl_(impl), plugin_(NULL), dirtyData_(false) {

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

    updateImplementationLists();
    parameterList_->DeleteAllItems();
            
    if (impl_.hasICDecoderPluginFile()) {
        try {
            dynamic_cast<wxTextCtrl*>(FindWindow(ID_IC_DEC_PLUGIN_FILE))->
                SetValue(WxConversion::toWxString(impl_.icDecoderPluginFile()));
        } catch (FileNotFound& e) {
            dynamic_cast<wxTextCtrl*>(FindWindow(ID_IC_DEC_PLUGIN_FILE))->
                SetValue(WxConversion::toWxString("File not found"));
        }
    }
    
    if (impl_.hasICDecoderPluginName()) {
        dynamic_cast<wxTextCtrl*>(FindWindow(ID_IC_DEC_PLUGIN_NAME))->
            SetValue(WxConversion::toWxString(impl_.icDecoderPluginName()));
    }
    
    if (impl_.hasICDecoderHDB()) {
        try {
            dynamic_cast<wxTextCtrl*>(FindWindow(ID_IC_DEC_HDB_FILE))->
                SetValue(WxConversion::toWxString(impl_.icDecoderHDB()));
        } catch (FileNotFound& e) {
            dynamic_cast<wxTextCtrl*>(FindWindow(ID_IC_DEC_HDB_FILE))->
                SetValue(WxConversion::toWxString("File not found"));
        }
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
 * Update the list of FU implementations for single RF
 */
void
ProcessorImplementationWindow::updateRFList(const std::string& rfName, int index) {
    const RFImplementationLocation rfImpl =
        impl_.rfImplementation(rfName);

    string hdb;
    int id = 0;
    try {
        hdb = Environment::shortHDBPath(rfImpl.hdbFile());
        id = rfImpl.id();
    } catch (FileNotFound& e) {
        hdb = "Warning: " + e.errorMessage();
    }
    rfList_->SetItem(index, 1, WxConversion::toWxString(id));
    rfList_->SetItem(index, 2, WxConversion::toWxString(hdb));
}



/**
 * Event handler for the automatic implementation selection button.
 */
void
ProcessorImplementationWindow::onAutoSelectImplementations(wxCommandEvent&) {
    AutoSelectImplementationsDialog dialog(this, machine_, impl_);
    dialog.ShowModal();
    updateImplementationLists();
}

/**
 * Updates the list views on RF, IU and FU pages.
 */
void 
ProcessorImplementationWindow::updateImplementationLists() {
    fuList_->DeleteAllItems();
    rfList_->DeleteAllItems();
    iuList_->DeleteAllItems();
    
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
                hdb = Environment::shortHDBPath(fuImpl.hdbFile());
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
                hdb = Environment::shortHDBPath(rfImpl.hdbFile());
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
                hdb = Environment::shortHDBPath(iuImpl.hdbFile());
                id = iuImpl.id();
            } catch (FileNotFound& e) {
                hdb = "Warning: " + e.errorMessage();
            }
            iuList_->SetItem(i, 1, WxConversion::toWxString(id));
            iuList_->SetItem(i, 2, WxConversion::toWxString(hdb));
        }
    }
}

/**
 * Call the dialog to select RF implementation and handle the return values
 * of the dialog.
 */
void ProcessorImplementationWindow::handleSelectRFImplementation(long item) {

    const RegisterFile& rf = *machine_.registerFileNavigator().item(item);
    if (impl_.hasRFImplementation(rf.name())) {
        BlockImplementationDialog dialog(
            this, rf, impl_.rfImplementation(rf.name()));

        if (dialog.ShowModal() == wxID_OK) {
            setDirty();
            updateRFList(rf.name(), item);
        }
    } else {
        UnitImplementationLocation* location =
            new RFImplementationLocation("", -1, rf.name());

        BlockImplementationDialog dialog(this, rf, *location);
        if (dialog.ShowModal() == wxID_OK) {
            impl_.addRFImplementation(location);
            setDirty();
            updateRFList(rf.name(), item);
        } else {
            delete location;
        }
    }
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

    handleSelectRFImplementation(item);
}

/**
 * Event handler for the RF Select implementation button.
 */
void
ProcessorImplementationWindow::onRFActivation(wxListEvent& e) {
    handleSelectRFImplementation(e.GetIndex());
}


void ProcessorImplementationWindow::handleSelectIUImplementation(long item) {

    const ImmediateUnit& iu = *machine_.immediateUnitNavigator().item(item);
    if (impl_.hasIUImplementation(iu.name())) {
        BlockImplementationDialog dialog(
            this, iu, impl_.iuImplementation(iu.name()));

        if (dialog.ShowModal() == wxID_OK) {
            setDirty();
        }
    } else {
        UnitImplementationLocation* location =
            new RFImplementationLocation("", -1, iu.name());

        BlockImplementationDialog dialog(this, iu, *location);
        if (dialog.ShowModal() == wxID_OK) {
            impl_.addIUImplementation(location);
            setDirty();
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

    handleSelectIUImplementation(item);
}

/**
 * Event handler for the IU Select implementation doubleclick.
 */
void
ProcessorImplementationWindow::onIUActivation(wxListEvent& e) {
    handleSelectIUImplementation(e.GetIndex());
}

/**
 * Update the list of FU implementations for single FU
 */
void ProcessorImplementationWindow::updateFUList(const std::string& fuName, int index) {
    const FUImplementationLocation fuImpl =
        impl_.fuImplementation(fuName);
    
    string hdb;
    int id = 0;
    try {
        hdb = Environment::shortHDBPath(fuImpl.hdbFile());
        id = fuImpl.id();
    } catch (FileNotFound& e) {
        hdb = "Warning: " + e.errorMessage();
    }
    
    fuList_->SetItem(index, 1, WxConversion::toWxString(id));
    fuList_->SetItem(index, 2, WxConversion::toWxString(hdb));
}

/** 
 * Handle calling of the implementation selection dialog and it's return value
 */
void ProcessorImplementationWindow::handleSelectFUImplementation(long item) {

    const FunctionUnit& fu = *machine_.functionUnitNavigator().item(item);

    if (impl_.hasFUImplementation(fu.name())) {
        BlockImplementationDialog dialog(
            this, fu, impl_.fuImplementation(fu.name()));

        if (dialog.ShowModal() == wxID_OK) {
            setDirty();
            updateFUList(fu.name(), item);
        }
    } else {
        UnitImplementationLocation* location =
            new FUImplementationLocation("", -1, fu.name());

        BlockImplementationDialog dialog(this, fu, *location);
        if (dialog.ShowModal() == wxID_OK) {
            impl_.addFUImplementation(location);
            setDirty();
            updateFUList(fu.name(), item);
        } else {
            delete location;
        }
    }
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

    handleSelectFUImplementation(item);
}

/**
 * Event handler for the FU Select implementation doubleclick.
 */
void
ProcessorImplementationWindow::onFUActivation(wxListEvent& e) {
    handleSelectFUImplementation(e.GetIndex());
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

        // check and possibly correct file paths defined in IDF
        checkImplementationFiles();

        try {
            if (impl_.hasICDecoderPluginName() &&
                impl_.hasICDecoderPluginFile()) {
                
                loadICDecoderPlugin(
                    impl_.icDecoderPluginName(),
                    impl_.icDecoderPluginFile());
            }
        } catch (Exception& e) {
            wxString msg = _T("Warning: could not load IC decoder plugin!");
            WarningDialog warningDialog(this, msg);
            warningDialog.ShowModal();
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
    if (dirtyData_) {
        wxString message = _T("Save before exit?\n");

        wxMessageDialog dialog(this, message, message, wxYES|wxCANCEL|wxNO);
        int rv = dialog.ShowModal();
        switch(rv) {
        case wxID_NO:
            break;
        case wxID_YES:
            doSaveIDF();
            return;
        case wxID_CANCEL:
        default:
            return;
        }
    }
    Close();
}


void
ProcessorImplementationWindow::doSaveIDF() {
    wxString message = _T("Save implementation.");
    wxString defaultDir = _T(".");

    wxDocument* doc = wxGetApp().docManager()->GetCurrentDocument();
    wxString defaultFile;
    if (doc != NULL) {
        defaultFile = doc->GetFilename();
        defaultFile.erase(defaultFile.rfind('.'));
        defaultFile += _T(".idf");

        // set default save location to same where the .adf file is
        string absolutePathToADF = FileSystem::absolutePathOf(
            WxConversion::toString(defaultFile));
        defaultDir = WxConversion::toWxString(
            FileSystem::directoryOfPath(absolutePathToADF));
        defaultFile = WxConversion::toWxString(
            FileSystem::fileOfPath(WxConversion::toString(defaultFile)));
    } else {
        defaultFile = _T(".idf");
    }

    wxString fileTypes = _T("Implementation Definition File (.idf)|*.idf");

    wxFileDialog dialog(
        this, message, defaultDir, defaultFile, fileTypes,
        wxSAVE | wxOVERWRITE_PROMPT);

    if (dialog.ShowModal() == wxID_OK) {
        string path = WxConversion::toString(dialog.GetPath());
        try {
            // Make local and default file paths relative.
            std::vector<string> searchPaths;
            searchPaths.push_back(FileSystem::currentWorkingDir());
            std::vector<string> hdbSearchPaths = Environment::hdbPaths();
            impl_.makeImplFilesRelative(searchPaths);
 
            IDFSerializer serializer;
            serializer.setDestinationFile(path);
            serializer.writeMachineImplementation(impl_);
            setDirty(false);
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
 * Event handler for the Save IDF button.
 */
void
ProcessorImplementationWindow::onSaveIDF(wxCommandEvent&) {
    doSaveIDF();
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
        setDirty(true);
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
        setDirty(true);
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
        setDirty(true);
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

    try {
        impl_.setICDecoderPluginFile(pluginFile);
    } catch (FileNotFound& e) {
    }
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
 * Checks if every file path defined in IDF is correct.
 *
 * If one or more file paths are invalid, warning dialog is shown to user.
 */
void
ProcessorImplementationWindow::checkImplementationFiles() {
    size_t missingFileCount;
    size_t alternativesCount;
    
    // check implementation files, and try to fix paths for unresolved files
    if (impl_.checkImplFiles(missingFileCount, alternativesCount)) {
        // if all the files were located, exit method
        return;
    }

    // every file was not found, get missing files and possible alternatives
    wxString missFileCountStr = WxConversion::toWxString(
        static_cast<int>(missingFileCount));
    wxString altFileCountStr = WxConversion::toWxString(
        static_cast<int>(alternativesCount));
    
    // form message for the dialog
    wxString message = missFileCountStr;
    message.Append(_T(" file(s) defined in IDF couldn't be located from"));
    message.Append(_T(" absolute paths or under working directory.\n"));
    message.Append(_T("\n"));

    if (alternativesCount > 0) {
        message.Append(altFileCountStr);
        message.Append(_T(" of them were replaced with file(s) found under"));
        message.Append(_T(" the default search paths."));
    } else {
        message.Append(_T("Any alternative file paths couldn't be"));
        message.Append(_T(" located for the missing files."));
    }
    
    WarningDialog warningDialog(this, message);
    warningDialog.ShowModal();
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

    item0->Add( item1, 0, wxFIXED_MINSIZE|wxGROW|wxALL, 5 );

    wxStaticLine *item8 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(200,-1), wxLI_HORIZONTAL );
    item0->Add( item8, 0, wxFIXED_MINSIZE|wxGROW|wxALL, 5 );

    wxGridSizer *item9 = new wxGridSizer( 2, 0, 0 );

    wxBoxSizer *item10 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item11 = new wxButton( parent, ID_LOAD_IDF, wxT("Load IDF..."), wxDefaultPosition, wxDefaultSize, 0 );
    item10->Add( item11, 0, wxFIXED_MINSIZE|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxButton *item12 = new wxButton( parent, ID_SAVE_IDF, wxT("Save IDF..."), wxDefaultPosition, wxDefaultSize, 0 );
    item10->Add( item12, 0, wxFIXED_MINSIZE|wxALIGN_CENTER|wxALL, 5 );

    wxButton *item13 = new wxButton( parent, ID_GENERATE, wxT("Generate Processor"), wxDefaultPosition, wxDefaultSize, 0 );
    item10->Add( item13, 0, wxFIXED_MINSIZE|wxALIGN_CENTER|wxALL, 5 );

    item9->Add( item10, 0, wxFIXED_MINSIZE|wxALIGN_CENTER_VERTICAL, 5 );

    wxBoxSizer *item14 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item15 = new wxButton( parent, ID_CLOSE, wxT("Close"), wxDefaultPosition, wxDefaultSize, 0 );
    item14->Add( item15, 0, wxFIXED_MINSIZE|wxALIGN_CENTER|wxALL, 5 );

    item9->Add( item14, 0, wxFIXED_MINSIZE|wxALIGN_CENTER_VERTICAL, 5 );

    item0->Add( item9, 0, wxFIXED_MINSIZE|wxGROW|wxALL, 5 );

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
    item0->Add( item1, 0, wxGROW|wxALL, 5 );

    wxBoxSizer *buttonSizer = new wxBoxSizer( wxHORIZONTAL );
    wxButton *autoSelButton = new wxButton( parent, ID_AUTO_SELECT_IMPL, wxT("Auto Select Implementations"), wxDefaultPosition, wxDefaultSize, 0 );
    buttonSizer->Add( autoSelButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxButton *item2 = new wxButton( parent, ID_SELECT_RF_IMPL, wxT("Select implementation..."), wxDefaultPosition, wxDefaultSize, 0 );
    buttonSizer->Add( item2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( buttonSizer, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

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
    item0->Add( item1, 0, wxGROW|wxALL, 5 );

    wxBoxSizer *buttonSizer = new wxBoxSizer( wxHORIZONTAL );
    wxButton *autoSelButton = new wxButton( parent, ID_AUTO_SELECT_IMPL, wxT("Auto Select Implementations"), wxDefaultPosition, wxDefaultSize, 0 );
    buttonSizer->Add( autoSelButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxButton *item2 = new wxButton( parent, ID_SELECT_FU_IMPL, wxT("Select implementation..."), wxDefaultPosition, wxDefaultSize, 0 );
    buttonSizer->Add( item2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( buttonSizer, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

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
    item3->Add( item4, 0, wxALL, 5 );

    item3->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxTextCtrl *item5 = new wxTextCtrl( parent, ID_DECOMPRESSOR_PATH, wxT(""), wxDefaultPosition, wxSize(250,-1), wxTE_READONLY );
    item3->Add( item5, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item6 = new wxButton( parent, ID_BROWSE_DECOMPRESSOR, wxT("Browse..."), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( item6, 0, wxALIGN_CENTER|wxALL, 5 );

    item1->Add( item3, 0, wxGROW|wxALL, 5 );

    item0->Add( item1, 0, wxGROW|wxALL, 5 );

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
    item2->Add( item3, 0, wxALL, 5 );

    wxTextCtrl *item4 = new wxTextCtrl( parent, ID_IC_DEC_PLUGIN_FILE, wxT(""), wxDefaultPosition, wxSize(80,-1), wxTE_READONLY );
    item2->Add( item4, 0, wxGROW|wxALL, 5 );

    wxButton *item5 = new wxButton( parent, ID_BROWSE_IC_DEC_PLUGIN, wxT("Browse..."), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item5, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item6 = new wxStaticText( parent, ID_TEXT, wxT("HDB file:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item6, 0, wxALL, 5 );

    wxTextCtrl *item7 = new wxTextCtrl( parent, ID_IC_DEC_HDB_FILE, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item2->Add( item7, 0, wxGROW|wxALL, 5 );

    wxButton *item8 = new wxButton( parent, ID_BROWSE_IC_DEC_HDB, wxT("Browse..."), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item8, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item9 = new wxStaticText( parent, ID_TEXT, wxT("Plugin name:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item9, 0, wxALL, 5 );

    wxTextCtrl *item10 = new wxTextCtrl( parent, ID_IC_DEC_PLUGIN_NAME, wxT(""), wxDefaultPosition, wxSize(80,-1), wxTE_READONLY );
    item2->Add( item10, 0, wxGROW|wxALL, 5 );

    item2->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item11 = new wxStaticText( parent, ID_TEXT, wxT("Description:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item11, 0, wxALL, 5 );

    wxTextCtrl *item12 = new wxTextCtrl( parent, ID_IC_DEC_PLUGIN_DESC, wxT(""), wxDefaultPosition, wxSize(320,80), wxTE_MULTILINE|wxTE_READONLY );
    item2->Add( item12, 0, wxGROW|wxALL, 5 );

    item1->Add( item2, 0, wxFIXED_MINSIZE|wxGROW|wxALL, 5 );

    wxFlexGridSizer *item13 = new wxFlexGridSizer( 1, 0, 0 );
    item13->AddGrowableCol( 0 );
    item13->AddGrowableRow( 1 );

    wxStaticText *item14 = new wxStaticText( parent, ID_TEXT, wxT("Plugin parameters:"), wxDefaultPosition, wxDefaultSize, 0 );
    item13->Add( item14, 0, wxALL, 5 );

    wxListCtrl *item15 = new wxListCtrl( parent, ID_PARAMETER_LIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL|wxSUNKEN_BORDER );
    item13->Add( item15, 0, wxGROW|wxALL, 5 );

    wxBoxSizer *item16 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText *item17 = new wxStaticText( parent, ID_TEXT, wxT("New value:"), wxDefaultPosition, wxDefaultSize, 0 );
    item16->Add( item17, 0, wxALIGN_CENTER|wxALL, 5 );

    wxTextCtrl *item18 = new wxTextCtrl( parent, ID_PARAMETER_VALUE, wxT(""), wxDefaultPosition, wxSize(200,-1), 0 );
    item16->Add( item18, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item19 = new wxButton( parent, ID_SET_VALUE, wxT("Set"), wxDefaultPosition, wxDefaultSize, 0 );
    item16->Add( item19, 0, wxALIGN_CENTER|wxALL, 5 );

    item13->Add( item16, 0, wxALIGN_CENTER, 5 );

    item1->Add( item13, 0, wxFIXED_MINSIZE|wxGROW|wxALL, 5 );

    item0->Add( item1, 0, wxGROW|wxALL, 5 );

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
    item0->Add( item1, 0, wxGROW|wxALL, 5 );

    wxBoxSizer *buttonSizer = new wxBoxSizer( wxHORIZONTAL );
    wxButton *autoSelButton = new wxButton( parent, ID_AUTO_SELECT_IMPL, wxT("Auto Select Implementations"), wxDefaultPosition, wxDefaultSize, 0 );
    buttonSizer->Add( autoSelButton, 0, wxALL, 5 );

    wxButton *item2 = new wxButton( parent, ID_SELECT_IU_IMPL, wxT("Select implementation..."), wxDefaultPosition, wxDefaultSize, 0 );
    buttonSizer->Add( item2, 0, wxALL, 5 );

    item0->Add( buttonSizer, 0, wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}
