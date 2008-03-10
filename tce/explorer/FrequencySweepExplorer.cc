/**
 * @file FrequencySweepExplorer.cc
 *
 * Explorer plugin that that uses algorithm that sets one target frequency as
 * requirement and sweeps all frequencies given by user.
 *
 * @author Jari M‰ntyneva 2007 (jari.mantyneva@tut.fi)
 * @author Esa M‰‰tt‰ 2008 (esa.maatta@tut.fi)
 * @note rating: red
 */

#include <vector>
#include <string>
#include <set>
#include <map>
#include <sstream>
#include <boost/format.hpp>

#include "DesignSpaceExplorerPlugin.hh"
#include "DSDBManager.hh"
#include "Machine.hh"
#include "TestApplication.hh"
#include "Program.hh"
#include "Instruction.hh"
#include "Move.hh"
#include "Terminal.hh"
#include "Operation.hh"
#include "StaticProgramAnalyzer.hh"
#include "ComponentImplementationSelector.hh"
#include "HDBRegistry.hh"
#include "ICDecoderEstimatorPlugin.hh"
#include "ControlUnit.hh"
#include "HWOperation.hh"
#include "StringTools.hh"
#include "ADFSerializer.hh"
#include "IDFSerializer.hh"
#include "Segment.hh"
#include "RFPort.hh"
#include "FullyConnectedCheck.hh"
#include "TemplateSlot.hh"
#include "CostEstimates.hh"
#include "Application.hh"
#include "Guard.hh"
#include "Exception.hh"
#include "FrequencySweep.hh"
#include "MachineResourceModifier.hh"
#include "CycleOptimizer.hh"
#include "MachineImplementation.hh"
#include "FUImplementationLocation.hh"


using namespace TTAProgram;
using namespace TTAMachine;
using namespace HDB;
using namespace IDF;
using std::endl;
using std::map;
using std::vector;
using std::string;
using std::set;

/**
 * Explorer plugin for finding processor configurations that
 * satisfy the runtime requirements with certain clock frequencies.
 *
 * Sweeps the user-set allowed frequency range with the given
 * steps and collects all configurations that are suitable for
 * running the given applications in the given clock frequencies.
 *
 * Uses all HDBs found in the HDB registry to create the configurations. 
 *
 * Supported parameters:
 *  - start_freq_mhz, lowest frequency in the swept range in MHz
 *  - end_freq_mhz, highest frequency in the range in MHz
 *  - step_freq_mhz, frequency step in MHz
 *
 *  - ic_dec, name of the ic decoder plugin, default is DefaultICDecoder
 *  - ic_hdb, name of the HDB that is used in IC estimation
 */
class FrequencySweepExplorer : public DesignSpaceExplorerPlugin {
public:
    DESCRIPTION("Frequency sweep algorithm.");
    
