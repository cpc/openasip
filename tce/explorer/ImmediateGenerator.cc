/**
 * @file ImmediateGenerator.cc
 *
 * Explorer plugin that creates or modifies machine instruction templates.
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
#include "TemplateSlot.hh"
#include "Exception.hh"

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
     * Explorer plugin that creates or modifies machine instruction templates.
     *
     * Supported parameters:
     * - print, print information about machines instruction templates.
     *
     * @param startPointConfigurationID Configuration to optimize.
     */
    virtual std::vector<RowID>
    explore(const RowID& startPointConfigurationID, const unsigned int&) {

        // XXX: does this plugin have to touch short immediates.

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
            // loads starting configuration
            DSDBManager::MachineConfiguration conf = 
                dsdb.configuration(startPointConfigurationID);
            
            // load machine from configuration
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

            if (!removeInsTemplateName_.empty()) {
                removeInsTemplate(*mach, removeInsTemplateName_);
            }

            if (!addInsTemplateName_.empty()) {
                addInsTemplate(*mach, addInsTemplateName_);
            }

            // print immediate info if print parameter given
            if (print_) {
                printImmediateTemplates(*mach);
            }

            if (createNewConfig_) {
                // create the new configuration to be saved to dsdb
                // do long immediates affect these
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
            }

        } catch (const Exception& e) {
            std::ostringstream msg(std::ostringstream::out);
            msg << "Error while using ImmediateGenerator:" << endl
                << e.errorMessage() << endl;
            errorOuput(msg.str());
            return result;
        }
        return result;
    }

