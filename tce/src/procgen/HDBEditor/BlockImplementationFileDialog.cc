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
 * @file BlockImplementationFileDialog.cc
 *
 * Implementation of BlockImplementationFileDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <wx/statline.h>
#include <wx/valgen.h>
#include "BlockImplementationFileDialog.hh"
#include "WxConversion.hh"
#include "ErrorDialog.hh"
#include "BlockImplementationFile.hh"
#include "FileSystem.hh"
#include "HDBManager.hh"
#include "HDBEditor.hh"
#include "HDBEditorMainFrame.hh"

using namespace HDB;

BEGIN_EVENT_TABLE(BlockImplementationFileDialog, wxDialog)
    EVT_BUTTON(ID_BROWSE, BlockImplementationFileDialog::onBrowse)
    EVT_BUTTON(wxID_OK, BlockImplementationFileDialog::onOK)
END_EVENT_TABLE()

/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param id Window identifier for the dialog window.
 * @param file BlockImplementationFile object to modify.
 */
BlockImplementationFileDialog::BlockImplementationFileDialog(
    wxWindow* parent, wxWindowID id, HDB::BlockImplementationFile& file) :
    wxDialog(parent, id, _T("Parameter")),
    file_(file) {

    createContents(this, true, true);

    path_ = WxConversion::toWxString(file_.pathToFile());
    format_ = file_.format();

    FindWindow(ID_PATH)->SetValidator(wxTextValidator(wxFILTER_ASCII, &path_));
    FindWindow(ID_FORMAT)->SetValidator(wxGenericValidator(&format_));

    TransferDataToWindow();
}


/**
 * The Destructor.
 */
BlockImplementationFileDialog::~BlockImplementationFileDialog() {
}

/**
 * Event handler for the Browse-button.
 *
 * Displays a wxFileDialog for selecting the implementation file.
 */
void
BlockImplementationFileDialog::onBrowse(wxCommandEvent&) {

    const HDBManager& manager = *wxGetApp().mainFrame().hdbManager();
    std::string hdbPath = manager.fileName();
    wxString defaultDir = WxConversion::toWxString(
    FileSystem::directoryOfPath(manager.fileName()).c_str());

    wxFileDialog dialog(
        this, _T("Choose a file"), defaultDir, _T(""),
        _T("VHDL files (*.vhd;*.vhdl)|*.vhd;*.vhdl|All files (*.*)|*.*"),
        (wxOPEN | wxFILE_MUST_EXIST));

    if (dialog.ShowModal() == wxID_OK) {

        std::string filename = WxConversion::toString(dialog.GetPath());
        std::string vhdlPath = FileSystem::directoryOfPath(filename);
        std::vector<std::string> vhdlPaths = Environment::vhdlPaths(hdbPath);
        
        for (std::vector<std::string>::iterator it = vhdlPaths.begin(); 
             it != vhdlPaths.end(); ++it) {
                 // check if VHDL path is relative to one of the search paths.
                 if (vhdlPath.substr(0, (*it).length()) == *it) {
            
                     filename = filename.substr((*it).length() + 1);
                     break;
                 }
             }

        path_ = WxConversion::toWxString(filename);
        TransferDataToWindow();
   }
}


/**
 * Event handler for the dialog OK-button.
 */
void
BlockImplementationFileDialog::onOK(wxCommandEvent&) {

    TransferDataFromWindow();

    path_ = path_.Trim(true).Trim(false);

    if (path_.IsEmpty()) {
        wxString message = _T("Path field must not be empty.");
        ErrorDialog dialog(this, message);
        dialog.ShowModal();
        return;
    }
    
    // Attempt to find the file in search paths    
    const HDBManager& manager = *wxGetApp().mainFrame().hdbManager();

    std::string hdbPath = manager.fileName();
    std::vector<std::string> vhdlPaths = 
        Environment::vhdlPaths(hdbPath);
    try {
        std::string filename = 
            FileSystem::findFileInSearchPaths(
                vhdlPaths, WxConversion::toString(path_));
        
        for (std::vector<std::string>::iterator it = vhdlPaths.begin(); 
             it != vhdlPaths.end(); ++it) {
                 if (filename.substr(0, (*it).length()) == *it) {     
                     Application::logStream()
                         << filename << " in search path " << *it << std::endl;
                     filename = filename.substr((*it).length() + 1);
                     path_ = WxConversion::toWxString(filename);
                     break;
                 }
             }
    } catch (FileNotFound f) {
        wxString message = _T("File not found from the VHDL search paths.");
        ErrorDialog dialog(this, message);
        dialog.ShowModal();
        return;
    }

    file_.setPathToFile(WxConversion::toString(path_));
    if (format_ == 0) {
        file_.setFormat(HDB::BlockImplementationFile::VHDL);
    } else {
        assert(false);
    }

    EndModal(wxID_OK);
}

/**
 * Creates the dialog contents.
 */
wxSizer*
BlockImplementationFileDialog::createContents(
    wxWindow *parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxFlexGridSizer *item1 = new wxFlexGridSizer( 3, 0, 0 );

    wxStaticText *item2 = new wxStaticText( parent, ID_LABEL_PATH, wxT("Path:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item2, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item3 = new wxTextCtrl( parent, ID_PATH, wxT(""), wxDefaultPosition, wxSize(200,-1), 0 );
    item1->Add( item3, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item4 = new wxButton( parent, ID_BROWSE, wxT("Browse..."), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item4, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item5 = new wxStaticText( parent, ID_LABEL_FORMAT, wxT("Format:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item5, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxString strs6[] = 
    {
        wxT("VHDL")
    };
    wxChoice *item6 = new wxChoice( parent, ID_FORMAT, wxDefaultPosition, wxSize(100,-1), 1, strs6, 0 );
    item1->Add( item6, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item1, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticLine *item7 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item0->Add( item7, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item8 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item9 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item8->Add( item9, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxButton *item10 = new wxButton( parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item8->Add( item10, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item8, 0, wxALIGN_CENTER|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}
