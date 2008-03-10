/**
 * @file DataAddressDef.cc
 *
 * Implementation of DataAddressDef class.
 *
 * @author Mikael Lepistö 2006 (mikael.lepisto@tut.fi)
 * @note rating: red
 */

#include "DataAddressDef.hh"

namespace TTAProgram {

/**
 * Creates data area definition containing address.
 *
 * @param start Starting address of definition.
 * @param size Number of MAUs that area represents.
 * @param dest Address where to data definition refers.
 */
DataAddressDef::DataAddressDef(Address start, int size, Address dest) :
    DataDefinition(start, size), dest_(dest) {

}

/**
 * Nothing to free.
 */
DataAddressDef::~DataAddressDef() {
}

/**
 * Returns true, class defines always initialized data area.
 * 
 * @return Always true.
 */
bool 
DataAddressDef::isInitialized() const {
    return true;
}

/**
 * Returns true, class contains always address reference.
  * 
 * @return Always true.
 */
bool 
DataAddressDef::isAddress() const {
    return true;
}

/**
 * Returns destination address, where to intialization value refers.
  * 
 * @return Destination address of initialized data area.
 */
Address
DataAddressDef::destinationAddress() const {
    return dest_;
}

/**
 * Set new destination address for the data definition.
 *
 * @param dest New destination address.
 */
void
DataAddressDef::setDestinationAddress(Address dest) {
    dest_ = dest;
}

/**
 * POM style copy constructor, which supports dynamic binding.
 *
 * @return Copy of the object. 
 */
DataDefinition* 
DataAddressDef::copy() const {
    DataAddressDef* newDef = new DataAddressDef(
        startAddress(), size(), destinationAddress());
    
    return newDef;
}

}
