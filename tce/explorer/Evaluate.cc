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

        // if configuration doesn't have a implementation, warn

        // check if adf given, TODO: check idf also
        if (configurationID == 0) {
            std::ostringstream msg(std::ostringstream::out);
            msg << "No configuration nor adf defined. Use -s <confID> to "
                << "define the configuration to be optimized or give adf "
                << "as plugin parameter." << endl;
            errorOuput(msg.str());
            return result;
        }

        DSDBManager& dsdb = db();
        // loads starting configuration
        DSDBManager::MachineConfiguration conf = 
            dsdb.configuration(configurationID);

        DesignSpaceExplorer explorer;
        explorer.setDSDB(dsdb);
        CostEstimates estimates;
        bool estimate = true;
        try {
            if (!explorer.evaluate(conf, estimates, estimate)) {
                debugLog(std::string("Evaluate failed."));
                result.push_back(configurationID);
                return result;
            }
        } catch (const Exception& e) {
            debugLog(std::string("Error in Evaluate plugin: ")
                    + e.errorMessage() + std::string(" ")
                    + e.errorMessageStack());
            result.push_back(configurationID);
            return result;
        }

        return result;
    }

private:
    /**
     * Reads the parameters given to the plugin.
     */
    void readParameters() {
        return;
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
};

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(Evaluate)