    /**
     * Explores from the given start configuration and uses 
     * InitialMachineExplorer with default parameters to create one if start
     * configuration is not given.
     *
     * @param startPointConfigurationID Configuration ID to start the
     * exploration from.
     * @return Returns a set of best found configuration IDs.
     */
    virtual std::vector<RowID>
    explore(const RowID& startPointConfigurationID, const unsigned int&) {

        readParameters();
        openHDBs();
        std::vector<RowID> result;
        
        RowID startPointConfID = startPointConfigurationID;
        if (startPointConfID == 0) {
            DesignSpaceExplorerPlugin* initialMachineExplorer =
                DesignSpaceExplorer::loadExplorerPlugin(
                    "InitialMachineExplorer", db());
            vector<RowID> initialResult = 
                initialMachineExplorer->explore(startPointConfigurationID);
            if (initialResult.size() == 1) {
                startPointConfID = initialResult.at(0);
            } else {
                return result;
            }
        }

        // other plugins used
        DesignSpaceExplorerPlugin* icOptimizer =
            DesignSpaceExplorer::loadExplorerPlugin(
                    "SimpleICOptimizer", db());
        DesignSpaceExplorerPlugin* minimizeMachine =
            DesignSpaceExplorer::loadExplorerPlugin(
                    "MinimizeMachine", db());
        DesignSpaceExplorerPlugin* growMachine =
            DesignSpaceExplorer::loadExplorerPlugin(
                    "GrowMachine", db());

        DSDBManager& dsdb = db();

        // Sweep is always done from the lowest frequency towards the highest
        // frequency.
        if (startMHz_ > endMHz_) {
            int tempMHz = startMHz_;
            startMHz_ = endMHz_;
            endMHz_ = tempMHz;
        }

        // helper for returning the stepped frequencies in order
        FrequencySweep sweeper(startMHz_, endMHz_, stepMHz_);

        // parameters for GrowMachine plugin
        DesignSpaceExplorerPlugin::ParameterTable growMachineParameters;
        DesignSpaceExplorerPlugin::Parameter superiorityPar;
        superiorityPar.name = "superiority";
        superiorityPar.value = "2";
        growMachineParameters.push_back(superiorityPar);
        growMachine->setParameters(growMachineParameters);

        // find new configurations by adding components until the cycle
        // count stops going down
        vector<RowID> cycleOptimizedConfs = 
            growMachine->explore(startPointConfID);
        if (verbose_ > 1) {
            std::ostringstream msg(std::ostringstream::out);
            msg << "GrowMachine plugin produced initial configs: ";
            for (int i = 0; i < cycleOptimizedConfs.size(); ++i) {
                msg << cycleOptimizedConfs.at(i) << " ";
            }
            msg << endl;
            verboseOuput(msg.str());
        }

        int currentFrequencyMHz = sweeper.nextFrequency();
        set<RowID> applicationIDs = dsdb.applicationIDs();
        set<RowID>::const_iterator applicationIter;
        vector<RowID>::const_iterator archIter;
        DesignSpaceExplorerPlugin::ParameterTable minMachineParameters;
        DesignSpaceExplorerPlugin::Parameter frequencyPar;
        frequencyPar.name = "frequency";
        while (currentFrequencyMHz != 0) {

            // for minimizeMachine plugin
            frequencyPar.value = Conversion::toString(currentFrequencyMHz);
            minMachineParameters.clear();
            minMachineParameters.push_back(frequencyPar);

            /* Find the configurations that are fast enough for the
               real time requirements of the applications at the
               currently examined frequency. */
            for (archIter = cycleOptimizedConfs.begin();
                 archIter != cycleOptimizedConfs.end();
                 archIter++) {

                bool isFastEnough = true;
                DSDBManager::MachineConfiguration configuration = 
                    dsdb.configuration(*archIter);
                for (applicationIter = applicationIDs.begin();
                     applicationIter != applicationIDs.end();
                     applicationIter++) {
                                        
                    ClockCycleCount cycleCount = 
                        dsdb.cycleCount(
                            *applicationIter, configuration.architectureID);
                    TestApplication testApplication(
                        dsdb.applicationPath(*applicationIter));

                    // test if application max runtime is set
                    if (testApplication.maxRuntime() < 1) {
                        continue;
                    }   

                    if ((cycleCount / (currentFrequencyMHz * 100000)) >
                        testApplication.maxRuntime()) {
                        // we can skip this architecture since it won't
                        // meet the speed requirements
                        isFastEnough = false;
                        break;
                    }
                }
                // if was fast enough for all apps
                if (isFastEnough) {
                    // calling MimimizeMachine plugin with confToMinimize 
                    // (architer) and currentFrequencyMHz
                    minimizeMachine->setParameters(minMachineParameters);
                    vector<RowID> minimizedConfs = 
                        minimizeMachine->explore(*archIter);

                    DSDBManager::MachineConfiguration minConf;
                    if (minimizedConfs.size() == 1) {
                        minConf = dsdb.configuration(minimizedConfs.at(0));
                    } else {
                        throw InvalidData(
                            __FILE__, __LINE__, __func__,
                            (boost::format(
                                "MinimizeMachine failed to optimize "
                                "configuration %d. Possible bug in Optimizer,"
                                " Estimator or missing data from HDB."
                            ) % *archIter).str());
                    }

                    if (verbose_ > 2) {
                        std::ostringstream msg(std::ostringstream::out);
                        msg << "MinimizeMachine plugin produced config: "
                            << minimizedConfs.at(0) << " (" 
                            << currentFrequencyMHz << " Mhz)" << endl;
                        verboseOuput(msg.str());
                    }

                    // selecting the component implementations
                    RowID selectedConf = 
                        selectComponents(
                            dsdb, minConf, currentFrequencyMHz);
                    
                    // IC optimization with SimpleICOptimizer plugin
                    vector<RowID> icOptimizedResult = 
                        icOptimizer->explore(selectedConf);
                    if (icOptimizedResult.size() == 1) {
                        if (verbose_) {
                            std::ostringstream msg(std::ostringstream::out);
                            msg << "Config " << icOptimizedResult.at(0) 
                                << " created for frequency " 
                                << currentFrequencyMHz << "." << endl;
                            verboseOuput(msg.str());
                        }
                        result.push_back(icOptimizedResult.at(0));
                    } else {
                        // should work always
                        // If doesn't work, there is possibly a bug in
                        // estimator or the ICOptimizer or the HDB is
                        // missing some vital data.
                        throw InvalidData(
                            __FILE__, __LINE__, __func__,
                            (boost::format(
                                "ICOptimizer failed to optimize configuration "
                                "%d. Possible bug in Optimizer, Estimator "
                                "or missing data from HDB.") % selectedConf).
                            str());
                    }

                    /// @todo Optimization of the instruction size
                    /// not required for 1st version!

                    /// @todo Final optimization/tuning
                    /// not required for 1st version!

                } else {
                    // the architecture was too slow
                }

            }
            // advance to next frequency
            currentFrequencyMHz = sweeper.nextFrequency();
        }
        
        // Idea:
        // All results will be given to a result explorer that returns
        // the given number of best configuration id:s that can be returned
        // to the user.
        // BestResultExplorer resultExplorer(dsdb, explorer.results());
        // result = resultExplorer.find(5);

        return result;
    }

private:
    /// Selector used by the plugin.
    ComponentImplementationSelector selector_;
    /// Set of operations used in the applications.
    std::set<std::string> operations_;
    /// Set of integer variables used in the applications.
    std::set<SIntWord> integerVariables_;
    /// Set of float variables used in the applications.
    std::set<int> floatVariables_;
    /// Set of immediate bit widths in the applications.
    std::map<int, int> immediateWidths_;
    /// Number of buses in the machine.
    int busCount_;
    /// Default value of busCount_
    static const int busCountDefault_ = 4;
    /// Program analyzer.
    StaticProgramAnalyzer analyzer_;
    /// Biggest instruction address used in the programs
    InstructionAddress biggestAddress_;

