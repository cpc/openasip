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
 * @file MemoryDialog.hh
 *
 * Declaration of MemoryDialog class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_MEMORY_DIALOG_HH
#define TTA_MEMORY_DIALOG_HH


#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/choice.h>
#include <wx/textctrl.h>

#include "BaseType.hh"
#include "OSEdListener.hh"

class OperationContext;
class MemoryControl;

/**
 * Dialog for monitoring the contents of the memory.
 */
class MemoryDialog : public wxDialog, public OSEdListener {
public:
    MemoryDialog(wxWindow* window);
    virtual ~MemoryDialog();

    virtual void handleEvent(OSEdInformer::EventId event);

private:
    /// Copying not allowed.
    MemoryDialog(const MemoryDialog&);
    /// Assignment not allowed.
    MemoryDialog& operator=(const MemoryDialog&);

    wxSizer* createContents(wxWindow* window, bool call_fit, bool set_sizer);
    void setTexts();
    void onClose(wxCommandEvent&);

    /**
     * Component ids.
     */
    enum {
        ID_BUTTON_CLOSE = 1000,
        ID_CONTROL_MEMORY
    };

    /// Memory control of the dialog.
    MemoryControl* memoryWindow_;

    DECLARE_EVENT_TABLE()
};

#endif
