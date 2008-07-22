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
