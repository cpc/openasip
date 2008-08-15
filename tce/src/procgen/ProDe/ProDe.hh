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
 * @file ProDe.hh
 *
 * Declaration of ProDe class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
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
