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
 * @file ImmediateGenerator.cc
 *
 * Explorer plugin that creates or modifies machine instruction templates.
 *
 * @author Esa M��tt� 2007 (esa.maatta-no.spam-tut.fi)
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
#include "Procedure.hh"
#include "MachineResourceModifier.hh"
#include "Conversion.hh"

using namespace TTAProgram;
using namespace TTAMachine;
using namespace HDB;
using std::endl;

/**
 * Explorer plugin that creates or modifies machine instruction template by
 * adding/removing immediates.
 */
class ImmediateGenerator : public DesignSpaceExplorerPlugin {
    PLUGIN_DESCRIPTION("Creates immediates for configuration.");

    ImmediateGenerator(): DesignSpaceExplorerPlugin(), 
        createNewConfig_(false),
        print_(false), 
        removeInsTemplateName_(""),
        addInsTemplateName_(""),
        modInsTemplateName_(""),
        width_(32),
        widthPart_(8),
        split_(false),
        dstImmUnitName_("") {

        // compulsory parameters
        // no compulsory parameters

        // parameters that have a default value
        addParameter(printPN_, BOOL, false, Conversion::toString(print_));
        addParameter(removeInsTemplateNamePN_, STRING, false, removeInsTemplateName_);
        addParameter(addInsTemplateNamePN_, STRING, false, addInsTemplateName_);
        addParameter(modInsTemplateNamePN_, STRING, false, modInsTemplateName_);
        addParameter(widthPN_, UINT, false, Conversion::toString(width_));
        addParameter(widthPartPN_, UINT, false, Conversion::toString(widthPart_));
        addParameter(splitPN_, BOOL, false, Conversion::toString(split_));
        addParameter(dstImmUnitNamePN_, STRING, false, dstImmUnitName_);
    }

    virtual bool requiresStartingPointArchitecture() const { return true; }
    virtual bool producesArchitecture() const { return true; }
    virtual bool requiresHDB() const { return false; }
    virtual bool requiresSimulationData() const { return false; }
    virtual bool requiresApplication() const { return false; }

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
                verboseLog(msg.str());
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
            verboseLog(msg.str());
            return result;
        }
        return result;
    }

private:
    /// Boolean value used to decide if new config is created.
    bool createNewConfig_;

    // parameter names
    static const std::string printPN_;
    static const std::string removeInsTemplateNamePN_;
    static const std::string addInsTemplateNamePN_;
    static const std::string modInsTemplateNamePN_;
    static const std::string widthPN_;
    static const std::string widthPartPN_;
    static const std::string splitPN_;
    static const std::string dstImmUnitNamePN_;

    // parameters
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
        readCompulsoryParameter(printPN_, print_);
        readCompulsoryParameter(removeInsTemplateNamePN_, removeInsTemplateName_);
        readCompulsoryParameter(addInsTemplateNamePN_, addInsTemplateName_);
        readCompulsoryParameter(modInsTemplateNamePN_, modInsTemplateName_);
        readCompulsoryParameter(widthPN_, width_);
        readCompulsoryParameter(widthPartPN_, widthPart_);
        readCompulsoryParameter(splitPN_, split_);
        readCompulsoryParameter(dstImmUnitNamePN_, dstImmUnitName_);
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
        verboseLog(msg.str());
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
            verboseLog(msg.str());
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
            verboseLog(msg.str());
            delete insTemplate;
            return;
        } catch (InvalidName& e) {
            std::ostringstream msg(std::ostringstream::out);
            msg << "Error while using ImmediateGenerator:" << endl
                << e.errorMessage() << endl;
            verboseLog(msg.str());
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
                verboseLog(msg.str());
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
                verboseLog(msg.str());
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
            verboseLog(msg.str());
            delete insTemplate;
            insTemplate = NULL;
            return;
        } catch (InvalidName& e) {
            std::ostringstream msg(std::ostringstream::out);
            msg << "Error while using ImmediateGenerator:" << endl
                << e.errorMessage() << endl;
            verboseLog(msg.str());
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
                verboseLog(msg.str());
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
            verboseLog(msg.str());
            delete insTemplate;
            insTemplate = NULL;
            return;
        }

        createNewConfig_ = true;
    }
};

// parameter names
const std::string ImmediateGenerator::printPN_("print");
const std::string ImmediateGenerator::removeInsTemplateNamePN_("remove_it_name");
const std::string ImmediateGenerator::addInsTemplateNamePN_("add_it_name");
const std::string ImmediateGenerator::modInsTemplateNamePN_("modify_it_name");
const std::string ImmediateGenerator::widthPN_("width");
const std::string ImmediateGenerator::widthPartPN_("width_part");
const std::string ImmediateGenerator::splitPN_("split");
const std::string ImmediateGenerator::dstImmUnitNamePN_("dst_imm_unit");

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(ImmediateGenerator)
