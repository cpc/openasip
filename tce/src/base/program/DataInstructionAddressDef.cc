/**
 * @file DataInstructionAddressDef.cc
 *
 * Implementation of DataInstructionAddressDef class.
 *
 * @author Mikael Lepistö 2006 (mikael.lepisto@tut.fi)
 * @note rating: red
 */

#include "DataInstructionAddressDef.hh"
#include "InstructionReference.hh"
#include "Instruction.hh"
#include "Address.hh"

namespace TTAProgram {

/**
 * Creates data area definition containing reference to an instruction.
 *
 * @param start Starting address of definition.
 * @param size Number of MAUs that area represents.
 * @param dest Instruction which is referred by the data definition.
 */
DataInstructionAddressDef::DataInstructionAddressDef(
    Address start, int size, InstructionReference& dest) :
    DataDefinition(start, size), dest_(dest) {
}

/**
 * Nothing to free.
 */
DataInstructionAddressDef::~DataInstructionAddressDef() {
}

/**
 * Returns true, class defines always initialized data area.
 * 
 * @return Always true.
 */
bool 
DataInstructionAddressDef::isInitialized() const {
    return true;
}

/**
 * Returns true, class contains always address reference.
  * 
 * @return Always true.
 */
bool 
DataInstructionAddressDef::isAddress() const {
    return true;
}

/**
 * Returns true if init data of definition contains an instruction address.
 *
 * @return True if init data of definition contains an instruction address.
 */
bool 
DataInstructionAddressDef::isInstructionAddress() const {
    return true;
}

/**
 * Returns destination address, where to intialization value refers.
  * 
 * @return Destination address of initialized data area.
 */
Address
DataInstructionAddressDef::destinationAddress() const {
    return dest_.instruction().address();
}

/**
 * Copying of this class is *not* allowed.

 * This class can't be copied, because InstructionReferece cannot
 * be copied correctly.
 *
 * @return Throws always an exception.
 */
DataDefinition* 
DataInstructionAddressDef::copy() const {
    throw WrongSubclass(
        __FILE__, __LINE__, __func__, 
        "DataInstructionAddress definition cannot be copied.");
}

}
