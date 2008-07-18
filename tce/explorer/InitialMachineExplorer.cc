/**
 * @file InitialMachineExplorer.cc
 *
 * Explorer plugin that generates initial machine according the programs
 * intented to run with the processor. The machine is capable to run all the
 * programs but not optimized in any way.
 *
 * @author Jari M‰ntyneva 2007 (jari.mantyneva@tut.fi)
 * @author Esa M‰‰tt‰ 2008 (esa.maatta@tut.fi)
 * @note rating: red
 */

#include <vector>
#include <string>
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

using namespace TTAProgram;
using namespace TTAMachine;
using namespace HDB;
using std::cerr;
using std::endl;
using std::map;

/**
 * Explorer plugin that generatesthe initial machine used with automatic
 * design space exploration. Uses all HDBs found in the HDB registry to
 * create the components. If none HDBs in the registry tries to load HDBs
 * from the default search paths.
 *
 * Supported parameters:
 *  - bus_count, number of buses used in the machine, default is 4.
 *  - imm_32, number of 32 bit immediates, default is 0
 *  - imm_16, number of 16 bit immediates, default is 0
 *  - imm_8, number of 8 bit immediates, default is 2
 *  - imm_slot_bus_index, index of the bus for template slot, default is 0
 *  - iu_size, number of registers in immediate units, default is 2
 *  - iu_num, number of immediate units, default is 1
 *  - iu_width, width of immediate units, default is 32
 *  - rf_size, number of registers in one register file, default is 4
 *  - max_rfs, maximum number of register files in the machine, default is 16
 *  - rf_reads, number of register read ports in register files, default is 1
 *  - rf_writes, number of register write ports in register file, default is 1
 *  - mau, size of minimum addressable unit, default is 8
 *  - build_idf, if parameter is set the idf file is built, not set as default
 *  - ic_dec, name of the ic decoder plugin, default is DefaultICDecoder
 *  - ic_hdb, name of the HDB that is used in IC estimation,
 *    default is asic_130nm_1.5V.hdb
 *  - adf, if idf is wanted to generated to some arhitecture, no default value.
 *    If adf parameter is given the idf is built.
 */
class InitialMachineExplorer : public DesignSpaceExplorerPlugin {
public:
    DESCRIPTION("Generates intial machine capable of running all given "
                "applications");
    
