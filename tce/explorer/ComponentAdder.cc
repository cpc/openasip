/**
 * @file ComponentAdder.cc
 *
 * Explorer plugin that adds machine components to a given machine.
 *
 * @author Esa Määttä 2008 (esa.maatta@tut.fi)
 * @note rating: red
 */

#include <vector>
#include <string>
#include "DesignSpaceExplorerPlugin.hh"
#include "DSDBManager.hh"
#include "Machine.hh"
#include "HDBRegistry.hh"
#include "StringTools.hh"
#include "RFPort.hh"
#include "ComponentImplementationSelector.hh"
#include "Exception.hh"

//using namespace TTAProgram;
using namespace TTAMachine;
using namespace HDB;
using std::endl;

/**
 * Explorer plugin that adds machine components to a given machine.
 *
 * Supported parameters:
 *  - rf_size, number of registers in one register file, default is 4
 *  - max_rfs, maximum number of register files in the machine, default is 16
 *  - rf_reads, number of register read ports in register files, default is 1
 *  - rf_writes, number of register write ports in register file, default is 1
 *  - build_idf, if parameter is set the idf file is built, not set as default
 *  - adf, if idf is wanted to generated to some arhitecture, no default value.
 *    If adf parameter is given the idf is built.
 */
class ComponentAdder : public DesignSpaceExplorerPlugin {
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
        assert(mach != NULL);

        // add components
        addComponents(mach);

        if (buildIdf_) {
            try {
                // add idf to configuration
                selector_.selectComponentsToConf(conf, dsdb, mach);
            } catch (const Exception& e) {
                std::ostringstream msg(std::ostringstream::out);
                msg << e.errorMessage() 
                    << " " << e.fileName() 
                    << " " << e.lineNum() << std::endl;
                errorOuput(msg.str());
            }
        } else {
            conf.hasImplementation = false;
        }

        // add machine to configuration
        conf.architectureID = dsdb.addArchitecture(*mach);

        // add new configuration to dsdb
        RowID confID = dsdb.addConfiguration(conf);
        result.push_back(confID);
        return result;
    }

private:
    /// Selector used by the plugin.
    ComponentImplementationSelector selector_;
    
    // register file variables
    std::string RFName_;
    int RFCount_;
    int RFSize_;
    int RFReadPorts_;
    int RFWritePorts_;
    bool addRF_;

    int verbose_;

    /// name of the adf file if wanted to use idf generation
    std::string adf_;
    /// do we build idf
    bool buildIdf_;

    /**
     * Reads the parameters given to the plugin.
     */
    void readParameters() {
        const std::string RFName = "rf_name";
        const std::string RFCount = "rf_count";
        const std::string RFSize = "rf_size";
        const std::string RFReadPorts = "rf_reads";
        const std::string RFWritePorts = "rf_writes";
        addRF_ = false;
        
        const std::string RFNameDefault = "rf";
        const int RFCountDefault = 1;
        const int RFSizeDefault = 4;
        const int RFReadPortsDefault = 1;
        const int RFWritePortsDefault = 1;
        
        const std::string adf = "adf";

        const std::string buildIdf = "build_idf";
        const bool buildIdfDefault = false;

        const std::string verbose = "verbose";
        const int verboseDefault = 0;

        if (hasParameter(RFName)) {
            try {
                RFName_ = parameterValue(RFName);
            } catch (const Exception& e) {
                parameterError(RFName, "String");
                RFName_ = RFNameDefault;
            }
            addRF_ = true;
        } else {
            // set defaut value to RFName
            RFName_ = RFNameDefault;
        }

        if (hasParameter(RFCount)) {
            try {
                RFCount_ =
                    Conversion::toUnsignedInt(parameterValue(RFCount));
            } catch (const Exception& e) {
                parameterError(RFCount, "Integer");
                RFSize_ = RFCountDefault;
            }
            addRF_ = true;
        } else {
            // set defaut value to RFCount
            RFCount_ = RFCountDefault;
        }

        if (hasParameter(RFSize)) {
            try {
                RFSize_ =
                    Conversion::toUnsignedInt(parameterValue(RFSize));
            } catch (const Exception& e) {
                parameterError(RFSize, "Integer");
                RFSize_ = RFSizeDefault;
            }
            addRF_ = true;
        } else {
            // set defaut value to RFSize
            RFSize_ = RFSizeDefault;
        }

        if (hasParameter(RFReadPorts)) {
            try {
                RFReadPorts_ =
                    Conversion::toUnsignedInt(parameterValue(RFReadPorts));
            } catch (const Exception& e) {
                parameterError(RFReadPorts, "Integer");
                RFReadPorts_ = RFReadPortsDefault;
            }
            addRF_ = true;
        } else {
            // set defaut value to RFReadPorts
            RFReadPorts_ = RFReadPortsDefault;
        }

        if (hasParameter(RFWritePorts)) {
            try {
                RFWritePorts_ =
                    Conversion::toUnsignedInt(parameterValue(RFWritePorts));
            } catch (const Exception& e) {
                parameterError(RFWritePorts, "Integer");
                RFWritePorts_ = RFWritePortsDefault;
            }
            addRF_ = true;
        } else {
            // set defaut value to RFWritePorts
            RFWritePorts_ = RFWritePortsDefault;
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

        if (hasParameter(buildIdf)) {
            try {
                buildIdf_ = booleanValue(parameterValue(buildIdf));
            } catch (const Exception& e) {
                parameterError(buildIdf, "Boolean");
                buildIdf_ = buildIdfDefault;
            }
        } else {
            buildIdf_ = buildIdfDefault;
        }
        
        // parameter for printing info about what is done
        if (hasParameter(verbose)) {
            try {
                verbose_ = Conversion::toInt(parameterValue(verbose));
            } catch (const Exception& e) {
                parameterError(verbose, "integer");
                verbose_ = verboseDefault;
            }
        } else {
            // set defaut value to verbose
            verbose_ = verboseDefault;
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
     * Builds the machine in basis of the analyzed data from the applications.
     *
     * @return The initial machine of NULL if an error occurred.
     */
    void addComponents(TTAMachine::Machine* mach) {
        // add register files
        if (addRF_) {
            addRegisterFiles(mach);
        }
        if (verbose_ && !addRF_) {
            std::ostringstream msg(std::ostringstream::out);
            msg << "Warning: No components were added." << endl;
            verboseOuput(msg.str());
        }
    }


    /**
     * Adds register file(s) to the machine
     *
     * TODO: add type and guard latency setting
     *
     * @return void 
     */
    void addRegisterFiles(TTAMachine::Machine* mach) {
        for (int i = 0; i < RFCount_; i++) {
            TTAMachine::RegisterFile* rf = new TTAMachine::RegisterFile(
                    RFName_ + Conversion::toString(i), RFSize_,
                    32, RFReadPorts_, RFWritePorts_, 0,
                    TTAMachine::RegisterFile::NORMAL);
            for (int n = 0; n < RFReadPorts_; n++) {
                TTAMachine::RFPort* port = new TTAMachine::RFPort(
                        "read" + Conversion::toString( + 1), *rf);
            }
            for (int n = 0; n < RFWritePorts_; n++) {
                TTAMachine::RFPort* port = new TTAMachine::RFPort(
                        "write" + Conversion::toString(n + 1), *rf);
            }
            mach->addRegisterFile(*rf);
        }
    }
};

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(ComponentAdder);
