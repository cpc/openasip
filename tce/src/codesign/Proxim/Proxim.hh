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
 * @file Proxim.hh
 *
 * Declaration of Proxim class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef PROXIM_HH
#define PROXIM_HH

#include <wx/app.h>
#include "CommandRegistry.hh"

class MainFrame;

class ProximMainFrame;
class wxPanel;
class ProximSimulationThread;
class GUIOptions;

/**
 * Represents the Proxim application.
 *
 * This is the top level class of the application. Derived from the wxApp
 * class, which does the low level initialization of wxWidgets. This class
 * is responsible for the application initialization and termination.
 */
class Proxim : public wxApp {
public:
    Proxim();
    virtual ~Proxim();
    virtual bool OnInit();
    virtual int OnExit();
    ProximSimulationThread* simulation();
    CommandRegistry& commandRegistry();
    GUIOptions& options();

private:
    void loadOptions();
    void createDefaultOptions();

    /// The application main frame.
    ProximMainFrame* mainFrame_;
    /// The application command registry.
    CommandRegistry* commandRegistry_;
    /// The simulation thread running the simulator backend.
    ProximSimulationThread* simulation_;
    /// Options defining toolbar, keyboard shortcuts etc.
    GUIOptions* options_;

    /// Name of the config file.
    static const std::string CONFIG_FILE_NAME;
    /// Name of the xml-schema file.
    static const std::string SCHEMA_FILE_NAME;
};

DECLARE_APP(Proxim)

#endif
