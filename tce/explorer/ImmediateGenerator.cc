/**
 * @file ImmediateGenerator.cc
 *
 * Explorer plugin that creates or modifies machine instruction template by
 * adding/removing immediates.
 *
 * @author Esa M‰‰tt‰ 2007 (esa.maatta@tut.fi)
 * @note rating: red
 */

#include <vector>
#include <string>
#include <set>

#include "DesignSpaceExplorerPlugin.hh"
#include "DSDBManager.hh"
#include "Machine.hh"
#include "TestApplication.hh"
#include "Program.hh"
#include "Instruction.hh"
#include "Move.hh"
#include "Terminal.hh"
#include "HDBRegistry.hh"
#include "ICDecoderEstimatorPlugin.hh"
#include "ControlUnit.hh"
#include "HWOperation.hh"
#include "StringTools.hh"
#include "Segment.hh"
#include "CostEstimates.hh"
#include "Application.hh"
#include "Exception.hh"
#include "SchedulerFrontend.hh"
#include "Procedure.hh"
#include "MachineResourceModifier.hh"


using namespace TTAProgram;
using namespace TTAMachine;
using namespace HDB;
using std::endl;

/**
 * Explorer plugin that creates or modifies machine instruction template by
 * adding/removing immediates.
 */
class ImmediateGenerator : public DesignSpaceExplorerPlugin {
public:
    DESCRIPTION("Creates immediates for configuration.");
    
    /**
     * Explorer plugin that creates or modifies machine instruction template by
     * adding/removing immediates.
     *
     * Supported parameters:
     * - length, length of the short immediate, default: 32
     * - extension, short-immediate extension, values sign/zero, default: sign
     *
     * @param startPointConfigurationID Configuration to optimize.
     */
    virtual std::vector<RowID>
    explore(const RowID& startPointConfigurationID, const unsigned int&) {

        std::vector<RowID> result;

        if (startPointConfigurationID == 0) {
            std::ostringstream msg(std::ostringstream::out);
            msg << "No configuration nor adf defined. Use -s <confID> to "
                << "define the configuration to be optimized." << endl;
            errorOuput(msg.str());
            return result;
        }

        readParameters();
        
        try {
            DSDBManager& dsdb = db();
            DSDBManager::MachineConfiguration conf = 
                dsdb.configuration(startPointConfigurationID);
            
            // loads starting configuration
            //Machine* origMach = NULL;
            Machine* mach = NULL;
            try {
                //origMach = dsdb.architecture(conf.architectureID);
                mach = dsdb.architecture(conf.architectureID);
            } catch (const Exception& e) {
                std::ostringstream msg(std::ostringstream::out);
                msg << e.errorMessage() << endl;
                errorOuput(msg.str());
                return result;
            }

            addShortImmediates(*mach);
            
            // DEBUG: dosn't need tpef (program) ??

            // create the new configuration to be saved to dsdb
            // do short immediates affect these
            DSDBManager::MachineConfiguration newConf;
            if (conf.hasImplementation) {
                newConf.hasImplementation = true;
                IDF::MachineImplementation* idf = 
                    dsdb.implementation(conf.implementationID);
                CostEstimator::Estimator estimator;
                CostEstimator::AreaInGates area = 
                    estimator.totalArea(*mach, *idf);
                CostEstimator::DelayInNanoSeconds longestPathDelay =
                    estimator.longestPath(*mach, *idf);                
                newConf.implementationID = 
                    dsdb.addImplementation(*idf, longestPathDelay, area);
            } else {
                newConf.hasImplementation = false;
            }
            newConf.architectureID = dsdb.addArchitecture(*mach);
            CostEstimates estimates;

            // DEBUG: no evaluation needed

            RowID confID = dsdb.addConfiguration(newConf);
            result.push_back(confID);

        } catch (const Exception& e) {
            std::ostringstream msg(std::ostringstream::out);
            msg << "Error while using SimpleICOptimizer:" << endl
                << e.errorMessage() << endl;
            errorOuput(msg.str());
            return result;
        }
        return result;
    }

private:
    /// short-immediate length
    int length_;
    /// short-immediate extension mode
    Machine::Extension extension_; 

    /**
     * Reads the parameters given to the plugin.
     */
    void readParameters() {
        // Parameter name of bus number in the machine.
        const std::string length = "length";
        const int lengthDefault = 32;
        const std::string extension = "extension";
        const Machine::Extension extensionDefault = Machine::SIGN;

        if (hasParameter(length)) {
            try {
                length_ = Conversion::toInt(parameterValue(length));
            } catch (const Exception& e) {
                parameterError(length, "integer");
                length_ = lengthDefault;
            }   
        } else {
            length_ = lengthDefault;
        }   

        if (hasParameter(extension)) {
            try {
                std::string eTemp;
                eTemp = parameterValue(extension);
                if (eTemp == "sign") {
                    extension_ = Machine::SIGN;
                }
                if (eTemp == "zero") {
                    extension_ = Machine::ZERO;
                }
            } catch (const Exception& e) {
                parameterError(extension, "String");
                extension_ = extensionDefault;
            }
        } else {
            // set defaut value to extension mode
            extension_ = extensionDefault;
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
     * XXX: not sane. just an stupid example.
     * Add short-immediates to busses (move slots).
     *
     * @param mach Machine to which busses short immediates are added.
     */
    void addShortImmediates(TTAMachine::Machine& mach) {
        Machine::BusNavigator busNav = mach.busNavigator();

        // round up
        int lengthPart = (static_cast<float>(length_) / busNav.count()) + 0.5;

        TTAMachine::Bus* busP = NULL;
        for (int bus = 0; bus < busNav.count(); bus++) {
            busP = busNav.item(bus);
            busP->setImmediateWidth(lengthPart);
            busP->setExtensionMode(extension_);
        }
    }
};

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(ImmediateGenerator);
