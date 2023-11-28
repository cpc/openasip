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
 * @file MDFDocument.cc
 *
 * Definition of MDFDocument class.
 *
 * @author Veli-Pekka Jääskeläinen (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include <boost/format.hpp>
#include <fstream>
#include "MDFDocument.hh"
#include "Exception.hh"
#include "ErrorDialog.hh"
#include "WxConversion.hh"
#include "ADFSerializer.hh"
#include "ProDeTextGenerator.hh"
#include "ProDeConstants.hh"
#include "FileSystem.hh"
#include "ProcessorConfigurationFile.hh"

using std::string;

IMPLEMENT_DYNAMIC_CLASS(MDFDocument, wxDocument)


/**
 * The constructor.
 */
MDFDocument::MDFDocument(): model_(NULL) {
}


/**
 * The destructor.
 */
MDFDocument::~MDFDocument() {
    if (model_ != NULL) {
        delete model_;
        model_ = NULL;
    }
}


/**
 * Creates a new document.
 *
 * @return True, if a new document was succesfully created, false otherwise.
 */
bool
MDFDocument::OnNewDocument() {
    model_ = new Model();
    model_->addObserver(this);
    return wxDocument::OnNewDocument();
}


/**
 * Opens an .adf or .cfg file. If the opened file extension is .cfg,
 * the architecture file name is read from the configuration.
 *
 * @param fileName Name of the file to read.
 * @return True if the file was succesfully loaded, false otherwise.
 */
bool
MDFDocument::OnOpenDocument(const wxString& fileName) {

    string configExtension = ProDeConstants::PROCESSOR_CONFIG_FILE_EXTENSION;
    string fileExtension =
	FileSystem::fileExtension(WxConversion::toString(fileName));

    if (fileExtension == configExtension) {
	// read .adf filename from the configuration
	return openCFG(WxConversion::toString(fileName));
    } else {
	return openADF(WxConversion::toString(fileName));
    }

    assert(false);
    return false;
}


/**
 * Reads architectrue definition file name from a processor configuration
 * file, and opens the .adf file.
 *
 * @param filename Configuration file to read.
 * @return True, if the adf defined in the .cfg was succesfully opened,
 *         false otherwise.
 */
bool
MDFDocument::openCFG(const string& filename) {

    std::fstream cfgFile(filename.c_str());

    if (cfgFile.fail()) {
	return false;
    }

    ProcessorConfigurationFile cfg(cfgFile);
    cfg.setPCFDirectory(FileSystem::directoryOfPath(filename));

    if (cfg.errors()) {
	// Errors in the .cfg file.
	// Display error strings in an error dialog.
	wxString message;
	for (int i = 0;i < cfg.errorCount();i++) {
	    message.Append(WxConversion::toWxString(cfg.errorString(i)));
	    message.Append(_T("\n\n"));
	}
	ErrorDialog dialog(GetDocumentWindow(), message);
	dialog.ShowModal();
	return false;
    }

    string adfName;

    try {
	adfName = cfg.architectureName();
    } catch (KeyNotFound& e) {
	wxString message = WxConversion::toWxString(e.errorMessage());
	ErrorDialog dialog(GetDocumentWindow(), message);
	dialog.ShowModal();
	return false;
    }

    if(openADF(adfName)) {
        SetFilename(WxConversion::toWxString(adfName), true);
        return true;
    } else {
        return false;
    }
}


/**
 * Opens an architecture definition file, and creates model of the
 * architecture.
 *
 * @param filename Architecture file to open.
 * @return True, if the file was succesfully opened, false otherwise.
 */
bool
MDFDocument::openADF(const string& filename) {
    try {
        model_ = new Model(filename);
        model_->addObserver(this);
    } catch (Exception const& e) {
        // Display an error dialog and return false.
        ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
        boost::format fmt =
            prodeTexts->text(ProDeTextGenerator::MSG_ERROR_LOADING_FILE);
        fmt % filename;
        wxString message = WxConversion::toWxString(fmt.str());
	message.Append(_T("\n"));
        message.Append(WxConversion::toWxString(e.errorMessage()));
        ErrorDialog errorDialog(GetDocumentWindow(), message);
        errorDialog.ShowModal();
        return false;
    }

    // Update document title.
    string title = FileSystem::fileOfPath(filename);
    SetTitle(WxConversion::toWxString(title));

    // File opened succesfully.
    Modify(false);
    UpdateAllViews();
    return true;
}


/**
 * Writes the machine object model to an mdf file using the mdf writer
 * component and sets the model unmodified.
 *
 * @param filename Name of the file into which the Model will be saved.
 */
bool
MDFDocument::OnSaveDocument(const wxString& filename) {

    ADFSerializer writer;
    writer.setDestinationFile(WxConversion::toString(filename));
    try {
        writer.writeMachine(*model_->getMachine());
    } catch (Exception& e) {
        ErrorDialog errorDialog(GetDocumentWindow(),
	    WxConversion::toWxString(e.errorMessage()));
        errorDialog.ShowModal();
        return false;
    }

    Modify(false);
    return true;
}


/**
 * Returns document's model.
 *
 * @return Document's model.
 */
Model*
MDFDocument::getModel() {
    return model_;
}


/**
 * Updates the document when the Model changes.
 */
void
MDFDocument::update() {
    if (model_->isModified()) {
        Modify(true);
        model_->setNotModified();
    }
    UpdateAllViews();
}


