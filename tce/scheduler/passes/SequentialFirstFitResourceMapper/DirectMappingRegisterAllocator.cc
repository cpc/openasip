/**
 * @file DirectMappingRegisterAllocator.cc
 *
 * Implementation of DirectMappingRegisterAllocator class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#include <string>

#include "DirectMappingRegisterAllocator.hh"
#include "Conversion.hh"
#include "Application.hh"
#include "CodeSnippet.hh"
#include "RegisterFile.hh"
#include "TerminalRegister.hh"
#include "NullPort.hh"
#include "Instruction.hh"
#include "Move.hh"
#include "UniversalMachine.hh"
#include "UnboundedRegisterFile.hh"
#include "NullRegisterFile.hh"
#include "MoveGuard.hh"
#include "Guard.hh"
#include "MapTools.hh"
#include "ControlUnit.hh"
#include "Program.hh"
#include "NullProcedure.hh"
#include "NullInstruction.hh"
#include "ProgramAnnotation.hh"
#include "MachineConnectivityCheck.hh"
#include "TerminalImmediate.hh"
#include "InstructionReferenceManager.hh"

#include "InterPassDatum.hh"
#include "InterPassData.hh"

using std::string;
using std::pair;
using namespace TTAMachine;
using namespace TTAProgram;

/////////////////////////////////////////////////////////////////////////////
// DirectMappingRegisterAllocator
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 */
DirectMappingRegisterAllocator::DirectMappingRegisterAllocator():
    StartableSchedulerModule() {
}

/**
 * Destructor.
 */
DirectMappingRegisterAllocator::~DirectMappingRegisterAllocator() {
}

void
DirectMappingRegisterAllocator::start()
    throw (Exception) {

    if (program_ == NULL || target_ == NULL) {
        string msg =
            "Source program and/or target architecture not defined!";
        throw ObjectNotInitialized(__FILE__, __LINE__, __func__, msg);
    }

    um_ = dynamic_cast<UniversalMachine*>(&program_->targetProcessor());
    if (um_ == NULL) {
        string msg = "Couldn't get UniversalMachine instance. "
            "Is source pure sequential?";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }

    RegisterFile* connectedRF = NULL;
    bool allConnected = true;
    std::vector<RegisterFile*> tempRegRFs = MachineConnectivityCheck::tempRegisterFiles(*target_);
    if (tempRegRFs.size() == 1) {
        connectedRF = tempRegRFs.at(0);
    }
    if (tempRegRFs.size() > 0) {
        allConnected = false;
    }
    typedef SimpleInterPassDatum<std::vector<std::pair<TTAMachine::RegisterFile*,int> > > TempRegData;
    TempRegData* tempRegData = new TempRegData;
    for (unsigned int i = 0; i < tempRegRFs.size(); i++) {
        tempRegData->push_back(
            std::pair<TTAMachine::RegisterFile*,int>(
                tempRegRFs.at(i), tempRegRFs.at(i)->numberOfRegisters()-1));
    }

    interPassData().setDatum("SCRATCH_REGISTERS",tempRegData);

    Machine::RegisterFileNavigator regNav =
        target_->registerFileNavigator();
 
    // find all registers that can be used for guards
    Machine::BusNavigator busNav = target_->busNavigator();
    for (int i = 0; i < busNav.count(); i++) {
        Bus* bus = busNav.item(i);
        for (int j = 0; j < bus->guardCount(); j++) {
            RegisterGuard* regGuard = 
                dynamic_cast<RegisterGuard*>(bus->guard(j));
            if (regGuard != NULL) {
                guards_.push_back(regGuard);
            }
        }
    }

    // done doing connectivity-related etc. checks
    for (int i = 0; i < regNav.count(); i++) {

        RegisterFile* rf = regNav.item(i);
        int count = rf->numberOfRegisters();

        // check if we need temp reg in this RF
        if (!allConnected) {
            if ((connectedRF == NULL && (rf->width() != 1))
                || connectedRF == rf) {
                count--; // reserve last for connectivity temp
            }
        }

        for (int j = 0; j < count; j++) {
            // does not use guard-capable regs as ordinary regs
            if (!hasGuard(*rf,j)) {
                registers_.push_back(
                    new TerminalRegister(*rf->port(0), j));
            }
        }
    }

    createStackInitialization();

    Program* newProgram = program_->copy();
    Procedure* proc = &newProgram->firstProcedure();

    while (proc != &NullProcedure::instance()) {
        Instruction* ins = &proc->firstInstruction();
        while (ins != &NullInstruction::instance()) {
            allocateRegisters(*ins);
            ins = &proc->nextInstruction(*ins);
        }

        proc = &newProgram->nextProcedure(*proc);
    }

    try {
        *program_ = *newProgram;
    } catch (const Exception& e) {
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, e.errorMessage());
    }
}

