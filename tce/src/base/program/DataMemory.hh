/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
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
