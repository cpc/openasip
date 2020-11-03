/**
 * @file CodeGenerator.hh
 *
 * Declaration of CodeGenerator class.
 *
 * @author Mikael Lepisto 2008 (mikael.lepisto@tut.fi)
 * @author Heikki Kultala 2015 (heikki.kultala-NO.SPAM-tut.fi)
 * @note rating: red
 */

#ifndef TTA_CODE_GENERATOR_HH
#define TTA_CODE_GENERATOR_HH

#include <set>

#include "MachineInfo.hh"
#include "ProgramOperation.hh"
#include "TCEString.hh"

namespace TTAMachine {
    class Machine;
    class Bus;
    class RegisterFile;
}

namespace TTAProgram {
    class Address;
    class Instruction;
    class InstructionReference;
    class InstructionReferenceManager;
    class Procedure;
    class TerminalRegister;
    class Terminal;
    class TerminalFUPort;
    class Move;
    class ProgramAnnotation;
    class MoveGuard;
    class CodeSnippet;
}

class UniversalMachine;

namespace TTAProgram {

/**
 * CodeGenerator generates code pieces for given Program.
 *
 * Contains various helper functions for making hand writing of POM
 * easier.
 *
 * @todo Rename to POMGenerator.
 */
class CodeGenerator {
public:
    typedef std::set<TCEString> RegisterSet;

    /// @todo This should probably take only Program
    CodeGenerator(
        const TTAMachine::Machine& mach);

    virtual ~CodeGenerator();

    TTAProgram::Instruction* addMoveToProcedure(
        TTAProgram::CodeSnippet& dstProcedure,
        TTAProgram::Terminal* srcTerminal,
        TTAProgram::Terminal* dstTerminal);

    void addAnnotatedMoveToProcedure(TTAProgram::CodeSnippet& dstProcedure,
                                     TTAProgram::Terminal* srcTerminal,
                                     TTAProgram::Terminal* dstTerminal,
                                     const TTAProgram::ProgramAnnotation&
                                     annotation);


    TTAProgram::Terminal*
    createTerminalRegister(
        const TTAMachine::RegisterFile& rf, int regNum, bool readPort) const;

    TTAProgram::Terminal* createTerminalRegister(
        const TCEString& name, bool readPort);

    TTAProgram::TerminalFUPort* createTerminalFUPort(
        const TCEString& opName, int operand);

    void loadTerminal(TTAProgram::CodeSnippet& dstProcedure,
                      TTAProgram::Terminal* srcTerminal,
                      TTAProgram::Terminal* dstTerminal);

    void storeTerminal(TTAProgram::CodeSnippet& dstProcedure,
                       TTAProgram::Terminal* dstTerminal,
                       TTAProgram::Terminal* srcTerminal);

    void loadFromAddress(TTAProgram::CodeSnippet& dstProcedure,
                         TTAProgram::Terminal* srcTerminal,
                         const TCEString& dstReg);

    void storeToAddress(TTAProgram::CodeSnippet& dstProcedure,
                        TTAProgram::Terminal* dstTerminal,
                        const TCEString& srcReg);

    void loadFromRegisterAddress(TTAProgram::CodeSnippet& dstProcedure,
                                 const TCEString& srcReg,
                                 const TCEString& dstReg);

    void storeToRegisterAddress(TTAProgram::CodeSnippet& dstProcedure,
                                const TCEString& dstReg,
                                const TCEString& srcReg);

    void incrementRegisterAddress(
        TTAProgram::CodeSnippet& dstProcedure, const TCEString& dstReg,
        int increment);

    void decrementRegisterAddress(
        TTAProgram::CodeSnippet& dstProcedure, const TCEString& dstReg,
        int decrement);

    void incrementStackPointer(
        TTAProgram::CodeSnippet& dstProcedure, const TCEString& spReg);

    void decrementStackPointer(
        TTAProgram::CodeSnippet& dstProcedure, const TCEString& spReg);

