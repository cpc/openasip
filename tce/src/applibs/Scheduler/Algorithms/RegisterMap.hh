/*
    Copyright (c) 2002-2009 Tampere University of Technology.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
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
