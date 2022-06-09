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
 * @file ProDe.hh
 *
 * Declaration of ProDe class.
 *
 * @author Veli-Pekka Jääskeläinen (vjaaskel-no.spam-cs.tut.fi)
 */

#ifndef TTA_MAC_EDIT_HH
#define TTA_MAC_EDIT_HH

#include <wx/app.h>
#include <string>

class wxDocManager;
class wxDocMDIParentFrame;
class MainFrame;
class ProDeOptions;
class CommandRegistry;

/**
 * Represents the ProDe application.
 *
 * This class is responsible for parsing the command line and
 * initializing the editor accordingly. If the editor is executed in
 * interactive mode, a graphical user interface will be
 * initialized. The class is derived from wxApp class, which does the
 * low level initialization of wxWindows automatically.
 */

class ProDe : public wxApp {
public:
    ProDe();
    bool OnInit();
    int OnExit();
    MainFrame* mainFrame() const;
    ProDeOptions* options() const;
    void setOptions(ProDeOptions* options);
    CommandRegistry* commandRegistry() const;
    wxDocManager* docManager() const;
    static std::string bitmapsDirPath();

private:
    void createDefaultOptions();

    /// Manages multiple documents.
    wxDocManager* docManager_;
    /// Main frame of the application.
    MainFrame* mainFrame_;
    /// editor options
    ProDeOptions* options_;
    /// editor command registry
    CommandRegistry* commandRegistry_;
};

DECLARE_APP(ProDe)

#endif