    int startMHz_;
    int endMHz_;
    unsigned int stepMHz_;

    static const unsigned int immSlotBusIndexDefault_ = 0;
    unsigned int immSlotBusIndex_;

    static const int registerFileSizeDefault_ = 4;
    static const int maxNumberOfRegisterFilesDefault_ = 16;
    static const int rfReadPortsDefault_ = 1;
    static const int rfWritePortsDefault_ = 1;
    int registerFileSize_;
    int maxNumberOfRegisterFiles_;
    int rfReadPorts_;
    int rfWritePorts_;
    int verbose_;

    /// name of the ic decoder plugin for idf
    std::string icDec_;
    /// name of the hdb used by ic decoder
    std::string icDecHDB_;

    /**
     * Selects the implementations for the machine configuration.
     *
     * @TODO: throw appropriate exceptions
     *
     * @param configuration MachineConfiguration of which architecture is used.
     * @param frequency The minimum frequency of the implementations.
     * @return RowID of the new machine configuration having adf and idf.
     * @exception Exception No suitable implementations found.
     */
    RowID
    selectComponents(
        DSDBManager& dsdb, const DSDBManager::MachineConfiguration& conf,
        int frequency)
        throw (Exception) {

        HDBRegistry& hdbRegistry = HDBRegistry::instance();
        for (int i = 0; i < hdbRegistry.hdbCount(); i++) {
            selector_.addHDB(hdbRegistry.hdb(i));
        }

        Machine* mach = dsdb.architecture(conf.architectureID);

        Machine::FunctionUnitNavigator fuNav = mach->functionUnitNavigator();
        Machine::RegisterFileNavigator rfNav = mach->registerFileNavigator();
        Machine::ImmediateUnitNavigator iuNav = mach->immediateUnitNavigator();
        IDF::MachineImplementation* idf = new IDF::MachineImplementation;
        
        // select implementations for funtion units
        for (int i = 0; i < fuNav.count(); i++) {
            FunctionUnit* fu = fuNav.item(i);

            map<const FUImplementationLocation*, CostEstimates*> fuMap =
                selector_.fuImplementations(*fu, frequency);
            map<const FUImplementationLocation*,
                CostEstimates*>::const_iterator iter = fuMap.begin();
            
            if (fuMap.size() != 0) {
                double longestPathDelay = 0;
                map<const IDF::FUImplementationLocation*,
                    CostEstimates*>::const_iterator wanted = iter;
                while (iter != fuMap.end()) {
                    if (iter->second == NULL) {
                        throw Exception(
                            __FILE__, __LINE__, __func__,
                            "No cost estimates found for FU: " 
                            + fu->name());
                    }
                    if (longestPathDelay < (*iter).second->longestPathDelay()) {
                        longestPathDelay = (*iter).second->longestPathDelay();
                        wanted = iter;
                    }
                    iter++;
                }
                const IDF::FUImplementationLocation* fuImpl = (*wanted).first;
                ObjectState* state = fuImpl->saveState();
                IDF::FUImplementationLocation* newFUImpl = 
                    new IDF::FUImplementationLocation(state);

                try {
                    idf->addFUImplementation(newFUImpl);
                } catch (Exception e) {
                    std::ostringstream msg(std::ostringstream::out);
                    msg << e.errorMessage() 
                        << " " << e.fileName() 
                        << " " << e.lineNum() << endl;
                    errorOuput(msg.str());
                }
            } else {
                throw Exception(
                    __FILE__, __LINE__, __func__,
                    "no implementations found for FU: " + fu->name());
            }
        }

        // select implementations for register files
        for (int i = 0; i < rfNav.count(); i++) {
            RegisterFile* rf = rfNav.item(i);
            map<const RFImplementationLocation*, CostEstimates*> rfMap;

            // check if the register is boolean register.
            if (rf->isUsedAsGuard()) {
                // select from guarded registers
                rfMap = selector_.rfImplementations(*rf, true, frequency);
            } else {
                // select from non guarded registers
                rfMap = selector_.rfImplementations(*rf, false, frequency);
            }
            map<const IDF::RFImplementationLocation*,
                CostEstimates*>::const_iterator iter = rfMap.begin();
            if (rfMap.size() != 0) {
                double longestPathDelay = 0;
                map<const IDF::RFImplementationLocation*,
                    CostEstimates*>::const_iterator wanted = iter;
                while (iter != rfMap.end()) {
                    if (iter->second == NULL) {
                        throw Exception(
                            __FILE__, __LINE__, __func__,
                            "No cost estimates found for RF: " 
                            + rf->name());
                    }
                    if (longestPathDelay < (*iter).second->longestPathDelay()) {
                        longestPathDelay = (*iter).second->longestPathDelay();
                        wanted = iter;
                    }
                    iter++;
                }
                const IDF::RFImplementationLocation* rfImpl = (*wanted).first;
                ObjectState* state = rfImpl->saveState();
                IDF::RFImplementationLocation* newRFImpl = 
                    new IDF::RFImplementationLocation(state);
                try {
                    idf->addRFImplementation(newRFImpl);
                } catch (Exception e) {
                    std::ostringstream msg(std::ostringstream::out);
                    msg << e.errorMessage() 
                        << " " << e.fileName() 
                        << " " << e.lineNum() << endl;
                    errorOuput(msg.str());
                }
            } else {
                throw Exception(
                    __FILE__, __LINE__, __func__,
                    "no implementations found for RF: " + rf->name());
            }
        }

        // select implementations for immediate units
        for (int index = 0; index < iuNav.count(); index++) {
            TTAMachine::ImmediateUnit* iu = iuNav.item(index);
            map<const IDF::IUImplementationLocation*, CostEstimates*> iuMap =
                selector_.iuImplementations(*iu, frequency);
            map<const IDF::IUImplementationLocation*,
                CostEstimates*>::const_iterator iter = iuMap.begin();
            if (iuMap.size() != 0) {
                double longestPathDelay = 0;
                map<const IDF::RFImplementationLocation*,
                    CostEstimates*>::const_iterator wanted = iter;
                while (iter != iuMap.end()) {
                    if (iter->second == NULL) {
                        throw Exception(
                            __FILE__, __LINE__, __func__,
                            "No cost estimates found for IU: " 
                            + iu->name());
                    }
                    if (longestPathDelay < (*iter).second->longestPathDelay()) {
                        longestPathDelay = (*iter).second->longestPathDelay();
                        wanted = iter;
                    }
                    iter++;
                }
                const IDF::IUImplementationLocation* iuImpl = (*wanted).first;
                ObjectState* state = iuImpl->saveState();
                IDF::IUImplementationLocation* newIUImpl = 
                    new IDF::IUImplementationLocation(state);
                try {
                    idf->addIUImplementation(newIUImpl);
                } catch (Exception e) {
                    std::ostringstream msg(std::ostringstream::out);
                    msg << e.errorMessage() 
                         << " " << e.fileName() 
                         << " " << e.lineNum() << endl;
                    errorOuput(msg.str());
                }
            } else {
                throw Exception(
                    __FILE__, __LINE__, __func__,
                    "no implementations found for IU: " + iu->name());
            }
        }

        // add the ic decoder plugin
        std::vector<std::string> icDecPaths =
            Environment::icDecoderPluginPaths();
        idf->setICDecoderPluginName(icDec_);
        idf->setICDecoderHDB(icDecHDB_);
        std::vector<std::string>::const_iterator iter = icDecPaths.begin();
        for (; iter != icDecPaths.end(); iter++) {
            std::string path = *iter;
            std::string file = 
                path + FileSystem::DIRECTORY_SEPARATOR + icDec_ + "Plugin.so";
            if (FileSystem::fileExists(file)) {
                idf->setICDecoderPluginFile(file);
                break;
            }
        }
        
        delete mach;
        mach = 0;
        
        DSDBManager::MachineConfiguration newConfiguration;
        newConfiguration.architectureID = conf.architectureID;
        newConfiguration.implementationID = dsdb.addImplementation(*idf, 0, 0);
        newConfiguration.hasImplementation = true;
        return dsdb.addConfiguration(newConfiguration);
    }



