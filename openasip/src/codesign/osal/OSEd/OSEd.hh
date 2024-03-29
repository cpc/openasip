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
 * @file OSEd.hh
 *
 * Declaration of OSEd class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OSED_HH
#define TTA_OSED_HH

#include <wx/app.h>

#include "OSEdMainFrame.hh"

class OSEdOptions;

/**
 * Main class for OSEd (Operation Set Editor).
 *
 * Initializes the application.
 */
class OSEd : public wxApp {
public:
    OSEd();
    virtual ~OSEd();
    
    virtual bool OnInit();
    virtual int OnExit();
    
    OSEdMainFrame* mainFrame() const;
    OSEdOptions* options() const;
    
private:
    /// Copying not allowed.
    OSEd(const OSEd&);
    /// Assignment not allowed.
    OSEd& operator=(const OSEd&);
    
    void createDefaultOptions();

    /// Main window of the application.
    OSEdMainFrame* mainFrame_;
    /// Options of the application.
    OSEdOptions* options_;
};

DECLARE_APP(OSEd)

#endif
