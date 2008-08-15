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
 * @file ImplementationSelector.cc
 *
 * @author Esa Määttä 2008 (esa.maatta@tut.fi)
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
public:
    DESCRIPTION("Creates implementation for given machine.");
    
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
            errorOuput(msg.str());
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
            errorOuput(msg.str());
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
            errorOuput(msg.str());
            return result;
        }
            
        // create a new configuration

        try {
            conf.architectureID = dsdb.addArchitecture(*mach);
        } catch (const RelationalDBException& e) {
            std::ostringstream msg(std::ostringstream::out);
            msg << "Error while adding ADF to the dsdb. "
                << "ADF probably too big." << endl;
            errorOuput(msg.str());
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
        // Parameter name of bus number in the machine.
        const std::string icDec = "ic_dec";
        const std::string icDecoderDefault = "DefaultICDecoder";
        const std::string icDecHDB = "ic_hdb";
        const std::string icDecHDBDefault = "asic_130nm_1.5V.hdb";
        const std::string adf = "adf";

        if (hasParameter(icDec)) {
            try {
                icDec_ = parameterValue(icDec);
            } catch (const Exception& e) {
                parameterError(icDec, "String");
                icDec_ = icDecoderDefault;
            }
        } else {
            // set defaut value to icDec
            icDec_ = icDecoderDefault;
        }

        if (hasParameter(icDecHDB)) {
            try {
                icDecHDB_ = parameterValue(icDecHDB);
            } catch (const Exception& e) {
                parameterError(icDecHDB, "String");
                icDecHDB_ = icDecHDBDefault;
            }
        } else {
            // set defaut value to icDecHDB
            icDecHDB_ = icDecHDBDefault;
        }

        if (hasParameter(adf)) {
            try {
                adf_ = parameterValue(adf);
            } catch (const Exception& e) {
                parameterError(adf, "string");
                adf_ = "";
            }
        } else {
            adf_ = "";
        }
    }

    
    /**
     * Print error message of invalid parameter to plugin error stream.
     *
     * @param param Name of the parameter that has invalid value.
     * @param type Type of the parameter ought to be.
     */
    void parameterError(const std::string& param, const std::string& type) {
        std::ostringstream msg(std::ostringstream::out);
        msg << "Invalid parameter value '" << parameterValue(param)
            << "' on parameter '" << param << "'. " << type 
            << " value expected." << std::endl;
        errorOuput(msg.str());
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

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(ImplementationSelector)
