/**
 * @file CodeCompressorPlugin.hh
 *
 * Declaration of CodeCompressorPlugin class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_CODE_COMPRESSOR_PLUGIN_HH
#define TTA_CODE_COMPRESSOR_PLUGIN_HH

#include <set>
#include <map>
#include <vector>

#include "Exception.hh"

class InstructionBitVector;
class BitVector;
class BinaryEncoding;
class MoveSlot;
class ImmediateSlotField;
class ImmediateControlField;
class SourceField;
class DestinationField;
class GuardField;
class SlotField;
class SocketCodeTable;
class PortCode;
class LImmDstRegisterField;

namespace TPEF {
    class Binary;
    class ImmediateElement;
    class CodeSection;
}

namespace TTAProgram {
    class Program;
    class Instruction;
    class Move;
    class Immediate;
    class Terminal;
}

namespace TTAMachine {
    class Machine;
    class InstructionTemplate;
    class ImmediateUnit;
    class Bus;
}

/**
 * The base class for different code compressors.
 */
class CodeCompressorPlugin {
public:

    /// Parameter struct.
    struct Parameter {
        std::string name; ///< Name of the parameter.
        std::string value; ///< Value of the parameter.
    };

    /// Table for passing plugin parameters.
    typedef std::vector<Parameter> ParameterTable;

    virtual ~CodeCompressorPlugin();

    void setParameters(ParameterTable parameters);
    void setPrograms(std::set<TPEF::Binary*> programs);
    void setMachine(const TTAMachine::Machine& machine);
    void setBEM(const BinaryEncoding& bem);

    const BinaryEncoding& binaryEncoding() const
        throw (NotAvailable);
    const TTAMachine::Machine& machine() const
        throw (NotAvailable);

    /**
     * Generates bit image of the program.
     *
     * @return The bit image.
     * @exception InvalidData If the BEM is erroneous.
     */
    virtual InstructionBitVector* compress(TPEF::Binary& program)
        throw (InvalidData) = 0;

    /**
     * Generates the HDL code of the decompressor block.
     *
     * @param stream The stream to write.
     */
    virtual void generateDecompressor(std::ostream& stream) = 0;

    /**
     * Prints description of the plugin to the given stream.
     */
    virtual void printDescription(std::ostream& stream) = 0;
    
    unsigned int memoryAddress(
        const TTAProgram::Instruction& instruction) const
        throw (InstanceNotFound);
    
    TTAProgram::Program& currentProgram() const
        throw (NotAvailable);
    
protected:
    CodeCompressorPlugin();

    InstructionBitVector* bemBits(const TTAProgram::Program& program)
        throw (InvalidData);

    void startNewProgram(TPEF::Binary& binary)
        throw (InvalidData);
    void addInstruction(
        const TTAProgram::Instruction& instruction, 
        InstructionBitVector* bits)
        throw (InvalidData, OutOfRange);
    void setInstructionToStartAtBeginningOfMAU(
        const TTAProgram::Instruction& instruction);
    void setAllInstructionsToStartAtBeginningOfMAU();
    InstructionBitVector* bemInstructionBits(
        const TTAProgram::Instruction&)
        throw (InvalidData);
    InstructionBitVector* programBits() const;

    bool hasParameter(const std::string& paramName) const;
    std::string parameterValue(const std::string& paramName) const
        throw (NotAvailable);

private:
    /// Set type for TPEF programs.
    typedef std::set<TPEF::Binary*> TPEFSet;
    /// Set type for Instructions.
    typedef std::set<const TTAProgram::Instruction*> InstructionSet;
    /// Set type for Immediates.
    typedef std::set<const TTAProgram::Immediate*> ImmediateSet;
    /// Set type for Terminals.
    typedef std::set<const TTAProgram::Terminal*> TerminalSet;
    /// Map type that maps Immediates to the Instructions they are 
    /// referring to.
    typedef std::map<const TTAProgram::Immediate*, 
                     const TTAProgram::Instruction*> RelocMap;
    /// Map type that maps Immediates to a vector of indexes.
    typedef std::map<const TTAProgram::Immediate*, 
                     std::vector<unsigned int> > IndexTable;
    /// Map type for addresses of instructions.
    typedef std::map<const TTAProgram::Instruction*, unsigned int> 
    InstructionAddressMap;

