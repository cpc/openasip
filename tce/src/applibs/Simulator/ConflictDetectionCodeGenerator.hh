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
 * @file ConflictDetectionCodeGenerator.hh
 *
 * Declaration of ConflictDetectionCodeGenerator class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen@tut.fi)
 * @note rating: red
 */

#ifndef CONFLICT_DETECTION_CODE_GENERATOR_HH
#define CONFLICT_DETECTION_CODE_GENERATOR_HH

#include "BaseType.hh"

#include <string>
#include <map>

namespace TTAMachine {
    class Machine;
    class FunctionUnit;
    class FUPort;
    class Port;
    class Unit;
    class HWOperation;
    class Guard;
    class RegisterFile;
    class ControlUnit;
}

namespace TTAProgram {
    class Instruction;
    class Terminal;
    class Move;
}

class CompiledSimSymbolGenerator;

/**
 * A class that generates C/C++ code for FU conflict detection cases
 * 
 * Used for the compiled simulation
 * 
 */
class ConflictDetectionCodeGenerator {
public:
    ConflictDetectionCodeGenerator(
        const TTAMachine::Machine& machine,
        const CompiledSimSymbolGenerator&,
        bool conflictDetectionEnabled = false);
    virtual ~ConflictDetectionCodeGenerator();
    
    std::string includes();
    std::string symbolDeclaration(const TTAMachine::FunctionUnit& fu);
    std::string extraInitialization();
    std::string notifyOfConflicts();
    std::string updateSymbolDeclarations();
    std::string advanceClockCode();
    std::string detectConflicts(const TTAMachine::HWOperation& op);
    
    bool conflictDetectionEnabled() const;
    
private:
    /// Copying not allowed.
    ConflictDetectionCodeGenerator(const ConflictDetectionCodeGenerator&);
    /// Assignment not allowed.
    ConflictDetectionCodeGenerator& operator=(const ConflictDetectionCodeGenerator&);
    
    /// The machine
    const TTAMachine::Machine& machine_;
    
    /// True, if the conflict detection is enabled
    bool conflictDetectionEnabled_;
    
    /// The FU resource conflict detector used, empty string if disabled.
    std::string conflictDetectorType_;
    /// The method in the resource conflict detector used to detect conflicts, 
    /// empty string if disabled.
    std::string conflictDetectorMethod_;
    /// The method to be called on the conflict detector on cycle advance.
    std::string conflictDetectorAdvanceCycle_;
    /// A method to be called on the conflict detector for initialization,
    /// empty if none.
    std::string conflictDetectorExtraInitMethod_;
    
    /// 1=fu.name(), 2=conflict detector object name
    typedef std::map<std::string, std::string> ConflictDetectorObjectNameMap;
    /// The resource conflict detector object name for each FU if any.
    ConflictDetectorObjectNameMap conflictDetectors_;
    
    /// Symbol generator
    const CompiledSimSymbolGenerator& symbolGen_;
};

#endif