    /**
     * Reads the parameters given to the plugin.
     */
    void readParameters() {
        const std::string startMHz = "start_freq_mhz";
        const std::string endMHz = "end_freq_mhz";
        const std::string stepMHz = "step_freq_mhz";
        const std::string icDec = "ic_dec";
        const std::string icDecoderDefault = "DefaultICDecoder";
        const std::string icDecHDB = "ic_hdb";
        const std::string icDecHDBDefault = "asic_130nm_1.5V.hdb";
        const std::string verbose = "verbose";
        const int verboseDefault = 0;

        if (hasParameter(startMHz)) {
            try {
                startMHz_ = Conversion::toInt(parameterValue(startMHz));
            } catch (const Exception& e) {
                parameterError(startMHz, "integer");
                string msg = "'start_freq_mhz' parameter needed";
                throw IllegalParameters(__FILE__, __LINE__, __func__, msg);
            }
        } else {
            string msg = "'start_freq_mhz' parameter needed";
                throw IllegalParameters(__FILE__, __LINE__, __func__, msg);
        }

        if (hasParameter(endMHz)) {
            try {
                endMHz_ = Conversion::toInt(parameterValue(endMHz));
            } catch (const Exception& e) {
                parameterError(endMHz, "integer");
                string msg = "'end_freq_mhz' parameter needed";
                throw IllegalParameters(__FILE__, __LINE__, __func__, msg);
            }
        } else {
            string msg = "'end_freq_mhz' parameter needed";
                throw IllegalParameters(__FILE__, __LINE__, __func__, msg);
        }
        
        if (hasParameter(stepMHz)) {
            try {
                stepMHz_ = Conversion::toInt(parameterValue(stepMHz));
            } catch (const Exception& e) {
                parameterError(stepMHz, "integer");
                string msg = "'step_freq_mhz' parameter needed";
                throw IllegalParameters(__FILE__, __LINE__, __func__, msg);
            }
        } else {
            string msg = "'step_freq_mhz' parameter needed";
            throw IllegalParameters(__FILE__, __LINE__, __func__, msg);
        }

        if (hasParameter(icDec)) {
            try {
                icDec_ = parameterValue(icDec);
            } catch (const Exception& e) {
                parameterError(icDec, "String");
                icDec_ = icDecoderDefault;
            }
        } else {
            // set defaut value to icDec
            icDec_ = icDecoderDefault;
        }

        if (hasParameter(icDecHDB)) {
            try {
                icDecHDB_ = parameterValue(icDecHDB);
            } catch (const Exception& e) {
                parameterError(icDecHDB, "String");
                icDecHDB_ = icDecHDBDefault;
            }
        } else {
            // set defaut value to icDecHDB
            icDecHDB_ = icDecHDBDefault;
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
     * Loads HDBs that are used into the registry.
     */
    void openHDBs() {
        HDBRegistry& hdbRegistry = HDBRegistry::instance();

        // if HDBRegistry contains no HDBManagers load from default paths
        if (hdbRegistry.hdbCount() == 0) {
            hdbRegistry.loadFromSearchPaths();
        }
    }

#if 0 
    // remove if not needed because of minimal.adf (28.12.07)
    /**
     * Analyzes the operation need and counts variables used in the
     * application.
     *
     * @todo is this needed? At least it's not called anywhere. 
     */
    void analyze() {
        try {
            DSDBManager& dsdb = db();
            std::set<RowID> applications = dsdb.applicationIDs();
            for (std::set<RowID>::const_iterator id = applications.begin();
                 id != applications.end(); id++) {
                
                TestApplication application(dsdb.applicationPath(*id));
                
                const UniversalMachine* umach = new UniversalMachine();
                TTAProgram::Program* program = 
                    TTAProgram::Program::loadFromTPEF(
                        application.applicationPath(), *umach);
                
                analyzer_.addProgram(*program);
                delete umach;
                umach = NULL;
                delete program;
                program = NULL;
            }
            operations_ = analyzer_.operationsUsed();
            integerVariables_ = analyzer_.integerRegisterIndexes();
            immediateWidths_ = analyzer_.immediateBitWidths();
            biggestAddress_ = analyzer_.biggestAddress();
        } catch (const Exception& e) {
            debugLog(std::string("Analyze failed in FrequencySweepExplorer. ")
                     + e.errorMessage() + std::string(" ") + e.fileName()
                     + std::string(" ") + Conversion::toString(e.lineNum()));
        }

        // needed memory
        InstructionAddress stackMem = 16384; // 2^14
        biggestAddress_ += stackMem;

        // round up to next power of two
        InstructionAddress x = 1;
        while (x < biggestAddress_ && x != 0) {
            x <<= 1;
        }
        // if x equals to zero make the highest possible address as biggest
        if (x == 0) {
            x--;
        }
        biggestAddress_ = x;
    }


    /**
     * Check if the fu has such operations that needs an address space.
     *
     * @todo This implementation is not robust. It should use the OSAL 
     * operand setting "memory address", that is, if there is at least one
     * operation of which operand is a memory address, we know the FU
     * needs an address space. The name matching is not enough.
     *
     * @return True if the fu needs an address space.
     */
    bool needsAddressSpace(const FunctionUnit* fu) const {

        for (int i = 0; i < fu->operationCount(); i++) {
            HWOperation* operation = fu->operation(i);
            std::string opName = StringTools::stringToUpper(operation->name());
            if (opName == "LDW" || opName == "LDQ" || opName == "LDH" ||
                opName == "LDBU" || opName == "LDHU" || opName == "STW" ||
                opName == "STQ" || opName == "STH") {
                return true;
            }
        }
        return false;
    }
#endif    
};

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(FrequencySweepExplorer);