    void addBitsForICField(
        const ImmediateControlField& icField,
        const TTAProgram::Instruction& instruction,
        BitVector& bitVector) const
        throw (InvalidData);
    void addBitsForMoveSlot(
        const MoveSlot& slot,
        const TTAProgram::Instruction& instruction,
        InstructionBitVector& bitVector)
        throw (InvalidData);
    void addBitsForSourceField(
        const SourceField& srcField,
        const TTAProgram::Move& move,
        InstructionBitVector& bitVector) const
        throw (InvalidData);
    static void addBitsForDestinationField(
        const DestinationField& dstField,
        const TTAProgram::Move& move,
        BitVector& bitVector)
        throw (InvalidData);
    static void addBitsForGuardField(
        const GuardField& guardField,
        const TTAProgram::Move& move,
        BitVector& bitVector)
        throw (InvalidData);
    void addBitsForImmediateSlot(
        const ImmediateSlotField& immSlot,
        const TTAProgram::Instruction& instruction,
        BitVector& bitVector)
        throw (InvalidData);
    void addBitsForDstRegisterField(
        const LImmDstRegisterField& field,
        const TTAProgram::Instruction& instruction,
        BitVector& bitVector)
        throw (InvalidData);
    std::string instructionTemplate(
        const TTAProgram::Instruction& instruction) const
        throw (InstanceNotFound);
    bool startsAtBeginningOfMAU(
        const TTAProgram::Instruction& instruction) const;
    unsigned int nextAddressablePositionFrom(unsigned int position) const;
    void encodeLongImmediate(
        const std::string& slotName,
        int slotWidth,
        const TTAProgram::Instruction& instruction, 
        BitVector& bitVector);
    void encodeMove(
        const MoveSlot& slot,
        const TTAProgram::Instruction& instruction,
        InstructionBitVector& bitVector) const
        throw (InvalidData);
    static void encodeNOP(const MoveSlot& slot, BitVector& bitVector)
        throw (InvalidData);
    static void encodeIUTerminal(
        const SlotField& field, 
        const TTAProgram::Terminal& terminal, 
        BitVector& bitVector)
        throw (InvalidData);
    static void encodeRFTerminal(
        const SlotField& field,
        const TTAProgram::Terminal& terminal,
        BitVector& bitVector)
        throw (InvalidData);
    static void encodeFUTerminal(
        const SlotField& field,
        const TTAProgram::Terminal& terminal,
        BitVector& bitVector)
        throw (InvalidData);
    void encodeImmediateTerminal(
        const SourceField& field,
        const TTAProgram::Terminal& terminal,
        InstructionBitVector& bitVector) const
        throw (InvalidData);
    static void encodeSlotField(
        const SlotField& field,
        const BitVector& componentIDBits,
        const BitVector& socketCodeBits,
        BitVector& BitVector);
    static BitVector socketCodeBits(
        const SocketCodeTable& socketCodes,
        const PortCode& portCode,
        unsigned int registerIndex);
    void initializeRelocations();
    static void addBits(
        unsigned int number, 
        int leftmostBit,
        int rightmostBit, 
        BitVector& bitVector);
    TTAProgram::Terminal* immediateTerminal(
        const TPEF::CodeSection& codeSection,
        unsigned int elementIndex,
        const TPEF::ImmediateElement& immElem, 
        const TTAProgram::Instruction& instruction) const;
    TTAProgram::Immediate* programImmediate(
        const TPEF::ImmediateElement& immElem,
        const TTAProgram::Instruction& instruction) const;

    /// TPEFs run in the machine.
    TPEFSet tpefPrograms_;
    /// The current TPEF.
    TPEF::Binary* currentTPEF_;
    /// The current program in POM.
    TTAProgram::Program* currentPOM_;
    /// The binary encoding map.
    const BinaryEncoding* bem_;
    /// The machine.
    const TTAMachine::Machine* machine_;
    /// Parameters for the plugin.
    ParameterTable parameters_;

    /// The bits of the program.
    InstructionBitVector* programBits_;
    /// Tells whether all the instructions starts at the beginning of MAU.
    bool allStartsAtBeginningOfMAU_;
    /// The size of the minimum addressable unit.
    int mau_;
    /// The offset of the instruction memory (the address of the first 
    /// instruction).
    int addressSpaceOffset_;

    /// Set of instructions that start at the beginning of MAU.
    InstructionSet instructionsAtBeginningOfMAU_;
    /// Set of (long) immediates that refer to an instruction.
    ImmediateSet immediatesToRelocate_;
    /// Set of (short) immediate terminals that refer to an instruction.
    TerminalSet terminalsToRelocate_;
    
    /// Contains information to which instructions the immediates are
    /// referring to.
    RelocMap relocMap_;
    /// A table of indexes used when encoding long immediates.
    IndexTable indexTable_;
    
    /// Contains the addresses of instructions.
    InstructionAddressMap instructionAddresses_;
};

#endif
