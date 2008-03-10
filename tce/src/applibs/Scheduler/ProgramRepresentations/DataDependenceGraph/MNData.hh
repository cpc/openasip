/**
 * @file MNData.hh
 *
 * Declaration of MNData class which is a class helping DDG construction.
 * MNData class is used as wrapper around MoveNode during 
 * DataDependenceGraph construction. MNData adds operations to easily 
 * check dependencies between moves etc.
 *
 * @author Heikki Kultala 2008 (heikki.kultala@tut.fi)
 * @note rating: red
 */

#ifndef TTA_MNDATA_HH
#define TTA_MNDATA_HH

#include <map>
#include <vector>

namespace TTAProgram {
    class TerminalRegister;
}

namespace TTAMachine {
    class RegisterFile;
}

class MemoryAliasAnalyzer;
class BasicBlock;
class UniversalMachine;

class MoveNode;

class MNData {
public:
    MNData(const MNData& mn);
    MNData(MoveNode* mn, 
           std::map<int,TTAProgram::TerminalRegister*>& registers,
           bool entrynode);
    
    virtual ~MNData();
    
    MoveNode& node() const;
    
    bool isRaw(const MNData& prev) const;
    bool isRawToPseudo(const MNData& prev) const;
    bool isRawToGuard(const MNData& prev) const;
    bool isWaw(const MNData& prev) const;
    bool isWar(const MNData& prev) const;
    bool writesTo(const TTAMachine::RegisterFile& rf,
                  unsigned int index) const;
    bool pseudoWritesTo(const TTAMachine::RegisterFile& rf,
                        unsigned int index) const;
    bool readsFrom(const TTAMachine::RegisterFile& rf,
                   unsigned int index) const;
    bool pseudoReadsFrom(const TTAMachine::RegisterFile& rf,
                         unsigned int index) const;
    bool guardReadsFrom(const TTAMachine::RegisterFile& rf,
                        unsigned int index) const;
    
    bool isReturn() const;
    bool isCall() const;
    int removeWritesOnWAW(const MNData& removedWrites);
    int removeReadsOtherWrites(const MNData& removedReads);
    
    bool operator==(const MNData& other) const;
    bool operator<(const MNData& other) const;
private:
    bool writesTo(const TTAProgram::TerminalRegister& tr) const;
    bool readsFrom(const TTAProgram::TerminalRegister& tr) const;
    bool pseudoWritesTo(const TTAProgram::TerminalRegister& tr) const;
    bool pseudoReadsFrom(const TTAProgram::TerminalRegister& tr) const;
    bool guardReadsFrom(const TTAProgram::TerminalRegister& tr) const;
    
    int removeWrite(const TTAProgram::TerminalRegister& terminal);
    int removeWrite(const TTAMachine::RegisterFile& rf, unsigned int index);
    
    int removeRead(const TTAProgram::TerminalRegister& terminal);
    int removeRead(const TTAMachine::RegisterFile& rf, unsigned int index);

    static bool terminalSmaller(const TTAProgram::TerminalRegister& t1,
                                const TTAProgram::TerminalRegister& t2);
    
    MoveNode* moveNode_;
    std::vector<TTAProgram::TerminalRegister*> regWrites_;
    std::vector<TTAProgram::TerminalRegister*> regReads_;
    
    std::vector<TTAProgram::TerminalRegister*> pseudoRegWrites_;
    std::vector<TTAProgram::TerminalRegister*> pseudoRegReads_;
    
    std::vector<TTAProgram::TerminalRegister*> guardRegReads_;
    
    
};

#endif
