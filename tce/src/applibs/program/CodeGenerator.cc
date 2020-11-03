/**
 * @file CodeGenerator.cc
 *
 * Implementation of CodeGenerator class.
 *
 * @author Mikael Lepistö 2008 (mikael.lepisto@tut.fi)
 * @note rating: red
 */

#include "CodeGenerator.hh"

#include <boost/format.hpp>

#include "CodeSnippet.hh"
#include "ControlUnit.hh"
#include "Conversion.hh"
#include "Guard.hh"
#include "HWOperation.hh"
#include "Instruction.hh"
#include "InstructionReference.hh"
#include "InstructionReferenceManager.hh"
#include "Machine.hh"
#include "MachineInfo.hh"
#include "MathTools.hh"
#include "Move.hh"
#include "MoveGuard.hh"
#include "MoveNode.hh"
#include "Operation.hh"
#include "Procedure.hh"
#include "Program.hh"
#include "ProgramAnnotation.hh"
#include "RFPort.hh"
#include "SpecialRegisterPort.hh"
#include "TerminalFUPort.hh"
#include "TerminalImmediate.hh"
#include "TerminalInstructionReference.hh"
#include "TerminalRegister.hh"
#include "TerminalSymbolReference.hh"
#include "UnboundedRegisterFile.hh"
#include "UniversalFunctionUnit.hh"
#include "UniversalMachine.hh"

