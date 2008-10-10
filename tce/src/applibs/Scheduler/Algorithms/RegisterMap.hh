/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file RegisterMap.hh
 *
 * Declaration of RegisterMap class
 *
 * Stores references to all registers on machine
 *
 * @author Heikki Kultala 2006 (heikki.kultala-no.spam-tut.fi)
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
