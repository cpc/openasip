/**
 * @file DataAddressDef.hh
 *
 * Declaration of DataAddressDef class.
 *
 * @author Mikael Lepistö 2006 (mikael.lepisto@tut.fi)
 * @note rating: red
 */

#ifndef TTA_DATA_ADDRESS_DEF_HH
#define TTA_DATA_ADDRESS_DEF_HH

#include "DataDefinition.hh"
#include "Address.hh"

namespace TTAProgram {
    
/**
 * Describes characteristics of a memory area containing 
 * an address.
 */
class DataAddressDef : public DataDefinition {
public:
    DataAddressDef(Address start, int size, Address dest);
    virtual ~DataAddressDef();
    
    virtual bool isInitialized() const;
    virtual bool isAddress() const;

    virtual Address destinationAddress() const;
    virtual void setDestinationAddress(Address dest);
    
    DataDefinition* copy() const;

private:
    /// Destination address of a data area.
    Address dest_;    
};

}

#endif