bool
DirectMappingRegisterAllocator::needsTarget() const {
    return true;
}

bool
DirectMappingRegisterAllocator::needsProgram() const {
    return true;
}

/**
 * Allocates registers used in moves of given instruction.
 */
void
DirectMappingRegisterAllocator::allocateRegisters(Instruction& ins) {

        for (int j = 0; j < ins.moveCount(); j++) {
            Move& move = ins.move(j);

            if (move.source().isGPR()) {
                const RegisterFile* regFile = &move.source().registerFile();
                string var = getVariableName(regFile, move.source().index());
                if (regBookkeeper_.isReserved(var)) {
                    TerminalRegister& reservedSource = regBookkeeper_.regForVar(var);
                    const RegisterFile& rf = reservedSource.registerFile();
                    int i = 0;
                    while (rf.port(i)->outputSocket() == NULL) {
                        i++;
                        assert(i < rf.portCount());
                    }
                    TerminalRegister* newSource =
                        new TerminalRegister(*rf.port(i), reservedSource.index());
                    move.setSource(newSource);
                } else {
                    if (var.at(0) != 'B') {
                        TerminalRegister& pendingSource = 
                            *findFreeRegister(var);
                        regBookkeeper_.reserveRegister(pendingSource, var);
                        const RegisterFile& rf = pendingSource.registerFile();
                        int i = 0;
                        while (rf.port(i)->outputSocket() == NULL) {
                            i++;
                            assert(i < rf.portCount());
                        }
                        TerminalRegister* newSource =
                            new TerminalRegister(
                                *rf.port(i), pendingSource.index());
                        move.setSource(newSource);
                    }
                }
                addSrcAnnotation(var, move);
            }

            if (move.destination().isGPR()) {

                const RegisterFile* regFile =
                    &move.destination().registerFile();
                string var = getVariableName(
                    regFile, move.destination().index());

                if (regBookkeeper_.isReserved(var)) {

                    TerminalRegister& reservedDst = regBookkeeper_.regForVar(var);
                    const RegisterFile& rf = reservedDst.registerFile();
                    int i = 0;
                    while (rf.port(i)->inputSocket() == NULL) {
                        i++;
                        assert(i < rf.portCount());
                    }
                    TerminalRegister* newDst =
                        new TerminalRegister(*rf.port(i), reservedDst.index());
                    move.setDestination(newDst);
                } else {
                    if (var.at(0) != 'B') {
                        TerminalRegister& pendingDst = *findFreeRegister(var);
                        regBookkeeper_.reserveRegister(pendingDst, var);
                        const RegisterFile& rf = pendingDst.registerFile();
                        int i = 0;
                        while (rf.port(i)->inputSocket() == NULL) {
                            i++;
                            assert(i < rf.portCount());
                        }
                        TerminalRegister* newDst =
                            new TerminalRegister(
                                *rf.port(i), pendingDst.index());
                        move.setDestination(newDst);
                    }
                }
                addDstAnnotation(var, move);
            }
        }
}

void
DirectMappingRegisterAllocator::addSrcAnnotation(
    const std::string& var, TTAProgram::Move& move) {
    int index = Conversion::toInt(var.substr(1));
    if (var.at(0) == 'R') {
        switch (index) {
        case 0:
            move.addAnnotation(
                ProgramAnnotation(ProgramAnnotation::ANN_REGISTER_RV_READ,""));
            break;
        case 1:
            move.addAnnotation(
                ProgramAnnotation(ProgramAnnotation::ANN_REGISTER_SP_READ,""));
            break;
        case 2:
        case 3:
        case 4:
        case 5:
            move.addAnnotation(
                ProgramAnnotation(
                    ProgramAnnotation::ANN_REGISTER_IPARAM_READ,
                    Conversion::toString(index-2)));
        default:
            break;
        }
    }
}

