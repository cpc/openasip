/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file StaticProgramAnalyzer.hh
 *
 * Declaration of a class that analyzes sequential program code.
 * 
 * @author Jari Mäntyneva 2007 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef STATIC_PROGRAM_ANALYZER_HH
#define STATIC_PROGRAM_ANALYZER_HH

#include <string>
#include <set>
#include <map>
#include "BaseType.hh"

namespace TTAProgram {
    class Program;
}

/**
 * Analyzes sequential programs operation use, register needs and
 * immediate widths.
 */
class StaticProgramAnalyzer {
public:
    StaticProgramAnalyzer();
    virtual ~StaticProgramAnalyzer();

    void addProgram(const TTAProgram::Program& program);
    std::set<std::string> operationsUsed() const;
    std::set<SIntWord> integerRegisterIndexes() const;
    std::map<int, int> immediateBitWidths() const;
    InstructionAddress biggestAddress() const;
    void resetCounters();

private:    
    /// Set of integer variables used in the applications.
    std::set<SIntWord> integerVariables_;
    /// Set of operations used in the applications.
    std::set<std::string> operations_;
    /// Set of immediate widths used in the applications.
    std::map<int, int> immediates_;
    /// Memory used by programs
    unsigned int biggestAddress_;

};


#endif