    /**
     * Explores the initial machine and returns the DSDB configuration ID of
     * the configuration contains the initial machine.
     */
    virtual std::vector<RowID>
    explore(const RowID&, const unsigned int&) {
        readParameters();
        setupSelector();
        std::vector<RowID> result;

        if (adf_ != "") {
            try {
                DSDBManager& dsdb = db();
                DSDBManager::MachineConfiguration conf;
                conf.hasImplementation = false;

            
                TTAMachine::Machine* mach = 
                    TTAMachine::Machine::loadFromADF(adf_);

                conf.architectureID = dsdb.addArchitecture(*mach);
                IDF::MachineImplementation* idf = buildIDF(mach);
                conf.implementationID = dsdb.addImplementation(*idf, 0, 0);
                conf.hasImplementation = true;
            
                RowID confID = dsdb.addConfiguration(conf);
                result.push_back(confID);

            } catch (const Exception& e) {
                std::cerr << "Error loading the adf" << std::endl;
                return result;
            }
            return result;
        }

        analyze();

        TTAMachine::Machine* mach = buildMachine();
        if (mach == NULL) {
            return result;
        }

        DSDBManager& dsdb = db();
        DSDBManager::MachineConfiguration conf;
        conf.hasImplementation = false;
        conf.architectureID = dsdb.addArchitecture(*mach);
        if (buildIdf_) {
            IDF::MachineImplementation* idf = buildIDF(mach);
            conf.implementationID = dsdb.addImplementation(*idf, 0, 0);
            
            conf.hasImplementation = true;
        }
        RowID confID = dsdb.addConfiguration(conf);

        result.push_back(confID);
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
    /// Set of immediate bit witdhs in the applications.
    std::map<int, int> immediateWidths_;
    /// Number of buses in the machine.
    int busCount_;
    /// Default value of busCount_
    static const int busCountDefault_ = 4;
    /// Program analyzer.
    StaticProgramAnalyzer analyzer_;
    /// Biggest instruction address used in the programs
    InstructionAddress biggestAddress_;

    static const int imm32BitDefault_ = 0;
    static const int imm16BitDefault_ = 0;
    static const int imm8BitDefault_ = 2;
    int imm32Bit_;
    int imm16Bit_;
    int imm8Bit_;

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

    static const int defaultPortWidth_ = 32;

    static const int mauSizeDefault_ = 8;
    int mauSize_;

    /// number of registers in immediate units, default is 2
    static const int iuSizeDefault_ = 2;
    int iuSize_;
    /// number of immediate units, default is 1
    static const int iuNumberDefault_ = 1;
    int iuNumber_;
    /// width of immediate units, default is 32
    static const int iuWidthDefault_ = 32;
    int iuWidth_;

    /// name of the ic decoder plugin for idf
    std::string icDec_;
    /// name of the hdb used by ic decoder
    std::string icDecHDB_;
    /// name of the adf file if wanted to use idf generation
    std::string adf_;
    /// do we build idf
    bool buildIdf_;

    /**
     * Reads the parameters given to the plugin.
     */
    void readParameters() {
        // Parameter name of bus number in the machine.
        const std::string busCountName = "bus_count";
        const std::string imm32BitName = "imm_32";
        const std::string imm16BitName = "imm_16";
        const std::string imm8BitName = "imm_8";
        const std::string immSlotBusIndex = "imm_slot_bus_index";
        const std::string registerFileSize = "rf_size";
        const std::string maxNumberOfRegisterFiles = "max_rfs";
        const std::string rfReadPorts = "rf_reads";
        const std::string rfWritePorts = "rf_writes";
        const std::string iuSize = "iu_size";
        const std::string iuNumber = "iu_num";
        const std::string iuWidth = "iu_width";
        const std::string mauSize = "mau";
        const std::string icDec = "ic_dec";
        const std::string icDecoderDefault = "DefaultICDecoder";
        const std::string icDecHDB = "ic_hdb";
        const std::string icDecHDBDefault = "asic_130nm_1.5V.hdb";
        const std::string adf = "adf";
        const std::string buildIdf = "build_idf";

        if (hasParameter(busCountName)) {
            try {
                busCount_ = Conversion::toInt(parameterValue(busCountName));
            } catch (const Exception& e) {
                parameterError(busCountName, "Integer");
                busCount_ = busCountDefault_;
            }
        } else {
            // set defaut value to busCount
            busCount_ = busCountDefault_;
        }
        if (hasParameter(imm32BitName)) {
            try {
                imm32Bit_ = Conversion::toInt(parameterValue(imm32BitName));
            } catch (const Exception& e) {
                parameterError(imm32BitName, "Integer");
                imm32Bit_ = imm32BitDefault_;
            }
        } else {
            // set defaut value to imm32Bit
            imm32Bit_ = imm32BitDefault_;
        }
        if (hasParameter(imm16BitName)) {
            try {
                imm16Bit_ = Conversion::toInt(parameterValue(imm16BitName));
            } catch (const Exception& e) {
                parameterError(imm16BitName, "Integer");
                imm16Bit_ = imm16BitDefault_;
            }
        } else {
            // set defaut value to imm16Bit
            imm16Bit_ = imm16BitDefault_;
        }
        if (hasParameter(imm8BitName)) {
            try {
                imm8Bit_ = Conversion::toInt(parameterValue(imm8BitName));
            } catch (const Exception& e) {
                parameterError(imm8BitName, "Integer");
                imm8Bit_ = imm8BitDefault_;
            }
        } else {
            // set defaut value to imm8Bit
            imm8Bit_ = imm8BitDefault_;
        }
        if (hasParameter(immSlotBusIndex)) {
            try {
                immSlotBusIndex_ =
                    Conversion::toUnsignedInt(parameterValue(immSlotBusIndex));
            } catch (const Exception& e) {
                parameterError(immSlotBusIndex, "UnsignedInteger");
                immSlotBusIndex_ = immSlotBusIndexDefault_;
            }
        } else {
            // set defaut value to immSlotBusIndex
            immSlotBusIndex_ = immSlotBusIndexDefault_;
        }
        if (hasParameter(registerFileSize)) {
            try {
                registerFileSize_ =
                    Conversion::toUnsignedInt(parameterValue(registerFileSize));
            } catch (const Exception& e) {
                parameterError(registerFileSize, "Integer");
                registerFileSize_ = registerFileSizeDefault_;
            }
        } else {
            // set defaut value to registerFileSize
            registerFileSize_ = registerFileSizeDefault_;
        }
        if (hasParameter(maxNumberOfRegisterFiles)) {
            try {
                maxNumberOfRegisterFiles_ =
                    Conversion::toUnsignedInt(
                        parameterValue(maxNumberOfRegisterFiles));
            } catch (const Exception& e) {
                parameterError(maxNumberOfRegisterFiles, "Integer");
                maxNumberOfRegisterFiles_ = maxNumberOfRegisterFilesDefault_;
            }
        } else {
            // set defaut value to maxNumberOfRegisterFiles
            maxNumberOfRegisterFiles_ = maxNumberOfRegisterFilesDefault_;
        }
        if (hasParameter(rfReadPorts)) {
            try {
                rfReadPorts_ =
                    Conversion::toUnsignedInt(parameterValue(rfReadPorts));
            } catch (const Exception& e) {
                parameterError(rfReadPorts, "Integer");
                rfReadPorts_ = rfReadPortsDefault_;
            }
        } else {
            // set defaut value to rfReadPorts
            rfReadPorts_ = rfReadPortsDefault_;
        }
        if (hasParameter(rfWritePorts)) {
            try {
                rfWritePorts_ =
                    Conversion::toUnsignedInt(parameterValue(rfWritePorts));
            } catch (const Exception& e) {
                parameterError(rfWritePorts, "Integer");
                rfWritePorts_ = rfWritePortsDefault_;
            }
        } else {
            // set defaut value to rfWritePorts
            rfWritePorts_ = rfWritePortsDefault_;
        }
        if (hasParameter(iuSize)) {
            try {
                iuSize_ =
                    Conversion::toUnsignedInt(parameterValue(iuSize));
            } catch (const Exception& e) {
                parameterError(iuSize, "Integer");
                iuSize_ = iuSizeDefault_;
            }
        } else {
            // set defaut value to iuSize
            iuSize_ = iuSizeDefault_;
        }
        if (hasParameter(iuNumber)) {
            try {
                iuNumber_ =
                    Conversion::toUnsignedInt(parameterValue(iuNumber));
            } catch (const Exception& e) {
                parameterError(iuNumber, "Integer");
                iuNumber_ = iuNumberDefault_;
            }
        } else {
            // set defaut value to iuNumber
            iuNumber_ = iuNumberDefault_;
        }
        if (hasParameter(iuWidth)) {
            try {
                iuWidth_ =
                    Conversion::toUnsignedInt(parameterValue(iuWidth));
            } catch (const Exception& e) {
                parameterError(iuWidth, "Integer");
                iuWidth_ = iuWidthDefault_;
            }
        } else {
            // set defaut value to iuWidth
            iuWidth_ = iuWidthDefault_;
        }
        if (hasParameter(mauSize)) {
            try {
                mauSize_ =
                    Conversion::toUnsignedInt(parameterValue(mauSize));
            } catch (const Exception& e) {
                parameterError(mauSize, "Unsigned integer");
                mauSize_ = mauSizeDefault_;
            }
        } else {
            // set defaut value to mauSize
            mauSize_ = mauSizeDefault_;
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
                buildIdf_ = false;
            }
        } else {
            buildIdf_ = false;
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
     * Analyzes the operation need and counts variables used in the
     * application.
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
            debugLog(std::string("Analyze failed in InitialMachineExplorer. ")
                     + e.errorMessage() + std::string(" ")
                     + e.errorMessageStack());
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
     * Builds the machine in basis of the analyzed data from the applications.
     *
     * @return The initial machine of NULL if an error occurred.
     */
    TTAMachine::Machine*
    buildMachine() {
        try {
            TTAMachine::Machine* mach = new TTAMachine::Machine();

            // add address spaces
            TTAMachine::AddressSpace* addressSpace = 
                new TTAMachine::AddressSpace(
                    "AS1", mauSize_, 0, biggestAddress_, *mach);
            TTAMachine::AddressSpace* addressSpace2 = 
                new TTAMachine::AddressSpace(
                    "AS2", mauSize_, 0, biggestAddress_, *mach);

            // generate the control unit
            CostEstimator::ICDecoderEstimatorPlugin plugin("");
            TTAMachine::ControlUnit* controlUnit = 
                plugin.generateControlUnit();
            
            // remove control unit's operations from the needed operations set
            for (int i = 0; i < controlUnit->operationCount(); i++) {
                operations_.erase(
                    StringTools::stringToLower(
                        controlUnit->operation(i)->name()));
            }

            // find out needed function units
            std::list<TTAMachine::FunctionUnit*> fuSet = 
                selector_.fuArchsByOpSetWithMinLatency(
                    operations_, defaultPortWidth_);

            // add function units into the machine
            std::list<TTAMachine::FunctionUnit*>::iterator fuIter =
                fuSet.begin();
            if (fuIter == fuSet.end()) {
                std::string message =
                    "No suitable funtion units found to satisfy "
                    "all needed operations: ";
                std::set<std::string>::const_iterator opIter =
                    operations_.begin();
                while (opIter != operations_.end()) {
                    message += (*opIter);
                    opIter++;
                    if (opIter != operations_.end()) {
                        message += ", ";
                    }
                }
                std::cerr << message << std::endl;
                delete mach;
                mach = NULL;
                return mach;
            }

            for (int i = 1; fuIter != fuSet.end(); fuIter++) {
                (*fuIter)->setName("fu" + Conversion::toString(i));
                mach->addFunctionUnit(*(*fuIter));
                if (needsAddressSpace(*fuIter)) {
                    (*fuIter)->setAddressSpace(addressSpace2);
                }
                i++;
            }

            // add registers
            int registersAdded = 0;
            for (int i = 0; i < integerVariables_.size(); i++) {
                TTAMachine::RegisterFile* rf = new TTAMachine::RegisterFile(
                    "rf" + Conversion::toString(i), registerFileSize_,
                    32, rfReadPorts_, rfWritePorts_, 0,
                    TTAMachine::RegisterFile::NORMAL);
                for (int n = 0; n < rfReadPorts_; n++) {
                    TTAMachine::RFPort* port = new TTAMachine::RFPort(
                        "write" + Conversion::toString(n + 1), *rf);
                }
                for (int n = 0; n < rfWritePorts_; n++) {
                    TTAMachine::RFPort* port = new TTAMachine::RFPort(
                        "read" + Conversion::toString(n + 1), *rf);
                }
                mach->addRegisterFile(*rf);
                registersAdded += registerFileSize_;
                if (registersAdded >= integerVariables_.size() ||
                    registersAdded >= maxNumberOfRegisterFiles_) {
                    break;
                }
            }
            // add boolean register
            TTAMachine::RegisterFile* booleanRF = new TTAMachine::RegisterFile(
                "boolean", 2, 1, 1, 1, 0, TTAMachine::RegisterFile::NORMAL);
            TTAMachine::RFPort* booleanWritePort = new TTAMachine::RFPort(
                "write1", *booleanRF);
            TTAMachine::RFPort* booleanReadPort = new TTAMachine::RFPort(
                "read1", *booleanRF);
            mach->addRegisterFile(*booleanRF);

            // generate buses
            int needed32BitImm = imm32Bit_;
            int needed16BitImm = imm16Bit_;
            int needed8BitImm = imm8Bit_;
            for (int i = 0; i < busCount_; i++) {
                TTAMachine::Bus* bus = NULL;
                if (needed32BitImm) {
                    bus = new TTAMachine::Bus(
                        "bus" + Conversion::toString(i), 32, 32,
                        TTAMachine::Machine::ZERO);
                    TTAMachine::Segment* segment = new TTAMachine::Segment(
                        "segment", *bus);
                    needed32BitImm--;
                } else if (needed16BitImm) {
                    bus = new TTAMachine::Bus(
                        "bus" + Conversion::toString(i), 32, 16,
                        TTAMachine::Machine::ZERO);
                    TTAMachine::Segment* segment = new TTAMachine::Segment(
                        "segment", *bus);
                    needed16BitImm--;
                } else if (needed8BitImm) {
                    bus = new TTAMachine::Bus(
                        "bus" + Conversion::toString(i), 32, 8,
                        TTAMachine::Machine::ZERO);
                    TTAMachine::Segment* segment = new TTAMachine::Segment(
                        "segment", *bus);
                    needed8BitImm--;
                } else {
                    bus = new TTAMachine::Bus(
                        "bus" + Conversion::toString(i), 32, 0,
                        TTAMachine::Machine::ZERO);
                    TTAMachine::Segment* segment = new TTAMachine::Segment(
                        "segment", *bus);
                }
                // add always true guard to the bus
                UnconditionalGuard* allwaysTrueGuard = 
                    new UnconditionalGuard(false, *bus);
                mach->addBus(*bus);
            }

            // add guards
            TTAMachine::Machine::BusNavigator busNav = mach->busNavigator();
            for (int i = 0; i < busNav.count(); i++) {
                Bus* bus = busNav.item(i);
                RegisterGuard* guard0 = 
                    new RegisterGuard(false, *booleanRF, 0, *bus);
                RegisterGuard* invGuard0 = 
                    new RegisterGuard(true, *booleanRF, 0, *bus);
                RegisterGuard* guard1 = 
                    new RegisterGuard(false, *booleanRF, 1, *bus);
                RegisterGuard* invGuard1 = 
                    new RegisterGuard(true, *booleanRF, 1, *bus);

            }

            // add immediate units if needed
            std::map<int, int>::const_iterator imm = immediateWidths_.end();
            bool createIU = false;
            while (imm != immediateWidths_.begin()) {
                imm--;
                if (imm32Bit_ != 0) {
                    if ((*imm).first > 32) {
                        createIU = true;
                        break;
                    }
                } else if (imm16Bit_ != 0) {
                    if ((*imm).first > 16) {
                        createIU = true;
                        break;
                    }
                } else if (imm8Bit_ != 0) {
                    if ((*imm).first > 8) {
                        createIU = true;
                        break;
                    }
                } else {
                    createIU = true;
                    break;
                }
            }
            if (createIU) {
                for (int i = 0; i < iuNumber_; i++) {
                    TTAMachine::ImmediateUnit* iu =
                        new TTAMachine::ImmediateUnit(
                            "imm" + Conversion::toString(i), iuSize_, iuWidth_,
                            1, 0, TTAMachine::Machine::ZERO);
                    TTAMachine::RFPort* iuPort = new TTAMachine::RFPort(
                        "read1", *iu);
                    mach->addImmediateUnit(*iu);
                    busNav = mach->busNavigator();
                    std::string busName =
                    "bus" + Conversion::toString(immSlotBusIndex_ + i);
                    if (busNav.hasItem(busName)) {
                        TTAMachine::InstructionTemplate* iTemplate =
                            new TTAMachine::InstructionTemplate(
                                "template", *mach);
                        iTemplate->addSlot(busName, iuWidth_, *iu);
                    } else {
                        std::cerr << "More buses needed for instruction "
                                  << "template slots" << std::endl;
                        delete mach;
                        mach = NULL;
                        return mach;
                    }
                }
            }

            // add control unit into the machine
            mach->setGlobalControl(*controlUnit);

            // set address space to the control unit
            controlUnit->setAddressSpace(addressSpace);

            // Fully connect the machine.
            FullyConnectedCheck check;
            check.fix(*mach);
            
            return mach;

        } catch (const Exception& e) {
            cerr << e.errorMessage() << " " << e.fileName() << " " 
                 << e.lineNum() << endl;
        }
        return NULL;
    }

    /**
     * Creates machine implementation for the machine components.
     *
     * @param mach The Machine.
     * @return The created IDF.
     */
    IDF::MachineImplementation* buildIDF(TTAMachine::Machine* mach) {

        IDF::MachineImplementation* idf = new IDF::MachineImplementation;

        // generate the fu part of idf
        TTAMachine::Machine::FunctionUnitNavigator fuNav =
            mach->functionUnitNavigator();
        for (int index = 0; index < fuNav.count(); index++) {
            TTAMachine::FunctionUnit* fu = fuNav.item(index);
            map<const IDF::FUImplementationLocation*, CostEstimates*> fuMap =
                selector_.fuImplementations(*fu);
            map<const IDF::FUImplementationLocation*,
                CostEstimates*>::const_iterator iter = fuMap.begin();

            if (fuMap.size() != 0) {
                const IDF::FUImplementationLocation* fuImpl = (*iter).first;
                ObjectState* state = fuImpl->saveState();
                IDF::FUImplementationLocation* newFUImpl = 
                    new IDF::FUImplementationLocation(state);
                try {
                    idf->addFUImplementation(newFUImpl);
                } catch (Exception e) {
                    cerr << e.errorMessage() 
                         << " " << e.fileName() 
                         << " " << e.lineNum() << endl;
                }
            } else {
                cerr << "No implementations found for FU: " << fu->name()
                     << endl;
            }
        }
        
        // rf part
        // selects the register that has biggest longest path delay
        TTAMachine::Machine::RegisterFileNavigator rfNav =
            mach->registerFileNavigator();
        for (int index = 0; index < rfNav.count(); index++) {
            TTAMachine::RegisterFile* rf = rfNav.item(index);
            map<const IDF::RFImplementationLocation*, CostEstimates*> rfMap;
            if (rf->isUsedAsGuard()) {
                // select from guarded registers
                rfMap = selector_.rfImplementations(*rf, true);
            } else {
                // select from non guarded registers
                rfMap = selector_.rfImplementations(*rf, false);
            }
            map<const IDF::RFImplementationLocation*,
                CostEstimates*>::const_iterator iter = rfMap.begin();
            if (rfMap.size() != 0) {
                double longestPathDelay = 0;
                map<const IDF::RFImplementationLocation*,
                    CostEstimates*>::const_iterator wanted = iter;

                while (iter != rfMap.end()) {
                    CostEstimates* estimate = (*iter).second;
                    if (estimate != NULL && 
                        longestPathDelay < estimate->longestPathDelay()) {
                        longestPathDelay = estimate->longestPathDelay();
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
                    cerr << e.errorMessage() 
                         << " " << e.fileName() 
                         << " " << e.lineNum() << endl;
                }
            } else {
                cerr << "No implementations found for register file: " 
                     << rf->name() << endl;
            }
        }

        // iu part
        TTAMachine::Machine::ImmediateUnitNavigator iuNav =
            mach->immediateUnitNavigator();
        for (int index = 0; index < iuNav.count(); index++) {
            TTAMachine::ImmediateUnit* iu = iuNav.item(index);
            map<const IDF::IUImplementationLocation*, CostEstimates*> iuMap =
                selector_.iuImplementations(*iu);
            map<const IDF::IUImplementationLocation*,
                CostEstimates*>::const_iterator iter = iuMap.begin();
            if (iuMap.size() != 0) {
                double longestPathDelay = 0;
                map<const IDF::RFImplementationLocation*,
                    CostEstimates*>::const_iterator wanted = iter;
                while (iter != iuMap.end()) {
                    CostEstimates* estimate = (*iter).second;

                    if (estimate != NULL && 
                        longestPathDelay < estimate->longestPathDelay()) {
                        longestPathDelay = estimate->longestPathDelay();
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
                    cerr << e.errorMessage() 
                         << " " << e.fileName() 
                         << " " << e.lineNum() << endl;
                }
            } else {
                cerr << "No implementations found for immediate unit: " 
                     << iu->name() << endl;
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
        
        return idf;
    }


    /**
     * Check if the fu has such operations that needs an address space.
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

    /**
     * Sets up the component implementation selector by adding the HDBs.
     */
    void setupSelector() {
        HDBRegistry& hdbRegistry = HDBRegistry::instance();
        // if HDBRegistry contains no HDBManagers load from default paths
        if (hdbRegistry.hdbCount() == 0) {
            hdbRegistry.loadFromSearchPaths();
        }
        // give all HDBs from registry to the selector to select from
        for (int i = 0; i < hdbRegistry.hdbCount(); i++) {
            HDBManager* hdb = &hdbRegistry.hdb(i);
            selector_.addHDB(*hdb);
        }
    }
};

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(InitialMachineExplorer);
