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
 * @file CallExplorerPlugin.hh
 *
 * Declaration of CallExplorerPluginWindow class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen-no.spam-.tut.fi)
 * @note rating: red
 */

#ifndef CALL_EXPLORER_PLUGIN_WINDOW_HH
#define CALL_EXPLORER_PLUGIN_WINDOW_HH

#include <wx/wx.h>
#include "PluginTools.hh"
#include "DesignSpaceExplorer.hh"
#include "ExplorerPluginParameter.hh"

namespace TTAMachine {
    class Machine;
}

class DesignSpaceExplorerPlugin;
class Model;

/**
 * Explorer plugin call window.
 */
class CallExplorerPluginWindow : public wxDialog {
public:
    CallExplorerPluginWindow(
        wxWindow* parent, TTAMachine::Machine& machine, Model& model);

    virtual ~CallExplorerPluginWindow();

private:

    virtual bool TransferDataToWindow();
    void onClose(wxCommandEvent& event);
    void onEdit(wxCommandEvent&);
    void onRun(wxCommandEvent& event);
    void onSelectPlugin(wxCommandEvent& event);
    void onParamActivate(wxListEvent& event);
    void onParamSelect(wxListEvent& event);
    void onParamDeSelect(wxListEvent& event);
    
    void doEdit();
    void updateParameterList();
    void setTexts();  
    ExplorerPluginParameter getParam(int paramNum);
    
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    
    

    /// Machine that is being modified.
    TTAMachine::Machine& machine_;
    /// The model
    Model& model_;
    
    /// Plugin names combo box
    wxComboBox* pluginNames_;
    /// description text field
    wxTextCtrl* pluginDescription_;
    /// Parameter list;
    wxListCtrl* parameterList_;
    /// Edit button
    wxButton* editButton;
    /// Run button
    wxButton* runButton;
    /// Selected parameter, or -1
    int selectedParam_;

    // Widget IDs.
    enum {
        ID_TEXT = 10000,
        ID_PLUGIN_CHOOSER,
        ID_DESCRIPTION_FIELD,
        ID_PARAM_LIST,
        ID_EDIT,
        ID_RUN
    };
    
    DesignSpaceExplorer explorer_;
    std::vector<DesignSpaceExplorerPlugin*> plugins_;
    DesignSpaceExplorerPlugin* selectedPlugin_;

    DECLARE_EVENT_TABLE()
};

#endif