    void popFromStack(TTAProgram::CodeSnippet& dstProcedure,
                      const TCEString& stackRegister,
                      TTAProgram::Terminal* dstTerminal);

    void popRegisterFromStack(TTAProgram::CodeSnippet& dstProcedure,
                              const TCEString& stackRegister,
                              const TCEString& dstReg);

    void pushToStack(TTAProgram::CodeSnippet& dstProcedure,
                     const TCEString& stackRegister,
                     TTAProgram::Terminal* srcTerminal);

    void pushRegisterToStack(TTAProgram::CodeSnippet& dstProcedure,
                             const TCEString& stackRegister,
                             const TCEString& srcReg);


    void popFromBuffer(TTAProgram::CodeSnippet& dstProcedure,
                       const TCEString& indexRegister,
                       TTAProgram::Terminal* dstTerminal);

    void popRegisterFromBuffer(TTAProgram::CodeSnippet& dstProcedure,
                               const TCEString& indexRegister,
                               const TCEString& dstReg);

    void pushToBuffer(TTAProgram::CodeSnippet& dstProcedure,
                      const TCEString& indexRegister,
                      TTAProgram::Terminal* srcTerminal);

    void pushRegisterToBuffer(TTAProgram::CodeSnippet& dstProcedure,
                              const TCEString& stackRegister,
                              const TCEString& srcReg);

    void pushInstructionReferenceToStack(
        TTAProgram::CodeSnippet& dstProcedure,
        const TCEString& stackRegister,
        TTAProgram::InstructionReference& srcAddr);

    void pushInstructionReferenceToBuffer(
        TTAProgram::CodeSnippet& dstProcedure,
        const TCEString& indexRegister,
        TTAProgram::InstructionReference& srcAddr);

    void registerJump(
        TTAProgram::CodeSnippet& dstProcedure,
        const TCEString& jumpAddrReg);

    void registerJump(
        TTAProgram::CodeSnippet& dstProcedure,
        const TCEString& jumpAddrReg,
        const TTAProgram::ProgramAnnotation& annotation);

    std::shared_ptr<TTAProgram::Move>
    createJump(TTAProgram::InstructionReference& dst);

    std::shared_ptr<TTAProgram::Move> createCall(
        TTAProgram::InstructionReference& callDst);

    void createExternalCall(
        TTAProgram::CodeSnippet& dstProcedure,
        const TCEString& procedureName);

    void createCall(
        TTAProgram::CodeSnippet& dstProcedure,
        TTAProgram::InstructionReference& callDst);

    void registerMove(
        TTAProgram::CodeSnippet& dstProcedure,
        const TCEString& srcReg, const TCEString& dstReg);

    void immediateMove(
        TTAProgram::CodeSnippet& dstProcedure,
        int imm, const TCEString& dstReg);

    TTAProgram::Procedure* createSchedYieldProcedure(
        TTAProgram::InstructionReferenceManager& refManager,
        const TCEString& name,
        const TCEString& schedProcedureName,
        const TCEString& stackReg,
        const TCEString& rvReg,
        const RegisterSet& saveRegs);

    static TTAProgram::MoveGuard* createInverseGuard(
        const TTAProgram::MoveGuard &mg, const TTAMachine::Bus* bus = NULL);

    std::vector<ProgramOperationPtr> createForLoopBufferInit(
        const MoveNode* dynamicLimitMove, int iterationCount, int divider, int loopSize);

    ProgramOperationPtr createBreakOperation(const MoveNode* jump);

    ProgramOperationPtr createWhileLoopBufferInit(int loopSize);

    std::shared_ptr<TTAProgram::Move> createMove(
        TTAProgram::Terminal* src, TTAProgram::Terminal* dst);

private:
    /// Target machine.
    const TTAMachine::Machine* mach_;
    /// Universal machine
    const UniversalMachine* uMach_;

    int stackAlignment_;
    MachineInfo::OperationSet opset_;
};
}

#endif