namespace TTAProgram {

CodeGenerator::CodeGenerator(const TTAMachine::Machine& mach)
    : mach_(&mach), uMach_(&UniversalMachine::instance()) {
    stackAlignment_ = MachineInfo::maxMemoryAlignment(mach);
    opset_ = MachineInfo::getOpset(mach);
}

CodeGenerator::~CodeGenerator() {}

TTAProgram::Instruction*
CodeGenerator::addMoveToProcedure(
    TTAProgram::CodeSnippet& dstProcedure,
    TTAProgram::Terminal* srcTerminal,
    TTAProgram::Terminal* dstTerminal) {

    TTAProgram::Instruction* newInstr =
        new TTAProgram::Instruction(
            TTAMachine::NullInstructionTemplate::instance());

    newInstr->addMove(std::make_shared<TTAProgram::Move>(
                          srcTerminal, dstTerminal, uMach_->universalBus()));

    dstProcedure.add(newInstr);
    return newInstr;
}

void
CodeGenerator::addAnnotatedMoveToProcedure(
    TTAProgram::CodeSnippet& dstProcedure,
    TTAProgram::Terminal* srcTerminal,
    TTAProgram::Terminal* dstTerminal,
    const TTAProgram::ProgramAnnotation& annotation) {

    TTAProgram::Instruction* newInstr =
        new TTAProgram::Instruction(
            TTAMachine::NullInstructionTemplate::instance());

    auto movePtr = std::make_shared<TTAProgram::Move>(
        srcTerminal, dstTerminal, uMach_->universalBus());

    movePtr->addAnnotation(annotation);
    newInstr->addMove(movePtr);

    dstProcedure.add(newInstr);
}

/**
 * If operand is not given return return address terminal.
 */
TTAProgram::TerminalFUPort*
CodeGenerator::createTerminalFUPort(const TCEString& opName, int operand = 0) {
    if (operand != 0) {

        OperationPool pool;
        const Operation& op = pool.operation(opName.c_str());
        if (op.isControlFlowOperation()) {
            TTAMachine::HWOperation& hwOp =
                *uMach_->controlUnit()->operation(opName);
            return new TTAProgram::TerminalFUPort(hwOp, operand);
        } else {
            TTAMachine::HWOperation& hwOp =
                *uMach_->universalFunctionUnit().operation(opName);
            return new TTAProgram::TerminalFUPort(hwOp, operand);
        }
    } else {
        return new TTAProgram::TerminalFUPort(
            *uMach_->controlUnit()->returnAddressPort());
    }
}

TTAProgram::Terminal*
CodeGenerator::createTerminalRegister(
    const TTAMachine::RegisterFile& rf, int regNum, bool readPort) const {
    for (int i = 0; i < rf.portCount(); i++) {
        if (readPort) {
            if (rf.port(i)->isOutput()) {
                return new TTAProgram::TerminalRegister(
                    *rf.port(i), regNum);
            }
        } else {
            if (rf.port(i)->isInput()) {
                return new TTAProgram::TerminalRegister(
                    *rf.port(i), regNum);
            }
        }
    }
    return NULL;
}


TTAProgram::Terminal*
CodeGenerator::createTerminalRegister(const TCEString& name, bool readPort) {

    if (name == "RA") {
        return createTerminalFUPort("RA");
    } else {
        const TTAMachine::RegisterFile* rf = NULL;
        size_t findResult = name.find(".");
        int regNum;
        if (findResult == std::string::npos) {
            rf = &uMach_->integerRegisterFile();
            regNum = Conversion::toInt(name.substr(1));

        }  else {
            TCEString rfName = name.substr(0, findResult);
            TTAMachine::Machine::RegisterFileNavigator regNav =
                mach_->registerFileNavigator();
            rf = regNav.item(rfName);
            regNum = Conversion::toInt(
            name.substr(findResult + 1, name.length()-findResult+1));
        }
        return createTerminalRegister(*rf, regNum, readPort);
    }

    return NULL;
}

/**
 * Loads terminal from address in a terminal.
 *
 * @param dstProcedure Procedure to add the moves to.
 * @param srcTerminal Address to load from.
 * @param dstTerminal Terminal to store loaded value.
 */
void
CodeGenerator::loadTerminal(
    TTAProgram::CodeSnippet& dstProcedure,
    TTAProgram::Terminal* srcTerminal,
    TTAProgram::Terminal* dstTerminal) {
    TCEString loadOp;
    int width = 0;
    if (dstTerminal->isGPR()) {
        width = dstTerminal->registerFile().width();
    }

    int defaultWidth = mach_->is64bit() ? 64 : 32;
    if (width < defaultWidth) {
        width = defaultWidth;
    }
    if (mach_->isLittleEndian()) {
        loadOp = (boost::format("ld%d") % width).str();
    } else {
        loadOp = "ldw";
    }
    if (opset_.count(loadOp) == 0) {
        abortWithError(
            (boost::format("Operation %s not found in the machine") % loadOp)
                .str());
    }

    // create terminal references
    TTAProgram::TerminalFUPort* load1Terminal =
        createTerminalFUPort(loadOp, 1);

    TTAProgram::TerminalFUPort* load2Terminal =
        createTerminalFUPort(loadOp, 2);

    addMoveToProcedure(dstProcedure, srcTerminal, load1Terminal);
    addMoveToProcedure(dstProcedure, load2Terminal, dstTerminal);
}

/**
 * Store terminal to address in a terminal.
 *
 * @param dstProcedure Procedure to add the moves to.
 * @param dstTerminal Address to store to.
 * @param srcTerminal Terminal to store.
 */
void
CodeGenerator::storeTerminal(
    TTAProgram::CodeSnippet& dstProcedure,
    TTAProgram::Terminal* dstTerminal,
    TTAProgram::Terminal* srcTerminal) {
    TCEString storeOp;
    int width = 0;
    if (srcTerminal->isGPR()) {
        width = srcTerminal->registerFile().width();
    }
    int defaultWidth = mach_->is64bit() ? 64 : 32;
    if (width < defaultWidth) {
        width = defaultWidth;
    }

    if (mach_->isLittleEndian()) {
        storeOp = (boost::format("st%d") % width).str();
    } else {
        storeOp = "stw";
    }
    if (opset_.count(storeOp) == 0) {
        abortWithError(
            (boost::format("Operation %s not found in the machine") % storeOp)
                .str());
    }

    TTAProgram::TerminalFUPort* stw1Terminal =
        createTerminalFUPort(storeOp, 1);

    TTAProgram::TerminalFUPort* stw2Terminal =
        createTerminalFUPort(storeOp, 2);

    addMoveToProcedure(dstProcedure, dstTerminal, stw1Terminal);
    addMoveToProcedure(dstProcedure, srcTerminal, stw2Terminal);
}

/**
 * Loads register from address in a terminal.
 *
 * @param dstProcedure Procedure to add the moves to.
 * @param srcTerminal Address to load from.
 * @param dstReg Register to store loaded value.
 */
void
CodeGenerator::loadFromAddress(
    TTAProgram::CodeSnippet& dstProcedure,
    TTAProgram::Terminal* srcTerminal,
    const TCEString& dstReg) {

    // create terminal references
    TTAProgram::Terminal* dstRegTerminal =
        createTerminalRegister(dstReg, false);

    loadTerminal(dstProcedure, srcTerminal, dstRegTerminal);
}

/**
 * Store register to address in a terminal.
 *
 * @param dstProcedure Procedure to add the moves to.
 * @param dstTerminal Address to store to.
 * @param srcReg Register to store.
 */
void
CodeGenerator::storeToAddress(
    TTAProgram::CodeSnippet& dstProcedure,
    TTAProgram::Terminal* dstTerminal,
    const TCEString& srcReg) {

    TTAProgram::Terminal* srcRegTerminal =
        createTerminalRegister(srcReg, true);

    storeTerminal(dstProcedure, dstTerminal, srcRegTerminal);
}

/**
 * Loads register from address in another register.
 *
 * @param dstProcedure Procedure to add the moves to.
 * @param srcReg Address to load from.
 * @param dstReg Register to store loaded value.
 */
void
CodeGenerator::loadFromRegisterAddress(
    TTAProgram::CodeSnippet& dstProcedure,
    const TCEString& srcReg,
    const TCEString& dstReg) {

    // create terminal references
    TTAProgram::Terminal* srcTerminal =
        createTerminalRegister(srcReg, true);

    loadFromAddress(dstProcedure, srcTerminal, dstReg);
}

/**
 * Store register to address in another register.
 *
 * @param dstProcedure Procedure to add the moves to.
 * @param dstReg Address to store to.
 * @param srcReg Register to store.
 */
void
CodeGenerator::storeToRegisterAddress(
    TTAProgram::CodeSnippet& dstProcedure,
    const TCEString& dstReg,
    const TCEString& srcReg) {

    // create terminal references
    TTAProgram::Terminal* dstTerminal =
        createTerminalRegister(dstReg, true);

    storeToAddress(dstProcedure, dstTerminal, srcReg);
}

/**
 * Increment address in a register by value of increment.
 *
 * @param dstProcedure Procedure to add the moves to.
 * @param dstReg Register to increment.
 * @param increment How much to increment.
 */
void
CodeGenerator::incrementRegisterAddress(
    TTAProgram::CodeSnippet& dstProcedure, const TCEString& dstReg,
    int increment) {

    TCEString addOp = mach_->is64bit() ? "add64" : "add";

    // create terminal references
    TTAProgram::Terminal* regReadTerminal =
        createTerminalRegister(dstReg, true);

    TTAProgram::Terminal* regWriteTerminal =
        createTerminalRegister(dstReg, false);

    // TODO: immediate creator function which calculates immWidth
    SimValue immVal(MathTools::requiredBitsSigned(increment));
    immVal = increment;
    TTAProgram::TerminalImmediate* imm4Terminal =
        new TTAProgram::TerminalImmediate(immVal);

    TTAProgram::TerminalFUPort* add1Terminal =
        createTerminalFUPort(addOp, 1);

    TTAProgram::TerminalFUPort* add2Terminal =
        createTerminalFUPort(addOp, 2);

    TTAProgram::TerminalFUPort* add3Terminal =
        createTerminalFUPort(addOp, 3);

    // dstProcedure->add(
    //      new CodeSnippet("sp -> ldw.1; ldw.2 -> dstReg; "
    //                      "4 -> add.1; sp -> add.2; add.3 -> sp"));

    addMoveToProcedure(dstProcedure, imm4Terminal, add1Terminal);
    addMoveToProcedure(dstProcedure, regReadTerminal, add2Terminal);
    addMoveToProcedure(dstProcedure, add3Terminal, regWriteTerminal);
}

/**
 * Decrement address in a register by value of decrement.
 *
 * @param dstProcedure Procedure to add the moves to.
 * @param dstReg Register to decrement.
 * @param decrement How much to decrement.
 */
void
CodeGenerator::decrementRegisterAddress(
    TTAProgram::CodeSnippet& dstProcedure, const TCEString& dstReg,
    int decrement) {

    TCEString subOp = mach_->is64bit() ? "sub64" : "sub";

    // create terminal references
    TTAProgram::Terminal* regReadTerminal =
        createTerminalRegister(dstReg, true);

    TTAProgram::Terminal* regWriteTerminal =
        createTerminalRegister(dstReg, false);

    // TODO: immediate creator function which calculates immWidth
    SimValue immVal(MathTools::requiredBitsSigned(decrement));
    immVal = decrement;
    TTAProgram::TerminalImmediate* imm4Terminal =
        new TTAProgram::TerminalImmediate(immVal);

    TTAProgram::TerminalFUPort* sub1Terminal =
        createTerminalFUPort(subOp, 1);

    TTAProgram::TerminalFUPort* sub2Terminal =
        createTerminalFUPort(subOp, 2);

    TTAProgram::TerminalFUPort* sub3Terminal =
        createTerminalFUPort(subOp, 3);

    // dstProcedure->add(
    //      new CodeSnippet("sp -> sub.1; 4 -> sub.2; sub.3 -> sp;"
    //                      "sp -> stw.1; srcTerm -> stw.2;"));
    addMoveToProcedure(dstProcedure, regReadTerminal, sub1Terminal);
    addMoveToProcedure(dstProcedure, imm4Terminal, sub2Terminal);
    addMoveToProcedure(dstProcedure, sub3Terminal, regWriteTerminal);
}

void
CodeGenerator::incrementStackPointer(
    TTAProgram::CodeSnippet& dstProcedure, const TCEString& spReg) {
    incrementRegisterAddress(dstProcedure, spReg, stackAlignment_);
}

void
CodeGenerator::decrementStackPointer(
    TTAProgram::CodeSnippet& dstProcedure, const TCEString& spReg) {
    decrementRegisterAddress(dstProcedure, spReg, stackAlignment_);
}

/**
 * Pops value from stack and stores it in a given terminal.
 *
 * @param dstProcedure Procedure to add the moves to.
 * @param stackRegister Stack pointer name.
 * @param dstTerminal Terminal to put the value into.
 */
void
CodeGenerator::popFromStack(
    TTAProgram::CodeSnippet& dstProcedure,
    const TCEString& stackRegister,
    TTAProgram::Terminal* dstTerminal) {

    TTAProgram::Terminal* stackTerminal =
        createTerminalRegister(stackRegister, true);
    loadTerminal(dstProcedure, stackTerminal, dstTerminal);

    incrementStackPointer(dstProcedure, stackRegister);
}

/**
 * Pops value from stack and stores it in a register.
 *
 * @param dstProcedure Procedure to add the moves to.
 * @param stackRegister Stack pointer name.
 * @param dstReg Register to put the value into.
 */
void
CodeGenerator::popRegisterFromStack(
    TTAProgram::CodeSnippet& dstProcedure,
    const TCEString& stackRegister,
    const TCEString& dstReg) {

    loadFromRegisterAddress(dstProcedure, stackRegister, dstReg);

    incrementStackPointer(dstProcedure, stackRegister);
}

/**
 * Push a value in a terminal to the stack.
 *
 * @param dstProcedure Procedure to add the moves to.
 * @param stackRegister Stack pointer name.
 * @param srcTerminal Value to push.
 */
void
CodeGenerator::pushToStack(
    TTAProgram::CodeSnippet& dstProcedure,
    const TCEString& stackRegister,
    TTAProgram::Terminal* srcTerminal) {
    decrementStackPointer(dstProcedure, stackRegister);

    TTAProgram::Terminal* stackTerminal =
        createTerminalRegister(stackRegister, true);
    storeTerminal(dstProcedure, stackTerminal, srcTerminal);
}

/**
 * Push a value in a register to the stack.
 *
 * @param dstProcedure Procedure to add the moves to.
 * @param stackRegister Stack pointer name.
 * @param srcTerminal Value to push.
 */
void
CodeGenerator::pushRegisterToStack(
    TTAProgram::CodeSnippet& dstProcedure,
    const TCEString& stackRegister,
    const TCEString& srcReg) {
    decrementStackPointer(dstProcedure, stackRegister);

    storeToRegisterAddress(dstProcedure, stackRegister, srcReg);
}

void
CodeGenerator::pushInstructionReferenceToStack(
    TTAProgram::CodeSnippet& dstProcedure, const TCEString& stackRegister,
    TTAProgram::InstructionReference& srcAddr) {
    // create terminal references
    TTAProgram::TerminalInstructionReference* srcTerminal =
        new TTAProgram::TerminalInstructionReference(srcAddr);

    pushToStack(dstProcedure, stackRegister, srcTerminal);
}

/**
 * TODO Decide how much the register value should be incremented in
 * the following buffer operations. Right now it's 4, but maybe it should be
 * calculated based on the terminal size. These operations aren't currently
 * used anywhere. For stack buffer operations you should use the above
 * functions, since they take into account the stack alignment.
 */

/**
 * Pops value from buffer and stores it in a given terminal.
 *
 * @param dstProcedure Procedure to add the moves to.
 * @param indexRegister Pointer to the buffer.
 * @param dstTerminal Terminal to put the value into.
 */
void
CodeGenerator::popFromBuffer(
    TTAProgram::CodeSnippet& dstProcedure,
    const TCEString& indexRegister,
    TTAProgram::Terminal* dstTerminal) {
    decrementRegisterAddress(dstProcedure, indexRegister, 4);

    TTAProgram::Terminal* indexTerminal =
        createTerminalRegister(indexRegister, true);
    loadTerminal(dstProcedure, indexTerminal, dstTerminal);
}

/**
 * Pops value from buffer and stores it in a register.
 *
 * @param dstProcedure Procedure to add the moves to.
 * @param indexRegister Pointer to the buffer.
 * @param dstReg Register to put the value into.
 */
void
CodeGenerator::popRegisterFromBuffer(
    TTAProgram::CodeSnippet& dstProcedure,
    const TCEString& indexRegister,
    const TCEString& dstReg) {
    decrementRegisterAddress(dstProcedure, indexRegister, 4);

    loadFromRegisterAddress(dstProcedure, indexRegister, dstReg);
}

/**
 * Push a value in a terminal to a buffer.
 *
 * @param dstProcedure Procedure to add the moves to.
 * @param indexRegister Pointer to the buffer.
 * @param srcTerminal Value to push.
 */
void
CodeGenerator::pushToBuffer(
    TTAProgram::CodeSnippet& dstProcedure,
    const TCEString& indexRegister,
    TTAProgram::Terminal* srcTerminal) {

    TTAProgram::Terminal* indexTerminal =
        createTerminalRegister(indexRegister, true);
    storeTerminal(dstProcedure, indexTerminal, srcTerminal);

    incrementRegisterAddress(dstProcedure, indexRegister, 4);
}

/**
 * Push a value in a register to a buffer.
 *
 * @param dstProcedure Procedure to add the moves to.
 * @param indexRegister Pointer to the buffer.
 * @param srcTerminal Value to push.
 */
void
CodeGenerator::pushRegisterToBuffer(
    TTAProgram::CodeSnippet& dstProcedure,
    const TCEString& indexRegister,
    const TCEString& srcReg) {

    storeToRegisterAddress(dstProcedure, indexRegister, srcReg);

    incrementRegisterAddress(dstProcedure, indexRegister, 4);
}

void
CodeGenerator::pushInstructionReferenceToBuffer(
    TTAProgram::CodeSnippet& dstProcedure,
    const TCEString& indexRegister,
    TTAProgram::InstructionReference& srcAddr) {

    // create terminal references
    TTAProgram::TerminalInstructionReference* srcTerminal =
        new TTAProgram::TerminalInstructionReference(srcAddr);

    pushToBuffer(dstProcedure, indexRegister, srcTerminal);
}

void
CodeGenerator::registerJump(
    TTAProgram::CodeSnippet& dstProcedure,
    const TCEString& jumpAddrReg) {

    TTAProgram::Terminal* jumpDestTerminal =
        createTerminalRegister(jumpAddrReg, true);

    TTAProgram::TerminalFUPort* jump1Terminal =
        createTerminalFUPort("jump", 1);

    // dstProcedure->add(new CodeSnippet("jumpAddrReg -> jmp.1;"));
    addMoveToProcedure(dstProcedure, jumpDestTerminal, jump1Terminal);
}

void
CodeGenerator::registerJump(
    TTAProgram::CodeSnippet& dstProcedure,
    const TCEString& jumpAddrReg,
    const TTAProgram::ProgramAnnotation& annotation) {

    TTAProgram::Terminal* jumpDestTerminal =
        createTerminalRegister(jumpAddrReg, true);

    TTAProgram::TerminalFUPort* jump1Terminal =
        createTerminalFUPort("jump", 1);

    addAnnotatedMoveToProcedure(
        dstProcedure, jumpDestTerminal, jump1Terminal, annotation);
}

std::shared_ptr<TTAProgram::Move>
CodeGenerator::createJump(TTAProgram::InstructionReference& dst) {

    TTAProgram::TerminalFUPort* jump1Terminal =
        createTerminalFUPort("jump", 1);

    TTAProgram::Terminal* jump0Terminal =
        new TTAProgram::TerminalInstructionReference(dst);

    return std::make_shared<TTAProgram::Move>(jump0Terminal, jump1Terminal,
                                         uMach_->universalBus());
}


/**
 * Creates a call move.
 */
std::shared_ptr<TTAProgram::Move>
CodeGenerator::createCall(TTAProgram::InstructionReference& callDst) {
    TTAProgram::TerminalInstructionReference* srcTerminal =
        new TTAProgram::TerminalInstructionReference(callDst);

    TTAProgram::TerminalFUPort* dstTerminal =
        createTerminalFUPort("call", 1);
    return std::make_shared<TTAProgram::Move>(
        srcTerminal, dstTerminal, uMach_->universalBus());
}

/**
 * Creates an external call move.
 */
void
CodeGenerator::createExternalCall(
    TTAProgram::CodeSnippet& dstProcedure,
    const TCEString& procedureName) {

    TTAProgram::TerminalSymbolReference* srcTerminal =
        new TTAProgram::TerminalSymbolReference(procedureName);

    TTAProgram::TerminalFUPort* dstTerminal =
        createTerminalFUPort("call", 1);

    addMoveToProcedure(dstProcedure, srcTerminal, dstTerminal);
}


/**
 * Creates a call move and adds it to the given procedure.
 */
void
CodeGenerator::createCall(
    TTAProgram::CodeSnippet& dstProcedure,
    TTAProgram::InstructionReference& callDst) {

    TTAProgram::TerminalInstructionReference* srcTerminal =
        new TTAProgram::TerminalInstructionReference(callDst);

    TTAProgram::TerminalFUPort* dstTerminal =
        createTerminalFUPort("call", 1);

    addMoveToProcedure(dstProcedure, srcTerminal, dstTerminal);
}

void
CodeGenerator::registerMove(
    TTAProgram::CodeSnippet& dstProcedure,
    const TCEString& srcReg, const TCEString& dstReg) {

    TTAProgram::Terminal* srcTerminal =
        createTerminalRegister(srcReg, true);

    TTAProgram::Terminal* dstTerminal =
        createTerminalRegister(dstReg, false);

    addMoveToProcedure(dstProcedure, srcTerminal, dstTerminal);
}

void
CodeGenerator::immediateMove(
    TTAProgram::CodeSnippet& dstProcedure,
    int imm, const TCEString& dstReg) {

    SimValue immVal(32);
    immVal = imm;
    TTAProgram::TerminalImmediate* srcTerminal =
        new TTAProgram::TerminalImmediate(immVal);

    TTAProgram::Terminal* dstTerminal =
        createTerminalRegister(dstReg, false);

    addMoveToProcedure(dstProcedure, srcTerminal, dstTerminal);
}

/**
 * Creates yield function which saves requested registers.
 *
 * @param refManager Reference manager of destination program.
 * @param name Name for the created procedure.
 * @param schedProcedure Scheduler function to call sp = schedule(sp)
 * @param stackReg Stack register name assigned for program.
 * @param rvReg Return value register name.
 * @param saveRegs Set of registers, which should be saved.
 */
TTAProgram::Procedure*
CodeGenerator::createSchedYieldProcedure(
    TTAProgram::InstructionReferenceManager& refManager,
    const TCEString& name,
    const TCEString& schedProcedureName,
    const TCEString& stackReg,
    const TCEString& rvReg,
    const RegisterSet& saveRegs) {

    TTAProgram::Procedure* retVal =
        new TTAProgram::Procedure(
            name, *mach_->controlUnit()->addressSpace());

    // *** Push registers to stack
    for(RegisterSet::const_iterator i = saveRegs.begin();
        i != saveRegs.end(); i++) {
        pushRegisterToStack(*retVal, stackReg, *i);
    }

    // *** Push return address to stack as well
    pushRegisterToStack(*retVal, stackReg, "RA");

    // *** Create empty instruction to be used as yield_return destination
    TTAProgram::Instruction* yeldReturnInstruction =
        new TTAProgram::Instruction(
            TTAMachine::NullInstructionTemplate::instance());

    TTAProgram::InstructionReference yeldReturnReference =
        refManager.createReference(*yeldReturnInstruction);

    // *** Push InstructionReference to sp reading instruction to stack
    pushInstructionReferenceToStack(*retVal, stackReg, yeldReturnReference);

    // *** Save sp, switch thread and update sp

    // put stackpointer address to be parameter
#ifdef ALL_STACK_PARAMETERS
    pushRegisterToStack(*retVal, stackReg, stackReg);
#else
    registerMove(*retVal, stackReg, rvReg);
#endif
    createExternalCall(*retVal, schedProcedureName);

    // read new sp value from rv register
    registerMove(*retVal, rvReg, stackReg);

#ifdef ALL_STACK_PARAMETERS
    // need to pop the parameter from stack
    incrementStackPointer(*retVal, stackReg);
#endif

    // read yeld address from stack
    popRegisterFromStack(*retVal, stackReg, "RA");

    // jump to yeld point
    registerJump(*retVal, "RA", TTAProgram::ProgramAnnotation(
                     TTAProgram::ProgramAnnotation::ANN_JUMP_TO_NEXT));

    // add previously created "label" instruction
    retVal->add(yeldReturnInstruction);

    // restore return address
    popRegisterFromStack(*retVal, stackReg, "RA");

    // read registers from stack
    for(RegisterSet::const_reverse_iterator i = saveRegs.rbegin();
        i != saveRegs.rend(); i++) {
        popRegisterFromStack(*retVal, stackReg, *i);
    }

    // jump to yeld point
    registerJump(*retVal, "RA");

    // remove the placeholder instr.
    refManager.replace(
        *yeldReturnInstruction, 
        retVal->nextInstruction(*yeldReturnInstruction));

    retVal->remove(*yeldReturnInstruction);
    // debug print created function

    return retVal;
}

/**
 * Creates a guard with same guard register etc but inverted.
 * The guard must be found from the given bus if bus given.
 *
 * @TODO: support for port guards
 *
 * @param mg guard to inverse
 * @param bus bus where the guard must be. if null same as bug of mg.
 * @return new MoveGuard that is given guard inverted, NULL if cannot create.
 */
TTAProgram::MoveGuard*
CodeGenerator::createInverseGuard(
    const TTAProgram::MoveGuard &mg, const TTAMachine::Bus* bus) {

    const TTAMachine::Guard& g = mg.guard();
    if (bus == NULL) {
        bus = g.parentBus();
    }

    bool inv = g.isInverted();
    const TTAMachine::RegisterGuard* rg =
        dynamic_cast<const TTAMachine::RegisterGuard*>(&g);
    if (rg != NULL) {
        const TTAMachine::RegisterFile* rf = rg->registerFile();
        int regIndex = rg->registerIndex();

        // fake guard to be bypassed as port guard?
        if (bus == nullptr) {
            return new TTAProgram::MoveGuard(
                *new TTAMachine::RegisterGuard(
                    !rg->isInverted(), *rg->registerFile(), rg->registerIndex(), nullptr));
        }
        // find guard
        for (int i = 0 ; i < bus->guardCount(); i++) {
            const TTAMachine::Guard *g2 = bus->guard(i);
            const TTAMachine::RegisterGuard* rg2 =
                dynamic_cast<const TTAMachine::RegisterGuard*>(g2);
            if (rg2) {
                if( rg2->registerFile() == rf &&
                    rg2->registerIndex() == regIndex &&
                    rg2->isInverted() == !inv ) {
                    return new TTAProgram::MoveGuard(*rg2);
                }
            }
        }
    }
    const TTAMachine::PortGuard* pg =
        dynamic_cast<const TTAMachine::PortGuard*>(&g);
    if (pg) {
        auto port = pg->port();

        // find guard
        for (int i = 0 ; i < bus->guardCount(); i++) {
            const TTAMachine::Guard *g2 = bus->guard(i);
            const TTAMachine::PortGuard* pg2 =
                dynamic_cast<const TTAMachine::PortGuard*>(g2);
            if (pg2 && pg2->port() == port &&
                pg2->isInverted() == !inv) {
                return new TTAProgram::MoveGuard(*pg2);
            }
        }
    }

    return NULL;
}
/*
 * Returns the operations in order.
 * Last is the lbufs op.
 * It may be preceeded by sub op.
 * The first may be a shift op.
 */
std::vector <ProgramOperationPtr>
CodeGenerator::createForLoopBufferInit(
    const MoveNode* dynamicLimitMove, int iterationCount, int loopSize, int divider) {

    std::vector<ProgramOperationPtr> res;
    // is power-of-2?
    if (divider & (divider-1)) {
        return res;
    }

    const char* opName = "lbufs";
    const TTAMachine::ControlUnit& cu = *mach_->controlUnit();
    if (!cu.hasOperation(opName)) {
        return res;
    }

    OperationPool pool;
    const Operation& lbufsOp = pool.operation(opName);
    SimValue loopSizeSV(MathTools::requiredBitsSigned(loopSize));
    loopSizeSV = loopSize;
    TTAProgram::TerminalImmediate* loopSizeSrc =
        new TTAProgram::TerminalImmediate(loopSizeSV);

    MoveNode* iterCountMN = NULL;
    ProgramOperationPtr shiftPO;
    ProgramOperationPtr subPO;

    // TODO: universalmachine is buggy. these should be in control unit,
    // not universalfunction unit
    TTAProgram::TerminalFUPort* iterCountDst =
        new TTAProgram::TerminalFUPort(
            *uMach_->universalFunctionUnit().operation(opName), 1);

    TTAProgram::TerminalFUPort* loopSizeDst =
        new TTAProgram::TerminalFUPort(
            *uMach_->universalFunctionUnit().operation(opName), 2);

    // static iteration count
    if (dynamicLimitMove == NULL) {
        SimValue iterCountSV(MathTools::requiredBitsSigned(iterationCount)); 
        iterCountSV = iterationCount;
        TTAProgram::Terminal* iterCountSrc = new TTAProgram::TerminalImmediate(iterCountSV);
        iterCountMN = new MoveNode(createMove(iterCountSrc, iterCountDst));
    } else {
        bool decrement = false;
        bool increment = false;
        switch (iterationCount) {
        case -1:
            decrement = true;
        case 0:
            break;
        default:
            if (iterationCount > 0)
                increment = true;
            else
                return res;
        }
        TTAProgram::Terminal* counterValSrc = NULL;
        if (dynamicLimitMove->isSourceOperation())
            counterValSrc = dynamicLimitMove->move().destination().copy();
        else
            counterValSrc = dynamicLimitMove->move().source().copy();

        // if divider not 1, need shift of to scale it down.
        TCEString adjustName = decrement ? "sub" : "add";
        if (mach_->is64bit()) {
            adjustName << "64";
        }
        if (decrement || increment) {
            const Operation& subOp = pool.operation(adjustName.c_str());
            subPO = std::make_shared<ProgramOperation>(subOp);

            // signed 1 is 2 bits.
            TTAProgram::TerminalImmediate* subAmountSrc =
                new TTAProgram::TerminalImmediate(
                    SimValue(abs(iterationCount),
                             MathTools::requiredBitsSigned(iterationCount)));

            TTAProgram::TerminalFUPort* subAmntDst = createTerminalFUPort(adjustName, 2);
            TTAProgram::TerminalFUPort* subRes = createTerminalFUPort(adjustName, 3);

            MoveNode* subAmtMN = new MoveNode(createMove(subAmountSrc, subAmntDst));
            subPO->addInputNode(*subAmtMN);
            subAmtMN->addDestinationOperationPtr(subPO);

            iterCountMN = new MoveNode(createMove(subRes, iterCountDst));
            subPO->addOutputNode(*iterCountMN);
            iterCountMN->setSourceOperationPtr(subPO);

            res.push_back(subPO);
            // first input not here but later
        }

        if (divider > 1) {
            TCEString shiftName = "shr";
            if (mach_->is64bit()) {
                shiftName << "64";
            }
            const Operation& shiftOp = pool.operation(shiftName.c_str());

            int shiftAmount = MathTools::ceil_log2(divider);

            SimValue shiftAmountSV(MathTools::requiredBitsSigned(shiftAmount));
            shiftAmountSV = shiftAmount;

            TTAProgram::TerminalImmediate* shiftAmountSrc =
                new TTAProgram::TerminalImmediate(shiftAmountSV);

            // create terminal references
            TTAProgram::TerminalFUPort* shiftValDst =
                createTerminalFUPort(shiftName, 1);
            TTAProgram::TerminalFUPort* shiftAmntDst =
                createTerminalFUPort(shiftName, 2);
            TTAProgram::TerminalFUPort* shiftRes =
                createTerminalFUPort(shiftName, 3);

            MoveNode* shiftValMN = new MoveNode(createMove(counterValSrc, shiftValDst));
            MoveNode* shiftAmtMN = new MoveNode(createMove(shiftAmountSrc, shiftAmntDst));

            shiftPO = std::make_shared<ProgramOperation>(shiftOp);
            shiftPO->addInputNode(*shiftValMN);
            shiftPO->addInputNode(*shiftAmtMN);
            shiftValMN->addDestinationOperationPtr(shiftPO);
            shiftAmtMN->addDestinationOperationPtr(shiftPO);

            // push before the possible sub
            res.insert(res.begin(),shiftPO);
            MoveNode* shift2dec = NULL;
            if (!(decrement||increment)) {
                iterCountMN = new MoveNode(createMove(shiftRes, iterCountDst));
                shiftPO->addOutputNode(*iterCountMN);
                iterCountMN->setSourceOperationPtr(shiftPO);
            } else {
                // use dsub operation
                TTAProgram::TerminalFUPort* subValDst =
                    createTerminalFUPort(adjustName, 1);
                shift2dec = new MoveNode(createMove(shiftRes, subValDst));
                subPO->addInputNode(*shift2dec);
                shift2dec->addDestinationOperationPtr(subPO);

                shiftPO->addOutputNode(*shift2dec);
                shift2dec->setSourceOperationPtr(shiftPO);
            }
        } else { // no shifting, maybe decrement
            if (decrement||increment) {
                // create terminal references
                TTAProgram::TerminalFUPort* subValDst =
                    createTerminalFUPort(adjustName, 1);
                MoveNode* subValMN = new MoveNode(createMove(counterValSrc, subValDst));
                subPO->addInputNode(*subValMN);
                subValMN->addDestinationOperationPtr(subPO);
            } else {
                iterCountMN = new MoveNode(createMove(counterValSrc, iterCountDst));
            }
        }
    }
    MoveNode* loopSizeMN = new MoveNode(createMove(loopSizeSrc, loopSizeDst));

    ProgramOperationPtr loopBusInitOp(new ProgramOperation(lbufsOp));
    loopBusInitOp->addInputNode(*iterCountMN);
    loopBusInitOp->addInputNode(*loopSizeMN);
    iterCountMN->addDestinationOperationPtr(loopBusInitOp);
    loopSizeMN->addDestinationOperationPtr(loopBusInitOp);

    res.push_back(loopBusInitOp);
    return res;
}

std::shared_ptr<TTAProgram::Move> CodeGenerator::createMove(
    TTAProgram::Terminal* src, TTAProgram::Terminal* dst) {
    return std::make_shared<TTAProgram::Move>(src, dst, uMach_->universalBus());
}

ProgramOperationPtr CodeGenerator::createBreakOperation(const MoveNode* jump) {
    // 4ever loop or buggy input?
    if (jump->move().isUnconditional()) {
        return nullptr;
    }

    const TTAMachine::Guard& guard = jump->move().guard().guard();
    auto rg = dynamic_cast<const TTAMachine::RegisterGuard*>(&guard);
    if (!rg) {
        return nullptr;
    }

    // TODO: change eq->ne or add xor to support non-inverted without
    // both ops.
    const char* opName = rg->isInverted() ? "lbufc" : "lbufz";
    const TTAMachine::ControlUnit& cu = *mach_->controlUnit();
    if (!cu.hasOperation(opName)) {
        return nullptr;
    }

    TTAProgram::TerminalFUPort* dst =
        new TTAProgram::TerminalFUPort(
            *uMach_->universalFunctionUnit().operation(opName), 1);

    // create terminal for reading register
    auto src = createTerminalRegister(
        *rg->registerFile(), rg->registerIndex(), true);

    MoveNode* loopSizeMN = new MoveNode(createMove(src, dst));

    OperationPool pool;
    const Operation& op = pool.operation(opName);
    ProgramOperationPtr po(new ProgramOperation(op));
    po->addInputNode(*loopSizeMN);
    loopSizeMN->addDestinationOperationPtr(po);
    return po;
}


/*
 * Returns the operations in order.
 * Last is the lbufs op.
 * It may be preceeded by sub op.
 * The first may be a shift op.
 */
ProgramOperationPtr
CodeGenerator::createWhileLoopBufferInit(int loopSize) {
    const char* opName = "infloop";
    const TTAMachine::ControlUnit& cu = *mach_->controlUnit();
    if (!cu.hasOperation(opName)) {
        return nullptr;
    }

    TTAProgram::TerminalFUPort* loopSizeDst =
        new TTAProgram::TerminalFUPort(
            *uMach_->universalFunctionUnit().operation(opName), 1);

    OperationPool pool;
    const Operation& lbufsOp = pool.operation(opName);
    SimValue loopSizeSV(MathTools::requiredBitsSigned(loopSize));
    loopSizeSV = loopSize;
    TTAProgram::TerminalImmediate* loopSizeSrc =
        new TTAProgram::TerminalImmediate(loopSizeSV);

    MoveNode* loopSizeMN = new MoveNode(createMove(loopSizeSrc, loopSizeDst));

    ProgramOperationPtr loopBufInitOp(new ProgramOperation(lbufsOp));
    loopBufInitOp->addInputNode(*loopSizeMN);
    loopSizeMN->addDestinationOperationPtr(loopBufInitOp);
    return loopBufInitOp;
}

}
