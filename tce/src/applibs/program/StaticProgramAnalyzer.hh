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
#include "Program.hh"
#include "Operation.hh"


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
