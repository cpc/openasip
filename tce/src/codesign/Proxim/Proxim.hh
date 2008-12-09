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
