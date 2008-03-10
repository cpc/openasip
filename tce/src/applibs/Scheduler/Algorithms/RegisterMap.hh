/**
 * @file RegisterMap.hh
 *
 * Declaration of RegisterMap class
 *
 * Stores references to all registers on machine
 *
 * @author Heikki Kultala 2006 (heikki.kultala@tut.fi)
 * @note rating: red
 */

#ifndef TTA_REGISTER_MAP_HH
#define TTA_REGISTER_MAP_HH

#include <vector>
#include <list>

namespace TTAMachine {
    class Machine;
    class RegisterGuard;
    class Bus;
    class RegisterFile;
}

namespace TTAProgram {
    class Terminal;
    class TerminalRegister;
}

class InterPassData;

class RegisterMap {
public:    
    RegisterMap(const TTAMachine::Machine& machine, InterPassData& interPassData);
    unsigned int gprCount() const;
    unsigned int brCount() const;
    unsigned int fprCount() const;
    const TTAProgram::TerminalRegister& terminal(int index) const;
    const TTAProgram::TerminalRegister& brTerminal(int index) const;

    TTAMachine::RegisterGuard* createGuard(int index, bool inv) const;

    ~RegisterMap();
private:
    std::vector<TTAMachine::RegisterGuard*> guards_;
    std::vector<TTAProgram::TerminalRegister*> gprs_;
    std::vector<TTAProgram::TerminalRegister*> fprs_;
    std::vector<TTAProgram::TerminalRegister*> bools_;
    const TTAMachine::Machine& machine_;

    void addRegFiles(
        std::list<TTAMachine::RegisterFile*>& rfs,
        std::vector<TTAProgram::TerminalRegister*>& regClass,
        bool allRfsConnected, TTAMachine::RegisterFile* connectedRF);

    bool hasGuard(TTAMachine::RegisterFile& rf, int index) const;
};

#endif
