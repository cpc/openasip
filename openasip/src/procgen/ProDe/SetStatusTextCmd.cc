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
 * @file SetStatusTextCmd.cc
 *
 * Definition of SetStatusTextCmdCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2004 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include <wx/docview.h>
#include "Application.hh"
#include "SetStatusTextCmd.hh"
#include "EditPart.hh"
#include "ProDe.hh"
#include "WxConversion.hh"
#include "ChildFrame.hh"

using std::string;

/**
 * The Constructor.
 *
 * @param text Status text to set.
 */
SetStatusTextCmd::SetStatusTextCmd(string text):
    ComponentCommand(), text_(text) {
}


/**
 * The Destructor.
 */
SetStatusTextCmd::~SetStatusTextCmd() {
}


/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
SetStatusTextCmd::Do() {
    wxView * view = wxGetApp().docManager()->GetCurrentView();
    if(view != NULL){
        ChildFrame* frame = dynamic_cast<ChildFrame*>(view->GetFrame());
        frame->setStatus(WxConversion::toWxString(text_));
        return true;
    }
    return false;
}
