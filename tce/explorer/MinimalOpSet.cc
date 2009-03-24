/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
    PLUGIN_DESCRIPTION("Minimal opset checker and fixer plugin.");

    MinimalOpSet(): DesignSpaceExplorerPlugin(), 
        adf_(""),
        idf_(""), 
        printMissingOps_(false) {

        // compulsory parameters
        // no compulsory parameters

        // parameters that have a default value
        addParameter(adfPN_, STRING, false, adf_);
        addParameter(idfPN_, STRING, false, idf_);
        addParameter(printMissingOpsPN_, BOOL, false, 
            Conversion::toString(printMissingOps_));
    }

    virtual bool producesArchitecture() const { return false; }

    virtual bool requiresHDB() const { return false; }

    virtual bool requiresSimulationData() const { return false; }
    
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
            verboseLog(msg.str());
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
            verboseLog(msg.str());
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
                verboseLog(missingOps.at(i) + " : operation is missing.");
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
    // parameter name variables
    static const std::string adfPN_;
    static const std::string idfPN_;
    static const std::string printMissingOpsPN_;
     
    // parameters
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
        readCompulsoryParameter(adfPN_, adf_);
        readCompulsoryParameter(idfPN_, idf_);
        readCompulsoryParameter(printMissingOpsPN_, printMissingOps_);
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
            verboseLog(msg.str());
            return false;
        }
        return true;
    }
};

// parameter names
const std::string MinimalOpSet::adfPN_("adf");
const std::string MinimalOpSet::idfPN_("idf");
const std::string MinimalOpSet::printMissingOpsPN_("print");

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(MinimalOpSet)
