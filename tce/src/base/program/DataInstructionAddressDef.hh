/**
 * @file DataInstructionAddressDef.hh
 *
 * Declaration of DataInstructionAddressDef class.
 *
 * @author Mikael Lepistö 2006 (mikael.lepisto@tut.fi)
 * @note rating: red
 */

#ifndef TTA_DATA_INSTRUCTION_ADDRESS_DEF_HH
#define TTA_DATA_INSTRUCTION_ADDRESS_DEF_HH

#include "DataDefinition.hh"

namespace TTAProgram {
    
    class Address;
    class InstructionReference;

/**
 * Describes characteristics of a memory area containing 
 * an address to an instruction.
 */
class DataInstructionAddressDef : public DataDefinition {
public:
    DataInstructionAddressDef(
        Address start, int size, InstructionReference& dest);

    virtual ~DataInstructionAddressDef();
    
    virtual bool isInitialized() const;
    virtual bool isAddress() const;
    virtual bool isInstructionAddress() const;

    virtual Address destinationAddress() const;

    DataDefinition* copy() const;

private:
    /// Destination instruction of data area.
    InstructionReference& dest_;
};

}

#endif
