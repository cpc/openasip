/*
    Copyright (c) 2002-2011 Tampere University.

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
 * @file ComponentAdder.cc
 *
 * Explorer plugin that adds machine components to a given machine.
 *
 * @author Esa MÃ¤Ã¤ttÃ¤ 2008 (esa.maatta-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2011
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
#include "Conversion.hh"

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
    PLUGIN_DESCRIPTION("Explorer plugin that adds machine components to a "
            "given machine.");

    ComponentAdder(): DesignSpaceExplorerPlugin(), 
        RFName_("rf"),
        RFCount_(1),
        RFSize_(4),
        RFReadPorts_(1),
        RFWritePorts_(1),
        adf_(""),
        buildIdf_(false) {

        // compulsory parameters
        // no compulsory parameters

        // parameters that have a default value
        addParameter(RFNamePN_, STRING, false, RFName_);
        addParameter(RFCountPN_, UINT, false, Conversion::toString(RFCount_));
        addParameter(RFSizePN_, UINT, false, Conversion::toString(RFSize_));
        addParameter(RFReadPortsPN_, UINT, false, Conversion::toString(RFReadPorts_));
        addParameter(RFWritePortsPN_, UINT, false, Conversion::toString(RFWritePorts_));
        addParameter(adfPN_, STRING, false, adf_);
        addParameter(buildIdfPN_, BOOL, false, Conversion::toString(buildIdf_));
    }

    virtual bool requiresStartingPointArchitecture() const { return false; }
    virtual bool producesArchitecture() const { return true; }
    virtual bool requiresHDB() const { return false; }
    virtual bool requiresSimulationData() const { return true; }
    virtual bool requiresApplication() const { return false; }
    
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
            verboseLog(msg.str());
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
            verboseLog(msg.str());
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
                verboseLog(msg.str());
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
    
    static const std::string RFNamePN_;
    static const std::string RFCountPN_;
    static const std::string RFSizePN_;
    static const std::string RFReadPortsPN_;
    static const std::string RFWritePortsPN_;
    static const std::string adfPN_;
    static const std::string buildIdfPN_;
    
    // register file variables
    std::string RFName_;
    int RFCount_;
    int RFSize_;
    int RFReadPorts_;
    int RFWritePorts_;
    /// name of the adf file if wanted to use idf generation
    std::string adf_;
    /// do we build idf
    bool buildIdf_;

    /**
     * Reads the parameters given to the plugin.
     */
    void readParameters() {
        readOptionalParameter(RFNamePN_, RFName_);
        readOptionalParameter(RFCountPN_, RFCount_);
        readOptionalParameter(RFSizePN_, RFSize_);
        readOptionalParameter(RFReadPortsPN_, RFReadPorts_);
        readOptionalParameter(RFWritePortsPN_, RFWritePorts_);
        readOptionalParameter(adfPN_, adf_);
        readOptionalParameter(buildIdfPN_, buildIdf_);
    }

    
    /**
     * Builds the machine in basis of the analyzed data from the applications.
     *
     * @return The initial machine of NULL if an error occurred.
     */
    void addComponents(TTAMachine::Machine* mach) {
        // add register files
        addRegisterFiles(mach);
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
            std::string RFName = RFName_ + Conversion::toString(i);

            const TTAMachine::Machine::RegisterFileNavigator& RFNav = 
                mach->registerFileNavigator();
            if (RFNav.hasItem(RFName)) {
                RFName = RFName_ + Conversion::toString(i+RFNav.count());
            }

            TTAMachine::RegisterFile* rf = new TTAMachine::RegisterFile(
                    RFName, RFSize_, 32, RFReadPorts_, RFWritePorts_, 0,
                    TTAMachine::RegisterFile::NORMAL);
            for (int n = 0; n < RFReadPorts_; n++) {
                new TTAMachine::RFPort("read" + 
                        Conversion::toString( + 1), *rf);
            }
            for (int n = 0; n < RFWritePorts_; n++) {
                new TTAMachine::RFPort("write" + 
                        Conversion::toString(n + 1), *rf);
            }
            try {
                mach->addRegisterFile(*rf);
            } catch (const ComponentAlreadyExists& e) {
                verboseLog("ComponentAdder: Tried to add RF with a already"
                    "existing name (" + RFName)
                Application::exitProgram(1);
            }
        }
    }
};

// parameters
const std::string ComponentAdder::RFNamePN_("rf_name");
const std::string ComponentAdder::RFCountPN_("rf_count");
const std::string ComponentAdder::RFSizePN_("rf_size");
const std::string ComponentAdder::RFReadPortsPN_("rf_reads");
const std::string ComponentAdder::RFWritePortsPN_("rf_writes");
const std::string ComponentAdder::adfPN_("adf");
const std::string ComponentAdder::buildIdfPN_("build_idf");

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(ComponentAdder)
