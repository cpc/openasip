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
 * @file ImplementationSelector.cc
 *
 * @author Esa Määttä 2008 (esa.maatta-no.spam-tut.fi)
 * @note rating: red
 */

#include <vector>
#include <string>
#include "DesignSpaceExplorerPlugin.hh"
#include "DSDBManager.hh"
#include "Machine.hh"
#include "TestApplication.hh"
#include "Program.hh"
#include "Instruction.hh"
#include "Move.hh"
#include "Terminal.hh"
#include "Operation.hh"
#include "StaticProgramAnalyzer.hh"
#include "ComponentImplementationSelector.hh"
#include "HDBRegistry.hh"
#include "ICDecoderEstimatorPlugin.hh"
#include "ControlUnit.hh"
#include "HWOperation.hh"
#include "StringTools.hh"
#include "ADFSerializer.hh"
#include "IDFSerializer.hh"
#include "Segment.hh"
#include "RFPort.hh"
#include "FullyConnectedCheck.hh"
#include "TemplateSlot.hh"
#include "CostEstimates.hh"
#include "Application.hh"
#include "Guard.hh"
#include "Exception.hh"

using namespace TTAProgram;
using namespace TTAMachine;
using namespace HDB;
using std::cerr;
using std::endl;
using std::map;

/**
 * Explorer plugin that selects implementations for units in a given adf.
 * Creates a new config with a idf.
 *
 * Supported parameters:
 *  - ic_dec, name of the ic decoder plugin, default is DefaultICDecoder
 *  - ic_hdb, name of the HDB that is used in IC estimation,
 *      default is asic_130nm_1.5V.hdb
 *  - adf, if idf is wanted to generated to some arhitecture, no default value.
 *      If adf parameter is given the idf is built.
 */
class ImplementationSelector : public DesignSpaceExplorerPlugin {
    PLUGIN_DESCRIPTION("Creates implementation for the given machine.");

    ImplementationSelector(): DesignSpaceExplorerPlugin(), 
        icDec_("DefaultICDecoder"),
        icDecHDB_("asic_130nm_1.5V.hdb"), 
        adf_("") {

        // compulsory parameters
        // no compulsory parameters

        // parameters that have a default value
        addParameter(icDecPN_, STRING, false, icDec_);
        addParameter(icDecHDBPN_, STRING, false, icDecHDB_);
        addParameter(adfPN_, STRING, false, adf_);
    }

    virtual bool requiresStartingPointArchitecture() const { return true; }
    virtual bool producesArchitecture() const { return false; }
    virtual bool requiresHDB() const { return true; }
    virtual bool requiresSimulationData() const { return false; }
    virtual bool requiresApplication() const { return false; }
    
    /**
     */
    virtual std::vector<RowID>
    explore(const RowID& configurationID, const unsigned int&) {
        readParameters();
        setupSelector();
        std::vector<RowID> result;

        // check if adf given
        if (configurationID == 0 && adf_ == "") {
            std::ostringstream msg(std::ostringstream::out);
            msg << "No configuration nor adf defined. Use -s <confID> to "
                << "define the configuration to be optimized or give adf "
                << "as plugin parameter." << endl;
            verboseLog(msg.str());
            return result;
        }

        DSDBManager& dsdb = db();
        DSDBManager::MachineConfiguration conf;
        conf.hasImplementation = false;
        TTAMachine::Machine* mach = NULL;

        // load the adf from file or from dsdb
        try {
            if (adf_ != "") {
                mach = TTAMachine::Machine::loadFromADF(adf_);
            } else {
                conf = dsdb.configuration(configurationID);
                mach = dsdb.architecture(conf.architectureID);
            }
        } catch (const Exception& e) {
            std::ostringstream msg(std::ostringstream::out);
            msg << "Error loading the adf." << std::endl;
            verboseLog(msg.str());
            return result;
        }

        IDF::MachineImplementation* idf = NULL;
        try {
            // building the idf
            idf = selector_.selectComponents(mach, icDec_, icDecHDB_);
        } catch (const Exception& e) {
            std::ostringstream msg(std::ostringstream::out);
            msg << e.errorMessage() 
                << " " << e.fileName() 
                << " " << e.lineNum() << std::endl;
            verboseLog(msg.str());
            return result;
        }
            
        // create a new configuration

        try {
            conf.architectureID = dsdb.addArchitecture(*mach);
        } catch (const RelationalDBException& e) {
            std::ostringstream msg(std::ostringstream::out);
            msg << "Error while adding ADF to the dsdb. "
                << "ADF probably too big." << endl;
            verboseLog(msg.str());
            return result;
        }
        conf.implementationID = dsdb.addImplementation(*idf, 0, 0);
        conf.hasImplementation = true;

        RowID confID = dsdb.addConfiguration(conf);
        result.push_back(confID);

        return result;
    }

private:
    /// Selector used by the plugin.
    ComponentImplementationSelector selector_;

    // parameter names
    static const std::string icDecPN_;
    static const std::string icDecHDBPN_;
    static const std::string adfPN_;

    // parameters
    /// name of the ic decoder plugin for idf
    std::string icDec_;
    /// name of the hdb used by ic decoder
    std::string icDecHDB_;
    /// name of the adf file if wanted to use idf generation
    std::string adf_;


    /**
     * Reads the parameters given to the plugin.
     */
    void readParameters() {
        readOptionalParameter(icDecPN_, icDec_);
        readOptionalParameter(icDecHDBPN_, icDecHDB_);
        readOptionalParameter(adfPN_, adf_);
    }

    
    /**
     * Sets up the component implementation selector by adding the HDBs.
     */
    void setupSelector() {
        HDBRegistry& hdbRegistry = HDBRegistry::instance();
        
        // if HDBRegistry contains no HDBManagers load from default paths
        if (hdbRegistry.hdbCount() == 0) {
            hdbRegistry.loadFromSearchPaths();
        }
        
        // give all HDBs from registry to the selector to select from
        for (int i = 0; i < hdbRegistry.hdbCount(); i++) {
            HDBManager* hdb = &hdbRegistry.hdb(i);
            selector_.addHDB(*hdb);
        }
    }
};

// parameter names
const std::string ImplementationSelector::icDecPN_("ic_dec");
const std::string ImplementationSelector::icDecHDBPN_("ic_hdb");
const std::string ImplementationSelector::adfPN_("adf");

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(ImplementationSelector)
