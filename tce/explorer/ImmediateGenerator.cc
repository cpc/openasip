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
 * @file ImmediateGenerator.cc
 *
 * Explorer plugin that creates or modifies machine instruction templates.
 *
 * @author Esa M‰‰tt‰ 2007 (esa.maatta-no.spam-tut.fi)
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
     * TODO: add some modifying functionality.
     *
     * Supported parameters:
     * - print, boolean, print information about machines instruction templates.
     * - remove_it_name, string, remove instruction template with a given name.
     * - add_it_name, string, add empty instruction template with a given name.
     * - modify_it_name, string, modify instruction template with a given name.
     * - width, int, instruction template supported width.
     * - width_part, int, minimum size of width per slot. Default 8.
     * - split, boolean, split immediate among slots.
     * - dst_imm_unit, string, destination immediate unit.
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
                if (split_) {
                    addSplitInsTemplate(*mach, addInsTemplateName_);    
                } else {
                    addInsTemplate(*mach, addInsTemplateName_);
                }
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
    /// minimum width on long instruction slot when splitting the template. 
    unsigned int widthPart_;
    /// make evenly bus/slot wise splitted template.
    bool split_;
    /// destination immediate unit name
    std::string dstImmUnitName_;
     
    /**
     * Reads the parameters given to the plugin.
     */
    void readParameters() {
        const std::string print = "print";
        const std::string removeInsTemplateName = "remove_it_name";
        const std::string addInsTemplateName = "add_it_name";
        const std::string modInsTemplateName = "modify_it_name";
        const std::string width = "width";
        const std::string widthPart = "width_part";
        const std::string split = "split";
        const std::string dstImmUnitName = "dst_imm_unit";

        const unsigned int widthDefault_ = 32;
        const unsigned int widthPartDefault_ = 8;
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

        if (hasParameter(widthPart)) {
            try {
                widthPart_ = Conversion::toUnsignedInt(parameterValue(widthPart));
            } catch (const Exception& e) {
                parameterError(widthPart, "Integer");
                widthPart_ = widthPartDefault_;
            }   
        } else {
            // set defaut value to widthPart
            widthPart_ = widthPartDefault_;
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
        
        if (hasParameter(dstImmUnitName)) {
            try {
                dstImmUnitName_ = parameterValue(dstImmUnitName);
            } catch (const Exception& e) {
                parameterError(dstImmUnitName, "string");
                dstImmUnitName_ = "";
            }   
        } else {
            dstImmUnitName_ = "";
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
            << " value expected." << endl;
        errorOuput(msg.str());
    }

    /**
     * Print info about instruction templates of a given machine.
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
     * Adds instruction template with a name given as parameter and add
     * bus wise splitted slots to the template.
     *
     * @param mach Target machine.
     * @param name Instruction template name to be added.
     */
    void addSplitInsTemplate(TTAMachine::Machine& mach, std::string name) {
        TTAMachine::InstructionTemplate* insTemplate = NULL; 

        // find target immediate unit for the instruction template slots
        Machine::ImmediateUnitNavigator IUNav = 
            mach.immediateUnitNavigator();
        
        TTAMachine::ImmediateUnit* immUnit = NULL;
        TTAMachine::ImmediateUnit* dstImmUnit = NULL;
        if (dstImmUnitName_.empty()) {
            // if no destination immediate unit specified take one if exists
            if (IUNav.count() > 0) {
                dstImmUnit = IUNav.item(0);
            } else {
                std::ostringstream msg(std::ostringstream::out);
                msg << "Error while using ImmediateGenerator:" << endl
                    << "No immediate units." << endl;
                errorOuput(msg.str());
                return;
            }
        } else {
            for (int iu = 0; iu < IUNav.count(); iu++) {
                immUnit = IUNav.item(iu);     
                if (immUnit->name() == dstImmUnitName_) {
                    dstImmUnit = immUnit; 
                } 
            }
            if (dstImmUnit == NULL) {
                std::ostringstream msg(std::ostringstream::out);
                msg << "Error while using ImmediateGenerator:" << endl
                    << "Given destination immediate unit \"" 
                    << dstImmUnitName_ << "\" was not found." << endl;
                errorOuput(msg.str());
                return;
            }
        }
        
        // add instruction template
        try {
            insTemplate = new InstructionTemplate(name, mach);
        } catch (ComponentAlreadyExists& e) {
            std::ostringstream msg(std::ostringstream::out);
            msg << "Error while using ImmediateGenerator:" << endl
                << e.errorMessage() << endl;
            errorOuput(msg.str());
            delete insTemplate;
            insTemplate = NULL;
            return;
        } catch (InvalidName& e) {
            std::ostringstream msg(std::ostringstream::out);
            msg << "Error while using ImmediateGenerator:" << endl
                << e.errorMessage() << endl;
            errorOuput(msg.str());
            delete insTemplate;
            insTemplate = NULL;
            return;
        }

        // TODO: split among immediate slots also?
        Machine::BusNavigator busNav = mach.busNavigator();

        int slotCount = (width_/widthPart_);

        // if too few busses to make even one widthPart_ length template slot
        if (busNav.count() < slotCount) {
            slotCount = busNav.count();
        }

        int overSpill = width_ - (slotCount * widthPart_);
        int widthAdd = 0;
        TTAMachine::Bus* busP = NULL;
        for (int bus = 0; bus < slotCount; bus++) {
            busP = busNav.item(bus);

            if (overSpill > 0) {
                if (overSpill < static_cast<int>(
                            busP->width() - widthPart_)) {
                    widthAdd = overSpill;
                    overSpill = -1; // all spilled
                } else {
                    widthAdd = busP->width() - widthPart_;
                    overSpill = overSpill - widthAdd;
                }
            } else {
                widthAdd = 0;
            }

            try {
                insTemplate->addSlot(busP->name(), widthPart_ + widthAdd, 
                        *dstImmUnit);
            } catch (const Exception& e) {
                std::ostringstream msg(std::ostringstream::out);
                msg << "Error while using ImmediateGenerator:" << endl
                    << e.errorMessage() << endl;
                errorOuput(msg.str());
                delete insTemplate;
                insTemplate = NULL;
                return;
            }
        }

        // check if all spilled
        if (overSpill > 0) {
            std::ostringstream msg(std::ostringstream::out);
            msg << "Error while using ImmediateGenerator:" << endl
                << "Immediate template generation failed, width=\"" << width_
                << "\" too great by: \"" << overSpill << "\"" << endl;
            errorOuput(msg.str());
            delete insTemplate;
            insTemplate = NULL;
            return;
        }

        createNewConfig_ = true;
    }
};

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(ImmediateGenerator)
