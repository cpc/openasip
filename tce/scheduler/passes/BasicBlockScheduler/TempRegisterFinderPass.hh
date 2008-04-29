/**
 * @file TempRegisterFinderPass.hh
 *
 * Class which finds free regs for temp reg copier.
 * 
 * @author Heikki Kultala 2008 (hkultala@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_TEMP_REGISTER_FINDER_PASS_HH
#define TTA_TEMP_REGISTER_FINDER_PASS_HH

#include "StartableSchedulerModule.hh"

namespace TTAProgram {
    class Move;
    class Procedure;
    class TerminalRegister;
}

class MoveNode;
class RegisterMap;
class DataDependenceGraph;
class DataDependenceEdge;
class ControlFlowGraph;

class TempRegisterFinderPass : public StartableSchedulerModule {
public:
    virtual ~TempRegisterFinderPass() {};
    virtual void start()        
        throw (Exception);
    bool needsProgram() const { return false; }
    bool needsTarget() const { return true; }
    virtual std::string shortDescription() const;
    virtual std::string longDescription() const;

private:
};



#endif
