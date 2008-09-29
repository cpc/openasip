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
 * @file MinimalOpSet.cc
 *
 * Explorer plugin that checks that given config or adf meets minimal opset
 * requirements stated by minimal machine adf or user given reference adf.
 *
 * @author Esa Määttä 2008 (esa.maatta-no.spam-tut.fi)
 * @note rating: red
 */

#include <vector>
#include <string>
#include "DesignSpaceExplorerPlugin.hh"
#include "DSDBManager.hh"
#include "Machine.hh"
#include "CostEstimates.hh"
#include "HDBRegistry.hh"
#include "StringTools.hh"
#include "Exception.hh"

#include "MinimalOpSetCheck.hh"

//using namespace TTAProgram;
using namespace TTAMachine;
using namespace HDB;

/**
 * Explorer plugin that checks that given config or adf meets minimal opset
 * requirements stated by minimal machine adf or user given reference adf.
 *
 * Supported parameters:
 */
class MinimalOpSet : public DesignSpaceExplorerPlugin {
public:
    DESCRIPTION("Minimal opset checker and fixer plugin.");
    
    /**
     * Explorer plugin that checks that given config or adf meets minimal opset
     * requirements stated by minimal machine adf or user given reference adf.
     */
    virtual std::vector<RowID>
    explore(const RowID& configurationID, const unsigned int&) {
        readParameters();
        std::vector<RowID> result;
        MinimalOpSetCheck minimalOpSetCheck = MinimalOpSetCheck();

        // make params for adf and idf, so no configuration needed
        if (configurationID == 0 && adf_ == "") {
            std::ostringstream msg(std::ostringstream::out);
            msg << "No configuration nor adf defined. Use -s <confID> to "
                << "define the configuration to be optimized or give adf "
                << "as plugin parameter." << std::endl;
            errorOuput(msg.str());
            return result;
        }

        DSDBManager& dsdb = db();
        DSDBManager::MachineConfiguration conf;
        conf.architectureID = -1; 
        conf.hasImplementation = false; 
        
        // load adf/idf from file if no configuration was given
        if (configurationID == 0) {
            if (!createConfig(adf_, idf_, dsdb, conf)) {
               return result; 
            }
        } else {
            // if starting configuration given load it
            conf = dsdb.configuration(configurationID);
        }

        // load machine from configuration
        // TODO: load machine from file is adf_ is specified.
        Machine* mach = NULL;
        try {
            mach = dsdb.architecture(conf.architectureID);
        } catch (const Exception& e) {
            std::ostringstream msg(std::ostringstream::out);
            msg << e.errorMessage() << std::endl;
            errorOuput(msg.str());
            return result;
        }

        // check minimal opset and print missing ops
        if (printMissingOps_) {
            std::vector<std::string> missingOps;
            minimalOpSetCheck.missingOperations(*mach, missingOps);
            if (missingOps.empty()) {
                verboseLog("MinimalOpSet: Configuration/machine has all"
                        " the operations in the minimal opset already.")
                return result;
            }
            for (unsigned int i = 0; i < missingOps.size(); ++i) {
                verboseLogC(missingOps.at(i) + " : operation is missing.", 1);
            }
        }

        // add FUs to the machine, so that it has all the operations in the
        // minimal opset
        try {
            minimalOpSetCheck.fix(*mach);
        } catch (const InvalidData& e) {
            verboseLog("MinimalOpSet: Configuration/machine has all the"
                    "operations in the minimal opset already.")
            return result;
        }

        // create the result config
        DSDBManager::MachineConfiguration newConf;
        newConf.architectureID = dsdb.addArchitecture(*mach);
        newConf.hasImplementation = false;

        // if old config had implementation create a new one for new config
        if (conf.hasImplementation) {
            createImplementation(newConf, newConf);
        }

        CostEstimates estimates;
        bool estimate = (newConf.hasImplementation ? true : false);
        try {
            if (!evaluate(newConf, estimates, estimate)) {
                debugLog(std::string("Evaluate failed."));
                return result;
            }
        } catch (const Exception& e) {
            debugLog(std::string("Error in Evaluate plugin: ")
                    + e.errorMessage() + std::string(" ")
                    + e.errorMessageStack());
            return result;
        }

        RowID newConfID = addConfToDSDB(newConf);
        if (newConfID != 0) {
                result.push_back(newConfID);
        }
        return result;
    }

private:
    /// name of the adf file to evaluate
    std::string adf_;
    /// name of the idf file to evaluate
    std::string idf_;
    /// print missing ops
    bool printMissingOps_;

    /**
     * Reads the parameters given to the plugin.
     */
    void readParameters() {
        const std::string adf = "adf";
        const std::string idf = "idf";
        const std::string printMissingOps = "print";
        const bool printMissingOpsDefault = false;

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

        if (hasParameter(idf)) {
            try {
                idf_ = parameterValue(idf);
            } catch (const Exception& e) {
                parameterError(idf, "string");
                idf_ = "";
            }
        } else {
            idf_ = "";
        }

        if (hasParameter(printMissingOps)) {
            try {
                printMissingOps_ =
                    booleanValue(parameterValue(printMissingOps));
            } catch (const Exception& e) {
                parameterError(printMissingOps, "Boolean");
                printMissingOps_ = printMissingOpsDefault;
            }
        } else {
            // set defaut value
            printMissingOps_ = printMissingOpsDefault;
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
        verboseLog(msg.str());
    }
    
    
    /**
     * Load adf and idf from files and store to given dsdb and config.
     *
     * @param adf Path of architecture definition file.
     * @param idf Path of implementation definition file.
     * @param dsdb Database where to store adf and idf.
     * @param conf Configuration for adf/idf ids.
     * @return True if creating config succeeded, else false.
     */
    bool createConfig(
        const std::string& adf,
        const std::string& idf,
        DSDBManager& dsdb,
        DSDBManager::MachineConfiguration& conf) {

        assert(adf != "");

        IDF::MachineImplementation* idfo = NULL;
        TTAMachine::Machine* mach = NULL;
        try {
            if (adf != "") {
                mach = TTAMachine::Machine::loadFromADF(adf);
                conf.architectureID = dsdb.addArchitecture(*mach);
            } else {
                return false;
            }
            if (idf != "") {
                idfo = IDF::MachineImplementation::loadFromIDF(idf);
                conf.implementationID = 
                    dsdb.addImplementation(*idfo, 0,0);
                conf.hasImplementation = true;
            } else {
                conf.hasImplementation = false;
            }
        } catch (const Exception& e) {
            std::ostringstream msg(std::ostringstream::out);
            msg << "Error loading the adf/idf." << std::endl;
            errorOuput(msg.str());
            return false;
        }
        return true;
    }
};

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(MinimalOpSet)
