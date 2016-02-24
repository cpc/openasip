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
 * @file InfoCommand.cc
 *
 * Implementation of InfoCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @author Viljami Korhonen 2007 (viljami.korhonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <iomanip>
#include <set>
#include <string>
#include <sstream>

#include "CompilerWarnings.hh"
IGNORE_CLANG_WARNING("-Wkeyword-macro")
#include <boost/regex.hpp>
POP_CLANG_DIAGS
#include "boost/tuple/tuple.hpp"

#include "InfoCommand.hh"
#include "Application.hh"
#include "FileSystem.hh"
#include "SimulatorFrontend.hh"
#include "SimulatorInterpreterContext.hh"
#include "SimControlLanguageCommand.hh"
#include "Exception.hh"
#include "SimulatorToolbox.hh"
#include "SimulatorTextGenerator.hh"
#include "SimValue.hh"
#include "Program.hh"
#include "Procedure.hh"
#include "Address.hh"
#include "Instruction.hh"
#include "Move.hh"
#include "Terminal.hh"
#include "MapTools.hh"
#include "Conversion.hh"
#include "SimulatorConstants.hh"
#include "StateData.hh"
#include "StringTools.hh"
#include "StopPointManager.hh"
#include "UniversalMachine.hh"
#include "UnboundedRegisterFile.hh"
#include "MachineState.hh"
#include "RegisterFileState.hh"
#include "Machine.hh"
#include "FunctionUnit.hh"
#include "DisassemblyFUPort.hh"
#include "Bus.hh"
#include "Segment.hh"
#include "BusState.hh"
#include "ControlUnit.hh"
#include "UtilizationStats.hh"
#include "SimulationStatisticsCalculator.hh"
#include "HWOperation.hh"
#include "UniversalFunctionUnit.hh"
#include "RFAccessTracker.hh"
#include "LongImmediateUnitState.hh"
#include "LongImmediateRegisterState.hh"

using std::string;

/**
 * Implementation of "info registers".
 */
class InfoRegistersCommand : public SimControlLanguageSubCommand {
public:

    /**
     * Constructor.
     */
    InfoRegistersCommand(SimControlLanguageCommand& parentCommand) : 
        SimControlLanguageSubCommand(parentCommand) {
    }

    /**
     * Destructor.
     */
    virtual ~InfoRegistersCommand() {
    }

    /**
     * Executes the "info registers" command.
     *
     * "info registers regfile regname" prints the value of register regname 
     * in register file regfile, where regfile is the name of a register file 
     * of the target processor, and regname is the name of a register that 
     * belongs to the specified register file. If regname is omitted,
     * values of all registers of the specified register file are displayed. 
     *
     * @param arguments Arguments to the command, including "info registers".
     * @return true in case execution was successful.
     */
    virtual bool execute(const std::vector<DataObject>& arguments) {

        if (!parent().checkProgramLoaded()) {
            return false;
        }

        const int argumentCount = arguments.size() - 2;
        if (!parent().checkArgumentCount(argumentCount, 1, 2)) {
            return false;
        }
        
        std::string registerFile = "";
        std::string registerName = "";
        std::string registerString = "";
        int registerIndex = -1;

        if (argumentCount == 1) {
            // 'ra' is treated as a special case
            if (StringTools::ciEqual(registerString, "ra")) {
                StateData& data = 
                    parent().simulatorFrontend().state(registerString);

                parent().interpreter()->setResult(
                    registerDescription(registerString, data.value()));
                return true;      
            } 
            
            registerFile = arguments[2].stringValue();
                
            try {
                parent().interpreter()->setResult(
                    parent().simulatorFrontend().registerFileValue(registerFile));
            } catch (const InstanceNotFound&) {
                parent().interpreter()->setError(
                    SimulatorToolbox::textGenerator().text(
                        Texts::TXT_REGISTER_NOT_FOUND).str());
                return false;
            }
            return true;

        } else if (argumentCount == 2) {
            // prints out the register in the given register file
            if (!parent().checkPositiveIntegerArgument(arguments[3])) {
                return false;
            }
            registerFile = arguments[2].stringValue();
            registerIndex = arguments[3].integerValue();
            registerName = 
                registerFile + "." + Conversion::toString(registerIndex);
        } else {
            abortWithError("Illegal count of arguments.");
        }

        // print a single register value
        try {
            parent().interpreter()->setResult(
                parent().simulatorFrontend().registerFileValue(
                    registerFile, registerIndex));

            return true;              
        } catch (const InstanceNotFound&) {
            parent().interpreter()->setError(
                SimulatorToolbox::textGenerator().text(
                    Texts::TXT_REGISTER_NOT_FOUND).str());
            return false;
        } 
        return true;
    }

    /**
     * Helper function to get register description in correct format.
     *
     * @param regName The name of the register to print.
     * @param value The current value of the register.
     */
    static std::string registerDescription(
        const std::string& regName, 
        const SimValue& value) {
        return regName + " " + 
            Conversion::toHexString(value.unsignedValue()) + " " + 
            Conversion::toString(value.intValue());
    }
};

/**
 * Implementation of "info immediates".
 */
class InfoImmediatesCommand : public SimControlLanguageSubCommand {
public:

    /**
     * Constructor.
     */
    InfoImmediatesCommand(SimControlLanguageCommand& parentCommand) : 
        SimControlLanguageSubCommand(parentCommand) {
    }

    /**
     * Destructor.
     */
    virtual ~InfoImmediatesCommand() {
    }

    /**
     * Executes the "info immediates" command.
     *
     * "info immediates iunit regname" prints the value of register regname 
     * in immediate unit iunit, where regfile is the name of a register file 
     * of the target processor, and regname is the name of a register that 
     * belongs to the specified register file. If regname is omitted, values 
     * of all registers of the specified immediate unit are displayed. 
     *
     * @param arguments Arguments to the command, including "info immediates".
     * @return true in case execution was successful.
     */
    virtual bool execute(const std::vector<DataObject>& arguments) {

        if (!parent().checkProgramLoaded()) {
            return false;
        }

        const int argumentCount = arguments.size() - 2;
       
        if (!parent().checkArgumentCount(argumentCount, 1, 2)) {
            return false;
        }
        
        const std::string unitName = arguments[2].stringValue();
        const TTAMachine::Machine::ImmediateUnitNavigator navigator = 
                parent().simulatorFrontend().machine().immediateUnitNavigator();
        if (!navigator.hasItem(unitName)) {
                parent().interpreter()->setError(
                    SimulatorToolbox::textGenerator().text(
                        Texts::TXT_IMMEDIATE_UNIT_NOT_FOUND).str());
                return false;                    
            }
            
        if (argumentCount == 1) {
            std::string output = "";
            bool firstReg = true;
            
            for (int i = 0; i < navigator.count(); ++i) {

                if (!firstReg) 
                    output += "\n";

                SimValue value = parent().simulatorFrontend().
                    immediateUnitRegisterValue(unitName, i);

                const std::string registerName = 
                    unitName + "." + Conversion::toString(i);

                output += InfoRegistersCommand::registerDescription(
                    registerName, value);
                firstReg = false;
            }
            parent().interpreter()->setResult(output);
            return true;

        } else if (argumentCount == 2) {
            // prints out the register in the given register file
            if (!parent().checkPositiveIntegerArgument(arguments[3])) {
                return false;
            }
            int registerIndex = arguments[3].integerValue();
            try {
                SimValue value = parent().simulatorFrontend().
                    immediateUnitRegisterValue(unitName, registerIndex);
                parent().interpreter()->setResult(value.intValue());
                return true;     
            } catch (const Exception& e) {
                parent().interpreter()->setError(
                    SimulatorToolbox::textGenerator().text(
                        Texts::TXT_REGISTER_NOT_FOUND).str());
                return false;
            }         
        }
        abortWithError("Shouldn't get here.");
        return false;
    }
};


/**
 * Implementation of "info regfiles".
 */
class InfoRegFilesCommand : public SimControlLanguageSubCommand {
public:
    /**
     * Constructor.
     */
    InfoRegFilesCommand(SimControlLanguageCommand& parentCommand) : 
        SimControlLanguageSubCommand(parentCommand) {
    }

    /**
     * Destructor.
     */
    virtual ~InfoRegFilesCommand() {
    }

    /**
     * Executes the "info regfiles" command.
     *
     * "info regfiles" prints the names of all the register files in the
     * machine.
     *
     * @param arguments No arguments should be given.
     * @return Always true.
     */
    virtual bool execute(const std::vector<DataObject>& arguments) {

        const int argumentCount = arguments.size() - 2;

        if (!parent().checkArgumentCount(argumentCount, 0, 0)) {
            return false;
        }
        const TTAMachine::Machine& mach = 
            parent().simulatorFrontend().machine();
        const TTAMachine::Machine::RegisterFileNavigator& nav = 
            mach.registerFileNavigator();
        std::string result = "";
        bool isFirst = true;
        for (int i = 0; i < nav.count(); ++i) {
            if (!isFirst) 
                result += "\n";
            result += nav.item(i)->name();
            isFirst = false;
        }
        parent().interpreter()->setResult(result);
        return true;
    }
};

/**
 * Implementation of "info iunits".
 */
class InfoIunitsCommand : public SimControlLanguageSubCommand {
public:
    /**
     * Constructor.
     */
    InfoIunitsCommand(SimControlLanguageCommand& parentCommand) : 
        SimControlLanguageSubCommand(parentCommand) {
    }

    /**
     * Destructor.
     */
    virtual ~InfoIunitsCommand() {
    }

    /**
     * Executes the "info iunits" command.
     *
     * "info iunits" prints the names of all the immediate units in the
     * machine.
     *
     * @param arguments No arguments should be given.
     * @return Always true.
     */
    virtual bool execute(const std::vector<DataObject>& arguments) {

        const int argumentCount = arguments.size() - 2;

        if (!parent().checkArgumentCount(argumentCount, 0, 0)) {
            return false;
        }
        const TTAMachine::Machine& mach = 
            parent().simulatorFrontend().machine();
        const TTAMachine::Machine::ImmediateUnitNavigator& nav = 
            mach.immediateUnitNavigator();
        std::string result = "";
        bool isFirst = true;
        for (int i = 0; i < nav.count(); ++i) {
            if (!isFirst) 
                result += "\n";
            result += nav.item(i)->name();
            isFirst = false;
        }
        parent().interpreter()->setResult(result);
        return true;
    }
};

/**
 * Implementation of "info busses".
 */
class InfoBussesCommand : public SimControlLanguageSubCommand {
public:
    /**
     * Constructor.
     */
    InfoBussesCommand(SimControlLanguageCommand& parentCommand) : 
        SimControlLanguageSubCommand(parentCommand) {
    }

    /**
     * Destructor.
     */
    virtual ~InfoBussesCommand() {
    }

    /**
     * Executes the "info busses" command.
     *
     * "info busses" prints information of the busses of the machine.
     *
     * @param arguments No arguments should be given.
     * @return Always true.
     */
    virtual bool execute(const std::vector<DataObject>& arguments) {

        const int argumentCount = arguments.size() - 2;

        if (!parent().checkArgumentCount(argumentCount, 0, 1)) {
            return false;
        }
        const TTAMachine::Machine& mach = 
            parent().simulatorFrontend().machine();

        if (argumentCount == 0) {
            const TTAMachine::Machine::BusNavigator& nav = 
                mach.busNavigator();
            std::string result = "";
            bool isFirst = true;
            for (int i = 0; i < nav.count(); ++i) {
                if (!isFirst) 
                    result += "\n";
                TTAMachine::Bus* bus = nav.item(i);
                assert(bus != NULL);
                result += bus->name() + ": ";
                bool isFirstSegment = true;
                for (int j = 0; j < bus->segmentCount(); ++j) {
                    if (!isFirstSegment)
                        result += " ";
                    result += bus->segment(j)->name();
                    isFirstSegment = false;
                }
                isFirst = false;
            }
            parent().interpreter()->setResult(result);
            return true;
        } else if (argumentCount == 1) {
            const TTAMachine::Machine::BusNavigator& nav = 
                mach.busNavigator();
            const std::string busName = arguments.at(2).stringValue();

            if (!nav.hasItem(busName)) {
                parent().interpreter()->setError(
                    SimulatorToolbox::textGenerator().text(
                        Texts::TXT_BUS_NOT_FOUND).str());
                return false;
            }

            TTAMachine::Bus* bus = nav.item(busName);
            assert(bus != NULL);
            std::string result = "";
            result += bus->name() + ": ";
            bool isFirstSegment = true;
            for (int j = 0; j < bus->segmentCount(); ++j) {
                if (!isFirstSegment)
                    result += " ";
                result += bus->segment(j)->name();
                isFirstSegment = false;
            }
            parent().interpreter()->setResult(result);
            return true;
        }
        return false;
    }
};

/**
 * Implementation of "info ports".
 */
class InfoPortsCommand : public SimControlLanguageSubCommand {
public:
    /**
     * Constructor.
     */
    InfoPortsCommand(SimControlLanguageCommand& parentCommand) : 
        SimControlLanguageSubCommand(parentCommand) {
    }

    /**
     * Destructor.
     */
    virtual ~InfoPortsCommand() {
    }

    /**
     * Executes the "info ports" command.
     *
     * "info ports" prints info of FU ports.
     *
     * @param arguments No arguments should be given.
     * @return Always true.
     */
    virtual bool execute(const std::vector<DataObject>& arguments) {

        const int argumentCount = arguments.size() - 2;

        if (!parent().checkArgumentCount(argumentCount, 1, 2)) {
            return false;
        }
        const TTAMachine::Machine& mach = 
            parent().simulatorFrontend().machine();

        const std::string functionUnit = arguments.at(2).stringValue();

        const TTAMachine::Machine::FunctionUnitNavigator nav =
            mach.functionUnitNavigator();

        TTAMachine::FunctionUnit* fu = NULL;
        if (nav.hasItem(functionUnit)) {
            fu = nav.item(functionUnit);
        } else if (functionUnit == mach.controlUnit()->name()) {
            fu = mach.controlUnit();
        }
        
        if (fu == NULL) {
            parent().interpreter()->setError(
                SimulatorToolbox::textGenerator().text(
                    Texts::TXT_FU_NOT_FOUND).str());
            return false;
        }

        if (argumentCount == 1) {
            std::string result = "";
            bool isFirst = true;
            for (int i = 0; i < fu->portCount(); ++i) {
                if (!isFirst) 
                    result += "\n";
                std::string portName = fu->port(i)->name();
                DisassemblyFUPort portString(functionUnit, portName);
                SimValue portValue = parent().simulatorFrontend().FUPortValue(
                    functionUnit, portName);
                result += InfoRegistersCommand::registerDescription(
                    portString.toString(), portValue);
                isFirst = false;
            }
            parent().interpreter()->setResult(result);
            return true;
        } else if (argumentCount == 2) {
            const std::string portName = arguments.at(3).stringValue();
            try {
                // @todo printing of double values (size > 32)
                SimValue portValue = parent().simulatorFrontend().FUPortValue(
                    functionUnit, portName);
                parent().interpreter()->setResult(portValue.intValue());
                return true;
            } catch (const Exception& e) {
                parent().interpreter()->setError(
                    SimulatorToolbox::textGenerator().text(
                        Texts::TXT_FUPORT_NOT_FOUND).str());
                return false;
            }   
        }
        return false;
    }
};

/**
 * Implementation of "info segments".
 */
class InfoSegmentsCommand : public SimControlLanguageSubCommand {
public:
    /**
     * Constructor.
     */
    InfoSegmentsCommand(SimControlLanguageCommand& parentCommand) : 
        SimControlLanguageSubCommand(parentCommand) {
    }

    /**
     * Destructor.
     */
    virtual ~InfoSegmentsCommand() {
    }

    /**
     * Executes the "info segments" command.
     *
     * Prints the values of segments in the given bus.
     *
     * @param arguments No arguments should be given.
     * @return Always true.
     */
    virtual bool execute(const std::vector<DataObject>& arguments) {

        const int argumentCount = arguments.size() - 2;

        if (!parent().checkArgumentCount(argumentCount, 1, 2)) {
            return false;
        }
        const TTAMachine::Machine& mach = 
            parent().simulatorFrontend().machine();

        const std::string busName = arguments.at(2).stringValue();

        const TTAMachine::Machine::BusNavigator nav =
            mach.busNavigator();
        
        if (!nav.hasItem(busName)) {
            parent().interpreter()->setError(
                SimulatorToolbox::textGenerator().text(
                    Texts::TXT_BUS_NOT_FOUND).str());
            return false;
        }

        TTAMachine::Bus* bus = nav.item(busName);
        assert(bus != NULL);
        MachineState& state = parent().simulatorFrontend().machineState();

        if (argumentCount == 1) {
            std::string result = "";
            BusState& busState = state.busState(busName);
            parent().interpreter()->setResult(busState.value().intValue());
            return true;
        } else if (argumentCount == 2) {
            parent().interpreter()->setResult(
                "Segmented busses not yet supported.");
            return false;
        }
        return false;
    }
};


/**
 * Implementation of "info funits".
 */
class InfoFunitsCommand : public SimControlLanguageSubCommand {
public:
    /**
     * Constructor.
     */
    InfoFunitsCommand(SimControlLanguageCommand& parentCommand) : 
        SimControlLanguageSubCommand(parentCommand) {
    }

    /**
     * Destructor.
     */
    virtual ~InfoFunitsCommand() {
    }

    /**
     * Executes the "info funits" command.
     *
     * "info funits" prints the names of all the function units in the
     * machine.
     *
     * @param arguments No arguments should be given.
     * @return Always true.
     */
    virtual bool execute(const std::vector<DataObject>& arguments) {

        const int argumentCount = arguments.size() - 2;

        if (!parent().checkArgumentCount(argumentCount, 0, 0)) {
            return false;
        }
        const TTAMachine::Machine& mach = 
            parent().simulatorFrontend().machine();
        const TTAMachine::Machine::FunctionUnitNavigator& nav = 
            mach.functionUnitNavigator();
        std::string result = "";
        bool isFirst = true;
        for (int i = 0; i < nav.count(); ++i) {
            if (!isFirst) 
                result += "\n";
            result += nav.item(i)->name();
            isFirst = false;
        }
        result += "\n" + mach.controlUnit()->name();
        parent().interpreter()->setResult(result);
        return true;
    }
};


/**
 * Implementation of "info proc".
 */
class InfoProcCommand : public SimControlLanguageSubCommand {
public:
    /**
     * Constructor.
     */
    InfoProcCommand(SimControlLanguageCommand& parentCommand) : 
        SimControlLanguageSubCommand(parentCommand) {
    }

    /**
     * Destructor.
     */
    virtual ~InfoProcCommand() {
    }

    /**
     * Executes the "info proc" command.
     *
     * @param arguments Arguments to the command, including the command.
     * @return true in case execution was successful.
     */
    virtual bool execute(const std::vector<DataObject>& arguments) {

        if (!parent().checkSimulationEnded() &&
            !parent().checkSimulationInitialized() &&
            !parent().checkSimulationStopped() &&
            !parent().simulatorFrontend().isSimulationRunning()) {

            return false;
        }

        const int argumentCount = arguments.size() - 2; 

        if (!parent().checkArgumentCount(argumentCount, 1, 1)) {
            return false;
        }

        const std::string command = 
            StringTools::stringToLower(arguments[2].stringValue());

        if (command == "cycles") {
            parent().interpreter()->setResult(
                (boost::format("%.0f") %
                 parent().simulatorFrontend().cycleCount()).str());
            return true;
        } else if (command == "stats") {
            std::stringstream result;
            
            const ClockCycleCount totalCycles = 
                parent().simulatorFrontend().cycleCount();
            
            result
                << std::endl 
                << "Calculating statistics..." << std::flush;

            const UtilizationStats& stats = 
                parent().simulatorFrontend().utilizationStatistics();

            result
                << "done." << std::endl;

            result
                << std::endl
                << "utilizations" << std::endl
                << "------------" << std::endl;

            const int COLUMN_WIDTH = 15;
            const TTAMachine::Machine& mach = 
                parent().simulatorFrontend().machine();
            std::set<std::string> operationsOfMachine;

            result
                << std::endl << "buses:" << std::endl << std::endl;
            
            const TTAMachine::Machine::BusNavigator& busNav = 
                mach.busNavigator();
            
            for (int i = 0; i < busNav.count(); ++i) {
                TTAMachine::Bus* bus = busNav.item(i);
                assert(bus != NULL);
                const ClockCycleCount writes = 
                    stats.busWrites(bus->name());
                if (writes == 0)
                    continue;

                result                    
                    << std::left << std::setw(COLUMN_WIDTH)
                    << bus->name() << " " 
                    << std::left << std::setw(COLUMN_WIDTH)
                    << Conversion::toString(writes * 100.0 / totalCycles) +
                    "% (" + Conversion::toString(writes) + " writes)"
                    << std::endl;
            }
                
            result
                << std::endl
                << "sockets:" << std::endl << std::endl;
            
            const TTAMachine::Machine::SocketNavigator& socketNav = 
                mach.socketNavigator();
            for (int i = 0; i < socketNav.count(); ++i) {
                TTAMachine::Socket* socket = socketNav.item(i);
                assert(socket != NULL);
                const ClockCycleCount writes = 
                    stats.socketWrites(socket->name());
                if (writes == 0) 
                    continue;

                result
                    << std::left << std::setw(COLUMN_WIDTH)
                    << socket->name() << " " 
                    << std::left << std::setw(COLUMN_WIDTH)
                    << Conversion::toString(writes * 100.0 / totalCycles) +
                    "% (" + Conversion::toString(writes) + " writes)"
                    << std::endl;
            }            
             
            result                    
                << std::endl
                << "operations executed in function units:" 
                << std::endl << std::endl;
            
            const TTAMachine::Machine::FunctionUnitNavigator& fuNav = 
                mach.functionUnitNavigator();
            for (int i = 0; i <= fuNav.count(); ++i) {
                TTAMachine::FunctionUnit* fu = NULL;
                if (i < fuNav.count())
                    fu = fuNav.item(i);
                else
                    fu = mach.controlUnit();
                assert(fu != NULL);
                const ClockCycleCount totalTriggersOfFU = 
                    stats.triggerCount(fu->name());

                if (totalTriggersOfFU == 0)
                    continue;

                result                        
                    << fu->name() << ":" << std::endl;

                for (int j = 0; j < fu->operationCount(); ++j) {
                    const TTAMachine::HWOperation* op = fu->operation(j);
                    assert(op != NULL);
                    const std::string operationUpper = 
                        StringTools::stringToUpper(op->name());
                    operationsOfMachine.insert(operationUpper);
                    const ClockCycleCount executions = 
                        stats.operationExecutions(
                            fu->name(), operationUpper);
                    if (executions == 0) 
                        continue;

                    result
                        << std::left << std::setw(COLUMN_WIDTH)
                        << operationUpper << " " 
                        << std::left << std::setw(COLUMN_WIDTH)
                        << Conversion::toString(
                            executions * 100.0 / totalTriggersOfFU) + 
                        "% of FU total (" + 
                        Conversion::toString(executions) + " executions)"
                        << std::endl;
                }

                result                   
                    << std::left << std::setw(COLUMN_WIDTH)
                    << "TOTAL" << " " 
                    << std::left << std::setw(COLUMN_WIDTH)
                    << Conversion::toString(
                        totalTriggersOfFU * 100.0 / totalCycles) + "% (" +
                    Conversion::toString(totalTriggersOfFU) + 
                    " triggers)" << std::endl << std::endl;
            }

            const TTAMachine::FunctionUnit& gcu = *mach.controlUnit();
            for (int j = 0; j < gcu.operationCount(); ++j) {
                const TTAMachine::HWOperation* op = gcu.operation(j);
                assert(op != NULL);
                const std::string operationUpper = 
                    StringTools::stringToUpper(op->name());
                operationsOfMachine.insert(operationUpper);
            }

            result
                << std::endl << "operations:" << std::endl << std::endl;
            
            for (std::set<std::string>::iterator i = 
                     operationsOfMachine.begin(); i != 
                     operationsOfMachine.end(); ++i) {
                const ClockCycleCount executions = 
                    stats.operationExecutions(*i);
                if (executions == 0)
                    continue;

                result
                    << std::left << std::setw(COLUMN_WIDTH)
                    << *i << " " << std::left << std::setw(COLUMN_WIDTH)
                    << Conversion::toString(executions * 100.0 / totalCycles) +
                    "% (" + Conversion::toString(executions) + " executions)" 
                    << std::endl;
            }
            
            result
                << std::endl
                << "FU port guard accesses:" << std::endl;
            
            UtilizationStats::FUOperationUtilizationIndex fuGuardAccesses =
                stats.FUGuardAccesses();
            
            // loop each FU
            for (UtilizationStats::FUOperationUtilizationIndex::iterator i = 
                fuGuardAccesses.begin(); i != fuGuardAccesses.end(); ++i) {
                
                std::string fuName = i->first;
                
                result
                    << std::endl
                    << fuName << ":"
                    << std::endl;
                
                // loop each FU port in the utilization list
                for (UtilizationStats::ComponentUtilizationIndex::iterator j = 
                    i->second.begin(); j != i->second.end(); ++j) {
                    std::string fuPort = j->first;
                    
                    ClockCycleCount count =
                    stats.FUGuardAccesses(fuName, fuPort);
                        result
                            << std::left << std::setw(COLUMN_WIDTH)
                            << fuPort << ": "
                            << Conversion::toString(count) << " reads"
                            << std::endl;
                }
            }

            result
                << std::endl
                << "register accesses:" << std::endl 
                << std::endl;
            
            const TTAMachine::Machine::RegisterFileNavigator& rfNav = 
                mach.registerFileNavigator();
            for (int i = 0; i < rfNav.count(); ++i) {
                TTAMachine::RegisterFile* rf = rfNav.item(i);
                assert(rf != NULL);

                result 
                    << rf->name() << ":" << std::endl;

                int regsUsedInFile = 0;
                int lastReg = 0;
                lastReg = rf->numberOfRegisters() - 1;

                for (int reg = 0; reg <= lastReg; ++reg) {
                    ClockCycleCount reads = 
                        stats.registerReads(rf->name(), reg);
                    ClockCycleCount guardReads = 
                        stats.guardRegisterReads(rf->name(), reg);
                    ClockCycleCount writes = 
                        stats.registerWrites(rf->name(), reg);
                    if (reads == 0 && writes == 0 && guardReads == 0)
                        continue;
                    ++regsUsedInFile;
                    result
                        << std::left << std::setw(COLUMN_WIDTH)
                        << reg << " " 
                        << std::left << std::setw(COLUMN_WIDTH)
                        << Conversion::toString(reads) + " reads, "
                        << std::left << std::setw(COLUMN_WIDTH + 5)
                        << Conversion::toString(guardReads) + " guard reads, " 
                        << std::left << std::setw(COLUMN_WIDTH)
                        << Conversion::toString(writes) + " writes"
                        << std::endl;
                }
                result
                    << "TOTAL " << regsUsedInFile << " registers used"
                    << std::endl << std::endl;
            }
                        
            result
                << std::endl
                << "immediate unit accesses:" << std::endl 
                << std::endl;
            
            const TTAMachine::Machine::ImmediateUnitNavigator& iuNav = 
                mach.immediateUnitNavigator();
            for (int i = 0; i < iuNav.count(); ++i) {
                TTAMachine::ImmediateUnit* iu = iuNav.item(i);
                assert(iu != NULL);

                result
                    << iu->name() << ":" << std::endl;

                int usedRegCount = 0;
                int lastReg = 0;
                lastReg = iu->numberOfRegisters() - 1;
                
                for (int reg = 0; reg <= lastReg; ++reg) {
                    ClockCycleCount reads = 
                        stats.registerReads(iu->name(), reg);
                    ClockCycleCount writes = 
                        stats.registerWrites(iu->name(), reg);
                    if (reads == 0 && writes == 0)
                        continue;
                    ++usedRegCount;
                    result
                        << std::left << std::setw(COLUMN_WIDTH)
                        << reg << " " 
                        << std::left << std::setw(COLUMN_WIDTH)
                        << Conversion::toString(reads) + " reads, " 
                        << std::left << std::setw(COLUMN_WIDTH)
                        << Conversion::toString(writes) + " writes"
                        << std::endl;
                }
                result
                    << "TOTAL " << usedRegCount << " registers used"
                    << std::endl << std::endl;
            }            


            if (parent().simulatorFrontend().rfAccessTracing()) {
                
                try {
                    const RFAccessTracker& rfAccessTracker = 
                        parent().simulatorFrontend().rfAccessTracker();

                    result
                        << std::endl
                        << "register file accesses" << std::endl
                        << "----------------------" << std::endl;

                    const int COL_WIDTH = 20;

                    result
                        << std::left << std::setw(COL_WIDTH)
                        << "register file"
                        << std::left << std::setw(COL_WIDTH)
                        << "writes"
                        << std::left << std::setw(COL_WIDTH)
                        << "reads"
                        << std::left << std::setw(COL_WIDTH)
                        << "count" << std::endl;

                    RFAccessTracker::ConcurrentRFAccessIndex::
                        const_iterator i =
                        rfAccessTracker.accessDataBase().begin();

                    for (; i != rfAccessTracker.accessDataBase().end(); ++i) {
                        result
                            << std::left << std::setw(COL_WIDTH)
                            << (*i).first.get<0>()
                            << std::left << std::setw(COL_WIDTH)
                            << (*i).first.get<1>()
                            << std::left << std::setw(COL_WIDTH)
                            << (*i).first.get<2>()
                            << std::left << std::setw(COL_WIDTH)
                            << (*i).second << std::endl;
                    }
                } catch (const InstanceNotFound&) {
                }
            }

            parent().interpreter()->setResult(result.str());
            return true;

        } else if (command == "mapping") {
            const TTAMachine::Machine& mach = 
                parent().simulatorFrontend().machine();
            const TTAMachine::Machine::AddressSpaceNavigator& nav = 
                mach.addressSpaceNavigator();
            for (int i = 0; i < nav.count(); ++i) {
                TTAMachine::AddressSpace& space = *nav.item(i);
                unsigned int sizeOfSpace = space.end() - space.start();
                parent().outputStream()
                    << std::left << std::setw(15)
                    << space.name()
                    << std::left 
                    << Conversion::toHexString(space.start(), 8)
                    << " - "
                    << Conversion::toHexString(space.end(), 8)
                    << " (" << sizeOfSpace << " ";
                if (space.width() == 8) {
                    parent().outputStream() << "bytes";
                } else {
                    parent().outputStream() 
                        << "words of size " << space.width() << " bits";
                }                    
                parent().outputStream() << ")" << std::endl;
            }
            return true;
        } else {
            parent().interpreter()->setError(
                SimulatorToolbox::textGenerator().text(
                    Texts::TXT_UNKNOWN_SUBCOMMAND).str());
            return false;
        }
    }
};


/**
 * Implementation of "info stats". The following sub-commands are supported:
 * "executed_operations", "register_reads" and "register_writes"
 */
class InfoStatsCommand : public SimControlLanguageSubCommand {
public:
    /**
     * Constructor.
     */
    InfoStatsCommand(SimControlLanguageCommand& parentCommand) : 
        SimControlLanguageSubCommand(parentCommand) {
    }

    /**
     * Destructor.
     */
    virtual ~InfoStatsCommand() {
    }

    /**
     * Executes the "info stats" command.
     *
     * @param arguments Arguments to the command, including the command.
     * @return true in case execution was successful.
     */
    virtual bool execute(const std::vector<DataObject>& arguments) {
        if (!parent().checkSimulationEnded() &&
            !parent().checkSimulationInitialized() &&
            !parent().checkSimulationStopped() &&
            !parent().simulatorFrontend().isSimulationRunning()) {
            return false;
        }

        const int argumentCount = arguments.size() - 2; 

        if (!parent().checkArgumentCount(argumentCount, 1, 1)) {
            return false;
        }

        const std::string command = 
            StringTools::stringToLower(arguments[2].stringValue());
        
        const UtilizationStats& stats = 
            parent().simulatorFrontend().utilizationStatistics();
                   
        const TTAMachine::Machine& mach = 
            parent().simulatorFrontend().machine();
        
        ClockCycleCount totalOperationExecutions = 0;
        ClockCycleCount totalRegisterReads = 0;
        ClockCycleCount totalRegisterWrites = 0;        

        const TTAMachine::Machine::FunctionUnitNavigator& fuNav = 
            mach.functionUnitNavigator();                            
            
        const TTAMachine::Machine::RegisterFileNavigator& rfNav = 
                mach.registerFileNavigator();                

        if (command == "executed_operations") {
            for (int i = 0; i <= fuNav.count(); ++i) {
                TTAMachine::FunctionUnit* fu = NULL;
                if (i < fuNav.count())
                    fu = fuNav.item(i);
                else
                    fu = mach.controlUnit();
                assert(fu != NULL);
                const ClockCycleCount totalTriggersOfFU =
                    stats.triggerCount(fu->name());
            
                if (totalTriggersOfFU == 0)
                    continue;
                        
                for (int j = 0; j < fu->operationCount(); ++j) {
                    const TTAMachine::HWOperation* op = fu->operation(j);
                    assert (op != NULL);
                    totalOperationExecutions += stats.operationExecutions(
                    fu->name(), StringTools::stringToUpper(op->name()));                    
                }
            }
            parent().interpreter()->setResult(
                   static_cast<double>(totalOperationExecutions));
            return true;
            
        } else if (command == "register_reads") {
            for (int i = 0; i < rfNav.count(); ++i) {
                TTAMachine::RegisterFile* rf = rfNav.item(i);
                assert(rf != NULL);

                int lastReg = 0;
                lastReg = rf->numberOfRegisters() - 1;
                for (int reg = 0; reg <= lastReg; ++reg) {
                    totalRegisterReads += stats.registerReads(rf->name(), reg);
                }
            }
            parent().interpreter()->setResult(
                   static_cast<double>(totalRegisterReads));
            return true;
            
        } else if (command == "register_writes") {
            for (int i = 0; i < rfNav.count(); ++i) {
                TTAMachine::RegisterFile* rf = rfNav.item(i);
                assert(rf != NULL);

                int lastReg = 0;
                lastReg = rf->numberOfRegisters() - 1;
                for (int reg = 0; reg <= lastReg; ++reg) {
                    totalRegisterWrites += stats.registerWrites(rf->name(), reg);
                }
            }
            parent().interpreter()->setResult(
                   static_cast<double>(totalRegisterWrites));
            return true;
            
        } else {
            parent().interpreter()->setError(
                SimulatorToolbox::textGenerator().text(
                    Texts::TXT_UNKNOWN_SUBCOMMAND).str());
            return false;
        }
    }
};


/**
 * Implementation of "info program".
 */
class InfoProgramCommand : public SimControlLanguageSubCommand {
public:
    /**
     * Constructor.
     */
    InfoProgramCommand(SimControlLanguageCommand& parentCommand) : 
        SimControlLanguageSubCommand(parentCommand) {
    }

    /**
     * Destructor.
     */
    virtual ~InfoProgramCommand() {
    }

    /**
     * Executes the "info program" command.
     *
     * Displays information about the status of the program: whether it is 
     * loaded or running, why it stopped.
     *
     * @param arguments Arguments to the command, including the command.
     * @return true in case execution was successful.
     */
    virtual bool execute(const std::vector<DataObject>& arguments) {
        const int argumentCount = arguments.size() - 2;         
        
        if (argumentCount == 0) {
            
            if (parent().simulatorFrontend().isSimulationStopped()) {
                parent().outputStream() 
                    << (SimulatorToolbox::textGenerator().text(
                            Texts::TXT_STATUS_STOPPED) % 
                        parent().simulatorFrontend().programCounter()).str() 
                    << std::endl;
                parent().printStopReasons();
            } else if (parent().simulatorFrontend().hasSimulationEnded()) {
                parent().outputStream() << SimulatorToolbox::textGenerator().text(
                    Texts::TXT_STATUS_FINISHED).str() << std::endl;
            } else if (parent().simulatorFrontend().isSimulationRunning()) {
                abortWithError(
                    "It should not be possible to do this while "
                    "simulation is running!");
            } else if (parent().simulatorFrontend().isSimulationInitialized()) {
                parent().outputStream() << SimulatorToolbox::textGenerator().text(
                    Texts::TXT_STATUS_INITIALIZED).str() << std::endl;
            } else {
                parent().outputStream() << SimulatorToolbox::textGenerator().text(
                    Texts::TXT_STATUS_NOT_INITIALIZED).str() << std::endl;
            }
            return true;

        } else if (argumentCount == 3) {

            if (!parent().simulatorFrontend().isProgramLoaded()) {
                parent().interpreter()->setError(
                    SimulatorToolbox::textGenerator().text(
                        Texts::TXT_STATUS_NOT_INITIALIZED).str());
                return false;
            }

            const std::string command = 
                StringTools::stringToLower(arguments[2].stringValue());
            
            if (command == "is_instruction_reference") {
                try {
                    const int addr = arguments[3].integerValue();
                    const int move = arguments[4].integerValue();
                    if (parent().simulatorFrontend().program().instructionAt(addr).
                        move(move).source().isInstructionAddress()) {
                        parent().interpreter()->setResult("1");
                        return true;
                    } else {
                        parent().interpreter()->setResult("0");
                        return true;
                    }
                } catch (const NumberFormatException& e) {
                    string msg = "Instruction address and move index parameters "
                        "must be integers.";
                    parent().interpreter()->setError(msg);
                    return false;
                } catch (const KeyNotFound & e) {
                    string msg = "Instruction address not found.";
                    parent().interpreter()->setError(msg);
                    return false;
                } catch (const OutOfRange& e) {
                    string msg = "Move slot not found.";
                    parent().interpreter()->setError(msg);
                    return false;
                } catch (const Exception& e) {
                    abortWithError(
                        "Unexpected exception in 'info program "
                        "is_instruction_reference' command!");
                }
            } else {
                parent().interpreter()->setError(
                    SimulatorToolbox::textGenerator().text(
                        Texts::TXT_UNKNOWN_SUBCOMMAND).str());
                return false;
            }
        } else {
            if (!(parent().checkArgumentCount(argumentCount, 0, 0) ||
                  parent().checkArgumentCount(argumentCount, 3, 3))) {
                return false;
            }
        }
        // should not end up here
        return false;
    }
};

/**
 * Implementation of "info breakpoints".
 */
class InfoBreakpointsCommand : public SimControlLanguageSubCommand {
public:
    /**
     * Constructor.
     */
    InfoBreakpointsCommand(SimControlLanguageCommand& parentCommand) : 
        SimControlLanguageSubCommand(parentCommand) {
    }

    /**
     * Destructor.
     */
    virtual ~InfoBreakpointsCommand() {
    }

    /**
     * Executes the "info breakpoints" command.
     *
     * Prints a table of all breakpoints. Each breakpoint is printed in 
     * a separate line. If argument is given, only the breakpoint with
     * handle num is printed.
     *
     * @param arguments Arguments to the command, including the command.
     * @return true in case execution was successful.
     */
    virtual bool execute(const std::vector<DataObject>& arguments) {
        const int argumentCount = arguments.size() - 2;         
        if (!parent().checkArgumentCount(argumentCount, 0, 1)) {
            return false;
        }

        if (argumentCount == 1 && 
            !parent().checkPositiveIntegerArgument(arguments[2])) {
            return false;
        }

        if (!parent().checkSimulationEnded() &&     
            !parent().checkSimulationStopped() &&
            !parent().checkSimulationInitialized()) {
            return false;
        }

        if (argumentCount == 1) {
            unsigned int breakpointHandle = 
                static_cast<unsigned int>(arguments[2].integerValue());
            return parent().printBreakpointInfo(breakpointHandle);
        } else {
            StopPointManager& bpManager = 
                parent().simulatorFrontend().stopPointManager();
            for (unsigned int i = 0; i < bpManager.stopPointCount(); ++i) {
                parent().printBreakpointInfo(bpManager.stopPointHandle(i));
            }
            return true;
        }        
    }
};



/**
 * Constructor.
 *
 * Sets the name of the command to the base class.
 */
InfoCommand::InfoCommand(bool isCompiledSimulation) :
    SimControlLanguageCommand("info"),
    isCompiledSimulation_(isCompiledSimulation) {

    if (!isCompiledSimulation_) {
        subCommands_["breakpoints"] = new InfoBreakpointsCommand(*this);
        subCommands_["busses"] = new InfoBussesCommand(*this);
        subCommands_["segments"] = new InfoSegmentsCommand(*this);
    }
    
    subCommands_["stats"] = new InfoStatsCommand(*this);
    subCommands_["registers"] = new InfoRegistersCommand(*this);
    subCommands_["proc"] = new InfoProcCommand(*this);
    subCommands_["program"] = new InfoProgramCommand(*this);
    subCommands_["watches"] = new InfoBreakpointsCommand(*this);
    subCommands_["regfiles"] = new InfoRegFilesCommand(*this);
    subCommands_["iunits"] = new InfoIunitsCommand(*this);
    subCommands_["funits"] = new InfoFunitsCommand(*this);
    subCommands_["ports"] = new InfoPortsCommand(*this);
    subCommands_["immediates"] = new InfoImmediatesCommand(*this);
}

/**
 * Destructor.
 */
InfoCommand::~InfoCommand() {
    MapTools::deleteAllValues(subCommands_);
}

/**
 * Executes the "info" command.
 *
 * Provides various information of the simulation.
 *
 * @param arguments Subcommand and possible arguments to the subcommand.
 * @return Always true if arguments are valid.
 */
bool 
InfoCommand::execute(const std::vector<DataObject>& arguments)
    throw (NumberFormatException) {

    assert(interpreter() != NULL);

    // "info" as such does not do anything, a subcommand is required.
    if (!checkArgumentCount(arguments.size() - 1, 1, INT_MAX)) {
        return false;
    } 

    SubCommandMap::iterator subCommandPosition = 
        subCommands_.find(
            StringTools::stringToLower(arguments.at(1).stringValue()));
    if (subCommandPosition == subCommands_.end()) {
        interpreter()->setError(
            SimulatorToolbox::textGenerator().text(
                Texts::TXT_UNKNOWN_SUBCOMMAND).str());
        return false;
    }

    assert((*subCommandPosition).second != NULL);
    SimControlLanguageSubCommand& subCommand = *((*subCommandPosition).second);

    return subCommand.execute(arguments);
}

/**
 * Returns the help text for this command.
 * 
 * Help text is searched from SimulatorTextGenerator.
 *
 * @return The help text.
 */
std::string 
InfoCommand::helpText() const {
    return SimulatorToolbox::textGenerator().text(
        Texts::TXT_INTERP_HELP_INFO).str();
}
