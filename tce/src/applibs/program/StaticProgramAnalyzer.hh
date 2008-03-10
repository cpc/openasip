/** 
 * @file StaticProgramAnalyzer.hh
 *
 * Declaration of a class that analyzes sequential program code.
 * 
 * @author Jari Mäntyneva 2007 (jari.mantyneva@tut.fi)
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
