/**
 * @file DataMemory.hh
 *
 * Declaration of DataMemory class.
 *
 * @author Mikael Lepistö 2006 (mikael.lepisto@tut.fi)
 * @author Pekka Jääskeläinen 2006 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_DATA_MEMORY_HH
#define TTA_DATA_MEMORY_HH

#include <map>
#include <vector>

#include "Exception.hh"
#include "Address.hh"

namespace TTAMachine {
    class AddressSpace;
}

namespace TTAProgram {    
    class DataDefinition;
    
/**
 * Represents an address space containing data.
 *
 * Data definitions are ordered in the DataMemory class so that largest index
 * contains the last data definition.
 */
class DataMemory {
public:
    DataMemory(const TTAMachine::AddressSpace& aSpace);
    virtual ~DataMemory();

    void addDataDefinition(DataDefinition* dataDef);
    DataDefinition& dataDefinition(Address address) const;
    DataDefinition& dataDefinition(int index) const;
    int dataDefinitionCount() const;

    void deleteDataDefinition(int index);

    const TTAMachine::AddressSpace& addressSpace() const;
    void setAddressSpace(const TTAMachine::AddressSpace& space);

    DataMemory* copy() const;

private:
    /// Map for data definitions.
    typedef std::map<AddressImage, DataDefinition*> DataDefMap;

    /// Address space of the memory.
    const TTAMachine::AddressSpace* ramSpace_;

    /// Data definitions for the address space.
    DataDefMap dataDefs_;

    /// Cache for indexing dataDefinitions
    mutable std::vector<DataDefinition*> indexCache_;
};

}

#endif