void
DirectMappingRegisterAllocator::addDstAnnotation(
    const std::string& var, TTAProgram::Move& move) {
    int index = Conversion::toInt(var.substr(1));
    if (var.at(0) == 'R') {
        switch (index) {
        case 0:
            move.addAnnotation(
                ProgramAnnotation(ProgramAnnotation::ANN_REGISTER_RV_SAVE,""));
            break;
        case 1:
            move.addAnnotation(
                ProgramAnnotation(ProgramAnnotation::ANN_REGISTER_SP_SAVE,""));
            break;
        case 2:
        case 3:
        case 4:
        case 5:
            move.addAnnotation(
                ProgramAnnotation(
                    ProgramAnnotation::ANN_REGISTER_IPARAM_SAVE,
                    Conversion::toString(index-2)));
        default:
            break;
        }
    }
}

/**
 * Allocates registers for guards in moves of given instruction.
 */
void
DirectMappingRegisterAllocator::allocateGuards(Instruction& ins) {

    for (int j = 0; j < ins.moveCount(); j++) {
        Move& move = ins.move(j);

        if (!move.isUnconditional()) {
            const RegisterGuard* guard =
                dynamic_cast<RegisterGuard*>(&move.guard().guard());
            if (guard == NULL) {
                string msg = "Unexpected guard in move.";
                throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
            }
            string var = getVariableName(guard->registerFile(), guard->registerIndex());
            TerminalRegister& guardReg = regForVar(var);

            // find one bus with matching guard and set it

            Machine::BusNavigator busNav = target_->busNavigator();
            for (int i = 0; i < busNav.count(); i++) {
                Bus* bus = busNav.item(i);
                for (int i = 0; i < bus->guardCount(); i++) {
                    RegisterGuard* regGuard = dynamic_cast<RegisterGuard*>(bus->guard(i));
                    if (regGuard != NULL &&
                        regGuard->registerFile() == &guardReg.registerFile() &&
                        regGuard->registerIndex() == (int)guardReg.index() &&
                        regGuard->isInverted() == guard->isInverted()) {
                        move.setGuard(new MoveGuard(*regGuard));
                        break;
                    }
                }
            }
        }
    }
}

/**
 * Finds a non-allocated register.
 */
