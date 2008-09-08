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
 * @file Evaluate.cc
 *
 * Explorer plugin that evaluates given configuration.
 *
 * @author Esa Määttä 2008 (esa.maatta@tut.fi)
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

//using namespace TTAProgram;
using namespace TTAMachine;
using namespace HDB;
using std::endl;

/**
 *
 * Supported parameters:
 * adf
 * idf
 * build_idf
 * estimate
 */
class Evaluate : public DesignSpaceExplorerPlugin {
public:
    DESCRIPTION("Generates intial machine capable of running all given "
                "applications");
    
    /**
     * Explorer plugin that adds machine components to a given machine with
     * adf parameter or with configuration id in dsdb.
     */
    virtual std::vector<RowID>
    explore(const RowID& configurationID, const unsigned int&) {
        readParameters();
        std::vector<RowID> result;

        // make params for adf and idf, so no configuration needed
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
        
        // load adf/idf from file if no configuration was given
        if (configurationID == 0) {
            if (!createConfig(adf_, idf_, dsdb, conf)) {
               return result; 
            }
        } else {
            // if starting configuration given load it
            conf = dsdb.configuration(configurationID);
        }

        CostEstimates estimates;
        bool estimate = (conf.hasImplementation ? true : false);
        try {
            if (!evaluate(conf, estimates, estimate)) {
                debugLog(std::string("Evaluate failed."));
                return result;
            }
        } catch (const Exception& e) {
            debugLog(std::string("Error in Evaluate plugin: ")
                    + e.errorMessage() + std::string(" ")
                    + e.errorMessageStack());
            return result;
        }

        verboseLogC(std::string("Evalution OK, ") 
                + (estimate ? "with" : "without") + " estimation.",1)
        // add new configuration to the database
        if (configurationID == 0) {
            RowID newConfID = addConfToDSDB(conf);
            if (newConfID != 0) {
                result.push_back(newConfID);
            }
        }
        return result;
    }

private:
    /// name of the adf file to evaluate
    std::string adf_;
    /// name of the idf file to evaluate
    std::string idf_;

    /**
     * Reads the parameters given to the plugin.
     */
    void readParameters() {
        const std::string adf = "adf";
        const std::string idf = "idf";

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

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(Evaluate)
