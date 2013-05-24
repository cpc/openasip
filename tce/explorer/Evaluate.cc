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
 * @file Evaluate.cc
 *
 * Explorer plugin that evaluates the given configuration.
 *
 * @author Esa Määttä 2008 (esa.maatta-no.spam-tut.fi)
 * @note rating: red
 */

#include <vector>
#include <set>
#include <string>
#include "DesignSpaceExplorerPlugin.hh"
#include "DSDBManager.hh"
#include "Machine.hh"
#include "CostEstimates.hh"
#include "HDBRegistry.hh"
#include "StringTools.hh"
#include "Exception.hh"
#include "Conversion.hh"

#include <iostream>
#include <iomanip>

//using namespace TTAProgram;
using namespace TTAMachine;
using namespace HDB;
using std::endl;
using std::cout;
using std::setw;

/**
 * Explorer plugin that evaluates a configuration and estimates it if the
 * configuration has an implementation.
 */
class Evaluate : public DesignSpaceExplorerPlugin {
    PLUGIN_DESCRIPTION("Evaluates and/or estimates a configuration.");
    
    Evaluate(): DesignSpaceExplorerPlugin(), 
        adf_(""), 
        idf_(""),
        print_(true) {

        // compulsory parameters
        // no compulsory parameters

        // parameters that have a default value
        addParameter(adfPN_, STRING, false, adf_);
        addParameter(idfPN_, STRING, false, idf_);
        addParameter(printPN_, BOOL, false, Conversion::toString(print_));
    }

    virtual bool requiresStartingPointArchitecture() const { return false; }
    virtual bool producesArchitecture() const { return false; }
    virtual bool requiresHDB() const { return false; }
    virtual bool requiresSimulationData() const { return true; }

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
            verboseLog(msg.str());
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
                verboseLog(std::string("Evaluate failed."))
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

        if (print_ && estimate) {
            printEstimates(estimates, conf);
        }

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
    // parameter names
    static const std::string adfPN_;
    static const std::string idfPN_;
    static const std::string printPN_;

    // parameters
    /// name of the adf file to evaluate
    std::string adf_;
    /// name of the idf file to evaluate
    std::string idf_;
    /// print evaluation results
    bool print_;

    /**
     * Reads the parameters given to the plugin.
     */
    void readParameters() {
        // optional parameters
        readOptionalParameter(adfPN_, adf_);
        readOptionalParameter(idfPN_, idf_);
        readOptionalParameter(printPN_, print_);
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
            delete mach;
            return false;
        }
        delete mach;
        return true;
    }


    /**
     * Print estimates
     *
     * @param estimates The cost estimates to be printed.
     */
    void printEstimates(
        const CostEstimates& estimates,
        const DSDBManager::MachineConfiguration& configuration) {
        std::ostream& log = std::cout;
        log.flags(std::ios::left);
        int fw = 27;

        log << setw(fw) << "Area: " << estimates.area() << endl;
        log << setw(fw) << "Longest path delay: " <<
            estimates.longestPathDelay() << endl;
        for (int i = 0; i < estimates.energies(); ++i) {
            log << "application " << i << setw(14) << " energy: " <<
                estimates.energy(i) << endl;
        }

        std::vector<ClockCycleCount> cycleCounts = 
            db().cycleCounts(configuration);

        for (int i = 0; i < (int)cycleCounts.size(); ++i) {
            log << "application " << i << " cycle count: " <<
                cycleCounts.at(i) << endl;
        }
    }
};

// parameters
const std::string Evaluate::adfPN_("adf");
const std::string Evaluate::idfPN_("idf");
const std::string Evaluate::printPN_("print");

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(Evaluate)
