/**
 * @file DataDefinition.hh
 *
 * Declaration of DataDefinition class.
 *
 * @author Mikael Lepistö 2006 (mikael.lepisto@tut.fi)
 * @author Pekka Jääskeläinen 2006 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_DATA_DEFINITION_HH
#define TTA_DATA_DEFINITION_HH

#include <vector>
#include "BaseType.hh"
#include "Exception.hh"
#include "Address.hh"

namespace TTAMachine {
    class AddressSpace;
}

namespace TTAProgram {
    
/**
 * Describes characteristics of an initialized or uninitialized 
 * memory area.
 */
class DataDefinition {
public:
    DataDefinition(
        Address start, 
        int size, MinimumAddressableUnit *initData = NULL)
        throw (OutOfRange);

    DataDefinition(
        Address start, 
        const std::vector<MinimumAddressableUnit>& initData)
        throw (OutOfRange);

    virtual ~DataDefinition();

    virtual Address startAddress() const;
    virtual void setStartAddress(Address start);
    
    virtual bool isInitialized() const;
    virtual MinimumAddressableUnit MAU(int index) const;
    virtual int size() const;

    virtual bool isAddress() const;
    virtual bool isInstructionAddress() const;
    virtual Address destinationAddress() const;
    virtual void setDestinationAddress(Address dest);
    
    virtual DataDefinition* copy() const;

private:
    /// Start address of definition.
    Address start_;

    /// Size of uninitialized data definition.
    int size_;
    
    /// Init data of definition.
    std::vector<MinimumAddressableUnit>* data_;

};

}

#endif
