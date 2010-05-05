/**
 * @file CodeGenerator.hh
 *
 * Declaration of CodeGenerator class.
 *
 * @author Mikael Lepistö 2008 (mikael.lepisto@tut.fi)
 * @note rating: red
 */

#ifndef TTA_CODE_GENERATOR_HH
#define TTA_CODE_GENERATOR_HH

#include <set>

#include "TCEString.hh"

namespace TTAMachine {
    class Machine;
    class Bus;
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
        const TTAMachine::Machine& mach,
        const UniversalMachine& uMach);

    virtual ~CodeGenerator();

    void addMoveToProcedure(TTAProgram::Procedure& dstProcedure,
                            TTAProgram::Terminal* srcTerminal,
                            TTAProgram::Terminal* dstTerminal);

    void addAnnotatedMoveToProcedure(TTAProgram::Procedure& dstProcedure,
                                     TTAProgram::Terminal* srcTerminal,
                                     TTAProgram::Terminal* dstTerminal,
                                     const TTAProgram::ProgramAnnotation&
                                     annotation);

    TTAProgram::Terminal* createTerminalRegister(
        const TCEString& name, bool readPort);

    TTAProgram::TerminalFUPort* createTerminalFUPort(
        const TCEString& opName, int operand);

    void loadTerminal(TTAProgram::Procedure& dstProcedure,
                      TTAProgram::Terminal* srcTerminal,
                      TTAProgram::Terminal* dstTerminal);

    void storeTerminal(TTAProgram::Procedure& dstProcedure,
                       TTAProgram::Terminal* dstTerminal,
                       TTAProgram::Terminal* srcTerminal);

    void loadFromAddress(TTAProgram::Procedure& dstProcedure,
                         TTAProgram::Terminal* srcTerminal,
                         const TCEString& dstReg);

    void storeToAddress(TTAProgram::Procedure& dstProcedure,
                        TTAProgram::Terminal* dstTerminal,
                        const TCEString& srcReg);

    void loadFromRegisterAddress(TTAProgram::Procedure& dstProcedure,
                                 const TCEString& srcReg,
                                 const TCEString& dstReg);

    void storeToRegisterAddress(TTAProgram::Procedure& dstProcedure,
                                const TCEString& dstReg,
                                const TCEString& srcReg);

    void incrementRegisterAddress(TTAProgram::Procedure& dstProcedure,
                                  const TCEString& dstReg);

    void decrementRegisterAddress(TTAProgram::Procedure& dstProcedure,
                                  const TCEString& dstReg);

    void popFromStack(TTAProgram::Procedure& dstProcedure,
                      const TCEString& stackRegister,
                      TTAProgram::Terminal* dstTerminal);

    void popRegisterFromStack(TTAProgram::Procedure& dstProcedure,
                              const TCEString& stackRegister,
                              const TCEString& dstReg);

    void pushToStack(TTAProgram::Procedure& dstProcedure,
                     const TCEString& stackRegister,
                     TTAProgram::Terminal* srcTerminal);

    void pushRegisterToStack(TTAProgram::Procedure& dstProcedure,
                             const TCEString& stackRegister,
                             const TCEString& srcReg);


    void popFromBuffer(TTAProgram::Procedure& dstProcedure,
                       const TCEString& indexRegister,
                       TTAProgram::Terminal* dstTerminal);

    void popRegisterFromBuffer(TTAProgram::Procedure& dstProcedure,
                               const TCEString& indexRegister,
                               const TCEString& dstReg);

    void pushToBuffer(TTAProgram::Procedure& dstProcedure,
                      const TCEString& indexRegister,
                      TTAProgram::Terminal* srcTerminal);

    void pushRegisterToBuffer(TTAProgram::Procedure& dstProcedure,
                              const TCEString& stackRegister,
                              const TCEString& srcReg);

    void pushInstructionReferenceToStack(
        TTAProgram::Procedure& dstProcedure,
        const TCEString& stackRegister,
        TTAProgram::InstructionReference& srcAddr);

    void pushInstructionReferenceToBuffer(
        TTAProgram::Procedure& dstProcedure,
        const TCEString& indexRegister,
        TTAProgram::InstructionReference& srcAddr);

    void registerJump(
        TTAProgram::Procedure& dstProcedure,
        const TCEString& jumpAddrReg);

    void registerJump(
        TTAProgram::Procedure& dstProcedure,
        const TCEString& jumpAddrReg,
        const TTAProgram::ProgramAnnotation& annotation);

    TTAProgram::Move* createJump(TTAProgram::InstructionReference& dst);

    TTAProgram::Move* createCall(
        TTAProgram::InstructionReference& callDst);

    void createCall(
        TTAProgram::Procedure& dstProcedure,
        TTAProgram::InstructionReference& callDst);

    void registerMove(
        TTAProgram::Procedure& dstProcedure,
        const TCEString& srcReg, const TCEString& dstReg);

    void immediateMove(
        TTAProgram::Procedure& dstProcedure,
        int imm, const TCEString& dstReg);

    TTAProgram::Procedure* createSchedYieldProcedure(
        TTAProgram::InstructionReferenceManager& refManager,
        const TCEString& name,
        TTAProgram::InstructionReference& schedProcedure,
        const TCEString& stackReg,
        const TCEString& rvReg,
        const RegisterSet& saveRegs);

    static TTAProgram::MoveGuard* createInverseGuard(
        TTAProgram::MoveGuard &mg, TTAMachine::Bus* bus = NULL);


private:
    /// Target machine.
    const TTAMachine::Machine* mach_;
    /// Universal machine
    const UniversalMachine* uMach_;
};
}

#endif
