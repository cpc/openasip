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