private:
    /// Boolean value used to decide if new config is created.
    bool createNewConfig_;
    /// print values
    bool print_;
    /// instruction template name to be removed.
    std::string removeInsTemplateName_;
    /// instruction template name to be added.
    std::string addInsTemplateName_;
    /// instruction template name to be modified.
    std::string modInsTemplateName_;
    /// width of the target template
    unsigned int width_;
    /// make evenly bus/slot wise splitted template.
    bool split_;
     
    /// short-immediate length
    //int length_;
    /// short-immediate extension mode
    //Machine::Extension extension_; 

    /**
     * Reads the parameters given to the plugin.
     */
    void readParameters() {
        const std::string print = "print";
        const std::string removeInsTemplateName = "remove_it_name";
        const std::string addInsTemplateName = "add_it_name";
        const std::string modInsTemplateName = "modify_it_name";
        const std::string width = "width";
        const std::string split = "split";

        const unsigned int widthDefault_ = 32;
        createNewConfig_ = false; // by default don't create a new config
        
        if (hasParameter(print)) {
            try {
                print_ = booleanValue(parameterValue(print));
            } catch (const Exception& e) {
                parameterError(print, "Boolean");
                print_ = false;
            }   
        } else {
            print_ = false;
        }   

        if (hasParameter(removeInsTemplateName)) {
            try {
                removeInsTemplateName_ = parameterValue(removeInsTemplateName);
            } catch (const Exception& e) {
                parameterError(removeInsTemplateName, "string");
                removeInsTemplateName_ = "";
            }   
        } else {
            removeInsTemplateName_ = "";
        }   

        if (hasParameter(addInsTemplateName)) {
            try {
                addInsTemplateName_ = parameterValue(addInsTemplateName);
            } catch (const Exception& e) {
                parameterError(addInsTemplateName, "string");
                addInsTemplateName_ = "";
            }   
        } else {
            addInsTemplateName_ = "";
        }   

        if (hasParameter(modInsTemplateName)) {
            try {
                modInsTemplateName_ = parameterValue(modInsTemplateName);
            } catch (const Exception& e) {
                parameterError(modInsTemplateName, "string");
                modInsTemplateName_ = "";
            }   
        } else {
            modInsTemplateName_ = "";
        }   

        if (hasParameter(width)) {
            try {
                width_ = Conversion::toUnsignedInt(parameterValue(width));
            } catch (const Exception& e) {
                parameterError(width, "Integer");
                width_ = widthDefault_;
            }   
        } else {
            // set defaut value to width
            width_ = widthDefault_;
        }   

        if (hasParameter(split)) {
            try {
                split_ = booleanValue(parameterValue(split));
            } catch (const Exception& e) {
                parameterError(split, "Boolean");
                split_ = false;
            }   
        } else {
            split_ = false;
        }   
        
        /*
                length_ = Conversion::toInt(parameterValue(length));
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
        */
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
            << " value expected." << endl;
        errorOuput(msg.str());
    }

    /**
     * Print (long) immediate templates of a given machine.
     *
     * @param mach Machine which instruction templates are to be printed.
     */
    void printImmediateTemplates(TTAMachine::Machine& mach) {
        Machine::InstructionTemplateNavigator ITNav = 
            mach.instructionTemplateNavigator();

        std::ostringstream msg(std::ostringstream::out);
        msg << "====== Instruction templates: =======" << endl;

        TTAMachine::InstructionTemplate* insTemplate = NULL;
        TTAMachine::TemplateSlot* tempSlot = NULL;
        for (int it = 0; it < ITNav.count(); it++) {
            insTemplate = ITNav.item(it);
            
            msg << "Instruction template: " << insTemplate->name() << endl;
            if (insTemplate->isEmpty()) {
                msg << "\tEmpty instruction template." << endl;
                continue;
            }

            msg << "\tSlot count: " << insTemplate->slotCount() << endl;
            for (int sc = 0; sc < insTemplate->slotCount(); sc++) {
                tempSlot = insTemplate->slot(sc);
                msg << "\t\tSlot name: " << tempSlot->slot() << endl;
                msg << "\t\tSlot width: " << tempSlot->width() << endl;
                msg << "\t\tSlot destination: " 
                    << tempSlot->destination()->name() << endl;
            }

            msg << "\tSupported Width: " << insTemplate->supportedWidth() << endl;
            msg << "\tNumber of Destinations: " 
                << insTemplate->numberOfDestinations() << endl;
        }
        errorOuput(msg.str());
    }

    /**
     * Removes instruction template with a name given as parameter.
     *
     * @param mach Target machine.
     * @param name Instruction template name to be removed.
     */
    void removeInsTemplate(TTAMachine::Machine& mach, std::string name) {
        Machine::InstructionTemplateNavigator ITNav = 
            mach.instructionTemplateNavigator();

        TTAMachine::InstructionTemplate* insTemplate = NULL;
        for (int it = 0; it < ITNav.count(); it++) {
            insTemplate = ITNav.item(it);
            if (insTemplate->name() == name) {
                mach.deleteInstructionTemplate(*insTemplate);
                createNewConfig_ = true;
            }
        }

        if (!createNewConfig_) {
            std::ostringstream msg(std::ostringstream::out);
            msg << "Error while using ImmediateGenerator:" << endl
                << "Instruction template with name \"" << name 
                << "\" was not found."  << endl;
            errorOuput(msg.str());
        }
    }

    /**
     * Adds instruction template with a name given as parameter.
     *
     * @param mach Target machine.
     * @param name Instruction template name to be added.
     */
    void addInsTemplate(TTAMachine::Machine& mach, std::string name) {
        TTAMachine::InstructionTemplate* insTemplate = NULL; 

        try {
            insTemplate = new InstructionTemplate(name, mach);
            //mach.addInstructionTemplate(*insTemplate);
        } catch (ComponentAlreadyExists& e) {
            std::ostringstream msg(std::ostringstream::out);
            msg << "Error while using ImmediateGenerator:" << endl
                << e.errorMessage() << endl;
            errorOuput(msg.str());
            delete insTemplate;
            return;
        } catch (InvalidName& e) {
            std::ostringstream msg(std::ostringstream::out);
            msg << "Error while using ImmediateGenerator:" << endl
                << e.errorMessage() << endl;
            errorOuput(msg.str());
            delete insTemplate;
            return;
        }
        createNewConfig_ = true;
    }

    /**
     * XXX: not sane. just a stupid example.
     * Add short-immediates to busses (move slots).
     *
     * @param mach Machine to which busses short immediates are added.
     */
    /*
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
    */
};

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(ImmediateGenerator);
