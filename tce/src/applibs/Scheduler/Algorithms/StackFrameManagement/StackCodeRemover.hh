/**
 * @file StackCodeRemover.hh
 *
 * Declaration of StackCodeRemover class
 *
 * Removes old stack frame code from procedures and collects the data in them.
 *
 */

#ifndef TTA_STACK_CODE_REMOVER_HH
#define TTA_STACK_CODE_REMOVER_HH


namespace TTAProgram {
    class Procedure;
    class Instruction;
    class programAnnotation;
}

class StackFrameData;

class StackCodeRemover {
public:
    StackFrameData* removeStackFrameCode(TTAProgram::Procedure& proc);
private:
    bool hasStackFrameAnnotation(TTAProgram::Instruction& ins);
    const TTAProgram::ProgramAnnotation getStackFrameAnnotation(
        TTAProgram::Instruction& ins) throw (NotAvailable);
};

#endif
