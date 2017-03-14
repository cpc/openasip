/*
    Copyright (c) 2002-2017 Tampere University of Technology.

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
 * @file FindOperationWindow.hh
 * 
 * Declaration of FindOperationWindow class.
 * 
 * @author Alex Hirvonen 2017 (alex.hirvonen-no.spam-gmail.com)
 * @note rating: red
 */


#ifndef TTA_FIND_OPERATION_WINDOW_HH
#define TTA_FIND_OPERATION_WINDOW_HH


#include <vector>
#include <string>
#include <wx/wx.h>

#include "ProximSimulatorWindow.hh"


/**
 * Window for searching certain operations in assembly window.
 */
class FindOperationWindow : public ProximSimulatorWindow {
public:
    FindOperationWindow(ProximMainFrame* parent, int id);
    virtual ~FindOperationWindow();
    virtual void reset();

private:
    void onInputText(wxCommandEvent& event);
    void onFindPrev(wxCommandEvent& event);
    void onFindNext(wxCommandEvent& event);
    bool find(std::string searchString);

    wxSizer* createContents(wxWindow *parent, bool call_fit, bool set_sizer);

    wxTextCtrl* opInput_;
    wxStaticText* infoLabel_;
    wxButton* findPrevBtn_;
    wxButton* findNextBtn_;

    /// List of code linenumbers where serached text was found
    std::vector<int> matchedLines;
    /// Currently displayed codeline index in matchedLines
    int matchedIndex;

    /// Widget IDs.
    enum {
        ID_OP_INPUT,
        ID_INFO_LABEL,
        ID_FIND_PREV,
        ID_FIND_NEXT,
    };
    DECLARE_EVENT_TABLE();
};
#endif
