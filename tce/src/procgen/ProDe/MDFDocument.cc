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
 * @file MDFDocument.cc
 *
 * Definition of MDFDocument class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
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
    } catch (Exception e) {
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
    Modify(true);
    UpdateAllViews();
}


