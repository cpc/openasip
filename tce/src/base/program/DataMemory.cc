/**
 * @file DataMemory.cc
 *
 * Implementation of DataMemory class.
 *
 * @author Mikael Lepistö 2006 (mikael.lepisto@tut.fi)
 * @note rating: red
 */

#include "DataMemory.hh"
#include "AddressSpace.hh"
#include "DataDefinition.hh"
#include "MapTools.hh"

namespace TTAProgram {

/**
 * Sets address space of the memory.
 * 
 * @param aSpace Address space of the memory.
 */
DataMemory::DataMemory(const TTAMachine::AddressSpace& aSpace) :
    ramSpace_(&aSpace) {
}

/**
 * Deletes all the added definitions.
 */
DataMemory::~DataMemory() {
    MapTools::deleteAllValues(dataDefs_);
}


/** 
 * Adds data to memory.
 *
 * Data definitions are sorted by starting address of defined area.
 *
 * @param dataDef Definition to add.
 */
void 
DataMemory::addDataDefinition(DataDefinition* dataDef) {
    dataDefs_[dataDef->startAddress().location()] = dataDef;
    // clear index cache since indices may change
    indexCache_.clear();
}

/** 
 * Finds the data definition, which contains the requested address.
 *
 * @param address Address whose data definition is requested.
 * @return Data definition, which contains requested address.
 */
DataDefinition& 
DataMemory::dataDefinition(Address address) const {
    std::map<AddressImage, DataDefinition*>::const_iterator iter = 
        dataDefs_.upper_bound(address.location());
    
    if (iter != dataDefs_.begin()) {
        iter--;
        DataDefinition* dataDef = (*iter).second;
        // this data definition should contain the requested address
        if (address.location() < dataDef->startAddress().location() + dataDef->size()) {
            return *dataDef;
        }
    }

    throw NotAvailable(
        __FILE__, __LINE__, __func__,
        "Address space: " + ramSpace_->name() +
        " does not contain data definition for requested address:" +
        Conversion::toString(address.location()));
}


/** 
 * Finds the data definition by index.
 *
 * Indices may change if new definitions are added to memory.
 *
 * @param index Index of the requested data definition.
 * @return Data definition, which contains requested address.
 */
DataDefinition& 
DataMemory::dataDefinition(int index) const {
    if (indexCache_.empty()) {
        // create cache
        for (std::map<AddressImage, DataDefinition*>::const_iterator iter = 
                 dataDefs_.begin(); iter != dataDefs_.end(); iter++) {

            indexCache_.push_back((*iter).second);
        }
    }

    return *indexCache_[index];
}

/**
 * Returns the number of data definitions stored in memory.
 *
 * @return Number of data definitions.
 */
int 
DataMemory::dataDefinitionCount() const {
    return dataDefs_.size();
}

/**
 * Deletes the data definition at the given index.
 *
 * @param index Which data definition to delete.
 */
void
DataMemory::deleteDataDefinition(int index) {

    if (index < 0 || index >= dataDefinitionCount()) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }

    DataDefMap::iterator iter = dataDefs_.begin();

    for (int i = 0; i < index; i++) {
        iter++;
    }

    dataDefs_.erase(iter);
    indexCache_.clear();
}

/**
 * Returns the address space of the data memory.
 *
 * @return The address space of the data memory.
 */
const TTAMachine::AddressSpace& 
DataMemory::addressSpace() const {
    return *ramSpace_;
}

/**
 * Set new address space for the data memory.
 *
 * @param space New address space.
 */
void
DataMemory::setAddressSpace(const TTAMachine::AddressSpace& space) {
    ramSpace_ = &space;
    for (int i = 0; i < dataDefinitionCount(); i++) {
        DataDefinition& def = dataDefinition(i);
        def.setStartAddress(Address(def.startAddress().location(), space));
    }
}

/**
 * POM style copy constructor, which supports dynamic binding.
 *
 * @return Copy of the object. 
 */
DataMemory* 
DataMemory::copy() const {
    DataMemory* memCopy = new DataMemory(*ramSpace_);
    
    for (int i = 0; i < dataDefinitionCount(); i++) {
        memCopy->addDataDefinition(dataDefinition(i).copy());
    }

    return memCopy;
}

}
