/**
 * @file CodeGenerator.cc
 *
 * Implementation of CodeGenerator class.
 *
 * @author Mikael Lepistö 2008 (mikael.lepisto@tut.fi)
 * @note rating: red
 */

#include "CodeGenerator.hh"

#include "CodeSnippet.hh"
#include "Machine.hh"
#include "ControlUnit.hh"
#include "InstructionReferenceManager.hh"
#include "InstructionReference.hh"
#include "TerminalRegister.hh"
#include "TerminalFUPort.hh"
#include "Move.hh"
#include "TerminalImmediate.hh"
#include "TerminalInstructionReference.hh"
#include "RFPort.hh"
#include "HWOperation.hh"
#include "UniversalFunctionUnit.hh"
#include "UniversalMachine.hh"
#include "SpecialRegisterPort.hh"
#include "ProgramAnnotation.hh"
#include "Operation.hh"
#include "Guard.hh"
#include "MoveGuard.hh"
#include "Program.hh"
#include "Procedure.hh"
#include "Instruction.hh"
#include "Conversion.hh"

namespace TTAProgram {

CodeGenerator::CodeGenerator(
    const TTAMachine::Machine& mach):
    mach_(&mach), uMach_(&UniversalMachine::instance()) {}

CodeGenerator::~CodeGenerator() {}

void
CodeGenerator::addMoveToProcedure(
    TTAProgram::CodeSnippet& dstProcedure,
    TTAProgram::Terminal* srcTerminal,
    TTAProgram::Terminal* dstTerminal) {

    TTAProgram::Instruction* newInstr =
        new TTAProgram::Instruction(
            TTAMachine::NullInstructionTemplate::instance());

    newInstr->addMove(
        std::make_shared<TTAProgram::Move>(
            srcTerminal, dstTerminal, uMach_->universalBus()));

    dstProcedure.add(newInstr);
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
CodeGenerator::createTerminalRegister(const TCEString& name, bool readPort) {

    if (name == "RA") {
        return createTerminalFUPort("RA");
    } else {
        int findResult = name.find(".");
        TCEString rfName = name.substr(0, findResult);

        int regNum = Conversion::toInt(
            name.substr(findResult + 1, name.length()-findResult+1));

        TTAMachine::Machine::RegisterFileNavigator regNav =
            mach_->registerFileNavigator();
        TTAMachine::RegisterFile* rf = regNav.item(rfName);

        for (int i = 0; i < rf->portCount(); i++) {
            if (readPort) {
                if (rf->port(i)->isOutput()) {
                    return new TTAProgram::TerminalRegister(
                        *rf->port(i), regNum);
                }
            } else {
                if (rf->port(i)->isInput()) {
                    return new TTAProgram::TerminalRegister(
                        *rf->port(i), regNum);
                }
            }
        }
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

    TCEString loadOp = mach_->isLittleEndian() ? "LD32" : "LDW";

    // create terminal references
    TTAProgram::TerminalFUPort* ldw1Terminal =
        createTerminalFUPort(loadOp, 1);

    TTAProgram::TerminalFUPort* ldw2Terminal =
        createTerminalFUPort(loadOp, 2);

    addMoveToProcedure(dstProcedure, srcTerminal, ldw1Terminal);
    addMoveToProcedure(dstProcedure, ldw2Terminal, dstTerminal);
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

    TCEString storeOp = mach_->isLittleEndian() ? "ST32" : "STW";

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
 * Increment address in a register by 4.
 *
 * @param dstProcedure Procedure to add the moves to.
 * @param dstReg Register to increment.
 */
void
CodeGenerator::incrementRegisterAddress(
    TTAProgram::CodeSnippet& dstProcedure,
    const TCEString& dstReg) {

    // create terminal references
    TTAProgram::Terminal* regReadTerminal =
        createTerminalRegister(dstReg, true);

    TTAProgram::Terminal* regWriteTerminal =
        createTerminalRegister(dstReg, false);

    // TODO: immediate creator function which calculates immWidth
    SimValue immVal(8);
    immVal = 4;
    TTAProgram::TerminalImmediate* imm4Terminal =
        new TTAProgram::TerminalImmediate(immVal);

    TTAProgram::TerminalFUPort* add1Terminal =
        createTerminalFUPort("add", 1);

    TTAProgram::TerminalFUPort* add2Terminal =
        createTerminalFUPort("add", 2);

    TTAProgram::TerminalFUPort* add3Terminal =
        createTerminalFUPort("add", 3);

    // dstProcedure->add(
    //      new CodeSnippet("sp -> ldw.1; ldw.2 -> dstReg; "
    //                      "4 -> add.1; sp -> add.2; add.3 -> sp"));

    addMoveToProcedure(dstProcedure, imm4Terminal, add1Terminal);
    addMoveToProcedure(dstProcedure, regReadTerminal, add2Terminal);
    addMoveToProcedure(dstProcedure, add3Terminal, regWriteTerminal);
}

/**
 * Decrement address in a register by 4.
 *
 * @param dstProcedure Procedure to add the moves to.
 * @param dstReg Register to decrement.
 */
void
CodeGenerator::decrementRegisterAddress(
    TTAProgram::CodeSnippet& dstProcedure,
    const TCEString& dstReg) {

    // create terminal references
    TTAProgram::Terminal* regReadTerminal =
        createTerminalRegister(dstReg, true);

    TTAProgram::Terminal* regWriteTerminal =
        createTerminalRegister(dstReg, false);

    // TODO: immediate creator function which calculates immWidth
    SimValue immVal(8);
    immVal = 4;
    TTAProgram::TerminalImmediate* imm4Terminal =
        new TTAProgram::TerminalImmediate(immVal);

    TTAProgram::TerminalFUPort* sub1Terminal =
        createTerminalFUPort("sub", 1);

    TTAProgram::TerminalFUPort* sub2Terminal =
        createTerminalFUPort("sub", 2);

    TTAProgram::TerminalFUPort* sub3Terminal =
        createTerminalFUPort("sub", 3);

    // dstProcedure->add(
    //      new CodeSnippet("sp -> sub.1; 4 -> sub.2; sub.3 -> sp;"
    //                      "sp -> stw.1; srcTerm -> stw.2;"));
    addMoveToProcedure(dstProcedure, regReadTerminal, sub1Terminal);
    addMoveToProcedure(dstProcedure, imm4Terminal, sub2Terminal);
    addMoveToProcedure(dstProcedure, sub3Terminal, regWriteTerminal);
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

    incrementRegisterAddress(dstProcedure, stackRegister);
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

    incrementRegisterAddress(dstProcedure, stackRegister);
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

    decrementRegisterAddress(dstProcedure, stackRegister);

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

    decrementRegisterAddress(dstProcedure, stackRegister);

    storeToRegisterAddress(dstProcedure, stackRegister, srcReg);
}

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

    decrementRegisterAddress(dstProcedure, indexRegister);

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

    decrementRegisterAddress(dstProcedure, indexRegister);

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

    incrementRegisterAddress(dstProcedure, indexRegister);
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

    incrementRegisterAddress(dstProcedure, indexRegister);
}

void
CodeGenerator::pushInstructionReferenceToStack(
    TTAProgram::CodeSnippet& dstProcedure,
    const TCEString& stackRegister,
    TTAProgram::InstructionReference& srcAddr) {

    // create terminal references
    TTAProgram::TerminalInstructionReference* srcTerminal =
        new TTAProgram::TerminalInstructionReference(srcAddr);

    pushToStack(dstProcedure, stackRegister, srcTerminal);
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

TTAProgram::Move*
CodeGenerator::createJump(TTAProgram::InstructionReference& dst) {

    TTAProgram::TerminalFUPort* jump1Terminal =
        createTerminalFUPort("jump", 1);

    TTAProgram::Terminal* jump0Terminal =
        new TTAProgram::TerminalInstructionReference(dst);

    return new TTAProgram::Move(jump0Terminal, jump1Terminal,
                                uMach_->universalBus());
}


/**
 * Creates a call move.
 */
TTAProgram::Move*
CodeGenerator::createCall(TTAProgram::InstructionReference& callDst) {
    TTAProgram::TerminalInstructionReference* srcTerminal =
        new TTAProgram::TerminalInstructionReference(callDst);

    TTAProgram::TerminalFUPort* dstTerminal =
        createTerminalFUPort("call", 1);
    return new TTAProgram::Move(
        srcTerminal, dstTerminal, uMach_->universalBus());
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
TTAProgram::CodeSnippet*
CodeGenerator::createSchedYieldProcedure(
    TTAProgram::InstructionReferenceManager& refManager,
    const TCEString& name,
    TTAProgram::InstructionReference& schedProcedure,
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
    createCall(*retVal, schedProcedure);

    // read new sp value from rv register
    registerMove(*retVal, rvReg, stackReg);

#ifdef ALL_STACK_PARAMETERS
    // need to pop the parameter from stack
    incrementRegisterAddress(*retVal, stackReg);
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
    bool inv = g.isInverted();
    const TTAMachine::RegisterGuard* rg =
        dynamic_cast<const TTAMachine::RegisterGuard*>(&g);
    if (rg != NULL) {
        TTAMachine::RegisterFile* rf = rg->registerFile();
        int regIndex = rg->registerIndex();

        if (bus == NULL) {
            bus = rg->parentBus();
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
    return NULL;
}
}