TerminalRegister*
DirectMappingRegisterAllocator::findFreeRegister(const std::string& var) const {

    if (registers_.empty()) {
        string msg = "Machine contains no registers!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }

    for (unsigned int i = 0; i < registers_.size(); i++) {
        TerminalRegister* reg = registers_.at(i);
        if (regBookkeeper_.isReserved(*reg)) {
            continue;
        } else if (var.at(0) == 'B'
                   && reg->registerFile().width() > 1) {
            continue;
        } else if (var.at(0) != 'B'
                   && reg->registerFile().width() <= 1) {
            continue;
        } else {
            return reg;
        }
    }

    string msg = "Not enough registers in machine! More than " +
        Conversion::toString(registers_.size()) +" required.";
    throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
}

/**
 * Connects the given register with a variable name and returns it.
 */
std::string
DirectMappingRegisterAllocator::getVariableName(
    const TTAMachine::RegisterFile* regFile, int index) const {

    string var = "";

    if (regFile == &um_->integerRegisterFile()) {
        var.append("R");
    } else if (regFile == &um_->doubleRegisterFile()) {
        var.append("F");
    } else if (regFile == &um_->booleanRegisterFile()) {
        var.append("B");
    } else {
        string msg = "Unexpected register file in machine!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }

    var.append(Conversion::toString(index));
    return var;
}

/**
 * Returns the register reserved for the given variable.
 *
 * @param var The variable.
 * @return The register reserved for the given variable.
 */
TerminalRegister&
DirectMappingRegisterAllocator::regForVar(const std::string& var) const {
    return regBookkeeper_.regForVar(var);
}

SCHEDULER_PASS(DirectMappingRegisterAllocator)

/////////////////////////////////////////////////////////////////////////////
// RegisterBookkeeper
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 */
RegisterBookkeeper::RegisterBookkeeper() {
}

/**
 * Destructor.
 */
RegisterBookkeeper::~RegisterBookkeeper() {
}

/**
 * Allocates a register for the given variable name.
 *
 * @param var The variable to be connected with the register.
 * @param reg The register to be allocated for the variable.
 * @exception Exception if a register has already been reserved for the
 * variable name or the given register has already been reserved.
 */
void
RegisterBookkeeper::reserveRegister(TerminalRegister& reg, std::string var) {
    reservedRegisters_.insert(
        pair<TerminalRegister*, string>(&reg, var));
}

/**
 * Tells whether the given register has been reserved or not.
 *
 * @param reg The register that is checked for availability.
 * @return True if the register has been allocated, false otherwise.
 */
bool
RegisterBookkeeper::isReserved(TerminalRegister& reg) const {
    return MapTools::containsKey(reservedRegisters_, &reg);
}

/**
 * Tells whether a register has been assigned to the given variable.
 *
 * @param var The variable.
 * @return True if a register has been reserved for the variable, false
 *         otherwise.
 */
bool
RegisterBookkeeper::isReserved(std::string var) const {
    return MapTools::containsValue(reservedRegisters_, var);
}

/**
 * Returns the register reserved for the given variable.
 *
 * @param var The variable.
 * @return The register reserved for the given variable.
 */
TerminalRegister&
RegisterBookkeeper::regForVar(const std::string& var) const {
    return *MapTools::keyForValue<TerminalRegister*>(
        reservedRegisters_, var);
}

/**
 * Makes the given register available for reallocation.
 *
 * @param reg The register to deallocate.
 */
void
RegisterBookkeeper::freeRegister(TerminalRegister& reg) {
    reservedRegisters_.erase(&reg);
}

/**
 * Returns the number of reserved registers.
 *
 * @return The number of reserved registers.
 */
int
RegisterBookkeeper::numReservedRegisters() const {
    return reservedRegisters_.size();
}


/**
 * Tells whether a register has a guard associated to it.
 *
 * @param rf Register File of the register
 * @param index index Index of the register.
 *
 * @return wheter there is a guard that uses the register.
 */
bool 
DirectMappingRegisterAllocator::hasGuard(
    TTAMachine::RegisterFile& rf, int index) const {
    for (unsigned int i = 0; i < guards_.size(); i++) {
        RegisterGuard* rg = guards_.at(i);
        if (rg->registerFile() == &rf && rg->registerIndex() == index) {
            return true;
        } 
    }
    return false;
}

void 
DirectMappingRegisterAllocator::createStackInitialization() {

    int lastAddress = 0;
    AddressSpace* instructionAS = target_->controlUnit()->addressSpace();
    const Machine::AddressSpaceNavigator& asNav = 
        target_->addressSpaceNavigator();
    for (int i = 0; i < asNav.count(); i++) {
        const AddressSpace* as = asNav.item(i);
        if (as != instructionAS) {
            lastAddress = as->end()&0xFFFFFFF8;
        }
    }
    if (lastAddress == 0) {
        throw IllegalMachine(
            __FILE__,__LINE__,__func__, "No Data address space in machine");
    }

    UniversalMachine& um = dynamic_cast<UniversalMachine&>(program_->targetProcessor());
    RegisterFile& rf = um.integerRegisterFile();

    Move* move = new Move(
        new TerminalImmediate(SimValue(lastAddress,32)),
        new TerminalRegister(*rf.port(0), 1), um.universalBus());

    Procedure& crt0 = program_->procedure(0);
    Instruction &oldFirstIns = crt0.instructionAtIndex(0);
    Instruction* ins = new Instruction;
    ins->addMove(move);
    crt0.insertBefore(oldFirstIns, ins);
    InstructionReferenceManager &irm = program_->instructionReferenceManager();
    irm.replace(oldFirstIns, *ins);
}
