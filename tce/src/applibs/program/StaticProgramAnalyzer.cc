/**
 * @file StaticProgramAnalyzer.cc
 *
 * Implementation of the StaticProgramAnalyzer class.
 *
 * @author Jari Mäntyneva 2007 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#include "StaticProgramAnalyzer.hh"
#include "Instruction.hh"
#include "Move.hh"
#include "Terminal.hh"
#include "StringTools.hh"
#include "DataMemory.hh"
#include "DataDefinition.hh"


using namespace TTAProgram;

/**
 * The constructor.
 */
StaticProgramAnalyzer::StaticProgramAnalyzer():
    biggestAddress_(0) {
}

/**
 * The destructor.
 */
StaticProgramAnalyzer::~StaticProgramAnalyzer() {
}

/**
 * Adds a new program to the analyzer and analyzes it. Results are added to
 * the previously analyzed results.
 *
 * @param program Sequential program to be analyzed.
 */
void
StaticProgramAnalyzer::addProgram(const TTAProgram::Program& program) {

    Instruction* instruction = &program.firstInstruction();
    Instruction* lastInstruction = &program.lastInstruction();
    while (instruction != lastInstruction) {
        for (int i = 0; i < instruction->moveCount(); i++) {
            Move& move = instruction->move(i);
            Terminal* source = &move.source();
            Terminal* destination = &move.destination();
            if (source->isGPR()) {
                integerVariables_.insert(source->index());
            } else if (source->isImmediate()) {
                immediates_[MathTools::requiredBits(
                        source->value().unsignedValue())] += 1;
            }
            if (destination->isFUPort()) {
                if (destination->isOpcodeSetting()) {
                    operations_.insert(
                        StringTools::stringToLower(
                            destination->operation().name()));
                }
            } else if (destination->isGPR()) {
                integerVariables_.insert(destination->index());
            }
            
        }
        instruction = &program.nextInstruction(*instruction);
    }
    // analyze the needed memory
    for (int i = 0; i < program.dataMemoryCount(); i++) {
        DataMemory& dataMemory = program.dataMemory(i);
        int definitionCount = dataMemory.dataDefinitionCount();
        DataDefinition& definition =
            dataMemory.dataDefinition(definitionCount - 1);
        Address address = definition.startAddress();
        InstructionAddress instructionAddress = address.location();
        instructionAddress += definition.size();
        if (biggestAddress_ < instructionAddress) {
            biggestAddress_ = instructionAddress;
        }
    }
}

/**
 * Returns set of operations used in the analyzed programs.
 *
 * Operation names in the se are in lower case.
 *
 * @return Set of operations used in the analyzed programs.
 */
std::set<std::string>
StaticProgramAnalyzer::operationsUsed() const {
    return operations_;
}

/**
 * Returns set of register indexes used in the analyzed programs.
 *
 * @return Set of register indexes used in the analyzed programs.
 */
std::set<SIntWord>
StaticProgramAnalyzer::integerRegisterIndexes() const {
    return integerVariables_;
}

/**
 * Returns set of immediate bit widths used in the analyzed programs.
 *
 * @return Set of immediate bit widths used in the analyzed programs.
 */
std::map<int, int>
StaticProgramAnalyzer::immediateBitWidths() const {
    return immediates_;
}

/**
 * Returns the biggest instruction address used in the analyzed programs.
 *
 * @return The biggest instruction address.
 */
InstructionAddress
StaticProgramAnalyzer::biggestAddress() const {
    return biggestAddress_;
}


/**
 * Resets the all counters used be the analyzer.
 */
void
StaticProgramAnalyzer::resetCounters() {
    operations_.clear();
    integerVariables_.clear();
}
